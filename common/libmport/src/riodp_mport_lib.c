/*
 * Copyright 2014, 2015 Integrated Device Technology, Inc.
 *
 * RapidIO mport device API library
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License(GPL) Version 2, or the BSD-3 Clause license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h> /* For size_t */
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

#include <linux/rio_cm_cdev.h>
#define CONFIG_RAPIDIO_DMA_ENGINE
#include <linux/rio_mport_cdev.h>

#include "rio_misc.h"
#include "string_util.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIO_MPORT_DEV_PATH "/dev/rio_mport"
#define RIO_CMDEV_PATH "/dev/rio_cm"

#define MAX_MPORT	32

struct rapidio_mport_mailbox {
	int fd;
	uint8_t mport_id;
};

struct rio_channel {
	uint16_t id;
	uint16_t remote_channel;
	uint8_t mport_id;
};

struct rapidio_mport_socket {
	struct rapidio_mport_mailbox *mbox;
	struct rio_channel ch;
};

int riomp_mgmt_mport_create_handle(uint32_t mport_id, int flags,
		riomp_mport_t *mport_handle)
{
	char path[32] = {0};
	int fd, ret;
	struct rapidio_mport_handle *hnd = NULL;

	if ((mport_id >= MAX_MPORT) || (NULL == mport_handle)) {
		return -(errno = EINVAL);
	}

	// XXX O_SYNC    = 0x101000 will break this scheme

	const int oflags = flags & 0xFFFF;

	snprintf(path, sizeof(path), RIO_MPORT_DEV_PATH "%d", mport_id);

	fd = open(path, O_RDWR | O_CLOEXEC | oflags);
	if (fd == -1) {
		return -errno;
	}

	hnd = (struct rapidio_mport_handle *)calloc(1,
			sizeof(struct rapidio_mport_handle));
	if (!(hnd)) {
		ret = -errno;
		close(fd);
		return ret;
	}

	hnd->fd = fd;
	hnd->mport_id = mport_id;

	*mport_handle = hnd;

	return 0;
}

int riomp_mgmt_mport_destroy_handle(riomp_mport_t *mport_handle)
{
	struct rapidio_mport_handle *hnd = *mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	close(hnd->fd);
	free(hnd);

	return 0;
}

int riomp_mgmt_get_handle_id(riomp_mport_t mport_handle, int *id)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	/*
	 * if the architecture does not support posix file handles return a different identifier.
	 */
	*id = hnd->fd;
	return 0;
}

int riomp_sock_mbox_init(void)
{
	return open(RIO_CMDEV_PATH, O_RDWR | O_CLOEXEC);
}

/** \brief Get the list of Mports installed
 *
 * \param[out] dev_ids List of Mports installed (See Notes)
 * \param[in, out] number_of_mports Just set it to RIO_MAX_MPORTS
 * \note dev_ids is an array of ((mport_id << 16) | destid)
 * \note Free dev_ids via riomp_mgmt_free_mport_list.
 * \returns 0 if OK, negative on error
 */
int riomp_mgmt_get_mport_list(mport_list_t **dev_ids, uint8_t *number_of_mports)
{
	mport_list_t entries = *number_of_mports;
	mport_list_t *list;
	int fd;
	int ret = -1;

	/* Open RapidIO Channel Manager */
	fd = riomp_sock_mbox_init();
	if (fd < 0) {
		return -1;
	}

	//@sonar:off - c:S3584 Allocated memory not released
	// The memory is not supposed to be released, it is supposed to be
	// handed back to the user.
	list = (mport_list_t *)calloc((entries + 1), sizeof(*list));
	if (NULL == list) {
		goto outfd;
	}

	/* Request MPORT list from the driver (first entry is list size) */
	list[0] = entries;
	if (ioctl(fd, RIO_CM_MPORT_GET_LIST, list)) {
		free(list);
		ret = errno;
		goto outfd;
	}

	/* Return list information */
	*dev_ids = &list[1]; /* pointer to the list */
	*number_of_mports = *list; /* return real number of mports */

	ret = 0;
	//@sonar:on

outfd:
	close(fd);
	return ret;
}

int riomp_mgmt_free_mport_list(mport_list_t **dev_ids)
{
	/* Get head of the list, because we did hide the list size and mport ID
	 * parameters
	 */
	mport_list_t *list;

	if (NULL == dev_ids) {
		return -1;
	}

	list = (*dev_ids) - 1;
	free(list);
	return 0;
}

int riomp_mgmt_get_ep_list(uint8_t mport_id, did_val_t **did_values,
		uint32_t *number_of_eps)
{
	int fd;
	int ret = 0;
	did_val_t entries;
	did_val_t *list;

	/* Open mport */
	fd = riomp_sock_mbox_init();
	if (fd < 0) {
		return -1;
	}

	/* Get list size */
	entries = mport_id;
	if (ioctl(fd, RIO_CM_EP_GET_LIST_SIZE, &entries)) {
#ifdef MPORT_DEBUG
		fprintf(stderr, "%s ep_get_list_size ioctl failed: %s\n", __func__, strerror(errno));
#endif
		ret = errno;
		goto outfd;
	}
#ifdef MPORT_DEBUG
	printf("RIODP: %s() has %d entries\n", __func__, entries);
#endif
	/* Get list */
	//@sonar:off - c:S3584 Allocated memory not released
	// The memory is not supposed to be released,
	// it is supposed to be handed back to the user.
	list = (did_val_t *)calloc((entries + 2), sizeof(*list));
	if (NULL == list) {
		ret = -1;
		goto outfd;
	}

	/* Get list (first entry is list size) */
	list[0] = entries;
	list[1] = mport_id;
	if (ioctl(fd, RIO_CM_EP_GET_LIST, list)) {
		free(list);
		ret = errno;
		goto outfd;
	}

	/* Pass to callee, first entry of list is entries in list */
	*did_values = &list[2];
	*number_of_eps = entries;
	//@sonar:on

outfd:
	close(fd);
	return ret;
}

int riomp_mgmt_free_ep_list(did_val_t **did_values)
{
	/* Get head of the list, because we did hide the list size and mport ID
	 * parameters
	 */
	did_val_t *list;

	if (NULL == did_values) {
		return -1;
	}

	list = (*did_values) - 2;
	free(list);
	return 0;
}

static inline enum rio_exchange convert_directio_type(
		enum riomp_dma_directio_type type)
{
	switch (type) {
	case RIO_DIRECTIO_TYPE_NWRITE:
		return RIO_EXCHANGE_NWRITE;
	case RIO_DIRECTIO_TYPE_NWRITE_R:
		return RIO_EXCHANGE_NWRITE_R;
	case RIO_DIRECTIO_TYPE_NWRITE_R_ALL:
		return RIO_EXCHANGE_NWRITE_R_ALL;
	case RIO_DIRECTIO_TYPE_SWRITE:
		return RIO_EXCHANGE_SWRITE;
	case RIO_DIRECTIO_TYPE_SWRITE_R:
		return RIO_EXCHANGE_SWRITE_R;
	default:
		return RIO_EXCHANGE_DEFAULT;
	}
}

static inline enum rio_transfer_sync convert_directio_sync(
		enum riomp_dma_directio_transfer_sync sync)
{
	//@sonar:off - c:S3458
	switch (sync) {
	default:
	case RIO_DIRECTIO_TRANSFER_SYNC:
		// sync as default is the smallest pitfall
		return RIO_TRANSFER_SYNC;
	case RIO_DIRECTIO_TRANSFER_ASYNC:
		return RIO_TRANSFER_ASYNC;
	case RIO_DIRECTIO_TRANSFER_FAF:
		return RIO_TRANSFER_FAF;
	}
	//@sonar:on
}

/*
 * Perform DMA data write to target transfer using user space source buffer
 */
int riomp_dma_write(riomp_mport_t mport_handle, did_val_t did_val,
		uint64_t tgt_addr, void *buf, uint32_t size,
		enum riomp_dma_directio_type wr_mode,
		enum riomp_dma_directio_transfer_sync sync,
		struct rapidio_mport_interleave *interleave)
{
	struct rio_transaction tran;
	struct rio_transfer_io xfer;
	int ret;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	xfer.rioid = did_val;
	xfer.rio_addr = tgt_addr;
	xfer.loc_addr = (uintptr_t)buf;
	xfer.length = size;
	xfer.handle = 0;
	xfer.offset = 0;
	xfer.method = convert_directio_type(wr_mode);
	if (interleave == NULL) {
		xfer.ssdist = 0;
		xfer.sssize = 0;
		xfer.dsdist = 0;
		xfer.dssize = 0;
	} else {
		xfer.ssdist = interleave->ssdist;
		xfer.sssize = interleave->sssize;
		xfer.dsdist = interleave->dsdist;
		xfer.dssize = interleave->dssize;
	}

	tran.block = (uintptr_t)&xfer;
	tran.count = 1;
	tran.transfer_mode = RIO_TRANSFER_MODE_TRANSFER;
	tran.sync = convert_directio_sync(sync);
	tran.dir = RIO_TRANSFER_DIR_WRITE;
	tran.pad0 = 0;

	ret = ioctl(hnd->fd, RIO_TRANSFER, &tran);
	return (ret < 0) ? -errno : ret;
}

/*
 * Perform DMA data write to target transfer using kernel space source buffer
 */
int riomp_dma_write_d(riomp_mport_t mport_handle, did_val_t did_val,
		uint64_t tgt_addr, uint64_t handle, uint32_t offset,
		uint32_t size, enum riomp_dma_directio_type wr_mode,
		enum riomp_dma_directio_transfer_sync sync,
		struct rapidio_mport_interleave *interleave)
{
	struct rio_transaction tran;
	struct rio_transfer_io xfer;
	int ret;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	xfer.rioid = did_val;
	xfer.rio_addr = tgt_addr;
	xfer.loc_addr = (uintptr_t)NULL;
	xfer.length = size;
	xfer.handle = handle; // According to drivers/rapidio/devices/rio_mport_cdev.c in rio_dma_transfer()
			      //    baddr = (dma_addr_t)xfer->handle;
	xfer.offset = offset;
	xfer.method = convert_directio_type(wr_mode);
	if (interleave == NULL) {
		xfer.ssdist = 0;
		xfer.sssize = 0;
		xfer.dsdist = 0;
		xfer.dssize = 0;
	} else {
		xfer.ssdist = interleave->ssdist;
		xfer.sssize = interleave->sssize;
		xfer.dsdist = interleave->dsdist;
		xfer.dssize = interleave->dssize;
	}

	tran.block = (uintptr_t)&xfer;
	tran.count = 1;
	tran.transfer_mode = RIO_TRANSFER_MODE_TRANSFER;
	tran.sync = convert_directio_sync(sync);
	tran.dir = RIO_TRANSFER_DIR_WRITE;
	tran.pad0 = 0;

	ret = ioctl(hnd->fd, RIO_TRANSFER, &tran);
	return (ret < 0) ? -errno : ret;
}

/*
 * Perform DMA data read from target transfer using user space destination buffer
 */
int riomp_dma_read(riomp_mport_t mport_handle, did_val_t did_val,
		uint64_t tgt_addr, void *buf, uint32_t size,
		enum riomp_dma_directio_transfer_sync sync,
		struct rapidio_mport_interleave *interleave)
{
	struct rio_transaction tran;
	struct rio_transfer_io xfer;
	int ret;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	xfer.rioid = did_val;
	xfer.rio_addr = tgt_addr;
	xfer.loc_addr = (uintptr_t)buf;
	xfer.length = size;
	xfer.handle = 0;
	xfer.offset = 0;
	if (interleave == NULL) {
		xfer.ssdist = 0;
		xfer.sssize = 0;
		xfer.dsdist = 0;
		xfer.dssize = 0;
	} else {
		xfer.ssdist = interleave->ssdist;
		xfer.sssize = interleave->sssize;
		xfer.dsdist = interleave->dsdist;
		xfer.dssize = interleave->dssize;
	}

	tran.block = (uintptr_t)&xfer;
	tran.count = 1;
	tran.transfer_mode = RIO_TRANSFER_MODE_TRANSFER;
	tran.sync = convert_directio_sync(sync);
	tran.dir = RIO_TRANSFER_DIR_READ;
	tran.pad0 = 0;

	ret = ioctl(hnd->fd, RIO_TRANSFER, &tran);
	return (ret < 0) ? -errno : ret;
}

/*
 * Perform DMA data read from target transfer using kernel space destination buffer
 */
int riomp_dma_read_d(riomp_mport_t mport_handle, did_val_t did_val,
		uint64_t tgt_addr, uint64_t handle, uint32_t offset,
		uint32_t size, enum riomp_dma_directio_transfer_sync sync,
		struct rapidio_mport_interleave *interleave)
{
	struct rio_transaction tran;
	struct rio_transfer_io xfer;
	int ret;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	xfer.rioid = did_val;
	xfer.rio_addr = tgt_addr;
	xfer.loc_addr = (uintptr_t)NULL;
	xfer.length = size;
	xfer.handle = handle;
	xfer.offset = offset;
	if (interleave == NULL) {
		xfer.ssdist = 0;
		xfer.sssize = 0;
		xfer.dsdist = 0;
		xfer.dssize = 0;
	} else {
		xfer.ssdist = interleave->ssdist;
		xfer.sssize = interleave->sssize;
		xfer.dsdist = interleave->dsdist;
		xfer.dssize = interleave->dssize;
	}

	tran.block = (uintptr_t)&xfer;
	tran.count = 1;
	tran.transfer_mode = RIO_TRANSFER_MODE_TRANSFER;
	tran.sync = convert_directio_sync(sync);
	tran.dir = RIO_TRANSFER_DIR_READ;
	tran.pad0 = 0;

	ret = ioctl(hnd->fd, RIO_TRANSFER, &tran);
	return (ret < 0) ? -errno : ret;
}

/*
 * Wait for DMA transfer completion
 */
int riomp_dma_wait_async(riomp_mport_t mport_handle, uint32_t cookie,
		uint32_t tmo)
{
	struct rio_async_tx_wait wparam;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	wparam.token = cookie;
	wparam.timeout = tmo;

	if (ioctl(hnd->fd, RIO_WAIT_FOR_ASYNC, &wparam)) {
		return -errno;
	}
	return 0;
}

/*
 * Allocate and map into RapidIO space a local kernel space data buffer
 * (for inbound RapidIO data read/write requests)
 */
int riomp_dma_ibwin_map(riomp_mport_t mport_handle, uint64_t *rio_base,
		uint32_t size, uint64_t *handle)
{
	struct rio_mmap ib;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd || NULL == rio_base || NULL == handle) {
		return -EINVAL;
	}

	memset(&ib, 0, sizeof(ib));
	ib.rio_addr = (*rio_base == RIOMP_MAP_ANY_ADDR ) ?
			RIO_MAP_ANY_ADDR : *rio_base;
	ib.length = size;
	ib.address = (*handle == RIOMP_MAP_ANY_ADDR ) ?
			RIO_MAP_ANY_ADDR : *handle;

	if (ioctl(hnd->fd, RIO_MAP_INBOUND, &ib)) {
		return -errno;
	}

	*handle = ib.handle;
	*rio_base = ib.rio_addr;
	return 0;
}

/*
 * Free and unmap from RapidIO space a local kernel space data buffer
 */
int riomp_dma_ibwin_free(riomp_mport_t mport_handle, uint64_t *handle)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (ioctl(hnd->fd, RIO_UNMAP_INBOUND, handle)) {
		return -errno;
	}
	return 0;
}

int riomp_dma_obwin_map(riomp_mport_t mport_handle, did_val_t did_val,
		uint64_t rio_base, uint32_t size, uint64_t *handle)
{
	struct rio_mmap ob;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	memset(&ob, 0, sizeof(ob));
	ob.rioid = did_val;
	ob.rio_addr = rio_base;
	ob.length = size;

	if (ioctl(hnd->fd, RIO_MAP_OUTBOUND, &ob)) {
		return -errno;
	}

	*handle = ob.handle;
	return 0;
}

int riomp_dma_obwin_free(riomp_mport_t mport_handle, uint64_t *handle)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (ioctl(hnd->fd, RIO_UNMAP_OUTBOUND, handle)) {
		return -errno;
	}
	return 0;
}

/*
 * Allocate a local kernel space data buffer for DMA data transfers
 */
int riomp_dma_dbuf_alloc(riomp_mport_t mport_handle, uint32_t size,
		uint64_t *handle)
{
	struct rio_dma_mem db;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd || NULL == handle) {
		return -EINVAL;
	}

	db.length = size;
	db.dma_handle = 0;
	db.address = (*handle == RIOMP_MAP_ANY_ADDR ) ?
			RIO_MAP_ANY_ADDR : *handle;

	if (ioctl(hnd->fd, RIO_ALLOC_DMA, &db)) {
		return -errno;
	}

	*handle = db.dma_handle;
	return 0;
}

/*
 * Free previously allocated local kernel space data buffer
 */
int riomp_dma_dbuf_free(riomp_mport_t mport_handle, uint64_t *handle)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (ioctl(hnd->fd, RIO_FREE_DMA, handle)) {
		return -errno;
	}
	return 0;
}

/*
 * map phys address range to process virtual memory
 */
int riomp_dma_map_memory(riomp_mport_t mport_handle, size_t size, off_t paddr,
		void **vaddr)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd || !size || !vaddr) {
		return -EINVAL;
	}

	*vaddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, hnd->fd,
			paddr);
	if (*vaddr == MAP_FAILED) {
		return -errno;
	}
	return 0;
}

/*
 * unmap phys address range to process virtual memory
 */
int riomp_dma_unmap_memory(size_t size, void *vaddr)
{
	if (!size || !vaddr) {
		return -EINVAL;
	}
	return munmap(vaddr, size);
}

/*
 * Query mport status/capabilities
 */
int riomp_mgmt_query(riomp_mport_t mport_handle,
		struct riomp_mgmt_mport_properties *qresp)
{
	struct rapidio_mport_handle *hnd = mport_handle;
	struct rio_mport_properties prop;

	if (!qresp || !hnd) {
		return -EINVAL;
	}

	memset(&prop, 0, sizeof(prop));
	if (ioctl(hnd->fd, RIO_MPORT_GET_PROPERTIES, &prop)) {
		return -errno;
	}

	qresp->did_val = prop.hdid;
	qresp->id = prop.id;
	qresp->index = prop.index;
	qresp->flags = prop.flags;
	qresp->sys_size = prop.sys_size;
	qresp->port_ok = prop.port_ok;
	qresp->link_speed = prop.link_speed;
	qresp->link_width = prop.link_width;
	qresp->dma_max_sge = prop.dma_max_sge;
	qresp->dma_max_size = prop.dma_max_size;
	qresp->dma_align = prop.dma_align;
	qresp->transfer_mode = prop.transfer_mode;
	qresp->cap_sys_size = prop.cap_sys_size;
	qresp->cap_addr_size = prop.cap_addr_size;
	qresp->cap_transfer_mode = prop.cap_transfer_mode;
	qresp->cap_mport = prop.cap_mport;

	return 0;
}

/*
 * Read from local (mport) device register
 */
int riomp_mgmt_lcfg_read(riomp_mport_t mport_handle, uint32_t offset,
		uint32_t size, uint32_t *data)
{
	struct rio_mport_maint_io mt;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd || NULL == data) {
		return -EINVAL;
	}

	// Klokwork appears to loose track of *data and claims it can be used uninitialized
	// Perhaps because of the mt_buffer wrapping, or perhaps it thinks -errno can be 0
	// on a successfull return
	*data = 0;

	memset(&mt, 0, sizeof(mt));
	mt.offset = offset;
	mt.length = size;
	mt.buffer = (uintptr_t)data;

	if (ioctl(hnd->fd, RIO_MPORT_MAINT_READ_LOCAL, &mt)) {
		return -errno;
	}
	return 0;
}

/*
 * Write to local (mport) device register
 */
int riomp_mgmt_lcfg_write(riomp_mport_t mport_handle, uint32_t offset,
		uint32_t size, uint32_t data)
{
	struct rio_mport_maint_io mt;
	struct rapidio_mport_handle *hnd = mport_handle;

	/* size is enforced to match 'data' parameter type */
	if ((NULL == hnd) || (sizeof(uint32_t) != size)) {
		return -EINVAL;
	}

	memset(&mt, 0, sizeof(mt));
	mt.offset = offset;
	mt.length = size;
	mt.buffer = (uintptr_t)&data;

	if (ioctl(hnd->fd, RIO_MPORT_MAINT_WRITE_LOCAL, &mt)) {
		return -errno;
	}
	return 0;
}

/*
 * Maintenance read from target RapidIO device register
 */
int riomp_mgmt_rcfg_read(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, uint32_t offset, uint32_t size, uint32_t *data)
{
	struct rio_mport_maint_io mt;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd || NULL == data) {
		return -EINVAL;
	}

	// Klokwork appears to loose track of *data and claims it can be used uninitialized
	// Perhaps because of the mt_buffer wrapping, or perhaps it thinks -errno can be 0
	// on a successfull return
	*data = 0;

	mt.rioid = did_val;
	mt.hopcount = hc;
	memset(&mt.pad0, 0, sizeof(mt.pad0));
	mt.offset = offset;
	mt.length = size;
	mt.buffer = (uintptr_t)data;

	if (ioctl(hnd->fd, RIO_MPORT_MAINT_READ_REMOTE, &mt)) {
		return -errno;
	}
	return 0;
}

/*
 * Maintenance write to target RapidIO device register
 */
int riomp_mgmt_rcfg_write(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, uint32_t offset, uint32_t size, uint32_t data)
{
	struct rio_mport_maint_io mt;
	struct rapidio_mport_handle *hnd = mport_handle;

	/* size is enforced to match 'data' parameter type */
	if ((NULL == hnd) || (sizeof(uint32_t) != size)) {
		return -EINVAL;
	}

	mt.rioid = did_val;
	mt.hopcount = hc;
	memset(&mt.pad0, 0, sizeof(mt.pad0));
	mt.offset = offset;
	mt.length = size;
	mt.buffer = (uintptr_t)&data;

	if (ioctl(hnd->fd, RIO_MPORT_MAINT_WRITE_REMOTE, &mt)) {
		return -errno;
	}
	return 0;
}

/*
 * Enable (register) receiving range of RapidIO doorbell events
 */
int riomp_mgmt_dbrange_enable(riomp_mport_t mport_handle,
		did_val_t did_val, uint16_t start, uint16_t end)
{
	struct rio_doorbell_filter dbf;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	dbf.rioid = did_val;
	dbf.low = start;
	dbf.high = end;
	dbf.pad0 = 0;

	if (ioctl(hnd->fd, RIO_ENABLE_DOORBELL_RANGE, &dbf)) {
		return -errno;
	}
	return 0;
}

/*
 * Disable (unregister) range of inbound RapidIO doorbell events
 */
int riomp_mgmt_dbrange_disable(riomp_mport_t mport_handle,
		did_val_t did_val, uint16_t start, uint16_t end)
{
	struct rio_doorbell_filter dbf;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	dbf.rioid = did_val;
	dbf.low = start;
	dbf.high = end;
	dbf.pad0 = 0;

	if (ioctl(hnd->fd, RIO_DISABLE_DOORBELL_RANGE, &dbf)) {
		return -errno;
	}
	return 0;
}

/*
 * Enable (register) filter for RapidIO port-write events
 */
int riomp_mgmt_pwrange_enable(riomp_mport_t mport_handle, uint32_t mask,
		uint32_t low, uint32_t high)
{
	struct rio_pw_filter pwf;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	pwf.mask = mask;
	pwf.low = low;
	pwf.high = high;
	pwf.pad0 = 0;

	if (ioctl(hnd->fd, RIO_ENABLE_PORTWRITE_RANGE, &pwf)) {
		return -errno;
	}
	return 0;
}

/*
 * Disable (unregister) filter for RapidIO port-write events
 */
int riomp_mgmt_pwrange_disable(riomp_mport_t mport_handle, uint32_t mask,
		uint32_t low, uint32_t high)
{
	struct rio_pw_filter pwf;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	pwf.mask = mask;
	pwf.low = low;
	pwf.high = high;
	pwf.pad0 = 0;

	if (ioctl(hnd->fd, RIO_DISABLE_PORTWRITE_RANGE, &pwf)) {
		return -errno;
	}
	return 0;
}

/*
 * Set event notification mask
 */
int riomp_mgmt_set_event_mask(riomp_mport_t mport_handle, unsigned int mask)
{
	unsigned int evt_mask = 0;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (mask & RIO_EVENT_DOORBELL) {
		evt_mask |= RIO_DOORBELL;
	}

	if (mask & RIO_EVENT_PORTWRITE) {
		evt_mask |= RIO_PORTWRITE;
	}

	if (ioctl(hnd->fd, RIO_SET_EVENT_MASK, evt_mask)) {
		return -errno;
	}
	return 0;
}

/*
 * Get current value of event mask
 */
int riomp_mgmt_get_event_mask(riomp_mport_t mport_handle, unsigned int *mask)
{
	int evt_mask = 0;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (!mask) {
		return -EINVAL;
	}

	if (ioctl(hnd->fd, RIO_GET_EVENT_MASK, &evt_mask))
	{
		return -errno;
	}

	*mask = 0;
	if (evt_mask & RIO_DOORBELL) {
		*mask |= RIO_EVENT_DOORBELL;
	}
	if (evt_mask & RIO_PORTWRITE) {
		*mask |= RIO_EVENT_PORTWRITE;
	}
	return 0;
}

/*
 * Get current event data
 */
int riomp_mgmt_get_event(riomp_mport_t mport_handle,
		struct riomp_mgmt_event *evt)
{
	struct rio_event revent;
	ssize_t bytes = 0;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (!evt) {
		return -EINVAL;
	}

	bytes = read(hnd->fd, &revent, sizeof(revent));
	if (bytes == -1) {
		return -errno;
	}
	if (bytes != sizeof(revent)) {
		return -EIO;
	}

	if (revent.header == RIO_EVENT_DOORBELL) {
		evt->u.doorbell.payload = revent.u.doorbell.payload;
		evt->u.doorbell.did_val = revent.u.doorbell.rioid;
	} else if (revent.header == RIO_EVENT_PORTWRITE) {
		memcpy(&evt->u.portwrite.payload, &revent.u.portwrite.payload,
				sizeof(evt->u.portwrite.payload));
	} else {
		return -EIO;
	}
	evt->header = revent.header;

	return 0;
}

/*
 * send an event (only doorbell supported)
 */
int riomp_mgmt_send_event(riomp_mport_t mport_handle,
		struct riomp_mgmt_event *evt)
{
	struct rapidio_mport_handle *hnd = mport_handle;
	struct rio_event sevent;
	char *p = (char*)&sevent;
	ssize_t ret;
	unsigned int len = 0;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (!evt) {
		return -EINVAL;
	}

	if (evt->header != RIO_EVENT_DOORBELL) {
		return -EOPNOTSUPP;
	}

	sevent.header = RIO_DOORBELL;
	sevent.u.doorbell.rioid = evt->u.doorbell.did_val;
	sevent.u.doorbell.payload = evt->u.doorbell.payload;

	while (len < sizeof(sevent)) {
		ret = write(hnd->fd, p + len, sizeof(sevent) - len);
		if (ret == -1) {
			return -errno;
		} else {
			len += ret;
		}
	}

	return 0;
}

/*
 * Set destination ID of local mport device
 */
int riomp_mgmt_destid_set(riomp_mport_t mport_handle, did_val_t did_val)
{
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	if (ioctl(hnd->fd, RIO_MPORT_MAINT_HDID_SET, &did_val)) {
		return -errno;
	}
	return 0;
}

/*
 * Create a new kernel device object
 */
int riomp_mgmt_device_add(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, ct_t ct, const char *name)
{
	struct rio_rdev_info dev;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	memset(&dev, 0, sizeof(dev));
	dev.destid = did_val;
	dev.hopcount = hc;
	dev.comptag = ct;
	if (name) {
		SAFE_STRNCPY(dev.name, name, sizeof(dev.name));
	} else {
		memset(dev.name, 0, sizeof(dev.name));
	}

	if (ioctl(hnd->fd, RIO_DEV_ADD, &dev)) {
		return -errno;
	}
	return 0;
}

/*
 * Delete existing kernel device object
 */
int riomp_mgmt_device_del(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, ct_t ct, const char *name)
{
	struct rio_rdev_info dev;
	struct rapidio_mport_handle *hnd = mport_handle;

	if (NULL == hnd) {
		return -EINVAL;
	}

	memset(&dev, 0, sizeof(dev));
	dev.destid = did_val;
	dev.hopcount = hc;
	dev.comptag = ct;
	if (name) {
		SAFE_STRNCPY(dev.name, name, sizeof(dev.name));
	} else {
		memset(dev.name, 0, sizeof(dev.name));
	}

	if (ioctl(hnd->fd, RIO_DEV_DEL, &dev)) {
		return -errno;
	}
	return 0;
}

/* Mailbox functions */
int riomp_sock_mbox_create_handle(uint8_t mport_id,
		uint8_t UNUSED_PARM(mbox_id), riomp_mailbox_t *mailbox)
{
	int fd;
	struct rapidio_mport_mailbox *lhandle = NULL;

	/* Open mport */
	fd = riomp_sock_mbox_init();
	if (fd < 0) {
		return -1;
	}

	/* Create handle */
	lhandle = (struct rapidio_mport_mailbox *)malloc(
			sizeof(struct rapidio_mport_mailbox));
	if (!(lhandle)) {
		close(fd);
		return -2;
	}

	lhandle->fd = fd;
	lhandle->mport_id = mport_id;
	*mailbox = lhandle;
	return 0;
}

int riomp_sock_socket(riomp_mailbox_t mailbox, riomp_sock_t *socket_handle)
{
	struct rapidio_mport_socket *handle = NULL;

	/* Create handle */
	handle = (struct rapidio_mport_socket *)calloc(1,
			sizeof(struct rapidio_mport_socket));
	if (!handle) {
		fprintf(stderr, "error in calloc\n");
		return -1;
	}

	handle->mbox = mailbox;
	handle->ch.id = 0;
	*socket_handle = handle;
	return 0;
}

int riomp_sock_send(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg *skt_msg, uint32_t size,
		volatile int *stop_req)
{
	struct rapidio_mport_socket *handle = socket_handle;
	struct rio_cm_msg msg;
	bool errCheck;
	bool stopCheck;
	int ret;

	do {
		msg.ch_num = handle->ch.id;
		msg.size = size;
		msg.rxto = 0;
		msg.msg = (__u64 )skt_msg;
		errno = 0;

		ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_SEND, &msg);
		errCheck = ((ETIME == errno) || (EINTR == errno)
				|| (EAGAIN == errno) || (EBUSY == errno));
		stopCheck = ((NULL == stop_req)
				|| ((NULL != stop_req) && !*stop_req));
	} while (ret && errCheck && stopCheck);

	if (ret) {
		return -errno;
	}
	return 0;
}

int riomp_sock_receive(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg **skt_msg,
		uint32_t timeout, volatile int *stop_req)
{
	struct rapidio_mport_socket *handle = socket_handle;
	struct rio_cm_msg msg;
	bool errCheck;
	bool stopCheck;
	int ret;

	do {
		msg.ch_num = handle->ch.id;
		msg.size = sizeof(rapidio_mport_socket_msg);
		msg.rxto = timeout;
		msg.msg = (__u64 )*skt_msg;
		errno = 0;

		ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_RECEIVE, &msg);
		errCheck = ((ETIME == errno) || (EINTR == errno)
				|| (EAGAIN == errno));
		stopCheck = ((NULL == stop_req)
				|| ((NULL != stop_req) && (!*stop_req)));
	} while (ret && errCheck && stopCheck);

	if (ret) {
		return -errno;
	}
	return 0;
}

int riomp_sock_release_receive_buffer(riomp_sock_t UNUSED_PARM(socket_handle),
		rapidio_mport_socket_msg *skt_msg) /* always 4k aligned buffers */
{
	free(skt_msg);
	return 0;
}

int riomp_sock_close(riomp_sock_t *socket_handle)
{
	int ret;
	struct rapidio_mport_socket *handle = *socket_handle;
	uint16_t ch_num;

	if (NULL == handle) {
		return -1;
	}

	ch_num = handle->ch.id;
	ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_CLOSE, &ch_num);
	if (ret < 0) {
		fprintf(stderr,
				"CLOSE IOCTL: returned %d for ch_num=%d (errno=%d, %s)\n",
				ret, (*socket_handle)->ch.id, errno,
				strerror(errno));
		ret = errno;
	}

	free(handle);
	*socket_handle = NULL;
	return ret;
}

int riomp_sock_mbox_destroy_handle(riomp_mailbox_t *mailbox)
{
	struct rapidio_mport_mailbox *mbox = *mailbox;

	if (mbox != NULL) {
		close(mbox->fd);
		free(mbox);
		return 0;
	}
	return -1;

}

int riomp_sock_bind(riomp_sock_t socket_handle, uint16_t local_channel)
{
	struct rapidio_mport_socket *handle = socket_handle;
	uint16_t ch_num;
	int ret;
	struct rio_cm_channel cdev;

	ch_num = local_channel;

	ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_CREATE, &ch_num);
	if (ret < 0) {
		return -errno;
	}

	memset(&cdev, 0, sizeof(cdev));
	cdev.id = ch_num;
	cdev.mport_id = handle->mbox->mport_id;

	handle->ch.id = cdev.id;
	handle->ch.mport_id = cdev.mport_id;

	ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_BIND, &cdev);
	if (ret < 0) {
		return -errno;
	}
	return 0;
}

int riomp_sock_listen(riomp_sock_t socket_handle)
{
	struct rapidio_mport_socket *handle = socket_handle;
	uint16_t ch_num;
	int ret;

	ch_num = handle->ch.id;

	ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_LISTEN, &ch_num);
	if (ret) {
		return -errno;
	}
	return 0;
}

int riomp_sock_accept(riomp_sock_t socket_handle, riomp_sock_t *conn,
		uint32_t timeout, volatile int *stop_req)
{
	struct rapidio_mport_socket *handle = socket_handle;
	struct rapidio_mport_socket *new_handle;
	struct rio_cm_accept param;
	bool errCheck;
	bool stopCheck;
	int ret;

	if (NULL == handle || NULL == conn) {
		return -1;
	}

	do {
		param.ch_num = handle->ch.id;
		param.pad0 = 0;
		param.wait_to = timeout;
		errno = 0;

		ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_ACCEPT, &param);
		errCheck = ((ETIME == errno) || (EINTR == errno)
				|| (EAGAIN == errno));
		stopCheck = ((NULL == stop_req)
				|| ((NULL != stop_req) && (!*stop_req)));
	} while (ret && errCheck && stopCheck);

	new_handle = *conn;
	if (new_handle) {
		new_handle->ch.id = param.ch_num;
	}

	return 0;
}

int riomp_sock_connect(riomp_sock_t socket_handle, did_val_t did_val,
		uint16_t channel, volatile int *stop_req)
{
	struct rapidio_mport_socket *handle = socket_handle;
	uint16_t ch_num = 0;
	struct rio_cm_channel cdev;
	bool errCheck;
	bool stopCheck;
	int ret;

	if (handle->ch.id == 0) {
		if (ioctl(handle->mbox->fd, RIO_CM_CHAN_CREATE, &ch_num)) {
			return -errno;
		}
		handle->ch.id = ch_num;
	}

	do {
		/* Configure and Send Connect IOCTL */
		handle->ch.remote_channel = channel;
		handle->ch.mport_id = handle->mbox->mport_id;

		cdev.id = handle->ch.id;
		cdev.remote_channel = channel;
		cdev.remote_destid = did_val;
		cdev.mport_id = handle->mbox->mport_id;
		errno = 0;

		ret = ioctl(handle->mbox->fd, RIO_CM_CHAN_CONNECT, &cdev);
		errCheck = ((ETIME == errno) || (EINTR == errno)
				|| (EAGAIN == errno));
		stopCheck = ((NULL == stop_req)
				|| ((NULL != stop_req) && (!*stop_req)));
	} while (ret && errCheck && stopCheck);

	if (ret) {
		return -errno;
	}
	return 0;
}

int riomp_sock_request_send_buffer(riomp_sock_t UNUSED_PARM(socket_handle),
		rapidio_mport_socket_msg **skt_msg) //always 4k aligned buffers
{
	// socket_handle won't be used for now

	// Always allocate maximum size buffers
	*skt_msg = (rapidio_mport_socket_msg *)calloc(1,
			sizeof(rapidio_mport_socket_msg));
	if (*skt_msg == NULL) {
		return -1;
	}
	return 0;
}

int riomp_sock_release_send_buffer(riomp_sock_t UNUSED_PARM(socket_handle),
		rapidio_mport_socket_msg *skt_msg) /* always 4k aligned buffers */
{
	free(skt_msg);
	return 0;
}

const char *speed_to_string(int speed)
{
	switch (speed) {
	case RIO_LINK_DOWN:
		return "LINK DOWN";
	case RIO_LINK_125:
		return "1.25Gb";
	case RIO_LINK_250:
		return "2.5Gb";
	case RIO_LINK_312:
		return "3.125Gb";
	case RIO_LINK_500:
		return "5.0Gb";
	case RIO_LINK_625:
		return "6.25Gb";
	default:
		return "ERROR";
	}
}

const char *width_to_string(int width)
{
	switch (width) {
	case RIO_LINK_1X:
		return "1x";
	case RIO_LINK_1XR:
		return "1xR";
	case RIO_LINK_2X:
		return "2x";
	case RIO_LINK_4X:
		return "4x";
	case RIO_LINK_8X:
		return "8x";
	case RIO_LINK_16X:
		return "16x";
	default:
		return "ERROR";
	}
}

void riomp_mgmt_display_info(struct riomp_mgmt_mport_properties *attr)
{
	printf("\n+++ SRIO mport configuration +++\n");
	printf("mport: hdid=%d, id=%d, idx=%d, flags=0x%x, sys_size=%s\n",
			attr->did_val, attr->id, attr->index, attr->flags,
			attr->sys_size ? "large" : "small");

	printf("link: speed=%s width=%s\n", speed_to_string(attr->link_speed),
			width_to_string(attr->link_width));

	if (attr->flags & RIO_MPORT_DMA) {
		printf("DMA: max_sge=%d max_size=%d alignment=%d (%s)\n",
				attr->dma_max_sge, attr->dma_max_size,
				attr->dma_align,
				(attr->flags & RIO_MPORT_DMA_SG) ?
						"HW SG" : "no HW SG");
	} else {
		printf("No DMA support\n");
	}
	printf("\n");
}

#ifdef __cplusplus
}
#endif

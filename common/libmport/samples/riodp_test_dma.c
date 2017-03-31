/*
 * Copyright 2014 Integrated Device Technology, Inc.
 *
 * User-space RapidIO DMA transfer test program.
 *
 * This program uses code fragments from Linux kernel dmaengine
 * framework test driver developed by Atmel and Intel. Please, see
 * drivers/dma/dmatest.c file in Linux kernel source code tree for more
 * details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * \file riodp_test_dma.c
 * \brief Test DMA data transfers to/from RapidIO device.
 *
 * This program can be invoked in two modes:
 *
 * 1. DMA transfer initiator (master).
 *
 * 2. Target inbound memory allocator.
 *
 * The program starts in Inbound Target Memory mode when option -i or -I
 * is specified. To avoid DMA transfer error messages target inbound memory
 * must be created first. When started in inbound memory mode, riodp_test_dma
 * program will display RapidIO base address assigned to the inbound window.
 * This address should be used to define target address of DMA data transfers.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h> /* For size_t */
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "rio_misc.h"
#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_mgmt.h"

/// @cond
/*
 * Initialization patterns. All bytes in the source buffer has bit 7
 * set, all bytes in the destination buffer has bit 7 cleared.
 *
 * Bit 6 is set for all bytes which are to be copied by the DMA
 * engine. Bit 5 is set for all bytes which are to be overwritten by
 * the DMA engine.
 *
 * The remaining bits are the inverse of a counter which increments by
 * one for each byte address.
 */
#define PATTERN_SRC		0x80
#define PATTERN_DST		0x00
#define PATTERN_COPY		0x40
#define PATTERN_OVERWRITE	0x20
#define PATTERN_COUNT_MASK	0x1f

struct dma_async_wait_param {
	uint32_t token; /* DMA transaction ID token */
	int err; /* error code returned to caller */
};

#define U8P uint8_t*

/// @endcond

/* Max data block length that can be transferred by DMA channel
 * by default configured for 32 scatter/gather entries of 4K.
 * Size returned by mport driver should be when available.
 * Shall less or eq. TEST_BUF_SIZE.
 */
static uint32_t max_sge = 32;
static uint32_t max_size = 4096;

/// Maximum number of mismatched bytes in buffer to print.
#define MAX_ERROR_COUNT 32
/// Default size of source and destination data buffers
#define TEST_BUF_SIZE (2 * 1024 * 1024)
/// Default size of RapidIO target inbound window 
#define DEFAULT_IBWIN_SIZE (2 * 1024 * 1024)

static riomp_mport_t mport_hnd;
static did_val_t tgt_did_val = RIO_LAST_DEV8;
static uint64_t tgt_addr;
static uint32_t offset = 0;
static uint16_t align = 0;
static uint32_t dma_size = 0x100;
static uint32_t ibwin_size = (uint32_t)NULL;
static int debug = 0;
static uint32_t tbuf_size = TEST_BUF_SIZE;

static void usage(char *program)
{
	printf("%s - test DMA data transfers to/from RapidIO device\n",
			program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("options are:\n");
	printf("Common:\n");
	printf("  --help (or -h)\n");
	printf("  -M mport_id\n");
	printf("  --mport mport_id\n");
	printf("    local mport device index (default 0)\n");
	printf("  -L xxxx\n");
	printf("  --laddr xxxx\n");
	printf("    physical address of reserved local memory to use (default any address)\n");
	printf("  -v turn off buffer data verification\n");
	printf("  --debug (or -d)\n");
	printf("DMA test mode only:\n");
	printf("  -c \n");
	printf("    clear the destination buffer by sending zeros first\n");
	printf("  -D xxxx\n");
	printf("  --destid xxxx\n");
	printf("    destination ID of target RapidIO device (default any)\n");
	printf("  -A xxxx\n");
	printf("  --taddr xxxx\n");
	printf("    memory address in target RapidIO device (default 0)\n");
	printf("  -S xxxx\n");
	printf("  --size xxxx\n");
	printf("    data transfer size in bytes (default 0x100)\n");
	printf("  -B xxxx\n");
	printf("    data buffer size (SRC and DST) in bytes (default %u)\n", TEST_BUF_SIZE);
	printf("  -O xxxx\n");
	printf("  --offset xxxx\n");
	printf("    offset in local data src/dst buffers (default 0)\n");
	printf("  -a n\n");
	printf("  --align n\n");
	printf("    data buffer address alignment (default 0)\n");
	printf("  -T n\n");
	printf("  --repeat n\n");
	printf("    repeat test n times (default 1)\n");
	printf("  -k use coherent kernel space buffer allocation\n");
	printf("  -r use random size and local buffer offset values\n");
	printf("  --faf use FAF DMA transfer mode (default SYNC)\n");
	printf("  --async use ASYNC DMA transfer mode (default SYNC)\n");
	printf("  -n xxxx[,yyyy]\n");
	printf("  --sinterleave xxxx[,yyyy]\n");
	printf("    source interleave setting size and distance");
	printf("  -N xxxx[,yyyy]\n");
	printf("  --dinterleave xxxx[,yyyy]\n");
	printf("    destination interleave setting size and distance");
	printf("Inbound Window mode only:\n");
	printf("  -i\n");
	printf("    allocate and map inbound window (memory) using default parameters\n");
	printf("  -I xxxx\n");
	printf("  --ibwin xxxx\n");
	printf("    inbound window (memory) size in bytes (default %u)\n",
			DEFAULT_IBWIN_SIZE);
	printf("  -R xxxx\n");
	printf("  --ibbase xxxx\n");
	printf("    inbound window base address in RapidIO address space (default any address)\n");
	printf("\n");
}

static struct timespec timediff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

static void dmatest_clear_srcs(uint8_t *buf, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < buf_size; i++) {
		buf[i] = 0;
	}
}

static void dmatest_init_srcs(uint8_t *buf, unsigned int start,
		unsigned int len, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < start; i++) {
		buf[i] = PATTERN_SRC | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < start + len; i++) {
		buf[i] = PATTERN_SRC | PATTERN_COPY | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < buf_size; i++) {
		buf[i] = PATTERN_SRC | (~i & PATTERN_COUNT_MASK);
	}
}

static void dmatest_init_dsts(uint8_t *buf, unsigned int start,
		unsigned int len, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < start; i++) {
		buf[i] = PATTERN_DST | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < start + len; i++) {
		buf[i] = PATTERN_DST | PATTERN_OVERWRITE
				| (~i & PATTERN_COUNT_MASK);
	}
	for (; i < buf_size; i++) {
		buf[i] = PATTERN_DST | (~i & PATTERN_COUNT_MASK);
	}
}

static void dmatest_dump_mem(uint8_t *buf, unsigned int start,
			      unsigned int buf_size)
{
	unsigned int i;
	unsigned int dumpLen;

	dumpLen = buf_size;
	if (dumpLen > 0x200) {
		dumpLen = 0x200;
	}

	for (i = 0; i < dumpLen; i++) {
		if (i % 16 == 0) {
			printf("\n%08x: %02x ", start + i, buf[i]);
		} else {
			printf("%02x ", buf[i]);
		}
	}
	printf("\n");
}

static void dmatest_mismatch(uint8_t actual, uint8_t pattern,
		unsigned int index, unsigned int counter, int is_srcbuf)
{
	uint8_t diff = actual ^ pattern;
	uint8_t expected = pattern | (~counter & PATTERN_COUNT_MASK);

	if (is_srcbuf) {
		printf("srcbuf[0x%x] overwritten! Expected %02x, got %02x\n",
				index, expected, actual);
	} else if ((pattern & PATTERN_COPY)
			&& (diff & (PATTERN_COPY | PATTERN_OVERWRITE))) {
		printf("dstbuf[0x%x] not copied! Expected %02x, got %02x\n",
				index, expected, actual);
	} else if (diff & PATTERN_SRC) {
		printf("dstbuf[0x%x] was copied! Expected %02x, got %02x\n",
				index, expected, actual);
	} else {
		printf("dstbuf[0x%x] mismatch! Expected %02x, got %02x\n",
				index, expected, actual);
	}
}

/**
 * \brief Verify a segment of the given buffer for the expected contents.
 *
 * Loop through buffer from start to end comparing against expected pattern
 * for the type of segment.  Depending on which part of the buffer was written,
 * The original contents should be there or the contents from the DMA.
 *
 * \param[in] buf       Buffer to check
 * \param[in] start     Start offset
 * \param[in] end       End offset
 * \param[in] counter   Cumulative 'offset' of the byte being examined (for error reporting)
 * \param[in] pattern   Expected pattern
 * \param[in] is_srcbuf Flag indicating if buffer is source buffer (for error reporting)
 *
 * \return Count of the number of verification failures found.
 */
static unsigned int dmatest_verify_buf_segment(uint8_t *buf, unsigned int start,
		unsigned int end, unsigned int counter, uint8_t pattern,
		int is_srcbuf)
{
	unsigned int i;
	unsigned int error_count = 0;
	uint8_t actual;
	uint8_t expected;
	unsigned int counter_orig = counter;

	counter = counter_orig;
	for (i = start; i < end; i++) {
		actual = buf[i];
		expected = pattern | (~counter & PATTERN_COUNT_MASK);
		if (actual != expected) {
			if (error_count < MAX_ERROR_COUNT) {
				dmatest_mismatch(actual, pattern, i, counter,
						is_srcbuf);
			}
			error_count++;
		}
		counter++;
	}

	if (error_count > MAX_ERROR_COUNT) {
		printf("%u errors suppressed\n", error_count - MAX_ERROR_COUNT);
	}
	return error_count;
}

/**
 * \brief Verify a segment with interleave.
 *
 * Loop through buffer from start to end comparing against expected pattern
 * for the type of segment.  Depending on which part of the buffer was written,
 * The original contents should be there or the contents from the DMA.
 *
 * \param[in] buf       Buffer to check
 * \param[in] start     Start offset
 * \param[in] end       End offset
 * \param[in] counter   Cumulative 'offset' of the byte being examined (for error reporting)
 * \param[in] pattern   Expected pattern
 * \param[in] is_srcbuf Flag indicating if buffer is source buffer (for error reporting)
 * \param[in] interleave parameters
 *
 * \return Count of the number of verification failures found.
 */
static unsigned int dmatest_verify_interleaved_buf_segment(uint8_t *buf,
		unsigned int start, unsigned int end, unsigned int counter,
		uint8_t pattern, int is_srcbuf,
		struct rapidio_mport_interleave *interleave)
{
	bool debug = false;
	unsigned int dest_count;
	unsigned int src_count;
	unsigned int error_count = 0;
	uint8_t actual;
	uint8_t expected;
	bool in_dst_stride = true;
	bool in_src_stride = true;
	uint16_t in_src_count = interleave->sssize;
	uint16_t in_dst_count = interleave->dssize;

	src_count = counter;
	for (dest_count = start; dest_count < end; dest_count++) {
		actual = buf[dest_count];

		if (in_dst_stride) {
			expected = pattern | (~src_count & PATTERN_COUNT_MASK);
		} else {
			expected = 0;
		}

		if (actual != expected) {
			if (error_count < MAX_ERROR_COUNT) {
				dmatest_mismatch(actual, pattern, dest_count,
						src_count, is_srcbuf);
			}
			error_count++;
		}

		if (debug) {
			printf("%02x %02x %3d %3d %3d %3d %3d %3d\n", actual,
					expected, dest_count, src_count,
					in_dst_stride, in_src_stride,
					in_dst_count, in_src_count);
		}

		if (in_dst_stride == in_src_stride) {
			src_count++;
		} else if (!in_dst_stride && in_src_stride) {
			/* no count */
		} else if (in_dst_stride && !in_src_stride) {
			src_count += in_src_count + 1;
			in_src_count = 0;
		}

		if (--in_src_count <= 0) {
			if (in_src_stride) {
				in_src_count = interleave->ssdist;
				in_src_stride = false;
			} else {
				in_src_count = interleave->sssize;
				in_src_stride = true;
			}
		}

		if (--in_dst_count <= 0) {
			if (in_dst_stride) {
				in_dst_count = interleave->dsdist;
				in_dst_stride = false;
			} else {
				in_dst_count = interleave->dssize;
				in_dst_stride = true;

				if (in_src_stride && in_src_count > 0) {
					src_count -= in_src_count;
					in_src_count = 0;
				}
			}
		}
	}

	if (error_count > MAX_ERROR_COUNT) {
		printf("%u errors suppressed\n", error_count - MAX_ERROR_COUNT);
	}

	return error_count;
}

static unsigned int dmatest_verify_buffer(uint8_t *buf, unsigned int src_off,
		unsigned int src_len, unsigned int buf_len, uint8_t pattern_src,
		uint8_t pattern_dst, uint8_t pattern_copy, int is_srcbuf,
		struct rapidio_mport_interleave *interleave)
{
	unsigned int error_count = 0;
	unsigned int counter = 0;
	bool simpleBuffer;

	/* check bytes before src_off */
	error_count = dmatest_verify_buf_segment(buf, 0, src_off, counter,
			pattern_dst, is_srcbuf);

	counter += src_off;

	if (NULL == interleave) {
		simpleBuffer = true;
	} else {
		simpleBuffer = ((0 == interleave->ssdist)
				&& (0 == interleave->sssize)
				&& (0 == interleave->dsdist)
				&& (0 == interleave->dssize));
	}

	if (simpleBuffer) {
		/* simple buffer verify */
		error_count += dmatest_verify_buf_segment(buf, src_off,
				src_off + src_len, counter,
				pattern_src | pattern_copy, is_srcbuf);
		counter += src_len;
	} else {
		error_count += dmatest_verify_interleaved_buf_segment(buf,
				src_off, src_off + src_len, counter,
				pattern_src | pattern_copy, is_srcbuf,
				interleave);
		counter += src_len;
	}

	/* and check remainder of buffer */
	error_count += dmatest_verify_buf_segment(buf, src_off + src_len,
			buf_len, counter, pattern_dst, is_srcbuf);

	return error_count;
}

/**
 * \brief This function is called by do_dma_test() to allocate source and
 * destination buffers in DMA Master mode.
 * 
 * If parameter handle is a valid pointer (not NULL), this function will allocate
 * a kernel-space contiguous data buffer and return its physical address into
 * variable specified by the handle. The physical address of the buffer will be mapped
 * into process address space and returned to a caller.

 * If handle == NULL, this function allocates a paged user-space data buffer.  
 *
 * \param[in] mport_hnd Handle of mport device to use
 * \param[in] size Buffer size in bytes
 * \param[in,out] handle Physical memory address of the memory segment
 *
 * \return A user-space pointer to the allocated buffer or NULL on failure.
 */
void *dmatest_buf_alloc(riomp_mport_t mport_hnd, uint32_t size,
		uint64_t *handle)
{
	void *buf_ptr = NULL;
	uint64_t h;
	int ret;

	if (handle) {
		h = *handle;

		ret = riomp_dma_dbuf_alloc(mport_hnd, size, &h);
		if (ret) {
			if (debug) {
				printf("riomp_dma_dbuf_alloc failed err=%d\n",
						ret);
			}
			return NULL;
		}

		ret = riomp_dma_map_memory(mport_hnd, size, h, &buf_ptr);
		if (ret) {
			perror("mmap");
			buf_ptr = NULL;
			ret = riomp_dma_dbuf_free(mport_hnd, handle);
			if (ret && debug) {
				printf("riomp_dma_dbuf_free failed err=%d\n",
						ret);
			}
		} else {
			*handle = h;
		}
	} else {
		buf_ptr = malloc(size);
		if (buf_ptr == NULL) {
			perror("malloc");
		}
	}

	return buf_ptr;
}

/**
 * \brief This function is called by do_dma_test() to free source and
 * destination buffers allocated in DMA Master mode.
 * 
 * If parameter handle is a valid pointer (not NULL), this function will unmap
 * and free a kernel-space contiguous data buffer, otherwise - user-space buffer.
 *
 * \param[in] mport_hnd Handle of mport device to use
 * \param[in] buf User-space buffer address
 * \param[in] size Buffer size in bytes
 * \param[in] handle Physical memory address of the memory segment
 *
 * \return None
 */
void dmatest_buf_free(riomp_mport_t mport_hnd, void *buf, uint32_t size,
		uint64_t *handle)
{
	if (handle && *handle) {
		int ret;

		ret = riomp_dma_unmap_memory(size, buf);
		if (ret) {
			perror("munmap");
		}

		ret = riomp_dma_dbuf_free(mport_hnd, handle);
		if (ret) {
			printf("riomp_dma_dbuf_free failed err=%d\n", ret);
		}
	} else if (buf) {
		free(buf);
	}
}

/**
 * \brief This function is called by main() if Inbound Target Memory mode was
 * specified.
 * 
 *
 * \param[in] rio_base Base RapidIO address for inbound window
 * \param[in] ib_size Inbound window and buffer size in bytes
 * \param[in] loc_addr Physical address in reserved memory range
 * \param[in] verify Flag to enable/disable data verification on exit
 *
 * \return 0 if successfull or error code returned by mport API.
 *
 * Performs the following steps:
 *
 */
int do_ibwin_test(uint64_t rio_base, uint32_t ib_size, uint64_t loc_addr,
		int verify)
{
	int ret;
	uint64_t ib_handle = loc_addr;
	void *ibmap;

	/** - Request mport's inbound window mapping */
	ret = riomp_dma_ibwin_map(mport_hnd, &rio_base, ib_size, &ib_handle);
	if (ret) {
		printf("Failed to allocate/map IB buffer err=%d\n", ret);
		return ret;
	}

	/** - Map associated kernel buffer into process address space */
	ret = riomp_dma_map_memory(mport_hnd, ib_size, ib_handle, &ibmap);
	if (ret) {
		perror("mmap");
		goto out;
	}

	memset(ibmap, 0, ib_size);

	printf(
			"\tSuccessfully allocated/mapped IB buffer (rio_base=0x%x_%08x)\n",
			(uint32_t)(rio_base >> 32),
			(uint32_t)(rio_base & 0xffffffff));

	if (debug) {
		printf("\t(h=0x%x_%x, loc=%p)\n", (uint32_t)(ib_handle >> 32),
				(uint32_t)(ib_handle & 0xffffffff), ibmap);
	}

	/** - Pause until a user presses Enter key */
	printf("\t.... press Enter key to exit ....\n");
	getchar();

	/** - Verify data before exit (if requested) */
	if (verify) {
		dmatest_verify_buf_segment((U8P)ibmap, 0, ib_size, 0,
				PATTERN_SRC | PATTERN_COPY, 0);
	}

	/** - Unmap kernel-space data buffer */
	ret = riomp_dma_unmap_memory(ib_size, ibmap);
	if (ret) {
		perror("munmap");
	}

out:
	/** - Release mport's inbound mapping window */
	ret = riomp_dma_ibwin_free(mport_hnd, &ib_handle);
	if (ret) {
		printf("Failed to release IB buffer err=%d\n", ret);
	}
	return 0;
}

static void *dma_async_wait(void *arg)
{
	struct dma_async_wait_param *param = (struct dma_async_wait_param *)arg;
	int ret;

	ret = riomp_dma_wait_async(mport_hnd, param->token, 3000);
	param->err = ret;
	return &param->err;
}

/**
 * \brief This function performs DMA write and read back to/from remote RapidIO
 * target device.
 *
 * Called by main() if DMA Master mode was specified.
 *
 * \param[in] random If non-zero, enables using random transfer size and offsets
 * within source and destination buffers.
 * \param[in] kbuf_mode If non-zero, use kernel-space contiguous buffers
 * \param[in] verify Flag to enable/disable data verification for each write-read cycle
 * \param[in] loop_count Number of write-read cycles to perform
 * \param[in] sync DMA transfer synchronization mode
 * \param[in] loc_addr Physical address in reserved memory range
 * \param[in] clear_buf Clear destination buffers when true
 * \param[in] interleave parameters
 *
 * \return 0 if successfull or error code returned by mport API.
 *
 * Performs the following steps:
 */
int do_dma_test(int random, int kbuf_mode, int verify, int loop_count,
		enum riomp_dma_directio_transfer_sync sync, uint64_t loc_addr,
		bool clear_buf,
		struct rapidio_mport_interleave *interleave)
{
	void *buf_src = NULL;
	void *buf_dst = NULL;
	unsigned int src_off, dst_off, len, src_len;
	uint64_t src_handle = RIOMP_MAP_ANY_ADDR;
	uint64_t dst_handle = RIOMP_MAP_ANY_ADDR;
	int i, ret = 0;
	uint32_t max_hw_size; /* max DMA transfer size supported by HW */
	enum riomp_dma_directio_transfer_sync rd_sync;
	struct timespec wr_starttime, wr_endtime;
	struct timespec rd_starttime, rd_endtime;
	float totaltime;

	if (kbuf_mode) {
		max_hw_size = max_size;
	} else {
		max_hw_size = (max_sge == 0) ?
				tbuf_size : (max_sge * getpagesize());
	}

	/* check specified DMA block size */
	if (dma_size > 0 && (dma_size > tbuf_size || dma_size > max_hw_size)) {
		printf("ERR: invalid buffer size parameter\n");
		printf("     max allowed buffer size: %d bytes\n",
				(max_hw_size > tbuf_size) ?
						tbuf_size : max_hw_size);
		return -1;
	}

	if (random) {
		/* If not specified, set max supported dma_size */
		if (dma_size == 0) {
			dma_size = (max_hw_size < tbuf_size) ?
					max_hw_size : tbuf_size;
		}
		printf(
				"\tRANDOM mode is selected for size/offset combination\n");
		printf("\t\tmax data transfer size: %d bytes\n", dma_size);
		srand(time(NULL));
	} else if (dma_size + offset > tbuf_size || dma_size == 0) {
		printf("ERR: invalid transfer size/offset combination\n");
		return -1;
	} else {
		printf("\tdma_size=%d offset=0x%x\n", dma_size, offset);
	}

	/** * Allocate source and destination buffers in specified space (kernel or user) */

	if (kbuf_mode && loc_addr != RIOMP_MAP_ANY_ADDR) {
		src_handle = loc_addr;
		dst_handle = loc_addr + tbuf_size;
	}

	buf_src = dmatest_buf_alloc(mport_hnd, tbuf_size,
			kbuf_mode ? &src_handle : NULL);
	if (buf_src == NULL) {
		printf("DMA Test: error allocating SRC buffer\n");
		ret = -1;
		goto out_src;
	}

	buf_dst = dmatest_buf_alloc(mport_hnd, tbuf_size,
			kbuf_mode ? &dst_handle : NULL);
	if (buf_dst == NULL) {
		printf("DMA Test: error allocating DST buffer\n");
		ret = -1;
		goto out_dst;
	}

	for (i = 1; i <= loop_count; i++) { /// * Enter write-read cycle
		struct timespec time, rd_time;
		pthread_t wr_thr, rd_thr;
		struct dma_async_wait_param wr_wait, rd_wait;

		if (random) {
			len = rand() % dma_size + 1;
			len = (len >> align) << align;
			if (!len) {
				len = 1 << align;
			}
			src_off = rand() % (tbuf_size - len + 1);
			dst_off = rand() % (tbuf_size - len + 1);

			src_off = (src_off >> align) << align;
			dst_off = (dst_off >> align) << align;
		} else {
			len = dma_size;
			src_off = offset;
			dst_off = offset;
		}

		/* source length may be larger if source interleave is configured */
		if ((0 == interleave->ssdist) && (0 == interleave->sssize)) {
			src_len = len;
		} else {
			src_len = len * (interleave->ssdist + interleave->sssize)
					/ interleave->ssdist;
		}

		printf("<%d>: len=0x%x src_off=0x%x dst_off=0x%x\n", i, len,
				src_off, dst_off);

		if (clear_buf) {
			if (debug) {
				printf("Clearing destination buffer\n");
			}

			dmatest_clear_srcs((U8P)buf_src, tbuf_size);
			if (kbuf_mode) {
				ret = riomp_dma_write_d(mport_hnd, tgt_did_val,
						tgt_addr, src_handle, src_off,
						len, RIO_DIRECTIO_TYPE_NWRITE_R,
						sync, NULL);
			} else {
				ret = riomp_dma_write(mport_hnd, tgt_did_val,
						tgt_addr,
						(U8P)buf_src + src_off, len,
						RIO_DIRECTIO_TYPE_NWRITE_R,
						sync, NULL);
			}

			if (ret < 0) {
				printf("DMA Test: Clearing destination buffer failed err=%d\n",
						ret);
				goto out;
			}
		}

		/** - If data verification is requested, fill src and dst buffers
		 * with predefined data */
		if (verify) {
			dmatest_init_srcs((U8P)buf_src, src_off, src_len,
					tbuf_size);
			dmatest_init_dsts((U8P)buf_dst, dst_off, len,
					tbuf_size);
			if (debug) {
				printf("Buffers before xfer:\n");
				dmatest_dump_mem((U8P)buf_src, src_off,
						tbuf_size);
				dmatest_dump_mem((U8P)buf_dst, dst_off,
						tbuf_size);
			}
		}

		if (debug) {
			printf("\tWrite %d bytes from src offset 0x%x\n", len,
					src_off);
		}
		clock_gettime(CLOCK_MONOTONIC, &wr_starttime);

		/** - Write data from local source buffer to remote target inbound buffer */
		if (kbuf_mode) {
			ret = riomp_dma_write_d(mport_hnd, tgt_did_val,
					tgt_addr, src_handle, src_off, len,
					RIO_DIRECTIO_TYPE_NWRITE_R, sync,
					interleave);
		} else {
			ret = riomp_dma_write(mport_hnd, tgt_did_val, tgt_addr,
					(U8P)buf_src + src_off, len,
					RIO_DIRECTIO_TYPE_NWRITE_R, sync,
					interleave);
		}

		/** - If in ASYNC DMA transfer mode, create waiting thread for write */
		if ((sync == RIO_DIRECTIO_TRANSFER_ASYNC) && (ret >= 0)) {
			wr_wait.token = ret;
			wr_wait.err = -1;
			ret = pthread_create(&wr_thr, NULL, dma_async_wait, (void *)&wr_wait);
			if (ret) {
				printf("\tERR: Failed to create WR wait thread. err=%d\n", ret);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &wr_endtime);
		if (ret) {
			printf("DMA Test: write DMA transfer failed err=%d\n",
					ret);
			goto out;
		}

		if (debug) {
			printf("\tRead %d bytes to dst offset 0x%x\n", len,
					dst_off);
		}

		/* RIO_TRANSFER_FAF is not available for read operations. Force SYNC instead. */
		rd_sync = (sync == RIO_DIRECTIO_TRANSFER_FAF) ?
				RIO_DIRECTIO_TRANSFER_SYNC : sync;

		clock_gettime(CLOCK_MONOTONIC, &rd_starttime);

		/** - Read back data from remote target inbound buffer into local destination buffer */
		if (kbuf_mode) {
			ret = riomp_dma_read_d(mport_hnd, tgt_did_val, tgt_addr,
					dst_handle, dst_off, len, rd_sync,
					NULL);
		} else {
			ret = riomp_dma_read(mport_hnd, tgt_did_val, tgt_addr,
					(U8P)buf_dst + dst_off, len, rd_sync,
					NULL);
		}

		/** - If in ASYNC DMA transfer mode, create waiting thread for reag */
		if ((sync == RIO_DIRECTIO_TRANSFER_ASYNC) && (ret >= 0)) {
			rd_wait.token = ret;
			rd_wait.err = -1;
			ret = pthread_create(&rd_thr, NULL, dma_async_wait, (void *)&rd_wait);
			if (ret) {
				printf("\tERR: Failed to create RD wait thread. err=%d\n", ret);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &rd_endtime);
		if (ret) {
			printf("DMA Test: read DMA transfer failed err=%d\n",
					ret);
			goto out;
		}

		rd_time = timediff(rd_starttime, rd_endtime);

		/** - If in ASYNC DMA transfer mode, wait for notification threads completion */
		if (sync == RIO_DIRECTIO_TRANSFER_ASYNC) {
			pthread_join(wr_thr, NULL);
			pthread_join(rd_thr, NULL);
			clock_gettime(CLOCK_MONOTONIC, &rd_endtime);

			if (wr_wait.err) {
				printf("Wait for DMA_WR: err=%d\n",
						wr_wait.err);
			}
			if (rd_wait.err) {
				printf("Wait for DMA_RD: err=%d\n",
						rd_wait.err);
			}
			if (wr_wait.err || rd_wait.err) {
				goto out;
			}
		}

		/** - If data verification is requested, verify data transfer results */
		if (verify) {
			unsigned int error_count;

			if (debug) {
				printf("Buffer contents after transfer:\n");
				dmatest_dump_mem((U8P)buf_src, src_off,
						tbuf_size);
				dmatest_dump_mem((U8P)buf_dst, dst_off,
						tbuf_size);
			}

			if (debug) {
				printf("\tVerifying source buffer...\n");
			}
			error_count = dmatest_verify_buffer((U8P)buf_src,
					src_off, src_len, tbuf_size,
					PATTERN_SRC, PATTERN_SRC, PATTERN_COPY,
					1, NULL);
			if (error_count) {
				printf(
						"\tSource buffer verification failed with %d errors\n",
						error_count);
				break;
			} else {
				if (debug) {
					printf("\ttSource buffer verification OK!\n");
				}
			}

			if (debug) {
				printf("\tVerifying destination buffer...\n");
			}
			error_count = dmatest_verify_buffer((U8P)buf_dst,
					dst_off, len, tbuf_size, PATTERN_SRC,
					PATTERN_DST, PATTERN_COPY, 0,
					interleave);

			if (error_count) {
				printf(
						"\tDestination buffer verification failed with %d errors\n",
						error_count);
				break;
			} else {
				if (debug) {
					printf("\ttDestination buffer verification OK!\n");
				}
			}
		} else {
			printf("\tBuffer verification is turned off!\n");
		}

		time = timediff(wr_starttime, wr_endtime);
		totaltime =
				((double)time.tv_sec
						+ (time.tv_nsec / 1000000000.0));
		if (sync != RIO_DIRECTIO_TRANSFER_SYNC) {
			printf("\t\tWR time: %4f s\n", totaltime);
		} else {
			printf("\t\tWR time: %4f s @ %4.2f MB/s\n", totaltime,
					(len / totaltime) / (1024 * 1024));
		}

		totaltime = ((double)rd_time.tv_sec
				+ (rd_time.tv_nsec / 1000000000.0));

		if (sync != RIO_DIRECTIO_TRANSFER_SYNC) {
			printf("\t\tRD time: %4f s\n", totaltime);
		} else {
			printf("\t\tRD time: %4f s @ %4.2f MB/s\n", totaltime,
					(len / totaltime) / (1024 * 1024));
		}

		time = timediff(wr_starttime, rd_endtime);
		totaltime =
				((double)time.tv_sec
						+ (time.tv_nsec / 1000000000.0));
		printf("\t\tFull Cycle time: %4f s\n", totaltime);
	} /// - Repeat if loop_count > 1

out:
	/** * Free source and destination buffers */
	dmatest_buf_free(mport_hnd, buf_dst, tbuf_size,
			kbuf_mode ? &dst_handle : NULL);

out_dst:
	dmatest_buf_free(mport_hnd, buf_src, tbuf_size,
			kbuf_mode ? &src_handle : NULL);

out_src:
	return ret;

}


/**
 * \brief Parse distance and optional size parameters from arg
 *
 * \param[in] arg  Command line parameter containing distance and optional size parameter
 *                 Parameters should look like dddd[,ssss].
 * \param[out] dist integer distance as parsed from arg
 * \param[out] size integer size as parsed from arg
 *
 * \retval EXIT_SUCCESS if all parsed successfully, EXIT_FAILURE otherwise
 */
int local_parse_dist_size(char *arg, uint16_t *dist, uint16_t *size)
{
	char *saveptr;
	char *distTok;
	char *sizeTok;

	sizeTok = strtok_r(arg, ",", &saveptr);
	distTok = strtok_r(NULL, ",", &saveptr);
	if (distTok == NULL) {
		distTok = sizeTok;
	}

	if (tok_parse_us(distTok, dist, 0)) {
		printf(TOK_ERR_US_HEX_MSG_FMT, "source distance");
		return (EXIT_FAILURE);
	}

	if (tok_parse_us(sizeTok, size, 0)) {
		printf(TOK_ERR_US_HEX_MSG_FMT, "source size");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * \brief Starting point for the test program
 *
 * \param[in] argc Command line parameter count
 * \param[in] argv Array of pointers to command line parameter null terminated
 *                 strings
 *
 * \retval 0 means success
 */
int main(int argc, char** argv)
{
	int c;
	char *program = argv[0];

	uint32_t mport_id = 0;
	int do_rand = 0;
	int kbuf_mode = 0; /* 0 - user space buffer, 1 = kernel contiguous buffer */
	int verify = 1;
	uint32_t repeat = 1;
	uint64_t rio_base = RIOMP_MAP_ANY_ADDR;
	uint64_t loc_addr = RIOMP_MAP_ANY_ADDR;
	enum riomp_dma_directio_transfer_sync sync = RIO_DIRECTIO_TRANSFER_SYNC;
	static const struct option options[] = {
			{ "destid", required_argument, NULL, 'D' },
			{ "taddr",  required_argument, NULL, 'A' },
			{ "size",   required_argument, NULL, 'S' },
			{ "offset", required_argument, NULL, 'O' },
			{ "align",  required_argument, NULL, 'a' },
			{ "repeat", required_argument, NULL, 'T' },
			{ "ibwin",  required_argument, NULL, 'I' },
			{ "ibbase", required_argument, NULL, 'R' },
			{ "mport",  required_argument, NULL, 'M' },
			{ "laddr",  required_argument, NULL, 'L' },
			{ "faf",    no_argument, NULL, 'F' },
			{ "async",  no_argument, NULL, 'Y' },
			{ "debug",  no_argument, NULL, 'd' },
			{ "help",   no_argument, NULL, 'h' },
			{ "sinterleave", required_argument, NULL, 'n'},
			{ "dinterleave", required_argument, NULL, 'N'},
	};

	struct riomp_mgmt_mport_properties prop;
	int has_dma = 1;
	int rc = EXIT_SUCCESS;
	int ret;
	bool sync_set = false;
	bool inbound_mode = false;
	bool clear_buf = false;
	struct rapidio_mport_interleave interleave;

	interleave.ssdist = 0;
	interleave.sssize = 0;
	interleave.dsdist = 0;
	interleave.dssize = 0;

	/** Parse command line options, if any */
	while (-1 != (c = getopt_long_only(argc, argv,
			"rvcdhikaFY:A:D:I:O:M:R:S:T:B:L:n:N:", options, NULL))) {
		switch (c) {
		case 'A':
			if (tok_parse_ull(optarg, &tgt_addr, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Target address");
				return (EXIT_FAILURE);
			}
			break;
		case 'L':
			if (tok_parse_ull(optarg, &loc_addr, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Local address");
				return (EXIT_FAILURE);
			}
			break;
		case 'a':
			if (tok_parse_us(optarg, &align, 0)) {
				printf(TOK_ERR_US_HEX_MSG_FMT,
						"Data alignment");
				return (EXIT_FAILURE);
			}
			break;
		case 'c':
			clear_buf = true;
			break;
		case 'D':
			if (tok_parse_did(optarg, &tgt_did_val, 0)) {
				printf(TOK_ERR_DID_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'O':
			if (tok_parse_ul(optarg, &offset, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "Offset");
				return (EXIT_FAILURE);
			}
			break;
		case 'S':
			if (tok_parse_ul(optarg, &dma_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Data transfer size");
				return (EXIT_FAILURE);
			}
			break;
		case 'B':
			if (tok_parse_ul(optarg, &tbuf_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Data buffer size");
				return (EXIT_FAILURE);
			}
			break;
		case 'T':
			if (tok_parse_ul(optarg, &repeat, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Number of repetitions");
				return (EXIT_FAILURE);
			}
			break;
		case 'k':
			kbuf_mode = 1;
			break;
		case 'r':
			do_rand = 1;
			break;
		case 'F':
			if (sync_set) {
				printf(
						"Only one of \'faf\' or \'sync\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			sync_set = true;
			sync = RIO_DIRECTIO_TRANSFER_FAF;
			break;
		case 'Y':
			if (sync_set) {
				printf(
						"Only one of \'faf\' or \'sync\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			sync_set = true;
			sync = RIO_DIRECTIO_TRANSFER_ASYNC;
			break;
			/* Inbound Memory (window) Mode options */
		case 'I':
			if (inbound_mode) {
				printf(
						"Only one of \'-\' or \'ibwin\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			inbound_mode = true;
			if (tok_parse_ul(optarg, &ibwin_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Inbound window memory size");
				return (EXIT_FAILURE);
			}
			break;
		case 'i':
			if (inbound_mode) {
				printf(
						"Only one of \'-\' or \'ibwin\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			inbound_mode = true;
			ibwin_size = DEFAULT_IBWIN_SIZE;
			break;
		case 'R':
			if (tok_parse_ull(optarg, &rio_base, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Inbound window base address");
				return (EXIT_FAILURE);
			}
			break;
			/* Options common for all modes */
		case 'M':
			if (tok_parse_mport_id(optarg, &mport_id, 0)) {
				printf(TOK_ERR_MPORT_MSG_FMT);
				return (EXIT_FAILURE);
			}
			break;
		case 'v':
			verify = 0;
			break;
		case 'd':
			debug = 1;
			break;
		case 'n':
			// source interleave parameter is <ssize>,<sdist>.
			// We will also accept a single digit in which case
			// both size and distance will be the same.
			if (EXIT_SUCCESS
					!= local_parse_dist_size(optarg,
							&interleave.sssize,
							&interleave.ssdist)) {
				return (EXIT_FAILURE);
			}
			break;
		case 'N':
			// destination interleave parameter is <dsize>,<ddist>.
			// We will also accept a single digit in which case
			// both size anddistance will be the same.
			if (EXIT_SUCCESS
					!= local_parse_dist_size(optarg,
							&interleave.dssize,
							&interleave.dsdist)) {
				return (EXIT_FAILURE);
			}
			break;
		case 'h':
			usage(program);
			exit(EXIT_SUCCESS);
			break;
//		case '?':
		default:
			/* Invalid command line option */
			if (isprint(optopt)) {
				printf("Unknown option '-%c\n", optopt);
			}
			usage(program);
			exit(EXIT_FAILURE);
		}
	}

	ret = riomp_mgmt_mport_create_handle(mport_id, 0, &mport_hnd);
	if (ret < 0) {
		printf("DMA Test: unable to open mport%d device err=%d\n",
				mport_id, ret);
		exit(EXIT_FAILURE);
	}

	if (!riomp_mgmt_query(mport_hnd, &prop)) {
		riomp_mgmt_display_info(&prop);

		if (prop.flags & RIO_MPORT_DMA) {
			align = prop.dma_align;
			max_sge = prop.dma_max_sge;
			max_size = prop.dma_max_size;
		} else {
			has_dma = 0;
		}

		if (prop.link_speed == 0) {
			printf("SRIO link is down. Test aborted.\n");
			rc = EXIT_FAILURE;
			goto out;
		}
	} else {
		printf("Failed to obtain mport information\n");
		printf("Using default configuration\n\n");
	}

	if (inbound_mode) {
		printf("+++ RapidIO Inbound Window Mode +++\n");
		printf("\tmport%d ib_size=0x%x PID:%d\n", mport_id, ibwin_size,
				(int)getpid());
		if (loc_addr != RIOMP_MAP_ANY_ADDR) {
			printf("\tloc_addr=0x%llx\n",
					(unsigned long long)loc_addr);
		}

		do_ibwin_test(rio_base, ibwin_size, loc_addr, verify);
	} else if (has_dma) {
		printf("+++ RapidIO DMA Test +++\n");
		printf(
				"\tmport%d destID=%d rio_addr=0x%llx align=%d repeat=%d PID:%d\n",
				mport_id, tgt_did_val,
				(unsigned long long)tgt_addr, align, repeat,
				(int)getpid());

		if (RIO_DIRECTIO_TRANSFER_SYNC == sync) {
			printf("\tsync=%d (%s)\n", sync, "SYNC");
		} else {

			printf("\tsync=%d (%s)\n", sync,
					(RIO_DIRECTIO_TRANSFER_FAF == sync) ?
							"FAF" : "ASYNC");
		}

		printf("\tsrc dist: %d size: %d  dest dist: %d size: %d\n",
				interleave.ssdist, interleave.sssize,
				interleave.dsdist, interleave.dssize);
		if (RIOMP_MAP_ANY_ADDR != loc_addr) {
			printf("\tloc_addr=0x%llx\n",
					(unsigned long long)loc_addr);
		}

		if (do_dma_test(do_rand, kbuf_mode, verify, repeat, sync,
				loc_addr, clear_buf, &interleave)) {
			printf("DMA test FAILED\n\n");
			rc = EXIT_FAILURE;
		}
	} else {
		printf("No DMA support. Test aborted.\n\n");
		rc = EXIT_FAILURE;
	}

out:
	riomp_mgmt_mport_destroy_handle(&mport_hnd);
	exit(rc);
}

#ifdef __cplusplus
}
#endif

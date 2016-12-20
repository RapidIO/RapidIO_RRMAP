#ifndef _RAPIDIO_MPORT_DMA_H_
#define _RAPIDIO_MPORT_DMA_H_
/*
 * TODO: Check copyright, should be rapidio.org?
 * Copyright 2014, 2015 Integrated Device Technology, Inc.
 *
 * Header file for RapidIO mport device library.
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

#ifdef __cplusplus
extern "C" {
#endif

#include "ct.h"
#include "rapidio_mport_mgmt.h"

/** @brief DirectIO transfer type */
enum riomp_dma_directio_type {
	//RIO_DIRECTIO_TYPE_DEFAULT,	/* Default method */
	RIO_DIRECTIO_TYPE_NWRITE,		/**< All packets using NWRITE */
	RIO_DIRECTIO_TYPE_SWRITE,		/**< All packets using SWRITE */
	RIO_DIRECTIO_TYPE_NWRITE_R,		/**< Last packet NWRITE_R, others NWRITE */
	RIO_DIRECTIO_TYPE_SWRITE_R,		/**< Last packet NWRITE_R, others SWRITE */
	RIO_DIRECTIO_TYPE_NWRITE_R_ALL,	/**< All packets using NWRITE_R */
};

/** @brief DirectIO transfer synchronization method */
enum riomp_dma_directio_transfer_sync {
	RIO_DIRECTIO_TRANSFER_SYNC,		/**< synchronous transfer */
	RIO_DIRECTIO_TRANSFER_ASYNC,	/**< asynchronous transfer */
	RIO_DIRECTIO_TRANSFER_FAF,		/**< fire-and-forget transfer only for write transactions */
};

/**
 * @brief Perform DMA data write to target transfer using user space source buffer
 *
 * @param[in] mport_handle port handle
 * @param[in] destid destination device ID
 * @param[in] tgt_addr target memory address
 * @param[in] buf pointer to userspace source buffer
 * @param[in] size number of bytes to transfer
 * @param[in] wr_mode DirectIO write mode
 * @param[in] sync transfer synchronization flag
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_write(riomp_mport_t mport_handle, uint16_t destid, uint64_t tgt_addr, void *buf, uint32_t size, enum riomp_dma_directio_type wr_mode, enum riomp_dma_directio_transfer_sync sync);

/**
 * @brief Perform DMA data write to target transfer using kernel space source buffer
 *
 * @param[in] mport_handle port handle
 * @param[in] destid destination device ID
 * @param[in] tgt_addr target memory address
 * @param[in] handle kernelspace buffer handle
 * @param[in] offset kernelspace buffer offset
 * @param[in] size number of bytes to transfer
 * @param[in] wr_mode DirectIO write mode
 * @param[in] sync transfer synchronization flag
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_write_d(riomp_mport_t mport_handle, uint16_t destid, uint64_t tgt_addr, uint64_t handle, uint32_t offset, uint32_t size, enum riomp_dma_directio_type wr_mode, enum riomp_dma_directio_transfer_sync sync);

/**
 * @brief Perform DMA data read from target transfer using user space destination buffer
 *
 * @param[in] mport_handle port handle
 * @param[in] destid destination device ID
 * @param[in] tgt_addr target memory address
 * @param[in] buf pointer to userspace source buffer
 * @param[in] size number of bytes to transfer
 * @param[in] sync transfer synchronization flag
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_read(riomp_mport_t mport_handle, uint16_t destid, uint64_t tgt_addr, void *buf, uint32_t size, enum riomp_dma_directio_transfer_sync sync);

/**
 * @brief Perform DMA data read from target transfer using kernel space destination buffer
 *
 * @param[in] mport_handle port handle
 * @param[in] destid destination device ID
 * @param[in] tgt_addr target memory address
 * @param[in] handle kernelspace buffer handle
 * @param[in] offset kernelspace buffer offset
 * @param[in] size number of bytes to transfer
 * @param[in] sync transfer synchronization flag
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_read_d(riomp_mport_t mport_handle, uint16_t destid, uint64_t tgt_addr, uint64_t handle, uint32_t offset, uint32_t size, enum riomp_dma_directio_transfer_sync sync);

/**
 * @brief Wait for DMA transfer completion
 *
 * @param[in] mport_handle port handle
 * @param[in] cookie WHAT IS THIS EXACTLY?
 * @param[in] tmo timeout in in msec, if 0 use driver's default timeout value
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_wait_async(riomp_mport_t mport_handle, uint32_t cookie, uint32_t tmo);

/**
 * @brief Allocate and map into RapidIO space a local kernel space data buffer
 *
 * This is used for inbound RapidIO data read/write requests. It opens a DirectIO
 * inbound window.
 *
 * @param[in] mport_handle port handle
 * @param[out] rio_base RapidIO address
 * @param[in] size number of bytes
 * @param[out] handle map handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
#define RIO_ANY_ADDR        (uint64_t)(~((uint64_t) 0))

int riomp_dma_ibwin_map(riomp_mport_t mport_handle, uint64_t *rio_base, uint32_t size, uint64_t *handle);

/**
 * @brief Free and unmap from RapidIO space a local kernel space data buffer
 *
 * This closes a DirectIO inbound window.
 *
 * @param[in] mport_handle port handle
 * @param[in] handle map handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_ibwin_free(riomp_mport_t mport_handle, uint64_t *handle);

/**
 * @brief Allocate and map into RapidIO space a local kernel space data buffer
 *
 * This is used for outbound RapidIO data read/write requests. It opens a DirectIO
 * outbound window.
 *
 * @param[in] mport_handle port handle
 * @param[in] destid destination ID of the target device
 * @param[in] rio_base RapidIO address
 * @param[in] size number of bytes
 * @param[out] handle map handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_obwin_map(riomp_mport_t mport_handle, uint16_t destid, uint64_t rio_base, uint32_t size, uint64_t *handle);

/**
 * @brief Free and unmap from RapidIO space a local kernel space data buffer
 *
 * This closes a DirectIO outbound window.
 *
 * @param[in] mport_handle port handle
 * @param[in] handle map handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_obwin_free(riomp_mport_t mport_handle, uint64_t *handle);

/**
 * @brief Allocate a local kernel space data buffer for DMA data transfers
 *
 * @param[in] mport_handle port handle
 * @param[in] size number of bytes
 * @param[out] handle buffer handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_dbuf_alloc(riomp_mport_t mport_handle, uint32_t size, uint64_t *handle);

/**
 * @brief Free previously allocated local kernel space data buffer
 *
 * @param[in] mport_handle port handle
 * @param[in] handle buffer handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_dbuf_free(riomp_mport_t mport_handle, uint64_t *handle);

/**
 * @brief map a kernel buffer to the userspace
 *
 * @param[in] mport_handle port handle
 * @param[in] size number of bytes
 * @param[in] paddr kernel buffer address
 * @param[out] vaddr userspace memory pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_map_memory(riomp_mport_t mport_handle, size_t size, off_t paddr, void **vaddr);

/**
 * @brief unmap a kernel buffer to the userspace map
 *
 * @param[in] size number of bytes
 * @param[in] vaddr userspace memory pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_dma_unmap_memory(size_t size, void *vaddr);

#ifdef __cplusplus
}
#endif
#endif /* _RAPIDIO_MPORT_DMA_H_ */

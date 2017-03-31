/*
 * Copyright 2017 Integrated Device Technology, Inc.
 * Copyright 2017 RapidIO.org
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

#ifndef __RAPIDIO_MPORT_MGMT_H__
#define __RAPIDIO_MPORT_MGMT_H__

#include <stdint.h>
#include "rio_route.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RapidIO base address for inbound requests
 *
 * Set to value defined below indicates that no specific
 * RIO-to-local address translation is requested
 * and driver should use direct (one-to-one) address mapping.
 */
#define RIOMP_MAP_ANY_ADDR (uint64_t)(~((uint64_t) 0))

/** @brief bitmask for doorbell events */
#define RIO_EVENT_DOORBELL	(1 << 0)

/** @brief bitmask for port write events */
#define RIO_EVENT_PORTWRITE	(1 << 1)

/** @brief RapidIO link speed */
enum riomp_mgmt_link_speed {
	RIO_LINK_DOWN = 0, /**< SRIO Link not initialized */
	RIO_LINK_125 = 1, /**< 1.25 GBaud  */
	RIO_LINK_250 = 2, /**< 2.5 GBaud   */
	RIO_LINK_312 = 3, /**< 3.125 GBaud */
	RIO_LINK_500 = 4, /**< 5.0 GBaud   */
	RIO_LINK_625 = 5 /**< 6.25 GBaud  */
};

/** @brief RapidIO link width */
enum riomp_mgmt_link_width {
	RIO_LINK_1X = 0, /**< 1 lane */
	RIO_LINK_1XR = 1, /**< 1 lane redundancy mode */
	RIO_LINK_2X = 3, /**< 2 lanes */
	RIO_LINK_4X = 2, /**< 4 lanes */
	RIO_LINK_8X = 4, /**< 8 lanes */
	RIO_LINK_16X = 5 /**< 16 lanes */
};

/** @brief mport handle flags */
enum riomp_mgmt_mport_flags {
	RIO_MPORT_DMA = (1 << 0), /**< supports DMA data transfers */
	RIO_MPORT_DMA_SG = (1 << 1), /**< DMA supports HW SG mode */
	RIO_MPORT_IBSG = (1 << 2), /**< inbound mapping supports SG */
};

/** @brief RapidIO mport properties */
struct riomp_mgmt_mport_properties {
	did_val_t did_val; /**< mport host device ID */
	uint8_t id; /**< Physical port ID number */
	uint8_t index; /**< Mport driver index numer */
	uint32_t flags; /**< Refer to driver for rio_mport_flags definitions. */
	uint32_t sys_size; /**< Default addressing size */
	uint8_t port_ok; /**< link status */
	uint8_t link_speed; /**< link speed */
	uint8_t link_width; /**< link width */
	uint32_t dma_max_sge; /**< Maximum DMA Scatter/Gather (SG) Entries */ 
	uint32_t dma_max_size; /**< Maximum bytes transferred per SG Entry */
	uint32_t dma_align; /**< SG addresses modulo dma_align must be 0. */
	uint32_t transfer_mode; /**< Default transfer mode */
	uint32_t cap_sys_size; /**< Capable system sizes */
	uint32_t cap_addr_size; /**< Capable addressing sizes */
	uint32_t cap_transfer_mode; /**< Capable transfer modes */
	uint32_t cap_mport; /**< Mport capabilities */
};

/** @brief doorbell event data */
struct riomp_mgmt_doorbell {
	did_val_t did_val; /**< RapidIO peer ID */
	uint16_t payload; /**< doorbell payload */
};

/** @brief port write event data */
struct riomp_mgmt_portwrite {
	uint32_t payload[16]; /**< standard port write data */
};

/** @brief RapidIO event */
struct riomp_mgmt_event {
	unsigned int header; /**< event kind, e.g. RIO_EVENT_DOORBELL or RIO_EVENT_PORTWRITE */
	union {
		struct riomp_mgmt_doorbell doorbell; /**< header is RIO_EVENT_DOORBELL */
		struct riomp_mgmt_portwrite portwrite; /**< header is RIO_EVENT_PORTWRITE */
	} u; /**< Event specific data */
};

/** @brief RapidIO mport handle */
struct rapidio_mport_handle {
	int fd; /**< posix api compatible fd to be used with poll/select */
	uint8_t mport_id;
};

/** @brief RapidIO mport handle */
typedef struct rapidio_mport_handle *riomp_mport_t;

typedef uint32_t mport_list_t;

/**
 * @brief read number of mports in the system
 *
 * @param[out] dev_ids mport list pointer
 * @param[inout] number_of_mports number of elements in dev_ids, will be updated by number of found items
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 * \note dev_ids is an array of values that combine mport id and the mport's
 *      destination ID, like so: ((mport_id << 16) | destid)
 * \note Free dev_ids via riomp_mgmt_free_mport_list.
 */
int riomp_mgmt_get_mport_list(mport_list_t **dev_ids,
		uint8_t *number_of_mports);

/**
 * @brief free mport list
 *
 * @param[in] dev_ids mport list pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_free_mport_list(mport_list_t **dev_ids);

/**
 * @brief read number of endpoints attached to a mport in the system
 *
 * @param[in] mport_id mport ID number
 * @param[out] did_values endpoint list pointer
 * @param[inout] number_of_eps number of elements in destids, will be updated by number of found items
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_get_ep_list(uint8_t mport_id, did_val_t **did_values,
		uint32_t *number_of_eps);

/**
 * @brief free endpoint list
 *
 * @param[in] did_values endpoint list pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_free_ep_list(did_val_t **did_values);

/**
 * @brief create mport handle
 *
 * @param[in] mport_id mport ID number
 * @param[in] flags handle property flags
 * @param[out] mport_handle new created mport hande
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_mport_create_handle(uint32_t mport_id, int flags,
		riomp_mport_t *mport_handle);

/**
 * @brief destroy mport handle
 *
 * @param[in] mport_handle mport handle to destroy
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_mport_destroy_handle(riomp_mport_t *mport_handle);

/**
 * @brief get unique mport ID number
 *
 * @param[in] mport_handle valid mport handle
 * @param[out] id mport ID number
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_get_handle_id(riomp_mport_t mport_handle, int *id);

/**
 * @brief get mport properties and status
 *
 * @param[in] mport_handle valid mport handle
 * @param[out] qresp mport properties pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_query(riomp_mport_t mport_handle,
		struct riomp_mgmt_mport_properties *qresp);

/**
 * @brief print mport status to stdout
 *
 * @param[in] prop mport properties pointer
 */
void riomp_mgmt_display_info(struct riomp_mgmt_mport_properties *prop);

/**
 * @brief set mport destination ID
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val mport destination ID
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_destid_set(riomp_mport_t mport_handle, did_val_t did_val);

/**
 * @brief read mport local CSR register
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] offset modulo four register offset
 * @param[in] size number of bytes to read (only 4 bytes supported by all devices)
 * @param[out] data read data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_lcfg_read(riomp_mport_t mport_handle, uint32_t offset,
		uint32_t size, uint32_t *data);

/**
 * @brief write mport local CSR register
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] offset modulo four register offset
 * @param[in] size number of bytes to read (only 4 bytes supported by all devices)
 * @param[in] data write data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_lcfg_write(riomp_mport_t mport_handle, uint32_t offset,
		uint32_t size, uint32_t data);

/**
 * @brief read remote device CSR register
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val Device destination ID
 * @param[in] hc hop count
 * @param[in] offset modulo four register offset
 * @param[in] size number of bytes to read (only 4 bytes supported by all devices)
 * @param[out] data read data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_rcfg_read(riomp_mport_t mport_handle, did_val_t did_val, hc_t hc,
		uint32_t offset, uint32_t size, uint32_t *data);

/**
 * @brief write mport local CSR register
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val Device destination ID
 * @param[in] hc hop count
 * @param[in] offset modulo four register offset
 * @param[in] size number of bytes to read (only 4 bytes supported by all devices)
 * @param[in] data write data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_rcfg_write(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, uint32_t offset, uint32_t size, uint32_t data);

/**
 * @brief enable a range of doorbell events
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val Device destination ID
 * @param[in] start range start value
 * @param[in] end range end value
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_dbrange_enable(riomp_mport_t mport_handle, did_val_t did_val,
		uint16_t start, uint16_t end);

/**
 * @brief disable a range of doorbell events
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val Device destination ID
 * @param[in] start range start value
 * @param[in] end range end value
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_dbrange_disable(riomp_mport_t mport_handle, did_val_t did_val,
		uint16_t start, uint16_t end);

/**
 * @brief enable a range of port write events
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] mask event mask (component tag)
 * @param[in] low range start value
 * @param[in] high range end value
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_pwrange_enable(riomp_mport_t mport_handle, uint32_t mask,
		uint32_t low, uint32_t high);

/**
 * @brief disable a range of port write events
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] mask event mask (component tag)
 * @param[in] low range start value
 * @param[in] high range end value
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_pwrange_disable(riomp_mport_t mport_handle, uint32_t mask,
		uint32_t low, uint32_t high);

/**
 * @brief set RapidIO events mask
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] mask event mask
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_set_event_mask(riomp_mport_t mport_handle, unsigned int mask);

/**
 * @brief get RapidIO events mask
 *
 * @param[in] mport_handle valid mport handle
 * @param[out] mask event mask
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_get_event_mask(riomp_mport_t mport_handle, unsigned int *mask);

/**
 * @brief get a pending RapidIO event
 *
 * The function blocks until an event is received.
 *
 * @param[in] mport_handle valid mport handle
 * @param[out] evt event data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_get_event(riomp_mport_t mport_handle,
		struct riomp_mgmt_event *evt);

/**
 * @brief send a RapidIO event
 *
 * The function is only valid for doorbell.
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] evt event data
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_send_event(riomp_mport_t mport_handle,
		struct riomp_mgmt_event *evt);

/**
 * @brief add kernel object
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val destination ID of object
 * @param[in] hc hopcount
 * @param[in] ct component tag CSR value
 * @param[in] name device name
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_device_add(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, ct_t ct, const char *name);

/**
 * @brief delete kernel object
 *
 * @param[in] mport_handle valid mport handle
 * @param[in] did_val destination ID of object
 * @param[in] hc hopcount
 * @param[in] ct component tag CSR value
 * @param[in] name device name
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_mgmt_device_del(riomp_mport_t mport_handle, did_val_t did_val,
		hc_t hc, ct_t ct, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* __RAPIDIO_MPORT_MGMT_H__ */

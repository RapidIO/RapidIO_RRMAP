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

#ifndef __RAPIDIO_MPORT_SOCK_H__
#define __RAPIDIO_MPORT_SOCK_H__

#include "rio_route.h"

#ifndef RIO_MAX_MSG_SIZE
#define RIO_MAX_MSG_SIZE (0x1000)
#endif
#define CM_HEADER_BYTES (0x18)

#ifdef __cplusplus
extern "C" {
#endif

/** @brief RapidIO mailbox handle */
typedef struct rapidio_mport_mailbox *riomp_mailbox_t;

/** @brief RapidIO socket handle */
typedef struct rapidio_mport_socket *riomp_sock_t;

#define RIO_SOCKET_MSG_SIZE 0x1000
#define RIO_SOCKET_RSVD_SIZE 20
#define RIO_SOCKET_PAYLOAD_SIZE (RIO_SOCKET_MSG_SIZE - RIO_SOCKET_RSVD_SIZE)

struct rapidio_mport_socket_msg_t {
	uint8_t do_not_modify[RIO_SOCKET_RSVD_SIZE];
	uint8_t payload[RIO_SOCKET_PAYLOAD_SIZE];
};

typedef union {
	uint8_t as_bytes[RIO_SOCKET_MSG_SIZE];
	struct rapidio_mport_socket_msg_t msg;
} rapidio_mport_socket_msg;

/**
 * @brief initialize RapidIO type 11 mailbox support
 *
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_mbox_init(void);

/**
 * @brief de-initialize RapidIO type 11 mailbox support
 *
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_mbox_exit(void);

/**
 * @brief create mailbox handle instance
 *
 * @param[in] mport_id mport ID number
 * @param[in] mbox_id mailbox number
 * @param[out] mailbox new created mailbox handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_mbox_create_handle(uint8_t mport_id, uint8_t mbox_id,
		riomp_mailbox_t *mailbox);

/**
 * @brief destroy mailbox handle instance
 *
 * @param[in] mailbox new created mailbox handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_mbox_destroy_handle(riomp_mailbox_t *mailbox);

/**
 * @brief create a socket handle
 *
 * @param[in] mailbox valid mailbox handle
 * @param[out] socket_handle new created socket handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_socket(riomp_mailbox_t mailbox, riomp_sock_t *socket_handle);

/**
 * @brief send data via RapidIO socket
 *
 * @param[in] socket_handle valid socket handle
 * @param[in] skt_msg pointer to message
 * @param[in] size number of bytes to send
 * @param[in] stop_req NULL if the parameter should be ignored.
 * 		if *stop_req is 0, reattempt the send in the event of
 * 		a non-fatal error i.e. errno = EAGAIN, EBUSY, EINTR.
 * 		if *stop_req is not 0, return immediately.
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_send(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg *skt_msg, uint32_t size,
		volatile int *stop_req);

/**
 * @brief receive data via RapidIO socket
 *
 * @param[in] socket_handle valid socket handle
 * @param[out] skt_msg pointer to message pointer
 * @param[in] timeout receive timeout in mSec. 0 = blocking
 * @param[in] stop_req NULL if the parameter should be ignored.
 * 		if *stop_req is 0, reattempt the send in the event of
 * 		a non-fatal error i.e. errno = EAGAIN, EBUSY, EINTR.
 * 		if *stop_req is not 0, return immediately.
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 *
 * skt_msg must be big enough to accept a maximum sized message.
 */
int riomp_sock_receive(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg **skt_msg,
		uint32_t timeout, volatile int *stop_req);

/**
 * @brief release receive data buffer
 *
 * @param[in] socket_handle valid socket handle
 * @param[in] skt_msg pointer to message
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_release_receive_buffer(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg *skt_msg);

/**
 * @brief close a RapidIO socket handle
 *
 * @param[in] socket_handle valid socket handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_close(riomp_sock_t *socket_handle);

/**
 * @brief bind a RapidIO socket to a channel
 *
 * @param[in] socket_handle valid socket handle
 * @param[in] local_channel local channel number
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_bind(riomp_sock_t socket_handle, uint16_t local_channel);

/**
 * @brief listen on a RapidIO socket for incoming connects
 *
 * @param[in] socket_handle valid socket handle
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_listen(riomp_sock_t socket_handle);

/**
 * @brief accept an incoming connection on a RapidIO socket
 *
 * @param[in] socket_handle valid socket handle
 * @param[out] conn connected socket handle
 * @param[in] timeout receive timeout in mSec. 0 = blocking
 * @param[in] stop_req NULL if the parameter should be ignored.
 * 		if *stop_req is 0, reattempt the send in the event of
 * 		a non-fatal error i.e. errno = EAGAIN, EBUSY, EINTR.
 * 		if *stop_req is not 0, return immediately.
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_accept(riomp_sock_t socket_handle, riomp_sock_t *conn,
		uint32_t timeout, volatile int *stop_req);

/**
 * @brief connect to a remote RapidIO socket
 *
 * @param[in] socket_handle valid socket handle
 * @param[in] did_val peer destination ID
 * @param[in] channel peer channel number
 * @param[in] stop_req NULL if the parameter should be ignored.
 * 		if *stop_req is 0, reattempt the send in the event of
 * 		a non-fatal error i.e. errno = EAGAIN, EBUSY, EINTR.
 * 		if *stop_req is not 0, return immediately.
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_connect(riomp_sock_t socket_handle, did_val_t did_val,
		uint16_t channel, volatile int *stop_req);

/**
 * @brief allocate send buffer
 *
 * @param[in] socket_handle valid socket handle
 * @param[out] skt_msg pointer to message pointer
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_request_send_buffer(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg **skt_msg);

/**
 * @brief release send buffer
 *
 * @param[in] socket_handle valid socket handle
 * @param[in] skt_msg pointer to message
 * @return status of the function call
 * @retval 0 on success
 * @retval -errno on error
 */
int riomp_sock_release_send_buffer(riomp_sock_t socket_handle,
		rapidio_mport_socket_msg *skt_msg);

#ifdef __cplusplus
}
#endif

#endif /* __RAPIDIO_MPORT_SOCK_H__ */

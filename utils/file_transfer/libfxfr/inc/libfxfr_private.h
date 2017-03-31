/*
****************************************************************************
Copyright (c) 2015, Integrated Device Technology Inc.
Copyright (c) 2015, RapidIO Trade Association
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************
*/

#ifndef __LIBFXFR_PRIVATE_H__
#define __LIBFXFR_PRIVATE_H__

/**
 * @file libfxfr_private.h
 * Private interface for debugging and server implementation
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "libfxfr.h"
#include "rapidio_mport_sock.h"

#define MAX_TX_SEGMENTS 2
#define TOTAL_TX_BUFF_SIZE 0x400000
#define MAX_TX_BUFF_SIZE   (TOTAL_TX_BUFF_SIZE/MAX_TX_SEGMENTS)

#ifdef __cplusplus
extern "C" {
#endif

struct buffer_info {
        uint8_t valid; /* 0 - info is not valid, window not in use.
                        * 1 - info is valid, window available
                        */
	bool is_an_ibwin; /* If an inbound window was allocated for this buffer,
			* then this is true.  The inbound window should be
			* unmapped when the file transfer server exits.
			*/
        uint64_t rio_base; /* RapidIO inbound base address */
        uint64_t length; /* Size of inbound window/memory in bytes */
        uint64_t handle; /* Handle to use for mport */
	char *ib_mem; /* Virtual address for inbound memory */
	uint8_t thr_valid; /* 0 if thread not valid, 
			    * 1 if thread is running.
			    */
        pthread_t xfer_thread; /* The thread handling this window */
        sem_t req_avail; /* server kicks this when new request comes in
                              * for this thread to process. */
        riomp_sock_t *req_skt; /* Socket from successfull connect request */
        char file_name[MAX_FILE_NAME+1]; /* Name of local file */
        int fd; /* File descriptor for received file */
        volatile uint8_t completed; /* 0 - currently processing request.
                           * 1 - processing complete, waiting on request_avail
			   */
        int rc;       /* return code, 0 means OK, others are stderr codes */
	rapidio_mport_socket_msg *msg_rx;
	rapidio_mport_socket_msg *msg_tx;
	struct fxfr_client_to_svr_msg *rxed_msg;
	struct fxfr_svr_to_client_msg *tx_msg;
	int debug;
	uint64_t bytes_rxed;
};

extern int rx_file(struct buffer_info *info, volatile int *abort_flag);

#ifdef __cplusplus
}
#endif

#endif /* __LIBFXFR_PRIVATE_H__ */

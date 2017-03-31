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

#ifndef __FMD_PEER_MSG_H__
#define __FMD_PEER_MSG_H__

/**
 * @file fmd_peer_msg.h
 * Definition of messages exchanged between the Fabric Management Daemons
 */

#include <stdint.h>

#include "rio_route.h"
#include "rio_ecosystem.h"
#include "rapidio_mport_sock.h"
#include "did.h"

#define FMD_P_MSG_RESP 0x80000000
#define FMD_P_MSG_FAIL 0x40000000

#define FMD_P_REQ_HELLO 3
#define FMD_P_REQ_MOD   7
#define FMD_P_REQ_FSET  9
#define FMD_P_RESP_HELLO (FMD_P_REQ_HELLO|FMD_P_MSG_RESP)
#define FMD_P_RESP_MOD   (FMD_P_REQ_MOD|FMD_P_MSG_RESP)
#define FMD_P_MSG_LAST_MSG_TYPE 0x10;

#define MAX_P_NAME 47

struct fmd_p_hello {
	char peer_name[MAX_P_NAME+1];
	uint32_t pid; /* Process ID */
	did_val_t did_val;
	uint32_t did_sz;
	ct_t ct;
	union {
		uint32_t hc_long; // Messaging alignment requires 4 bytes
		hc_t hc_short; // Reminder that this is hc_t.
	};
};

typedef struct fmd_p_hello fmd_s_hello_req;
typedef struct fmd_p_hello fmd_m_hello_resp;

#define FMD_P_OP_ADD ((uint32_t)(0xADD1BEEF))  
#define FMD_P_OP_DEL ((uint32_t)(0xDEADBEEF))

#define FMD_SLAVE_MPORT_NAME "MPORT0"
#define FMD_SLAVE_MASTER_NAME "FMD_MAST"

struct fmd_m_peer_mod_req {
	uint32_t op;
	did_val_t did_val;
	uint32_t did_sz;
	union {
		uint32_t hc_long; // Messaging alignment requires 4 bytes
		hc_t hc_short; // Reminder that this is hc_t.
	};
	ct_t ct;
	uint32_t is_mp;
	uint32_t flag;
	char name[MAX_P_NAME+1];
};

struct fmd_s_peer_mod_resp {
	did_val_t did_val;
	uint32_t did_sz;
	union {
		uint32_t hc_long; // Messaging alignment requires 4 bytes
		hc_t hc_short; // Reminder that this is hc_t.
	};
	ct_t ct;
	uint32_t is_mp;
	uint32_t flag;
	uint32_t rc; /* 0 means success */
};

/* Note, this message is sent from master to slave and from slave to master */
/* No response. */
struct fmd_flag_set_req {
	did_val_t did_val;
	uint32_t did_sz;
	ct_t ct;
	uint32_t flag;
};

/* Format of messages that can be sent by the Master FMD  to the Slave FMD */

struct fmd_mast_to_slv_msg {
	uint8_t unused[RIO_SOCKET_RSVD_SIZE];
	uint32_t msg_type;
	did_val_t dest_did_val;
	uint32_t dest_did_sz;
	union {
		fmd_m_hello_resp hello_rsp;
		struct fmd_m_peer_mod_req mod_rq;
		struct fmd_flag_set_req fset;
	};
};

/* NOTE: Assumes that sizeof(fmd_mast_req_msg) is a multiple of 4 */
#define FMD_P_M2S_SZ (sizeof(struct fmd_mast_to_slv_msg))
#define FMD_P_M2S_CM_SZ (FMD_P_M2S_SZ+(FMD_P_M2S_SZ%8))

struct fmd_slv_to_mast_msg {
	uint8_t unused[RIO_SOCKET_RSVD_SIZE];
	uint32_t msg_type;
	did_val_t src_did_val;
	uint32_t src_did_sz;
	union {
		fmd_s_hello_req hello_rq;
		struct fmd_s_peer_mod_resp mod_rsp;
		struct fmd_flag_set_req fset;
	};
};

/* NOTE: Assumes that sizeof(fmd_mast_req_msg) is a multiple of 4 */
#define FMD_P_S2M_SZ (sizeof(struct fmd_slv_to_mast_msg))
#define FMD_P_S2M_CM_SZ (FMD_P_S2M_SZ+(FMD_P_S2M_SZ%8))

#endif /* __FMD_PEER_MSG_H__ */


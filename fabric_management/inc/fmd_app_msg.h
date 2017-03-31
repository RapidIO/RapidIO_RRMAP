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

#ifndef __FMD_APP_MSG_H__
#define __FMD_APP_MSG_H__

/**
 * @file fmd_app_msg.h
 * Definition of messages exchanged between the Fabric Management Daemon
 * and the Fabric Management Library
 */

#include <stdint.h>

#define FMD_APP_MSG_RESP 0x80000000
#define FMD_APP_MSG_FAIL 0x40000000

#define FMD_REQ_HELLO 6
#define FMD_RESP_HELLO (FMD_REQ_HELLO|FMD_APP_MSG_RESP)
#define FMD_APP_MSG_LAST_MSG_TYPE 0x10;

#define MAX_APP_NAME 48

#define MAX_DD_FN_SZ 63
#define MAX_DD_MTX_FN_SZ 63

/* Requests sent from library to FMD */

struct fmd_app_hello_req {
	char app_name[MAX_APP_NAME+1];
	uint32_t app_pid;
	uint32_t flag;
};

struct fmd_app_hello_resp {
	uint32_t sm_dd_mtx_idx;
	char dd_fn[MAX_DD_FN_SZ+1];
	char dd_mtx_fn[MAX_DD_MTX_FN_SZ+1];
	uint32_t fmd_update_pd; // Seconds between updates to chg_idx/chg_time
};

/* Format of messages that can be sent by the library */

struct libfmd_dmn_app_msg { /* Messages sent FROM application TO FMD */
	uint32_t msg_type;
	union {
		struct fmd_app_hello_req hello_req;
		struct fmd_app_hello_resp hello_resp;
	};
};

#endif /* __FMD_APP_MSG_H__ */


/*
****************************************************************************
Copyright (c) 2014, Integrated Device Technology Inc.
Copyright (c) 2014, RapidIO Trade Association
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

#ifndef __CFG_PRIVATE_H__
#define __CFG_PRIVATE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>

#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include "ct.h"
#include "RapidIO_Routing_Table_API.h"
#include "RapidIO_Port_Config_API.h"
#include "riocp_pe.h"
#include "fmd_dd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_OP_MODE_SLAVE 0
#define CFG_OP_MODE_MASTER 1
#define CFG_DEV08 0
#define CFG_DEV16 1
#define CFG_DEV32 2
#define CFG_DEVID_MAX (CFG_DEV32+1)

struct int_cfg_ep;

struct int_mport_info {
	uint32_t num;
	riocp_pe_handle mp_h;
	ct_t ct; /* Updated when MPORT is initialized */
	int op_mode;
	uint8_t mem_sz;	/* Memory size to use for this network */
	struct dev_id devids[CFG_DEVID_MAX];
	struct int_cfg_ep *ep; /* Link to endpoint definition for this MPORT */
	int ep_pnum; /* EP port number that matches this MPORT */
};

#define CFG_MAX_MPORTS 4
#define CFG_DFLT_INIT_DD 0
#define CFG_DFLT_RUN_CONS 1
#define CFG_DFLT_MAST_DEVID_SZ FMD_DEV08
#define CFG_DFLT_MAST_DEVID 1
#define CFG_DFLT_MAST_INTERVAL 5
#define CFG_INVALID_CT 0
#define CFG_MAX_EP 70
#define CFG_MAX_EP_PORT 1
#define CFG_MAX_SW 20
#define CFG_MAX_CONN_PORT 24
#define CFG_MAX_CONN (CFG_MAX_SW*CFG_MAX_CONN_PORT)
#define CFG_MAX_DEVS (CFG_MAX_SW+CFG_MAX_CONN)

#define OTHER_END(x) ((1 == x)?0:((0==x)?1:2))

#define CFG_SLAVE -1

struct int_cfg_rapidio {
	rio_pc_pw_t max_pw;
	rio_pc_pw_t op_pw;
	rio_pc_ls_t ls;
	int idle2; /* 0 for idle1, 1 for idle2 */
	int em; /* 0 for no error management, 1 to enable error management */
};

struct int_cfg_ep_port {
	int valid;
	uint32_t port;
	ct_t ct;
	struct int_cfg_rapidio rio;
	struct dev_id devids[CFG_DEVID_MAX];
	struct int_cfg_conn *conn;
	int conn_end; /* index of *conn for this switch */
};

struct int_cfg_ep {
	int valid;
	char *name;
	int port_cnt;
	struct int_cfg_ep_port ports[CFG_MAX_EP_PORT];
};

struct int_cfg_sw_port {
	int valid;
	int port;
	struct int_cfg_rapidio rio;
	struct int_cfg_conn *conn;
	int conn_end; /* index of *conn for this switch */
	// One routing table for each devID size
	bool rt_valid[CFG_DEVID_MAX];
	rio_rt_state_t rt[CFG_DEVID_MAX];
};

struct int_cfg_sw {
	int valid;
	char *name;
	char *dev_type;
	did_val_t did_val;
	uint32_t did_sz;
	hc_t hc;
	ct_t ct;
	struct int_cfg_sw_port ports[CFG_MAX_SW_PORT];
	// One routing table for each devID size
	bool rt_valid[CFG_DEVID_MAX]; 
	rio_rt_state_t rt[CFG_DEVID_MAX];
};

struct int_cfg_conn_pe {
	int port_num;
	int ep; /* 1 - Endpoint, 0 - Switch */
	union {
		struct int_cfg_sw *sw_h;
		struct int_cfg_ep *ep_h;
	};
};

struct int_cfg_conn {
	int valid;
	struct int_cfg_conn_pe ends[2];
};

struct int_cfg_parms {
	char *dd_mtx_fn;
	char *dd_fn;
	int init_err;
	int mast_idx;	/* Idx of the mport_info that is master */
	uint32_t max_mport_info_idx; /* Maximum number of mports */
	struct int_mport_info mport_info[CFG_MAX_MPORTS];
	did_val_t mast_did_val;	/* Master CFG location information */
	uint32_t mast_did_sz;
	uint32_t mast_cm_port; 	/* Master CFG location information */
	uint32_t ep_cnt;
	struct int_cfg_ep eps[CFG_MAX_EP];
	uint32_t sw_cnt;
	struct int_cfg_sw sws[CFG_MAX_SW];
	uint32_t conn_cnt;
	struct int_cfg_conn cons[CFG_MAX_CONN];
	bool auto_config;
};

extern struct int_cfg_parms *cfg;

#ifdef __cplusplus
}
#endif

#endif /* __CFG_PRIVATE_H__ */

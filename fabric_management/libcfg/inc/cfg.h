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

#ifndef __CFG_H__
#define __CFG_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "rio_route.h"
#include "did.h"
#include "rrmap_config.h"
#include "RapidIO_Routing_Table_API.h"
#include "RapidIO_Port_Config_API.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_OP_MODE_SLAVE 0
#define CFG_OP_MODE_MASTER 1
#define CFG_DEV08 0
#define CFG_DEV16 1
#define CFG_DEV32 2
#define CFG_DEVID_MAX (CFG_DEV32+1)

#define CFG_MAX_SW_PORT 24 //18

#define CFG_MEM_SZ_34 (uint8_t)(34)
#define CFG_MEM_SZ_50 (uint8_t)(50)
#define CFG_MEM_SZ_66 (uint8_t)(66)
#define CFG_MEM_SZ_DEFAULT CFG_MEM_SZ_34

struct dev_id {
	uint32_t valid;
	hc_t hc;
	did_val_t did_val;
};

struct cfg_mport_info {
	uint32_t num; /* Kernel index of this mport */
	ct_t ct; /* Updated when MPORT is initialized */
	struct dev_id devids[CFG_DEVID_MAX]; /* Device IDs for each size */
};

struct cfg_ep_port {
	int valid;
	rio_pc_pw_t op_pw;
	rio_pc_ls_t ls;
	struct dev_id devids[CFG_DEVID_MAX];
};

struct cfg_sw_port {
	int valid;
	int port;
	rio_pc_pw_t op_pw;
	rio_pc_ls_t ls;
	rio_rt_state_t *rt[CFG_DEVID_MAX];
};

struct cfg_sw {
	int num_ports;
	struct cfg_sw_port sw_pt[CFG_MAX_SW_PORT];
	rio_rt_state_t *rt[CFG_DEVID_MAX];
};

struct cfg_dev {
	const char *name; /* System name of device */
	ct_t ct; /* Component tag value */
	uint32_t is_sw; /* 0 - endpoint, 1 - switch */
	struct cfg_ep_port ep_pt;
	struct cfg_sw sw_info;
};

int cfg_parse_file(char *cfg_fn, char **dd_mtx_fn, char **dd_fn,
		did_t *m_did, uint32_t *m_cm_port, uint32_t *m_mode);
int cfg_find_mport(uint32_t mport, struct cfg_mport_info *mp);
int cfg_get_mp_mem_sz(uint32_t mport, uint8_t *mem_sz );
int cfg_find_dev_by_ct(ct_t ct, struct cfg_dev *dev);
int cfg_get_conn_dev(ct_t ct, int pt, struct cfg_dev *dev, int *conn_pt);
bool cfg_auto(void);

#ifdef __cplusplus
}
#endif

#endif /* __CFG_H__ */

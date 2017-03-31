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

#ifndef __FMD_DD_H__
#define __FMD_DD_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <riocp_pe.h>

#include "rio_route.h"
#include "did.h"
#include "rrmap_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FMD_DEV08 0
#define FMD_DEV16 1
#define FMD_DEV32 2
#define FMD_MAX_DEVID_SZ (FMD_DEV32+1)
#define FMD_MAX_DEVS 64
#define FMD_MAX_DEVID 255
#define FMD_MAX_NAME 47

struct fmd_dd_dev_info {
	ct_t ct;
	did_t did;
	hc_t hc;
	uint32_t is_mast_pt;
	uint8_t flag;
	char name[FMD_MAX_NAME+1];
};

struct fmd_dd {
	uint32_t chg_idx;
	struct timespec chg_time;
	ct_t md_ct;
	uint32_t num_devs;
	uint32_t loc_mp_idx;
	struct fmd_dd_dev_info devs[FMD_MAX_DEVS];
};

struct fmd_dd_ticks {
	uint32_t chg_idx;
	struct timespec chg_time;
};

struct fmd_dd_events {
	uint32_t in_use; /* 0 - Unallocated, 1 - Process number  */
	uint32_t proc; /* 0 - Unused, 1 - Process number of intended user */
	uint32_t waiting; /* 0 - waiting for event, 1 - processing event */
	sem_t dd_event; /* sem_post() whenever the dd changes */
};

#define FMD_MAX_APPS 10

struct fmd_dd_mtx {
	uint32_t mtx_ref_cnt;
	uint32_t dd_ref_cnt; /* R/W field for reference count to fmd_dd */
	uint32_t init_done;
	sem_t sem;
	struct fmd_dd_events dd_ev[FMD_MAX_APPS];
};

extern int fmd_dd_mtx_open(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx);

int fmd_dd_open(char *dd_fn, int *dd_fd, struct fmd_dd **dd,
		struct fmd_dd_mtx *dd_mtx);

extern int fmd_dd_atomic_copy(struct fmd_dd *dd,
		struct fmd_dd_mtx *dd_mtx, uint32_t *num_devs,
		struct fmd_dd_dev_info *devs, uint32_t max_devs);

extern int fmd_dd_atomic_copy_ticks(struct fmd_dd *dd,
		struct fmd_dd_mtx *dd_mtx, struct fmd_dd_ticks *ticks);

extern void fmd_dd_cleanup(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx_p, char *dd_fn, int *dd_fd,
		struct fmd_dd **dd_p, int dd_rw);

extern void bind_dd_cmds(struct fmd_dd **dd, struct fmd_dd_mtx **dd_mtx,
			char *dd_fn, char *dd_mtx_fn);

#ifdef __cplusplus
}
#endif

#endif /* __FMD_DD_H__ */

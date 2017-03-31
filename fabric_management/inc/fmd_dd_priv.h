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

#ifndef __FMD_DD_PRIV_H__
#define __FMD_DD_PRIV_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <riocp_pe.h>

#include "rrmap_config.h"
#include "fmd_dd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int fmd_dd_init(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx, char *dd_fn, int *dd_fd,
		struct fmd_dd **dd);
extern void fmd_dd_cleanup(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx_p, char *dd_fn, int *dd_fd,
		struct fmd_dd **dd_p, int dd_rw);

extern void fmd_dd_incr_chg_idx(struct fmd_dd *dd, int dd_rw);
extern uint32_t fmd_dd_get_chg_idx(struct fmd_dd *dd);

#ifdef __cplusplus
}
#endif

#endif /* __FMD_DD_PRIV_H__ */

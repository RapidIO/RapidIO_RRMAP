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

#ifndef __PE_MPDRV_PRIVATE_H__
#define __PE_MPDRV_PRIVATE_H__

#include "rio_ecosystem.h"
#include "RapidIO_Port_Config_API.h"
#include "RapidIO_Routing_Table_API.h"
#include "RapidIO_Statistics_Counter_API.h"
#include "RapidIO_Error_Management_API.h"
#include "rapidio_mport_mgmt.h"
#include "pe_mpdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mpsw_drv_pe_state {
        uint32_t                   rc;      /* RC of last routine call      */
        rio_pc_rst_handling      dev_rst; /* Device reset handling config */
        rio_pc_get_config_out_t  pc;      /* Standard config vals */
        rio_pc_get_status_out_t  ps;      /* Standard status vals */
        rio_rt_state_t           g_rt;    /* Global Routing Table for device */
        rio_rt_state_t           pprt[RIO_MAX_PORTS]; /* Per port RT */
        rio_sc_p_ctrs_val_t      sc[RIO_MAX_PORTS]; /* Statistics counters */
        rio_sc_dev_ctrs_t        sc_dev; /* Device info for stats counters */
        rio_em_cfg_pw_t          em_pw_cfg; /* Event Management Portwrite Cfg */
        rio_em_dev_rpt_ctl_out_t em_notfn; /* Device notification control */
};

/** @brief Access info linked to dev_h->accessInfo
 */
struct mpsw_drv_pe_acc_info {
	uint32_t local; /* Device is a local master port */
	uint32_t maint_valid; /* 1 if maint handle is valid, 0 if not */
	riomp_mport_t maint; /* Mport handle to access this device */
	struct riomp_mgmt_mport_properties props; /* Mport properties */
};

/** @brief Driver private information structure for pe
 */
struct mpsw_drv_private_data {
	int	is_mport;
	int	dev_h_valid;
	DAR_DEV_INFO_t	dev_h; /* Device driver handle */
	struct mpsw_drv_pe_state st; /* Device state */
};

#ifdef __cplusplus
}
#endif

#endif /* __PE_MPDRV_PRIVATE_H__ */

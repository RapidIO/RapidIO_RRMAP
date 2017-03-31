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

#ifndef __PE_MPDRV_H__
#define __PE_MPDRV_H__

#include <stdint.h>
#include <stdbool.h>

#include "riocp_pe.h"
#include "RapidIO_Device_Access_Routines_API.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t SRIO_API_ReadRegFunc(DAR_DEV_INFO_t *d_info, uint32_t offset,
		uint32_t *readdata);
uint32_t SRIO_API_WriteRegFunc(DAR_DEV_INFO_t *d_info, uint32_t offset,
		uint32_t writedata);
void SRIO_API_DelayFunc(uint32_t delay_nsec, uint32_t delay_sec);

// See riocp_drv definitions for comments (driver.h)
int RIOCP_WU mpsw_drv_init_pe(struct riocp_pe *pe, struct riocp_pe *peer,
		char *name);
int RIOCP_WU mpsw_drv_destroy_pe(struct riocp_pe *pe);
int RIOCP_WU mpsw_drv_recover_port(struct riocp_pe *pe, uint8_t port,
		uint8_t lp_port);
int RIOCP_WU mpsw_drv_get_route_entry(struct riocp_pe *pe, uint8_t port,
		did_t did, pe_rt_val *rt_val);
int RIOCP_WU mpsw_drv_set_route_entry(struct riocp_pe *pe, uint8_t port,
		did_t did, pe_rt_val rt_val);
int RIOCP_WU mpsw_drv_get_port_state(struct riocp_pe *pe, uint8_t port,
		struct riocp_pe_port_state_t *state);
int RIOCP_WU mpsw_drv_reset_port(struct riocp_pe *pe, uint8_t port,
		bool reset_lp);

int RIOCP_WU mpsw_get_mport_regs(int mp_num, struct mport_regs *regs);
int RIOCP_WU mpsw_enable_pe(struct riocp_pe *pe, pe_port_t port);

int RIOCP_WU mpsw_drv_reg_rd(struct riocp_pe *pe, uint32_t offset,
		uint32_t *val);
int RIOCP_WU mpsw_drv_reg_wr(struct riocp_pe *pe, uint32_t offset,
		uint32_t val);
int RIOCP_WU mpsw_drv_raw_reg_wr(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t addr, uint32_t val);
int RIOCP_WU mpsw_drv_raw_reg_rd(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t addr, uint32_t *val);

#ifdef __cplusplus
}
#endif

#endif /* __PE_MPDRV_H__ */

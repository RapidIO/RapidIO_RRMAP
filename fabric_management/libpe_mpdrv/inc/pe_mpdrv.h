/* libriocp_pe register read/write driver based on librio_switch and libmport */
/* libriocp_pe PE driver based on librio_switch.                              */
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

#include "riocp_pe.h"
#include "DAR_DevDriver.h"

#ifndef __PE_MPDRV_H__
#define __PE_MPDRV_H__

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t SRIO_API_ReadRegFunc(DAR_DEV_INFO_t *d_info, uint32_t offset,
							uint32_t *readdata);
extern uint32_t SRIO_API_WriteRegFunc(DAR_DEV_INFO_t *d_info, uint32_t  offset,
                                			uint32_t  writedata);
extern void SRIO_API_DelayFunc(uint32_t delay_nsec, uint32_t delay_sec);

extern struct riocp_pe_driver pe_mpsw_driver;
extern struct riocp_reg_rw_driver pe_mpsw_rw_driver;


#ifdef __cplusplus
}
#endif

#endif /* __PE_MPDRV_H__ */

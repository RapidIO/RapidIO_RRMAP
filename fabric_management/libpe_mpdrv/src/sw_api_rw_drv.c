/*  Read/Wrte/Delay routines for RapidIO Switch APIs, compatible with      */
/*  riocp_pe driver.                                                       */
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

#include <time.h>

#include "libtime_utils.h"
#include "rio_misc.h"
#include "DSF_DB_Private.h"
#include "pe_mpdrv_private.h"
#include "riocp_pe_internal.h"
#include "RapidIO_Utilities_API.h"

#ifdef __cplusplus
extern "C" {
#endif

static int get_acc_p(DAR_DEV_INFO_t *d_info, uint32_t offset,
		riocp_pe_handle *pe_h, struct mpsw_drv_pe_acc_info **acc_p)
{
	riocp_pe_handle mport_pe_h;

	if ((NULL == d_info) || (offset >= 0x01000000)) {
		goto exit;
	}

	*pe_h = (riocp_pe_handle)d_info->privateData;
	if (NULL == d_info->accessInfo) {

		/* Not an MPORT, get MPORT access information */
		mport_pe_h = (*pe_h)->mport;
		if (NULL == mport_pe_h) {
			goto exit;
		}

		if (NULL == mport_pe_h->minfo) {
			goto exit;
		}
		*acc_p = (struct mpsw_drv_pe_acc_info *)(mport_pe_h->minfo->private_data);
	} else {
		*acc_p = (struct mpsw_drv_pe_acc_info *)(d_info->accessInfo);
	}

	return 0;
exit:
	return 1;
}

uint32_t SRIO_API_ReadRegFunc(DAR_DEV_INFO_t *d_info, uint32_t offset,
		uint32_t *readdata)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint32_t x;
	struct mpsw_drv_pe_acc_info *acc_p;
	riocp_pe_handle pe_h;

	if (get_acc_p(d_info, offset, &pe_h, &acc_p)) {
		goto exit;
	}

	if (RIOCP_PE_IS_MPORT(pe_h)) {
		rc = riomp_mgmt_lcfg_read(acc_p->maint, offset, sizeof(x), &x) ?
						RIO_ERR_ACCESS:RIO_SUCCESS;
	} else {
		rc = riomp_mgmt_rcfg_read(acc_p->maint, pe_h->did_reg_val,
				pe_h->hopcount, offset, sizeof(x), &x) ?
				RIO_ERR_ACCESS : RIO_SUCCESS;
	}

	if (RIO_SUCCESS == rc) {
		*readdata = x;
	}

exit:
	return rc;
}

uint32_t SRIO_API_WriteRegFunc(DAR_DEV_INFO_t *d_info, uint32_t offset,
		uint32_t writedata)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	struct mpsw_drv_pe_acc_info *acc_p;
	riocp_pe_handle pe_h;

	if (get_acc_p(d_info, offset, &pe_h, &acc_p)) {
		goto exit;
	}

	if (RIOCP_PE_IS_MPORT(pe_h)) {
		rc = riomp_mgmt_lcfg_write(acc_p->maint, offset,
				sizeof(writedata), writedata) ?
				RIO_ERR_ACCESS : RIO_SUCCESS;
	} else {
		rc = riomp_mgmt_rcfg_write(acc_p->maint, pe_h->did_reg_val,
				pe_h->hopcount, offset, sizeof(writedata),
				writedata) ? RIO_ERR_ACCESS : RIO_SUCCESS;
	}

exit:
	return rc;
}

void SRIO_API_DelayFunc(uint32_t delay_nsec, uint32_t delay_sec)
{
	struct timespec delay = {delay_sec, delay_nsec};
	time_sleep(&delay);
}

#ifdef __cplusplus
}
#endif

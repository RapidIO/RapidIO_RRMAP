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
#include <cstddef>
#include "DAR_DevDriver.h"
#include "IDT_DSF_DB_Private.h"
#include "IDT_Statistics_Counter_API.h"
#include "rio_standard.h"
#include "rio_ecosystem.h"

#ifdef __cplusplus
extern "C" {
#endif

char *sc_names[(uint8_t)(idt_sc_last)+2] = {
    (char *)"Disabled__",
    (char *)"Enabled___",
    (char *)"UC_REQ_PKT", // Tsi57x start
    (char *)"UC_ALL_PKT",
    (char *)"Retry___CS",
    (char *)"All_____CS",
    (char *)"UC_4B_Data",
    (char *)"MCast__PKT",
    (char *)"MECS____CS",
    (char *)"MC_4B_Data", // Tsi57x end
    (char *)"PktAcc__CS", // CPS1848 start
    (char *)"ALL____PKT",
    (char *)"PktNotA_CS",
    (char *)"Drop___PKT",
    (char *)"DropTTLPKT", // CPS1848 end
    (char *)"FAB____PKT", // RXS start
    (char *)"8B_DAT_PKT",
    (char *)"8B_DAT_PKT",
    (char *)"RAW_BWIDTH", // RXS end
    (char *)"PCI_M__PKT",
    (char *)"PCI_M__PKT",
    (char *)"PCI__D_PKT",
    (char *)"PCI__D_PKT",
    (char *)"PCI_BG_PKT",
    (char *)"PCI_BG_PKT",
    (char *)"NWR____PKT",
    (char *)"NWR_OK_PKT",
    (char *)"DB_____PKT",
    (char *)"DB__OK_PKT",
    (char *)"MSG____PKT",
    (char *)"MSG____PKT",
    (char *)"MSG_RTYPKT",
    (char *)"MSG_RTYPKT",
    (char *)"DMA____PKT",
    (char *)"DMA____PKT",
    (char *)"BRG____PKT",
    (char *)"BRG____PKT",
    (char *)"BRG_ERRPKT",
    (char *)"MWR____PKT",
    (char *)"MWR_OK_PKT",
    (char *)"Last______",
    (char *)"Invalid___"
};

const char *sc_other_if_names_PCIe = (char *)"PCIExp";
const char *sc_other_if_names_FABRIC = (char *)"FABRIC";
const char *sc_other_if_names_Invalid = (char *)"INVALID";
const char *sc_other_if_names_UNKNOWN = (char *)"UNKNOWN";

uint32_t idt_sc_other_if_names(DAR_DEV_INFO_t *dev_h, const char **name)
{
	if ((NULL == dev_h) || (NULL == name)) {
		return RIO_ERR_NULL_PARM_PTR;
	};

	*name = sc_other_if_names_Invalid;
	switch(VEND_CODE(dev_h)) {
	case RIO_VEND_IDT:
		switch(DEV_CODE(dev_h)) {
		case RIO_DEVI_IDT_CPS1848:
		case RIO_DEVI_IDT_CPS1432:
		case RIO_DEVI_IDT_CPS1616:
		case RIO_DEVI_IDT_SPS1616:
		case RIO_DEVI_IDT_RXS2448:
		case RIO_DEVI_IDT_RXS1632:
			*name = sc_other_if_names_FABRIC;
			break;

		case RIO_DEVI_IDT_TSI721: // No configuration required.
			*name = sc_other_if_names_PCIe;
			break;

		default: *name = sc_other_if_names_UNKNOWN;
			return RIO_ERR_NO_DEVICE_SUPPORT;
		}
		break;
	case RIO_VEND_TUNDRA:
		*name = sc_other_if_names_Invalid;
		return RIO_ERR_NO_DEVICE_SUPPORT;
	default: *name = sc_other_if_names_UNKNOWN;
		return RIO_ERR_NO_DEVICE_SUPPORT;
	};
	return RIO_SUCCESS;
}

	
	
/* User function calls for a routing table configuration */
uint32_t idt_sc_init_dev_ctrs (
    DAR_DEV_INFO_t             *dev_info,
    idt_sc_init_dev_ctrs_in_t  *in_parms,
    idt_sc_init_dev_ctrs_out_t *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_sc_init_dev_ctrs(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

uint32_t idt_sc_read_ctrs(
    DAR_DEV_INFO_t           *dev_info,
    idt_sc_read_ctrs_in_t    *in_parms,
    idt_sc_read_ctrs_out_t   *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_sc_read_ctrs(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

#ifdef __cplusplus
}
#endif

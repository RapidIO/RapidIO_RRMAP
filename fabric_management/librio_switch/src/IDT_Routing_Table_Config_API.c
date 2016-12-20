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
#include <IDT_DSF_DB_Private.h>

#ifdef __cplusplus
extern "C" {
#endif

char *idt_em_disc_reason_names[ (uint8_t)(idt_rt_disc_last) ] = {
   (char *)"NoDiscard" ,  // idt_rt_disc_not
   (char *)"RteInvalid",  // idt_rt_disc_rt_invalid
   (char *)"Deliberate",   // idt_rt_disc_deliberately
   (char *)"PrtUnavail",   // idt_rt_disc_port_unavail
   (char *)"PrtPwrDwn" ,   // idt_rt_disc_port_pwdn
   (char *)"PrtFail"   ,   // idt_rt_disc_port_fail
   (char *)"PrtNoLp"   ,   // idt_rt_disc_port_no_lp
   (char *)"LkoutOrDis",   // idt_rt_disc_port_lkout_or_dis
   (char *)"InpOutpDis",   // idt_rt_disc_port_in_out_dis
   (char *)"MCEmpty"   ,   // idt_rt_disc_mc_empty
   (char *)"MC1bit"    ,   // idt_rt_disc_mc_one_bit
   (char *)"MCMultMask",   // idt_rt_disc_mc_mult_masks
   (char *)"DPInvalid" ,   // idt_rt_disc_dflt_pt_invalid
   (char *)"DPDelibrat",   // idt_rt_disc_dflt_pt_deliberately
   (char *)"DPPrtUaval",   // idt_rt_disc_dflt_pt_unavail
   (char *)"DPPwrDwn"  ,   // idt_rt_disc_dflt_pt_pwdn
   (char *)"DPFail"    ,   // idt_rt_disc_dflt_pt_fail
   (char *)"DPNoLp"    ,   // idt_rt_disc_dflt_pt_no_lp
   (char *)"DPLkoutDis",   // idt_rt_disc_dflt_pt_lkout_or_dis
   (char *)"DPInpOutpD",   // idt_rt_disc_dflt_pt_in_out_dis
   (char *)"ProbeABORT"    // idt_rt_disc_probe_abort
};

/* User function calls for a routing table configuration */
uint32_t idt_rt_initialize  ( DAR_DEV_INFO_t           *dev_info,
                            idt_rt_initialize_in_t   *in_parms,
                            idt_rt_initialize_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_initialize(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

uint32_t idt_rt_probe       ( DAR_DEV_INFO_t           *dev_info,
                            idt_rt_probe_in_t        *in_parms,
                            idt_rt_probe_out_t       *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_probe(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}


uint32_t idt_rt_probe_all( DAR_DEV_INFO_t          *dev_info,
                         idt_rt_probe_all_in_t   *in_parms,
                         idt_rt_probe_all_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_probe_all(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

uint32_t idt_rt_set_all( DAR_DEV_INFO_t        *dev_info,
                       idt_rt_set_all_in_t   *in_parms,
                       idt_rt_set_all_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_set_all(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

uint32_t idt_rt_set_changed( DAR_DEV_INFO_t            *dev_info,
                           idt_rt_set_changed_in_t   *in_parms,
                           idt_rt_set_changed_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_set_changed(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

uint32_t idt_rt_alloc_mc_mask( DAR_DEV_INFO_t        *dev_info,
                       idt_rt_alloc_mc_mask_in_t   *in_parms,
                       idt_rt_alloc_mc_mask_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_alloc_mc_mask(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}
uint32_t idt_rt_dealloc_mc_mask( DAR_DEV_INFO_t        *dev_info,
                       idt_rt_dealloc_mc_mask_in_t   *in_parms,
                       idt_rt_dealloc_mc_mask_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_dealloc_mc_mask(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}
uint32_t idt_rt_change_rte( DAR_DEV_INFO_t        *dev_info,
                       idt_rt_change_rte_in_t   *in_parms,
                       idt_rt_change_rte_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_change_rte(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}
uint32_t idt_rt_change_mc_mask( DAR_DEV_INFO_t        *dev_info,
                       idt_rt_change_mc_mask_in_t   *in_parms,
                       idt_rt_change_mc_mask_out_t  *out_parms )
{
    uint32_t rc = DAR_DB_INVALID_HANDLE;

    NULL_CHECK;

    if ( VALIDATE_DEV_INFO(dev_info) )
    {
        if ( IDT_DSF_INDEX(dev_info) < DAR_DB_MAX_DRIVERS )
            rc = IDT_DB[IDT_DSF_INDEX(dev_info)].idt_rt_change_mc_mask(
                    dev_info, in_parms, out_parms
                 );
    }

    return rc;
}

#ifdef __cplusplus
}
#endif

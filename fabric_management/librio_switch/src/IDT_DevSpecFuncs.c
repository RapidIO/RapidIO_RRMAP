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
#include <IDT_Common.h>
#include <DAR_DB_Private.h>
#include <IDT_CPS_Gen1_API.h>
#include <IDT_CPS_Gen2_API.h>
#include <IDT_Tsi57x_API.h>
#include <IDT_DSF_DB_Private.h>
#include <CPS1848_registers.h>
#include <DAR_Utilities.h>
#include "IDT_RXS_API.h"
#include <string.h>

#ifdef IDT_TSI721_SUPPORT
#include <IDT_Tsi721_API.h>
#endif

#include "IDT_RXS_API.h"


#ifdef __cplusplus
extern "C" {
#endif

uint32_t num_idt_drivers_in_use;
IDT_DSF_DB_t IDT_DB[DAR_DB_MAX_DRIVERS];

uint32_t CPS_rioSetAssmblyInfo( DAR_DEV_INFO_t *dev_info, 
                                      uint32_t  AsmblyVendID, 
                                      uint16_t  AsmblyRev    ) 
{
   uint32_t rc = DARRegWrite( dev_info, CPS1848_ASSY_IDENT_CAR_OVRD, AsmblyVendID );

   if (RIO_SUCCESS == rc) 
     rc = DARRegWrite(dev_info, CPS1848_ASSY_INF_CAR_OVRD, (uint32_t)(AsmblyRev));

   return rc;
}

/* Default routines */
uint32_t IDT_DSF_rt_initialize( DAR_DEV_INFO_t           *dev_info, 
                              idt_rt_initialize_in_t   *in_parms, 
                              idt_rt_initialize_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_rt_probe( DAR_DEV_INFO_t      *dev_info, 
                         idt_rt_probe_in_t   *in_parms, 
                         idt_rt_probe_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_rt_probe_all( DAR_DEV_INFO_t          *dev_info, 
                             idt_rt_probe_all_in_t   *in_parms, 
                             idt_rt_probe_all_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_rt_set_all( DAR_DEV_INFO_t        *dev_info, 
                           idt_rt_set_all_in_t   *in_parms, 
                           idt_rt_set_all_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_rt_set_changed( DAR_DEV_INFO_t            *dev_info, 
                               idt_rt_set_changed_in_t   *in_parms, 
                               idt_rt_set_changed_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}

uint32_t IDT_DSF_rt_alloc_mc_mask( DAR_DEV_INFO_t              *dev_info, 
                                 idt_rt_alloc_mc_mask_in_t   *in_parms, 
                                 idt_rt_alloc_mc_mask_out_t  *out_parms )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mc_idx;

   NULL_CHECK;

   if (!in_parms->rt) {
      out_parms->imp_rc = RT_ALLOC_MC_MASK(1);
      goto IDT_rt_alloc_mc_mask_exit;
   };

   for (mc_idx = 0; mc_idx < NUM_MC_MASKS(dev_info); mc_idx++) 
   {
      if (!in_parms->rt->mc_masks[mc_idx].in_use &&
          !in_parms->rt->mc_masks[mc_idx].allocd   ) 
      {
         out_parms->mc_mask_rte = IDT_DSF_FIRST_MC_MASK + mc_idx;
         out_parms->imp_rc = RIO_SUCCESS;
         in_parms->rt->mc_masks[mc_idx].allocd = true;
         rc = RIO_SUCCESS;
         break;
      };
   };
   
   if (RIO_SUCCESS != rc) {
      out_parms->imp_rc      = RT_ALLOC_MC_MASK(2);
      out_parms->mc_mask_rte = IDT_DSF_BAD_MC_MASK;
      rc = RIO_ERR_INSUFFICIENT_RESOURCES;
   };

IDT_rt_alloc_mc_mask_exit:
   return rc;
} 

uint32_t IDT_DSF_rt_dealloc_mc_mask( DAR_DEV_INFO_t                *dev_info, 
                                   idt_rt_dealloc_mc_mask_in_t   *in_parms, 
                                   idt_rt_dealloc_mc_mask_out_t  *out_parms )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mc_idx;
   uint16_t dev_rte, dom_rte;

   out_parms->imp_rc = RIO_SUCCESS;

   NULL_CHECK;

   if (!in_parms->rt) {
      out_parms->imp_rc = RT_DEALLOC_MC_MASK(1);
      goto IDT_DSF_rt_dealloc_mc_mask_exit;
   };

   mc_idx = in_parms->mc_mask_rte - IDT_DSF_FIRST_MC_MASK;

   if (mc_idx >= IDT_DSF_MAX_MC_MASK) {
      out_parms->imp_rc = RT_DEALLOC_MC_MASK(2);
      goto IDT_DSF_rt_dealloc_mc_mask_exit;
   };

   rc = RIO_SUCCESS;

   for (dev_rte = 0; dev_rte < IDT_DAR_RT_DOM_TABLE_SIZE; dev_rte++) {
      if (in_parms->rt->dev_table[dev_rte].rte_val == in_parms->mc_mask_rte) {
         in_parms->rt->dev_table[dev_rte].changed = true;
	 in_parms->rt->dev_table[dev_rte].rte_val = IDT_DSF_RT_NO_ROUTE;
      };
   };

   for (dom_rte = 0; dom_rte < IDT_DAR_RT_DOM_TABLE_SIZE; dom_rte++) {
      if (in_parms->rt->dom_table[dom_rte].rte_val == in_parms->mc_mask_rte) {
         in_parms->rt->dom_table[dom_rte].changed = true;
	 in_parms->rt->dom_table[dom_rte].rte_val = IDT_DSF_RT_NO_ROUTE;
      };
   };

   if (in_parms->rt->mc_masks[mc_idx].in_use) {
      dev_rte = in_parms->rt->mc_masks[mc_idx].mc_destID & 0x00FF;
      in_parms->rt->dev_table[dev_rte].changed = true;
      in_parms->rt->dev_table[dev_rte].rte_val = IDT_DSF_RT_NO_ROUTE;
   };

   if ((in_parms->rt->mc_masks[mc_idx].in_use) || (in_parms->rt->mc_masks[mc_idx].allocd)) {
      in_parms->rt->mc_masks[mc_idx].mc_destID = 0;
      in_parms->rt->mc_masks[mc_idx].tt        = tt_dev8;
      in_parms->rt->mc_masks[mc_idx].mc_mask   = 0;
      in_parms->rt->mc_masks[mc_idx].in_use    = false;
      in_parms->rt->mc_masks[mc_idx].allocd    = false;
      in_parms->rt->mc_masks[mc_idx].changed   = true;
   };

IDT_DSF_rt_dealloc_mc_mask_exit:
   return rc;
}

uint32_t IDT_DSF_rt_change_rte( DAR_DEV_INFO_t           *dev_info, 
                              idt_rt_change_rte_in_t   *in_parms, 
                              idt_rt_change_rte_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}

uint32_t IDT_DSF_rt_change_mc_mask( DAR_DEV_INFO_t                *dev_info, 
                                   idt_rt_change_mc_mask_in_t   *in_parms, 
                                   idt_rt_change_mc_mask_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


void check_multicast_routing ( DAR_DEV_INFO_t     *dev_info, 
                               idt_rt_probe_in_t  *in_parms, 
                               idt_rt_probe_out_t *out_parms ) 
{
   uint8_t  mc_idx, bit;
   uint32_t mc_mask;
   bool   found = false;

   for (mc_idx = 0; mc_idx < NUM_MC_MASKS(dev_info); mc_idx++) {
      if ((in_parms->tt == in_parms->rt->mc_masks[mc_idx].tt    ) &&
          (                in_parms->rt->mc_masks[mc_idx].in_use) ) {
         if (tt_dev8 == in_parms->tt) {
            mc_mask = 0x00FF;
	 } else {
            mc_mask = 0xFFFF;
	 };

         if ((in_parms->destID & mc_mask) == (in_parms->rt->mc_masks[mc_idx].mc_destID & mc_mask)) {
            if (found) {
               out_parms->reason_for_discard = idt_rt_disc_mc_mult_masks;
               out_parms->valid_route        = false;
	       break;
	    } else {
	       found = true;
               out_parms->routing_table_value = mc_idx + IDT_DSF_FIRST_MC_MASK;
	       for (bit = 0; bit < NUM_PORTS(dev_info); bit++)
	          out_parms->mcast_ports[bit] = ((uint32_t)(1 << bit) & in_parms->rt->mc_masks[mc_idx].mc_mask)?true:false;
	       if (in_parms->rt->mc_masks[mc_idx].mc_mask) {
                  if ((uint32_t)((uint32_t)(1) << in_parms->probe_on_port) == in_parms->rt->mc_masks[mc_idx].mc_mask) {
                     out_parms->reason_for_discard = idt_rt_disc_mc_one_bit;
	          } else {
                     out_parms->reason_for_discard = idt_rt_disc_not;
		     out_parms->valid_route        = true;
		  };
	       } else {
                  out_parms->reason_for_discard     = idt_rt_disc_mc_empty;
	       };
	    };
         };
      };
   };

   return;
};

// Determines route, and then determines packet discard based purely
// on the settings of the routing table.

void check_unicast_routing( DAR_DEV_INFO_t     *dev_info, 
                            idt_rt_probe_in_t  *in_parms, 
                            idt_rt_probe_out_t *out_parms ) 
{
    uint8_t idx;
    uint32_t phys_rte, rte = 0;
    bool  dflt_pt;

	if (NULL == dev_info)
		return;

    if (tt_dev16 == in_parms->tt) {
       idx = (uint8_t)((in_parms->destID & (uint16_t)(0xFF00)) >> 8);
       rte = in_parms->rt->dom_table[idx].rte_val;
    };

    if ((tt_dev8 == in_parms->tt) || (IDT_DSF_RT_USE_DEVICE_TABLE == rte)) {
       idx = (uint8_t)(in_parms->destID & 0x00FF) ;
       rte = in_parms->rt->dev_table[idx].rte_val;
    };

    out_parms->routing_table_value = rte;
    out_parms->valid_route         = true;
    out_parms->reason_for_discard  = idt_rt_disc_not;
    dflt_pt = (IDT_DSF_RT_USE_DEFAULT_ROUTE == rte)?true:false;

    phys_rte = (dflt_pt)?in_parms->rt->default_route : rte;

    if (IDT_DSF_RT_NO_ROUTE == phys_rte) {
       out_parms->valid_route         = false;
       out_parms->reason_for_discard  = (dflt_pt)?idt_rt_disc_dflt_pt_deliberately:
	                                          idt_rt_disc_deliberately;
    } else {
       if (phys_rte >= NUM_PORTS(dev_info)) {
          out_parms->valid_route         = false;
          out_parms->reason_for_discard  = (dflt_pt)?idt_rt_disc_dflt_pt_invalid:
		                                     idt_rt_disc_rt_invalid;
       };
    };
       
    return;
};

uint32_t IDT_DSF_pc_get_config( DAR_DEV_INFO_t           *dev_info, 
                              idt_pc_get_config_in_t   *in_parms, 
                              idt_pc_get_config_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_set_config( DAR_DEV_INFO_t           *dev_info, 
                              idt_pc_set_config_in_t   *in_parms, 
                              idt_pc_set_config_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_get_status( DAR_DEV_INFO_t           *dev_info, 
                              idt_pc_get_status_in_t   *in_parms, 
                              idt_pc_get_status_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_reset_port( DAR_DEV_INFO_t           *dev_info, 
                              idt_pc_reset_port_in_t   *in_parms, 
                              idt_pc_reset_port_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_reset_link_partner(
    DAR_DEV_INFO_t                   *dev_info, 
    idt_pc_reset_link_partner_in_t   *in_parms, 
    idt_pc_reset_link_partner_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_clr_errs( DAR_DEV_INFO_t         *dev_info, 
                            idt_pc_clr_errs_in_t   *in_parms, 
                            idt_pc_clr_errs_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_secure_port( DAR_DEV_INFO_t            *dev_info, 
                               idt_pc_secure_port_in_t   *in_parms, 
                               idt_pc_secure_port_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}


uint32_t IDT_DSF_pc_dev_reset_config( DAR_DEV_INFO_t                 *dev_info, 
                                    idt_pc_dev_reset_config_in_t   *in_parms, 
                                    idt_pc_dev_reset_config_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}

uint32_t IDT_DSF_pc_probe( DAR_DEV_INFO_t      *dev_info, 
                         idt_pc_probe_in_t   *in_parms, 
                         idt_pc_probe_out_t  *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
}

uint32_t default_idt_pc_probe( DAR_DEV_INFO_t      *dev_info,
                             idt_pc_probe_in_t   *in_parms,
                             idt_pc_probe_out_t  *out_parms)
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t regVal, regVal2;

    idt_pc_get_status_in_t  stat_in;
    idt_pc_get_status_out_t stat_out;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;

    out_parms->status = port_los;

    NULL_CHECK;

    if (in_parms->port >= NUM_PORTS(dev_info)) {
       out_parms->imp_rc = PC_PROBE(1);
       goto default_pc_probe_exit;
    };

    out_parms->imp_rc = RIO_SUCCESS;

    stat_in.ptl.num_ports = 1;
	stat_in.ptl.pnums[0] = in_parms->port;
    rc = idt_pc_get_status( dev_info, &stat_in, &stat_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = stat_out.imp_rc;
       goto default_pc_probe_exit;
    };

    if (!(stat_out.ps[0].port_ok)) {
       out_parms->imp_rc = PC_PROBE(8);
       goto default_pc_probe_exit;
    };

    cfg_in.ptl.num_ports = 1;
	cfg_in.ptl.pnums[0] = in_parms->port;
    rc = idt_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto default_pc_probe_exit;
    };
    
    if (cfg_out.pc[0].pw == stat_out.ps[0].pw) {
       out_parms->imp_rc = PC_PROBE(0x11);
       out_parms->status = port_ok;
    } else {
       out_parms->imp_rc = PC_PROBE(0x12);
       out_parms->status = port_degr;
    };

    rc = DARRegRead( dev_info, RIO_SPX_ERR_STAT(dev_info->extFPtrForPort, dev_info->extFPtrPortType, in_parms->port), &regVal );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_PROBE(0x19);
       goto default_pc_probe_exit;
    };

    if (regVal & (RIO_SPX_ERR_STAT_ERR        |
                  RIO_SPX_ERR_STAT_IES  |
                  RIO_SPX_ERR_STAT_IRS        |
                  RIO_SPX_ERR_STAT_OES |
                  RIO_SPX_ERR_STAT_ORS        )) {
       out_parms->imp_rc = PC_PROBE(0x20);
       out_parms->status = port_err;
       goto default_pc_probe_exit;
    };

    if (regVal & (RIO_SPX_ERR_STAT_FAIL |
                  RIO_SPX_ERR_STAT_DROP )) {
       out_parms->imp_rc = PC_PROBE(0x30);
       out_parms->status = port_err;
       goto default_pc_probe_exit;
    };

    rc = DARRegRead( dev_info, RIO_SPX_ACKID_ST(dev_info->extFPtrForPort, dev_info->extFPtrPortType, in_parms->port), &regVal );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_PROBE(0x40);
       goto default_pc_probe_exit;
    };

    // NOTE: If ackIDs aren't in sync, querying the link partner's ackID 
    // will cause an output error-stopped condition and a PORT_FAIL condition 
    // on CPS Gen2 devices.
       
    rc = DARRegWrite( dev_info, RIO_SPX_LM_REQ(dev_info->extFPtrForPort, dev_info->extFPtrPortType, in_parms->port),
													RIO_SPX_LM_REQ_CMD_LR_IS);
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_PROBE(0x50);
       goto default_pc_probe_exit;
    };

    rc = DARRegRead( dev_info, RIO_SPX_LM_RESP(dev_info->extFPtrForPort, dev_info->extFPtrPortType, in_parms->port), &regVal2);
    if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_PROBE(0x51);
          goto default_pc_probe_exit;
    };
    if (!(regVal2 & RIO_SPX_LM_RESP_VLD)) {
        rc = RIO_ERR_SW_FAILURE;
        out_parms->imp_rc = PC_PROBE(0x52);
        goto default_pc_probe_exit;
    };

    if (((regVal2 & RIO_SPX_LM_RESP_ACK_ID3) >> 5) !=
         (regVal  & RIO_SPX_ACKID_ST_OUTB  )) {
       out_parms->imp_rc = PC_PROBE(0x70);
       out_parms->status = port_err;
       goto default_pc_probe_exit;
    };

default_pc_probe_exit:
    return rc;
}

uint32_t IDT_DSF_em_cfg_pw    ( DAR_DEV_INFO_t                *dev_info, 
                              idt_em_cfg_pw_in_t            *in_parms, 
                              idt_em_cfg_pw_out_t           *out_parms )
{
   NULL_CHECK;
   return RIO_STUBBED;
};

uint32_t IDT_DSF_em_cfg_set    ( DAR_DEV_INFO_t                *dev_info, 
                               idt_em_cfg_set_in_t           *in_parms, 
                               idt_em_cfg_set_out_t          *out_parms )
{
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t IDT_DSF_em_cfg_get    ( DAR_DEV_INFO_t                *dev_info, 
                               idt_em_cfg_get_in_t           *in_parms, 
                               idt_em_cfg_get_out_t          *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t IDT_DSF_em_dev_rpt_ctl (DAR_DEV_INFO_t                *dev_info, 
                               idt_em_dev_rpt_ctl_in_t        *in_parms, 
                               idt_em_dev_rpt_ctl_out_t       *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t IDT_DSF_em_parse_pw   ( DAR_DEV_INFO_t                *dev_info, 
                               idt_em_parse_pw_in_t          *in_parms, 
                               idt_em_parse_pw_out_t         *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t idt_sc_port_list( DAR_DEV_INFO_t *dev_info ,
		         uint32_t          start_rc ,
		         uint8_t           p_cnt_in ,
			 uint8_t          *p_list_in,
			 uint8_t          *p_cnt_out,
			 uint8_t          *p_list_out) 
{
   uint8_t idx;
   uint32_t rc = RIO_SUCCESS;
   bool dup_port_num[IDT_MAX_PORTS];

   if (RIO_ALL_PORTS == p_cnt_in) {
      *p_cnt_out = NUM_PORTS(dev_info);
      for (idx = 0; idx < NUM_PORTS(dev_info); idx++)
         p_list_out[idx] = idx;
   } else {
      for (idx = 0; idx < IDT_MAX_PORTS; idx++)
         dup_port_num[idx] = false;

      *p_cnt_out = p_cnt_in;
      for (idx = 0; idx < p_cnt_in; idx++) {
         p_list_out[idx] = p_list_in[idx];
	     if ((p_list_out[idx] >= NUM_PORTS(dev_info)) ||
	        (dup_port_num[p_list_in[idx]]          )) {
            rc = start_rc + idx;
	        break;
	     };
	     dup_port_num[p_list_in[idx]] = true;
      };
   };
   return rc;
};

uint32_t IDT_DSF_sc_init_dev_ctrs ( DAR_DEV_INFO_t                *dev_info, 
                                  idt_sc_init_dev_ctrs_in_t     *in_parms, 
                                  idt_sc_init_dev_ctrs_out_t    *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t IDT_DSF_sc_read_ctrs ( DAR_DEV_INFO_t         *dev_info, 
                              idt_sc_read_ctrs_in_t  *in_parms, 
                              idt_sc_read_ctrs_out_t *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

void add_int_event( idt_em_get_int_stat_in_t   *in_parms, 
                    idt_em_get_int_stat_out_t  *out_parms,
                    uint8_t                       pnum,
                    idt_em_events_t             event ) 
{
   if (out_parms->num_events < in_parms->num_events) {
      in_parms->events[out_parms->num_events].event    = event;
      in_parms->events[out_parms->num_events].port_num = pnum;
      out_parms->num_events++;
   } else {
      out_parms->too_many = true;
   };
}; 

uint32_t IDT_DSF_em_get_int_stat( DAR_DEV_INFO_t                *dev_info, 
                                idt_em_get_int_stat_in_t      *in_parms, 
                                idt_em_get_int_stat_out_t     *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

void add_pw_event( idt_em_get_pw_stat_in_t   *in_parms, 
                   idt_em_get_pw_stat_out_t  *out_parms,
                 uint8_t                      pnum,
                 idt_em_events_t            event ) 
{
   if (out_parms->num_events < in_parms->num_events) {
      in_parms->events[out_parms->num_events].event    = event;
      in_parms->events[out_parms->num_events].port_num = pnum;
      out_parms->num_events++;
   } else {
      out_parms->too_many = true;
   };
} 

uint32_t IDT_DSF_em_get_pw_stat( DAR_DEV_INFO_t                *dev_info, 
                               idt_em_get_pw_stat_in_t       *in_parms, 
                               idt_em_get_pw_stat_out_t      *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

uint32_t IDT_DSF_em_clr_events( DAR_DEV_INFO_t                 *dev_info, 
                              idt_em_clr_events_in_t         *in_parms, 
                              idt_em_clr_events_out_t        *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};
 
uint32_t IDT_DSF_em_create_events( DAR_DEV_INFO_t              *dev_info,
                                 idt_em_create_events_in_t   *in_parms, 
                                 idt_em_create_events_out_t  *out_parms )
{ 
    NULL_CHECK;
    return RIO_STUBBED;
};

void IDT_DSF_init_driver( IDT_DSF_DB_t *dsf) {
   dsf->dev_type = 0;
   dsf->idt_pc_get_config         = IDT_DSF_pc_get_config;
   dsf->idt_pc_set_config         = IDT_DSF_pc_set_config;
   dsf->idt_pc_get_status         = IDT_DSF_pc_get_status;
   dsf->idt_pc_reset_port         = IDT_DSF_pc_reset_port;
   dsf->idt_pc_reset_link_partner = IDT_DSF_pc_reset_link_partner;
   dsf->idt_pc_clr_errs           = IDT_DSF_pc_clr_errs;
   dsf->idt_pc_secure_port        = IDT_DSF_pc_secure_port;
   dsf->idt_pc_dev_reset_config   = IDT_DSF_pc_dev_reset_config;
   dsf->idt_pc_probe              = IDT_DSF_pc_probe;

   dsf->idt_rt_initialize        = IDT_DSF_rt_initialize;
   dsf->idt_rt_probe             = IDT_DSF_rt_probe;
   dsf->idt_rt_probe_all         = IDT_DSF_rt_probe_all;
   dsf->idt_rt_set_all           = IDT_DSF_rt_set_all;
   dsf->idt_rt_set_changed       = IDT_DSF_rt_set_changed;
   dsf->idt_rt_alloc_mc_mask     = IDT_DSF_rt_alloc_mc_mask;
   dsf->idt_rt_dealloc_mc_mask   = IDT_DSF_rt_dealloc_mc_mask;
   dsf->idt_rt_change_rte        = IDT_DSF_rt_change_rte;
   dsf->idt_rt_change_mc_mask    = IDT_DSF_rt_change_mc_mask;

   dsf->idt_em_cfg_pw             = IDT_DSF_em_cfg_pw;
   dsf->idt_em_cfg_set            = IDT_DSF_em_cfg_set;
   dsf->idt_em_cfg_get            = IDT_DSF_em_cfg_get;
   dsf->idt_em_dev_rpt_ctl        = IDT_DSF_em_dev_rpt_ctl;
   dsf->idt_em_parse_pw           = IDT_DSF_em_parse_pw;
   dsf->idt_em_get_int_stat       = IDT_DSF_em_get_int_stat;
   dsf->idt_em_get_pw_stat        = IDT_DSF_em_get_pw_stat;
   dsf->idt_em_clr_events         = IDT_DSF_em_clr_events;
   dsf->idt_em_create_events      = IDT_DSF_em_create_events;

   dsf->idt_sc_init_dev_ctrs      = IDT_DSF_sc_init_dev_ctrs;
   dsf->idt_sc_read_ctrs          = IDT_DSF_sc_read_ctrs;
}
   
void IDT_init_DSF_DB( void )
{
    int32_t idx;
    IDT_DSF_DB_t dsf;

    num_idt_drivers_in_use = 0;
    
    IDT_DSF_init_driver( &dsf );
    for ( idx = 0; idx < DAR_DB_MAX_DRIVERS; idx++ )
    {
	IDT_DB[idx] = dsf;
    }
}


uint32_t IDT_DSF_bind_driver( IDT_DSF_DB_t *dsf, uint32_t *dsf_index )
{
    uint32_t rc = DAR_DB_NO_HANDLES;

    if ( num_idt_drivers_in_use < DAR_DB_MAX_DRIVERS )
    {
	    IDT_DB[num_idt_drivers_in_use] = *dsf;

        /* Compose the DSF handle... */
        *dsf_index = ((uint32_t)(dsf->dev_type & 0x0000FFFF) << 16) 
                   + num_idt_drivers_in_use;

        num_idt_drivers_in_use++;
        rc = RIO_SUCCESS;
    }

    return rc;
}


uint32_t IDT_DSF_bind_DAR_routines( 
    uint32_t (*ReadRegCall )( DAR_DEV_INFO_t *dev_info, 
                                    uint32_t  offset, 
                                    uint32_t *readdata ),
    uint32_t (*WriteRegCall)( DAR_DEV_INFO_t *dev_info, 
                                    uint32_t  offset, 
                                    uint32_t  writedata ),
    void   (*WaitSecCall) ( uint32_t delay_nsec,
                            uint32_t delay_sec )
    )
{
    DARDB_init();
	IDT_init_DSF_DB();

#ifdef IDT_CPS_GEN2_DAR_WANTED
    bind_CPSGEN2_DAR_support();
    bind_CPSGEN2_DSF_support();
#endif

#ifdef IDT_CPS_GEN1_DAR_WANTED
    bind_CPSGEN1_DAR_support();
    bind_CPSGEN1_DSF_support();
#endif

#ifdef IDT_TSI57X_DAR_WANTED
    bind_tsi57x_DAR_support();
    bind_tsi57x_DSF_support();
#endif

#ifdef IDT_TSI721_DAR_WANTED
    bind_tsi721_DAR_support();
    bind_tsi721_DSF_support();
#endif

#ifdef IDT_RXSx_DAR_WANTED
    bind_rxs_DAR_support();
    bind_rxs_DSF_support();
#endif

    ReadReg  = ReadRegCall;
    WriteReg = WriteRegCall;
    WaitSec  = WaitSecCall;

    return RIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

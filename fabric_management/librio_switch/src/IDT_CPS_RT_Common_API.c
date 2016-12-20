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
#include <DAR_DB_Private.h>
#include <IDT_DSF_DB_Private.h>
#include <IDT_Routing_Table_Config_API.h>
#include <IDT_CPS_Common_Test.h>
#include <CPS1848_registers.h>
#include <CPS1616_registers.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_CPS_MC_MASKS(x) ((NUM_MC_MASKS(x) > CPS_MAX_MC_MASKS)? \
			CPS_MAX_MC_MASKS : NUM_MC_MASKS(x)) 

#define CPS_RT_USE_DEVICE_TABLE                 (0x000000DD)
#define CPS_RT_USE_DEFAULT_ROUTE                (0x000000DE)
#define CPS_RT_NO_ROUTE                         (0x000000DF)

#define CPS_FIRST_MC_MASK                       (0x00000040)
#define CPS_MAX_MC_MASK                         (0x00000028)
#define CPS_LAST_MC_MASK                        (0x00000067)

#define CPS_RTE_PT_0                            (0x00000000)
#define CPS_RTE_PT_LAST                         (0x00000012)

#define RTE_SET_COMMON_0      (RT_FIRST_SUBROUTINE_0+0x0100) // 0x100100
#define PROGRAM_RTE_ENTRIES_0 (RT_FIRST_SUBROUTINE_0+0x1900)
#define PROGRAM_MC_MASKS_0    (RT_FIRST_SUBROUTINE_0+0x1A00)
#define READ_MC_MASKS_0       (RT_FIRST_SUBROUTINE_0+0x1B00)
#define READ_RTE_ENTRIES_0    (RT_FIRST_SUBROUTINE_0+0x1C00)

// CPS Gen1 and Gen2 switches have exactly the same Routing Table programming
// model.  The routing table routines are therefore implemented in this 
// file for both switch families.
//
uint32_t rt_rte_translate_std_to_CPS(DAR_DEV_INFO_t *dev_info,
				uint32_t std_in, uint32_t *cps_out) 
{

	switch(std_in) {
	case RIO_RTE_DFLT_PORT: *cps_out = CPS_RT_USE_DEFAULT_ROUTE;
				goto success;
 	case RIO_RTE_DROP: *cps_out = CPS_RT_NO_ROUTE;
				goto success;
	case RIO_RTE_LVL_G0: *cps_out = CPS_RT_USE_DEVICE_TABLE;
				goto success;
	default:
		if (RIO_RTV_IS_PORT(std_in)) {
			if (RIO_RTV_GET_PORT(std_in) < NUM_PORTS(dev_info)) {
				*cps_out = std_in;
				goto success;
			}
		}
		if (RIO_RTV_IS_MC_MSK(std_in)) {
			if (RIO_RTV_GET_MC_MSK(std_in) < IDT_CPS_MAX_MC_MASK) {
				*cps_out =
					CPS_MC_PORT(RIO_RTV_GET_MC_MSK(std_in));
				goto success;
			}
		}
		break;
	}
	return RIO_ERR_INVALID_PARAMETER;

success:
	return RIO_SUCCESS;
}

uint32_t rt_rte_translate_CPS_to_std(DAR_DEV_INFO_t *dev_info,
				uint32_t cps_in, uint32_t *std_out)
{
	switch(cps_in) {
	case CPS_RT_USE_DEFAULT_ROUTE: *std_out = RIO_RTE_DFLT_PORT;
				goto success;
 	case CPS_RT_NO_ROUTE: *std_out = RIO_RTE_DROP;
				goto success;
	case CPS_RT_USE_DEVICE_TABLE: *std_out = RIO_RTE_LVL_G0;
				goto success;
	default:
		if (cps_in < NUM_PORTS(dev_info)) {
			*std_out = RIO_RTV_PORT(cps_in);
			goto success;
		}
		if (IS_CPS_MC_PORT(cps_in)) {
			*std_out = RIO_RTV_MC_MSK(IS_CPS_MC_MASK_NO(cps_in));
			goto success;
		};
		break;
	}
	return RIO_ERR_INVALID_PARAMETER;

success:
	return RIO_SUCCESS;
}

/* initializes the routing table hardware and/or routing table state structure.
*/

uint32_t IDT_CPS_rt_set_changed (
    DAR_DEV_INFO_t            *dev_info, 
    idt_rt_set_changed_in_t   *in_parms, 
    idt_rt_set_changed_out_t  *out_parms
); 

uint32_t IDT_CPS_rt_initialize(
    DAR_DEV_INFO_t           *dev_info,
    idt_rt_initialize_in_t   *in_parms,
    idt_rt_initialize_out_t  *out_parms
) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t destID;
    uint32_t mc_idx;
    idt_rt_set_changed_in_t  all_in;
    idt_rt_set_changed_out_t all_out;
    idt_rt_state_t           rt_state; 

    // Validate parameters
    
    if (  (in_parms->default_route      >= NUM_CPS_PORTS(dev_info))  &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->default_route)  )
    {
        out_parms->imp_rc = RT_INITIALIZE(1);
        goto idt_CPS_rt_initialize_exit;
    }

    if ( (in_parms->default_route_table_port >= NUM_CPS_PORTS(dev_info)) &&
         !( (IDT_DSF_RT_USE_DEFAULT_ROUTE == in_parms->default_route_table_port) ||
            (IDT_DSF_RT_NO_ROUTE          == in_parms->default_route_table_port)) )
    {
        out_parms->imp_rc = RT_INITIALIZE(2);
        goto idt_CPS_rt_initialize_exit;
    }

    if ( (in_parms->set_on_port >= NUM_CPS_PORTS(dev_info)  )  &&
        !(RIO_ALL_PORTS         == in_parms->set_on_port))
    {
        out_parms->imp_rc = RT_INITIALIZE(3);
        goto idt_CPS_rt_initialize_exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    all_in.set_on_port = in_parms->set_on_port;

    if (!in_parms->rt)
       all_in.rt = &rt_state;
    else
       all_in.rt = in_parms->rt;

    all_in.rt->default_route = in_parms->default_route;

    // Configure initialization of all of the routing table entries
    for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
    {
        all_in.rt->dev_table[destID].changed = true ;
        all_in.rt->dev_table[destID].rte_val = in_parms->default_route_table_port;
    };
    
    all_in.rt->dom_table[0].changed = true ;
    all_in.rt->dom_table[0].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;

    for (destID = 1; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
    {
        all_in.rt->dom_table[destID].changed = true ;
        all_in.rt->dom_table[destID].rte_val = in_parms->default_route_table_port;
    };
    
    // Configure initialization of multicast masks and associations as necessary. 
    for (mc_idx = 0; mc_idx < IDT_DSF_MAX_MC_MASK; mc_idx++) 
    {
       all_in.rt->mc_masks[mc_idx].mc_destID = 0;
       all_in.rt->mc_masks[mc_idx].tt        = tt_dev8;
       all_in.rt->mc_masks[mc_idx].mc_mask   = 0;
       all_in.rt->mc_masks[mc_idx].in_use    = false;
       all_in.rt->mc_masks[mc_idx].allocd    = false;
       if ((mc_idx < CPS_MAX_MC_MASKS) && (mc_idx < IDT_DSF_MAX_MC_MASK)) {
          all_in.rt->mc_masks[mc_idx].changed   = true ;
       } else {
          all_in.rt->mc_masks[mc_idx].changed   = false ;
       };
    };

    if (in_parms->update_hw) {
       uint8_t port, start_port, end_port;
       uint32_t ops;

       if (RIO_ALL_PORTS == in_parms->set_on_port ) {
          start_port = 0;
          end_port   = NUM_CPS_PORTS(dev_info) - 1;
       } else {
          start_port = end_port = in_parms->set_on_port;
       };

       /* Clear self-association MC bit */
       for (port = start_port; port <= end_port; port++) {
          rc = DARRegRead( dev_info, CPS1848_PORT_X_OPS(port), &ops );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = RT_INITIALIZE(4);
             goto idt_CPS_rt_initialize_exit;
          };

          /* Self association bit off */
          ops &= ~CPS1848_PORT_X_OPS_SELF_MCAST_EN;
   
          rc = DARRegWrite( dev_info, CPS1848_PORT_X_OPS(port), ops ) ;
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = RT_INITIALIZE(5);
             goto idt_CPS_rt_initialize_exit;
          };
       };
       rc = IDT_CPS_rt_set_changed(dev_info, &all_in, &all_out );
    } else {
       rc = RIO_SUCCESS;
    }
     
    if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = all_out.imp_rc;
    }

    idt_CPS_rt_initialize_exit:

    return rc;
}   

uint32_t idt_CPS_check_port_for_discard( DAR_DEV_INFO_t     *dev_info, 
                                       idt_rt_probe_in_t  *in_parms, 
                                       idt_rt_probe_out_t *out_parms ) 
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t ctlData;
   uint8_t  port;
   bool  dflt_rt = (IDT_DSF_RT_USE_DEFAULT_ROUTE == out_parms->routing_table_value)?true:false;
   idt_pc_get_config_in_t  cfg_in;
   idt_pc_get_config_out_t cfg_out;
   idt_pc_get_status_in_t  stat_in;
   idt_pc_get_status_out_t stat_out;

   port = (dflt_rt)?in_parms->rt->default_route:out_parms->routing_table_value;

   if (NUM_CPS_PORTS(dev_info) <= port) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(1);
      goto idt_CPS_check_port_for_discard_exit;
   };

   cfg_in.ptl.num_ports = 1;
   cfg_in.ptl.pnums[0] = port;
   rc = IDT_CPS_pc_get_config( dev_info, &cfg_in, &cfg_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(2);
      goto idt_CPS_check_port_for_discard_exit;
   };

   stat_in.ptl.num_ports = 1;
   stat_in.ptl.pnums[0] = port;
   rc = IDT_CPS_pc_get_status( dev_info, &stat_in, &stat_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(3);
      goto idt_CPS_check_port_for_discard_exit;
   };

   if (!cfg_out.pc[0].port_available) {
      out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_unavail:idt_rt_disc_port_unavail;
   } else { 
      if (!cfg_out.pc[0].powered_up) {
         out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_pwdn:idt_rt_disc_port_pwdn;
      } else {
          if (!stat_out.ps[0].port_ok) {
             if (cfg_out.pc[0].xmitter_disable) {
                out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_lkout_or_dis:
		                                      idt_rt_disc_port_lkout_or_dis;
             } else {
                out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_no_lp:idt_rt_disc_port_no_lp;
	     };
	  } else {
             if (stat_out.ps[0].port_error) {
                out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_fail:idt_rt_disc_port_fail;
             } else {
                if (cfg_out.pc[0].port_lockout) {
                   out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_lkout_or_dis:
			                                 idt_rt_disc_port_lkout_or_dis;
		} else {
                   rc = DARRegRead( dev_info, CPS1848_PORT_X_CTL_1_CSR(port), &ctlData );
                   if (RIO_SUCCESS != rc) {
                      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
                      out_parms->imp_rc = RT_PROBE(4);
                      goto idt_CPS_check_port_for_discard_exit;
                   };
            
                  if ( (RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) != 
                      ((RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) & ctlData)) {
                     out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_in_out_dis:idt_rt_disc_port_in_out_dis;
                  };
               }
            }
         }
      }
   }

   rc = RIO_SUCCESS;

idt_CPS_check_port_for_discard_exit:

    if (idt_rt_disc_not != out_parms->reason_for_discard)
       out_parms->valid_route = false;

    return rc;
}

/* This function probes the hardware status of a routing table entry for 
 *   the specified port and destination ID
*/
uint32_t IDT_CPS_rt_probe(
    DAR_DEV_INFO_t      *dev_info,
    idt_rt_probe_in_t   *in_parms,
    idt_rt_probe_out_t  *out_parms
)
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t bit;
    uint32_t regVal;

    out_parms->imp_rc                 = RIO_SUCCESS;
    out_parms->valid_route            = false;
    out_parms->routing_table_value    = RIO_ALL_PORTS;
    for (bit = 0; bit < NUM_CPS_PORTS(dev_info); bit++)
        out_parms->mcast_ports[bit] = false;
    out_parms->reason_for_discard     = idt_rt_disc_probe_abort;

    if (   ((NUM_CPS_PORTS(dev_info) <= in_parms->probe_on_port) &&
            (RIO_ALL_PORTS       != in_parms->probe_on_port))  ||
           ( !in_parms->rt           ) ) {
       out_parms->imp_rc = RT_PROBE(0x11);
       goto idt_CPS_rt_probe_exit;
    }
        
    rc = DARRegRead( dev_info, CPS1848_PKT_TTL_CSR, &regVal ) ;
    if ( RIO_SUCCESS != rc ) {
       out_parms->imp_rc = RT_PROBE(0x12);
       goto idt_CPS_rt_probe_exit;
    };
    out_parms->time_to_live_active = (regVal & CPS1848_PKT_TTL_CSR_TTL)?true:false;

    rc = DARRegRead( dev_info, CPS1848_DEVICE_CTL_1, &regVal ) ;
    if ( RIO_SUCCESS != rc ) {
       out_parms->imp_rc = RT_PROBE(0x13);
       goto idt_CPS_rt_probe_exit;
    };
    out_parms->trace_function_active  = (regVal & CPS1848_DEVICE_CTL_1_TRACE_EN)?true:false;
    out_parms->filter_function_active = false;

    if (RIO_ALL_PORTS != in_parms->probe_on_port) {
       rc = DARRegRead( dev_info, CPS1848_PORT_X_OPS(in_parms->probe_on_port), &regVal ) ;
       if ( RIO_SUCCESS != rc ) {
          out_parms->imp_rc = RT_PROBE(0x14);
          goto idt_CPS_rt_probe_exit;
       };

       out_parms->trace_function_active  = (regVal & CPS1848_PORT_X_OPS_ANY_TRACE )?true:false;
       out_parms->filter_function_active = (regVal & CPS1848_PORT_X_OPS_ANY_FILTER)?true:false;
    };

    rc = RIO_SUCCESS;

    // Note, no failure possible...
    check_multicast_routing( dev_info, in_parms, out_parms );

    /* Done if hit in multicast masks. */
    if (RIO_ALL_PORTS != out_parms->routing_table_value) 
       goto idt_CPS_rt_probe_exit;

    /*  Determine routing table value for the specified destination ID.
     *  If out_parms->valid_route is true 
     *  the valid values for out_parms->routing_table_value are
     *  - a valid port number, OR
     *  - IDT_DSF_RT_USE_DEFAULT_ROUTE
     *  When out_parms->routing_table_value is IDT_DSF_RT_USE_DEFAULT_ROUTE, the
     *  default route is a valid switch port number.
     */

    check_unicast_routing( dev_info, in_parms, out_parms );

    if (out_parms->valid_route) {
       rc = idt_CPS_check_port_for_discard( dev_info, in_parms, out_parms );
    }
    
idt_CPS_rt_probe_exit:
    return rc;
}   

#define READ_MC_MASKS(x) (READ_MC_MASKS_0+x)

uint32_t read_mc_masks( DAR_DEV_INFO_t            *dev_info, 
		      uint8_t                      pnum,
                      idt_rt_state_t            *rt,
                      uint32_t                    *imp_rc  )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t mask_idx;
   uint32_t reg_val, port_mask = ((uint32_t)(1) << NUM_CPS_PORTS(dev_info)) - 1;
   idt_rt_dealloc_mc_mask_in_t  d_in_parm;
   idt_rt_dealloc_mc_mask_out_t d_out_parm;

   d_in_parm.rt = rt;
   for (mask_idx = NUM_CPS_MC_MASKS(dev_info); mask_idx < IDT_DSF_MAX_MC_MASK; mask_idx++ ) 
   {
      d_in_parm.mc_mask_rte = IDT_DSF_FIRST_MC_MASK + mask_idx;
      rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &d_in_parm, &d_out_parm );
      if (RIO_SUCCESS != rc) 
      {
         *imp_rc = d_out_parm.imp_rc;
         goto read_mc_masks_exit;
      };
   };

   for (mask_idx = 0; mask_idx < NUM_CPS_MC_MASKS(dev_info); mask_idx++)  {
      rc = DARRegRead( dev_info, CPS1848_PORT_X_MCAST_MASK_Y(pnum, mask_idx), &reg_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_MC_MASKS(0x01);
         goto read_mc_masks_exit;
      };

      rt->mc_masks[mask_idx].allocd    = false;
      rt->mc_masks[mask_idx].changed   = false;
      rt->mc_masks[mask_idx].tt        = tt_dev8;
      rt->mc_masks[mask_idx].in_use    = false;
      rt->mc_masks[mask_idx].mc_destID = 0x0;
      rt->mc_masks[mask_idx].mc_mask   = reg_val & port_mask;
   };

read_mc_masks_exit:
   return rc;
}

#define READ_RTE_ENTRIES(x) (READ_RTE_ENTRIES_0+x)

uint32_t read_rte_entries( DAR_DEV_INFO_t            *dev_info,
                         uint8_t                      pnum,
                         idt_rt_state_t            *rt,
                         uint32_t                    *imp_rc  )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t destID, rte_val, first_mc_destID;
   bool  found_one = false;

   // Fill in default route value

   rc = DARRegRead( dev_info, CPS1848_RTE_DEFAULT_PORT_CSR, &rte_val );
   if (RIO_SUCCESS != rc) {
      *imp_rc = READ_RTE_ENTRIES(0x01);
      goto read_rte_entries_exit;
   };

   rt->default_route = (uint8_t)(rte_val & CPS1848_RTE_DEFAULT_PORT_CSR_DEFAULT_PORT);
   if ( rt->default_route >= NUM_CPS_PORTS(dev_info)) {
      rt->default_route = IDT_DSF_RT_NO_ROUTE;
   }

   // Read all of the domain routing table entries.
   //
   // CPS Programming model assumes that device IDs of the form 0x00yy are
   // routed using the device routing table.  This is accomplished by ensuring
   // that the RIO_DOMAIN register is always 0.  Note that RIO_DOMAIN is a 
   // global register, affecting routing on all ports.

   rt->dom_table[0].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;
   rt->dom_table[0].changed = false;
   first_mc_destID = 0;

   for (destID = 1; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
   {
      rt->dom_table[destID].changed = false;

      // Read routing table entry for deviceID
      rc = DARRegRead( dev_info, CPS1848_PORT_X_DOM_RTE_TABLE_Y(pnum, destID), &rte_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(0x04);
         goto read_rte_entries_exit;
      }

      rte_val &= CPS1848_PORT_X_DOM_RTE_TABLE_Y_PORT;
      rc = rt_rte_translate_CPS_to_std(dev_info, rte_val, &rte_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(0x05);
         goto read_rte_entries_exit;
      }
      rt->dom_table[destID].rte_val = (uint32_t)(rte_val);

      if (IDT_DSF_RT_USE_DEVICE_TABLE == rte_val) {
	 if (!found_one) {
            first_mc_destID = (uint16_t)(destID) << 8;
	    found_one = true;
	 };
      } else {
	 if ((IDT_DSF_RT_USE_DEFAULT_ROUTE != rte_val) &&
             (IDT_DSF_RT_NO_ROUTE          != rte_val) &&
	     (NUM_CPS_PORTS(dev_info)          <= rte_val) ) { 
            rt->dom_table[destID].rte_val = IDT_DSF_RT_NO_ROUTE;
	 };
      };
   };
   
   // Read all of the device routing table entries.
   // Update multicast entries as we go...
   //
   for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
   {
      uint32_t mask_idx;

      rt->dev_table[destID].changed = false;

      rc = DARRegRead( dev_info, CPS1848_PORT_X_DEV_RTE_TABLE_Y(pnum, destID), &rte_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(8);
         goto read_rte_entries_exit;
      }

      rte_val &= CPS1848_PORT_X_DEV_RTE_TABLE_Y_PORT;
      rc = rt_rte_translate_CPS_to_std(dev_info, rte_val, &rte_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(0x09);
         goto read_rte_entries_exit;
      }
      rt->dev_table[destID].rte_val = (uint32_t)(rte_val);

      mask_idx = MC_MASK_IDX_FROM_ROUTE(rte_val);
      if ((IDT_DSF_BAD_MC_MASK != mask_idx) && !(rt->mc_masks[mask_idx].in_use)) {
         rt->mc_masks[mask_idx].tt        = tt_dev16;
         rt->mc_masks[mask_idx].in_use    = true;
         rt->mc_masks[mask_idx].mc_destID = first_mc_destID + destID;
      };
         
      if (  ((rte_val >= NUM_CPS_PORTS(dev_info)) && (rte_val < IDT_DSF_FIRST_MC_MASK))        ||
	    ((rte_val >= IDT_DSF_BAD_MC_MASK) && (IDT_DSF_RT_NO_ROUTE          != rte_val) 
	                                      && (IDT_DSF_RT_USE_DEFAULT_ROUTE != rte_val)) ) {
         rt->dev_table[destID].rte_val = IDT_DSF_RT_NO_ROUTE;
      };
   };
   
read_rte_entries_exit:
   return rc;
}

/* This function returns the complete hardware state of packet routing
 * in a routing table state structure.
 *
 * The routing table hardware must be initialized using idt_rt_initialize() 
 *    before calling this routine.
*/
uint32_t IDT_CPS_rt_probe_all(
    DAR_DEV_INFO_t          *dev_info,
    idt_rt_probe_all_in_t   *in_parms,
    idt_rt_probe_all_out_t  *out_parms
) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  probe_port;

    out_parms->imp_rc = RIO_SUCCESS;
    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->probe_on_port ) && 
           ( in_parms->probe_on_port >= NUM_CPS_PORTS(dev_info)    ) ) ||
         ( !in_parms->rt) ) 
    {
        out_parms->imp_rc = RT_PROBE_ALL(1);
        goto idt_CPS_rt_probe_all_exit;
    }

    probe_port = (RIO_ALL_PORTS == in_parms->probe_on_port)?0:in_parms->probe_on_port;

    rc = read_mc_masks( dev_info, probe_port, in_parms->rt, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc)
       goto idt_CPS_rt_probe_all_exit;
    
    rc = read_rte_entries( dev_info, probe_port, in_parms->rt, &out_parms->imp_rc );

idt_CPS_rt_probe_all_exit:
    return rc;
}

#define MC_MASK_ADDR(b,m) (b+(4*m))
#define PROGRAM_MC_MASKS(x) (PROGRAM_MC_MASKS_0+x)

#define SET_ALL     true
#define SET_CHANGED false

uint32_t CPS_program_mc_masks ( DAR_DEV_INFO_t        *dev_info, 
                              idt_rt_set_all_in_t   *in_parms, 
			      bool                   set_all, // true if all entries should be set
	                      uint32_t                *imp_rc )
{
    uint32_t rc = RIO_SUCCESS;
    // Note that the base address for CPS1848, CPS1432, CPS1616, SPS1616
    // are all the same.
    uint32_t mask_num;
    uint32_t base_addr, mask_mask;

    if (IDT_CPS1848_DEV_ID == DEV_CODE(dev_info)) {
       mask_mask = CPS1848_BCAST__MCAST_MASK_X_PORT_MASK;
    } else {
       mask_mask = CPS1616_BCAST__MCAST_MASK_X_PORT_MASK;
    };

    if (RIO_ALL_PORTS == in_parms->set_on_port) {
       base_addr = CPS1848_BCAST__MCAST_MASK_X(0);
    } else {
       base_addr = CPS1848_PORT_X_MCAST_MASK_Y(in_parms->set_on_port,0);
    };

    for (mask_num = 0; mask_num < NUM_CPS_MC_MASKS(dev_info); mask_num++) {
       if (in_parms->rt->mc_masks[mask_num].changed || set_all) {
	  if ( in_parms->rt->mc_masks[mask_num].mc_mask & ~mask_mask ) {
             rc = RIO_ERR_INVALID_PARAMETER;
             *imp_rc = PROGRAM_MC_MASKS(3);
             goto CPS_program_mc_masks_exit; 
          };
          rc = DARRegWrite( dev_info, MC_MASK_ADDR(base_addr, mask_num), 
                            in_parms->rt->mc_masks[mask_num].mc_mask & mask_mask );
          if (RIO_SUCCESS != rc) {
             *imp_rc = PROGRAM_MC_MASKS(4);
             goto CPS_program_mc_masks_exit; 
          };
	  in_parms->rt->mc_masks[mask_num].changed = false;
       };
    };
        
CPS_program_mc_masks_exit: 
    return rc;
};

#define DEV_RTE_ADDR(b,n) (b+(4*n))
#define DOM_RTE_ADDR(b,n) (b+(4*n))
#define PROGRAM_RTE_ENTRIES(x) (PROGRAM_RTE_ENTRIES_0+x)

uint32_t CPS_program_rte_entries ( DAR_DEV_INFO_t        *dev_info, 
                                 idt_rt_set_all_in_t   *in_parms, 
			         bool                   set_all, // true if all entries should be set
	                         uint32_t                *imp_rc ) 
{
    uint32_t rc = RIO_SUCCESS;
    // Note that the base address for CPS1848, CPS1432, CPS1616, SPS1616
    // are all the same.
    uint16_t rte_num;
    uint32_t dev_rte_base, dom_rte_base, cps_val;

    rc = rt_rte_translate_std_to_CPS(dev_info, in_parms->rt->default_route,
						&cps_val);
    if (RIO_SUCCESS != rc) {
        *imp_rc = PROGRAM_RTE_ENTRIES(0x06);
        goto CPS_program_rte_entries_exit;
    }
    rc = DARRegWrite(dev_info, CPS1848_RTE_DEFAULT_PORT_CSR, cps_val);
    if (RIO_SUCCESS != rc) {
       *imp_rc = PROGRAM_RTE_ENTRIES(0x10);
       goto CPS_program_rte_entries_exit; 
    };

    if (RIO_ALL_PORTS == in_parms->set_on_port) {
       dev_rte_base = CPS1848_BCAST_DEV_RTE_TABLE_X(0);
       dom_rte_base = CPS1848_BCAST_DOM_RTE_TABLE_X(0);
    } else {
       dev_rte_base = CPS1848_PORT_X_DEV_RTE_TABLE_Y(in_parms->set_on_port, 0);
       dom_rte_base = CPS1848_PORT_X_DOM_RTE_TABLE_Y(in_parms->set_on_port, 0);
    };

    // DOMAIN REGISTER MUST ALWAYS BE 0
    // THIS MAKES 16 BIT DESTIDS OF THE FORM 0x00YY
    // EQUIVALENT TO 8 BIT DESTID ROUTING
    
    rc = DARRegWrite( dev_info, CPS1848_RIO_DOMAIN, 0 );
    if (RIO_SUCCESS != rc) {
       *imp_rc = PROGRAM_RTE_ENTRIES(0);
       goto CPS_program_rte_entries_exit; 
    }; 
    
    for (rte_num = 0; rte_num < IDT_DAR_RT_DOM_TABLE_SIZE; rte_num++) {
       if (in_parms->rt->dom_table[rte_num].changed || set_all) {
	  // Validate value to be programmed.
	  if (in_parms->rt->dom_table[rte_num].rte_val >= NUM_CPS_PORTS(dev_info)) {
	     // Domain table can be a port number, use device table, use default route, or drop.
	     if ((in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_USE_DEVICE_TABLE ) &&
	         (in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_USE_DEFAULT_ROUTE) &&
	         (in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_NO_ROUTE         )) {
                rc = RIO_ERR_INVALID_PARAMETER;
                *imp_rc = PROGRAM_RTE_ENTRIES(1);
                goto CPS_program_rte_entries_exit; 
             };
          };
          rc = rt_rte_translate_std_to_CPS(dev_info,
			in_parms->rt->dom_table[rte_num].rte_val, &cps_val);
          if (RIO_SUCCESS != rc) {
             *imp_rc = PROGRAM_RTE_ENTRIES(0x07);
             goto CPS_program_rte_entries_exit;
          }

          rc = DARRegWrite( dev_info, DOM_RTE_ADDR(dom_rte_base, rte_num),
								cps_val);
          if (RIO_SUCCESS != rc) {
             *imp_rc = PROGRAM_RTE_ENTRIES(2);
             goto CPS_program_rte_entries_exit; 
          };
	  in_parms->rt->dom_table[rte_num].changed = false;
       };
    };
        
    for (rte_num = 0; rte_num < IDT_DAR_RT_DEV_TABLE_SIZE; rte_num++) {
       if (in_parms->rt->dev_table[rte_num].changed || set_all) {
	  // Validate value to be programmed.
	  if (in_parms->rt->dev_table[rte_num].rte_val >= NUM_CPS_PORTS(dev_info)) {
	     // Device table can be a port number, a multicast mask, use default route, or drop.
	     if ((MC_MASK_IDX_FROM_ROUTE(in_parms->rt->dev_table[rte_num].rte_val) 
				                           == IDT_DSF_BAD_MC_MASK         ) &&
	         (in_parms->rt->dev_table[rte_num].rte_val != IDT_DSF_RT_USE_DEFAULT_ROUTE) &&
	         (in_parms->rt->dev_table[rte_num].rte_val != IDT_DSF_RT_NO_ROUTE         )) {
                rc = RIO_ERR_INVALID_PARAMETER;
                *imp_rc = PROGRAM_RTE_ENTRIES(3);
                goto CPS_program_rte_entries_exit; 
             };
          };
          rc = rt_rte_translate_std_to_CPS(dev_info,
			in_parms->rt->dev_table[rte_num].rte_val, &cps_val);
          if (RIO_SUCCESS != rc) {
             *imp_rc = PROGRAM_RTE_ENTRIES(0x08);
             goto CPS_program_rte_entries_exit;
          }

          rc = DARRegWrite( dev_info, DEV_RTE_ADDR(dev_rte_base, rte_num),
								cps_val);
          if (RIO_SUCCESS != rc) {
             *imp_rc = PROGRAM_RTE_ENTRIES(4);
             goto CPS_program_rte_entries_exit; 
          };
          in_parms->rt->dev_table[rte_num].changed = false;
       };
    };
        
CPS_program_rte_entries_exit: 
    return rc;
};

#define RTE_SET_COMMON(x) (RTE_SET_COMMON_0+x)

uint32_t idt_CPS_rt_set_common( DAR_DEV_INFO_t        *dev_info, 
                              idt_rt_set_all_in_t   *in_parms, 
                              idt_rt_set_all_out_t  *out_parms,
			      bool                   set_all  ) // true if all entries should be set
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;

    out_parms->imp_rc = RIO_SUCCESS;

    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->set_on_port ) && 
           ( in_parms->set_on_port >= NUM_CPS_PORTS(dev_info)    ) ) ||
         ( !in_parms->rt) ) 
    {
        out_parms->imp_rc = RTE_SET_COMMON(1);
        goto idt_CPS_rt_set_common_exit;
    }

    if ((NUM_CPS_PORTS(dev_info) <= in_parms->rt->default_route) &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->rt->default_route))   {
        out_parms->imp_rc = RTE_SET_COMMON(2);
        goto idt_CPS_rt_set_common_exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    rc = CPS_program_mc_masks( dev_info, in_parms, set_all, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_CPS_rt_set_common_exit;
    }

    rc = CPS_program_rte_entries( dev_info, in_parms, set_all, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) 
       goto idt_CPS_rt_set_common_exit;

idt_CPS_rt_set_common_exit:

    return rc;
};

      
/* This function sets the routing table hardware to match every entry
 *    in the routing table state structure. 
 * After idt_rt_set_all is called, no entries are marked as changed in
 *    the routing table state structure.
*/
uint32_t IDT_CPS_rt_set_all ( DAR_DEV_INFO_t        *dev_info, 
                            idt_rt_set_all_in_t   *in_parms, 
                            idt_rt_set_all_out_t  *out_parms)
{
    return idt_CPS_rt_set_common(dev_info, in_parms, out_parms, SET_ALL);
}

/* This function sets the the routing table hardware to match every entry
 *    that has been changed in the routing table state structure. 
 * Changes must be made using idt_rt_alloc_mc_mask, idt_rt_deallocate_mc_mask,
 *    idt_rt_change_rte, and idt_rt_change_mc.
 * After idt_rt_set_changed is called, no entries are marked as changed in
 *    the routing table state structure.
*/
uint32_t IDT_CPS_rt_set_changed (
    DAR_DEV_INFO_t            *dev_info, 
    idt_rt_set_changed_in_t   *in_parms, 
    idt_rt_set_changed_out_t  *out_parms
) 
{
    return idt_CPS_rt_set_common(dev_info, in_parms, out_parms, SET_CHANGED);
}

/* This function updates an idt_rt_state_t structure to
 * change a routing table entry, and tracks changes.
 */
uint32_t IDT_CPS_rt_change_rte(
    DAR_DEV_INFO_t           *dev_info, 
    idt_rt_change_rte_in_t   *in_parms, 
    idt_rt_change_rte_out_t  *out_parms
) 
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;

   out_parms->imp_rc = RIO_SUCCESS;

   if (!in_parms->rt) {
      out_parms->imp_rc = RT_CHANGE_RTE(1);
      goto idt_CPS_rt_change_rte_exit;
   };

   // Validate rte_value 
   if ( (IDT_DSF_RT_USE_DEVICE_TABLE  != in_parms->rte_value) &&
        (IDT_DSF_RT_USE_DEFAULT_ROUTE != in_parms->rte_value) &&
        (IDT_DSF_RT_NO_ROUTE          != in_parms->rte_value) &&
        (in_parms->rte_value >= NUM_CPS_PORTS(dev_info))) {
      out_parms->imp_rc = RT_CHANGE_RTE(2);
      goto idt_CPS_rt_change_rte_exit;
   }

   if ( (IDT_DSF_RT_USE_DEVICE_TABLE  == in_parms->rte_value) && (!in_parms->dom_entry)) {
      out_parms->imp_rc = RT_CHANGE_RTE(3);
      goto idt_CPS_rt_change_rte_exit;
   };

   rc = RIO_SUCCESS;

   // Do not allow any changes to index 0 of the domain table.
   // This must be set to "IDT_DSF_RT_USE_DEVICE_TABLE" at all times,
   // as this is the behavior required by the CPS RIO Domain register.
   
   if (in_parms->dom_entry && !in_parms->idx)
      goto idt_CPS_rt_change_rte_exit;

   // If the entry has not already been changed, see if it is being changed
   if (in_parms->dom_entry) {
      if (  !in_parms->rt->dom_table[in_parms->idx].changed ) {
         if (in_parms->rt->dom_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dom_table[in_parms->idx].changed = true;
      };
      in_parms->rt->dom_table[in_parms->idx].rte_val = in_parms->rte_value;
   } else {
      if (  !in_parms->rt->dev_table[in_parms->idx].changed ) {
         if (in_parms->rt->dev_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dev_table[in_parms->idx].changed = true;
      };
      in_parms->rt->dev_table[in_parms->idx].rte_val = in_parms->rte_value;
   };

idt_CPS_rt_change_rte_exit:
   return rc;
}

// Make sure that we're not orphaning a multicast mask...

uint32_t tidy_routing_table( DAR_DEV_INFO_t *dev_info, 
		           uint8_t           dev_idx,
			   idt_rt_state_t *rt     ,
			   uint32_t         *fail_pt) 
{
   uint32_t rc = RIO_SUCCESS;
   uint16_t srch_idx;
   bool   found_one = false;

   if ((rt->dev_table[dev_idx].rte_val >= IDT_DSF_FIRST_MC_MASK) && 
       (rt->dev_table[dev_idx].rte_val <  IDT_DSF_BAD_MC_MASK  )) {
      for (srch_idx = 0; (srch_idx < IDT_DAR_RT_DEV_TABLE_SIZE) && !found_one; srch_idx++) {
	 if (dev_idx == srch_idx)
            continue;
         if (rt->dev_table[dev_idx].rte_val == rt->dev_table[srch_idx].rte_val)
            found_one = true;
      };

      if (!found_one) {
         idt_rt_dealloc_mc_mask_in_t  in_parms;
         idt_rt_dealloc_mc_mask_out_t out_parms;
	 in_parms.rt = rt;
	 in_parms.mc_mask_rte = rt->dev_table[dev_idx].rte_val;
         rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &in_parms, &out_parms );
	 if (RIO_SUCCESS != rc) {
	    *fail_pt = out_parms.imp_rc;
	 };
      };
   };
   return rc;
};

/* This function updates an idt_rt_state_t structure to
 * change a multicast mask value, and tracks changes.
 */
uint32_t IDT_CPS_rt_change_mc_mask(
    DAR_DEV_INFO_t               *dev_info, 
    idt_rt_change_mc_mask_in_t   *in_parms, 
    idt_rt_change_mc_mask_out_t  *out_parms
)
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  chg_idx, dom_idx, dev_idx;
   uint32_t illegal_ports   = ~((1 << IDT_MAX_PORTS      ) - 1);
   uint32_t avail_ports     =   (1 << NUM_CPS_PORTS(dev_info)) - 1;

   out_parms->imp_rc = RIO_SUCCESS;

   if (!in_parms->rt) {
      out_parms->imp_rc = CHANGE_MC_MASK(1);
      goto idt_CPS_rt_change_mc_mask_exit;
   }

   // Check destination ID value against tt, and that the multicast mask
   // does not select ports which do not exist on the CPS device.
   if ( (in_parms->mc_info.mc_destID > IDT_LAST_DEV16_DESTID   ) ||
        ((in_parms->mc_info.mc_destID > IDT_LAST_DEV8_DESTID) &&
         (tt_dev8 == in_parms->mc_info.tt                     )) ||
        (in_parms->mc_info.mc_mask & illegal_ports             )) {
      out_parms->imp_rc = CHANGE_MC_MASK(2);
      goto idt_CPS_rt_change_mc_mask_exit;
   }
 
   if (!in_parms->mc_info.in_use) {
      idt_rt_dealloc_mc_mask_in_t  d_in_parm;
      idt_rt_dealloc_mc_mask_out_t d_out_parm;

      d_in_parm.mc_mask_rte = in_parms->mc_mask_rte;
      d_in_parm.rt          = in_parms->rt;

      rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &d_in_parm, &d_out_parm );
      if (RIO_SUCCESS != rc) {
	 out_parms->imp_rc = d_out_parm.imp_rc;
      };
      goto idt_CPS_rt_change_mc_mask_exit;
   };

   // Allow requests to change masks not supported by CPS family
   // but there's nothing to do...

   chg_idx = MC_MASK_IDX_FROM_ROUTE(in_parms->mc_mask_rte);
   
   if (chg_idx >= NUM_CPS_MC_MASKS(dev_info)) {
      rc = RIO_ERR_INVALID_PARAMETER;
      out_parms->imp_rc = CHANGE_MC_MASK(3);
      goto idt_CPS_rt_change_mc_mask_exit;
   }

   // If entry has not already been changed, see if it is being changed
   if ( !in_parms->rt->mc_masks[chg_idx].changed ) {
      if ((in_parms->rt->mc_masks[chg_idx].mc_mask != in_parms->mc_info.mc_mask) ||
          (in_parms->rt->mc_masks[chg_idx].in_use  != in_parms->mc_info.in_use ))  {
         in_parms->rt->mc_masks[chg_idx].changed = true;
      };
   };

   // Note: The multicast mask must be in use now.  We must make sure that
   // the routing tables are set appropriately.
   dom_idx = (in_parms->mc_info.mc_destID & 0xFF00) >> 8;
   if ((tt_dev16 == in_parms->mc_info.tt) && (dom_idx) 
       && (IDT_DSF_RT_USE_DEVICE_TABLE != in_parms->rt->dom_table[dom_idx].rte_val)) {
      in_parms->rt->dom_table[dom_idx].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;
      in_parms->rt->dom_table[dom_idx].changed = true;
   };

   dev_idx = (in_parms->mc_info.mc_destID & 0x00FF);
   if (in_parms->mc_mask_rte != in_parms->rt->dev_table[dev_idx].rte_val) {
      rc = tidy_routing_table(dev_info, dev_idx, in_parms->rt, &out_parms->imp_rc);
      if (RIO_SUCCESS != rc) 
         goto idt_CPS_rt_change_mc_mask_exit;

      in_parms->rt->dev_table[dev_idx].rte_val = in_parms->mc_mask_rte;
      in_parms->rt->dev_table[dev_idx].changed = true;
   };
	 
   in_parms->rt->mc_masks[chg_idx].in_use    = true                       ;
   in_parms->rt->mc_masks[chg_idx].mc_destID = in_parms->mc_info.mc_destID;
   in_parms->rt->mc_masks[chg_idx].tt        = in_parms->mc_info.tt       ;
   in_parms->rt->mc_masks[chg_idx].mc_mask   = (in_parms->mc_info.mc_mask & avail_ports);

   rc = RIO_SUCCESS;
idt_CPS_rt_change_mc_mask_exit:
   return rc;
}

#ifdef __cplusplus
}
#endif

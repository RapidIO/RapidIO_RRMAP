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

#ifndef __IDT_DSF_DB_PRIVATE_H__
#define __IDT_DSF_DB_PRIVATE_H__

#include <DAR_DB_Private.h>
#include <IDT_Common.h>
#include <IDT_Routing_Table_Config_API.h>
#include <IDT_Port_Config_API.h>
#include <IDT_Error_Management_API.h>
#include <IDT_Statistics_Counter_API.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Utility routines, useful for implementing DSF functions
 */

extern void check_unicast_routing( DAR_DEV_INFO_t     *dev_info, 
                                   idt_rt_probe_in_t  *in_parms, 
                                   idt_rt_probe_out_t *out_parms );

extern void check_multicast_routing( DAR_DEV_INFO_t     *dev_info, 
                                     idt_rt_probe_in_t  *in_parms, 
                                     idt_rt_probe_out_t *out_parms );

extern void add_pw_event( idt_em_get_pw_stat_in_t   *in_parms, 
                          idt_em_get_pw_stat_out_t  *out_parms,
                          uint8_t                      pnum,
                          idt_em_events_t            event );

extern void add_int_event( idt_em_get_int_stat_in_t   *in_parms, 
                           idt_em_get_int_stat_out_t  *out_parms,
                           uint8_t                       pnum,
                           idt_em_events_t             event );

// Generic routines for allocating/deallocating a multicast mask.
extern uint32_t IDT_DSF_rt_alloc_mc_mask( DAR_DEV_INFO_t              *dev_info, 
                                        idt_rt_alloc_mc_mask_in_t   *in_parms, 
                                        idt_rt_alloc_mc_mask_out_t  *out_parms );

extern uint32_t IDT_DSF_rt_dealloc_mc_mask( DAR_DEV_INFO_t                *dev_info, 
                                          idt_rt_dealloc_mc_mask_in_t   *in_parms, 
                                          idt_rt_dealloc_mc_mask_out_t  *out_parms );

// Routine for creating an array of ports
// If p_cnt_in = RIO_ALL_PORTS, p_cnt_out = NUM_PORTS, p_list_out = {0,...,NUM_PORTS-1}.
// If p_cnt_in != RIO_ALL_PORTS, p_cnt_out = p_cnt_in, p_list_out = p_list_in.
//
// If any member of p_list_in is out of range, returns start_rc + index.
// Otherwise, returns RIO_SUCCESS.

extern uint32_t idt_sc_port_list( DAR_DEV_INFO_t  *dev_info ,
		                uint32_t           start_rc ,
		                uint8_t            p_cnt_in ,
			        uint8_t           *p_list_in,
			        uint8_t           *p_cnt_out,
			        uint8_t           *p_list_out);

// Default implementation of idt_pc_probe, using standard registers and
//    standard routines.

extern uint32_t default_idt_pc_probe( DAR_DEV_INFO_t      *dev_info,
                                    idt_pc_probe_in_t   *in_parms,
                                    idt_pc_probe_out_t  *out_parms);

// Device driver structure...
typedef struct IDT_DSF_DB_t_TAG
{
  uint32_t dev_type; /* Device type supported by this driver... */

  // Port Configuration Routines
  uint32_t (*idt_pc_get_config)  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_pc_get_config_in_t   *in_parms, 
                                 idt_pc_get_config_out_t  *out_parms );

  uint32_t (*idt_pc_set_config)  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_pc_set_config_in_t   *in_parms, 
                                 idt_pc_set_config_out_t  *out_parms );

  uint32_t (*idt_pc_get_status)  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_pc_get_status_in_t   *in_parms, 
                                 idt_pc_get_status_out_t  *out_parms );

  uint32_t (*idt_pc_reset_port)  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_pc_reset_port_in_t   *in_parms, 
                                 idt_pc_reset_port_out_t  *out_parms );

  uint32_t (*idt_pc_reset_link_partner) (
                                 DAR_DEV_INFO_t                   *dev_info, 
                                 idt_pc_reset_link_partner_in_t   *in_parms, 
                                 idt_pc_reset_link_partner_out_t  *out_parms );

  uint32_t (*idt_pc_clr_errs)    ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_pc_clr_errs_in_t   *in_parms, 
                                 idt_pc_clr_errs_out_t  *out_parms );

  uint32_t (*idt_pc_secure_port) ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_pc_secure_port_in_t   *in_parms, 
                                 idt_pc_secure_port_out_t  *out_parms );

  uint32_t (*idt_pc_dev_reset_config) (
                                 DAR_DEV_INFO_t                 *dev_info, 
                                 idt_pc_dev_reset_config_in_t   *in_parms, 
                                 idt_pc_dev_reset_config_out_t  *out_parms );

  uint32_t (*idt_pc_probe) ( DAR_DEV_INFO_t      *dev_info, 
                           idt_pc_probe_in_t   *in_parms, 
                           idt_pc_probe_out_t  *out_parms );

  // Routing Table Management Routines
  uint32_t (*idt_rt_initialize)  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_rt_initialize_in_t   *in_parms, 
                                 idt_rt_initialize_out_t  *out_parms );

  uint32_t (*idt_rt_probe)       ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_probe_in_t         *in_parms, 
                                 idt_rt_probe_out_t        *out_parms );

  uint32_t (*idt_rt_probe_all)   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_probe_all_in_t     *in_parms, 
                                 idt_rt_probe_all_out_t    *out_parms );

  uint32_t (*idt_rt_set_all)     ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_all_in_t       *in_parms, 
                                 idt_rt_set_all_out_t      *out_parms );

  uint32_t (*idt_rt_set_changed) ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_changed_in_t   *in_parms, 
                                 idt_rt_set_changed_out_t  *out_parms );

  uint32_t (*idt_rt_alloc_mc_mask) ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_rt_alloc_mc_mask_in_t  *in_parms, 
                                 idt_rt_alloc_mc_mask_out_t *out_parms );

  uint32_t (*idt_rt_dealloc_mc_mask) ( DAR_DEV_INFO_t               *dev_info, 
                                     idt_rt_dealloc_mc_mask_in_t  *in_parms, 
                                     idt_rt_dealloc_mc_mask_out_t *out_parms );

  uint32_t (*idt_rt_change_rte) ( DAR_DEV_INFO_t          *dev_info, 
                                idt_rt_change_rte_in_t  *in_parms, 
                                idt_rt_change_rte_out_t *out_parms );

  uint32_t (*idt_rt_change_mc_mask) ( DAR_DEV_INFO_t              *dev_info, 
                                    idt_rt_change_mc_mask_in_t  *in_parms, 
                                    idt_rt_change_mc_mask_out_t *out_parms );

  // Event Management Routines
  uint32_t (*idt_em_cfg_pw)        ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_cfg_pw_in_t            *in_parms, 
                                   idt_em_cfg_pw_out_t           *out_parms );

 uint32_t (*idt_em_cfg_set)        ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_cfg_set_in_t           *in_parms, 
                                   idt_em_cfg_set_out_t          *out_parms );

 uint32_t (*idt_em_cfg_get)        ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_cfg_get_in_t           *in_parms, 
                                   idt_em_cfg_get_out_t          *out_parms );

 uint32_t (*idt_em_dev_rpt_ctl)    ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_dev_rpt_ctl_in_t        *in_parms, 
                                   idt_em_dev_rpt_ctl_out_t       *out_parms );

 uint32_t (*idt_em_parse_pw)       ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_parse_pw_in_t          *in_parms, 
                                   idt_em_parse_pw_out_t         *out_parms );

 uint32_t (*idt_em_get_int_stat)   ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_get_int_stat_in_t      *in_parms, 
                                   idt_em_get_int_stat_out_t     *out_parms );

 uint32_t (*idt_em_get_pw_stat)    ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_get_pw_stat_in_t       *in_parms, 
                                   idt_em_get_pw_stat_out_t      *out_parms );

 uint32_t (*idt_em_clr_events)     ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_clr_events_in_t        *in_parms, 
                                   idt_em_clr_events_out_t       *out_parms );
 
 uint32_t (*idt_em_create_events)  ( DAR_DEV_INFO_t                *dev_info, 
                                   idt_em_create_events_in_t     *in_parms, 
                                   idt_em_create_events_out_t    *out_parms );
				   
  // Statistics Counter Generic Routines
  // Note: There are implementation specific counter configuration
  //       routines defined in IDT_Statistics_Counter_API.h
  
  uint32_t (*idt_sc_init_dev_ctrs) ( DAR_DEV_INFO_t             *dev_info,
                                   idt_sc_init_dev_ctrs_in_t  *in_parms,
                                   idt_sc_init_dev_ctrs_out_t *out_parms );  

  uint32_t (*idt_sc_read_ctrs    ) ( DAR_DEV_INFO_t           *dev_info,
                                   idt_sc_read_ctrs_in_t    *in_parms,
                                   idt_sc_read_ctrs_out_t   *out_parms );

} IDT_DSF_DB_t;

#define IDT_DSF_INDEX(dev_info) (dev_info->dsf_h & 0x0000FFFF)

extern IDT_DSF_DB_t IDT_DB[DAR_DB_MAX_DRIVERS];

void   IDT_DSF_init_driver( IDT_DSF_DB_t *dsf);
uint32_t IDT_DSF_bind_driver( IDT_DSF_DB_t *dsf, uint32_t *dsf_index );

// IDT_DSF_bind_DAR_routines 
uint32_t IDT_DSF_bind_DAR_routines( 
    uint32_t (*ReadReg )( DAR_DEV_INFO_t *dev_info, 
                                uint32_t  offset, 
                                uint32_t *readdata ),
    uint32_t (*WriteReg)( DAR_DEV_INFO_t *dev_info, 
                                uint32_t  offset, 
                                uint32_t  writedata ),
    void   (*WaitSec) ( uint32_t delay_nsec,
                        uint32_t delay_sec ) );
#define NULL_CHECK if ((!dev_info) || (!in_parms) || (!out_parms)) return RIO_ERR_NULL_PARM_PTR;

#ifdef __cplusplus
}
#endif

#endif /* __IDT_DSF_DB_PRIVATE_H__ */


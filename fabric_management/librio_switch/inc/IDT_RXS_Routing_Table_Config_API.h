/*
****************************************************************************
Copyright (c) 2016, Integrated Device Technology Inc.
Copyright (c) 2016, RapidIO Trade Association
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

#ifndef __IDT_RXS_ROUTING_TABLE_CONFIG_API_H__
#define __IDT_RXS_ROUTING_TABLE_CONFIG_API_H__


#ifdef __cplusplus
extern "C" {
#endif

#define IDT_RXS_MAX_MC_MASKS                      0xFF

#define IDT_RXS_RTE_SET_COMMON_0                  (RT_FIRST_SUBROUTINE_0+0x0100)
#define IDT_RXS_PROGRAM_RTE_ENTRIES_0             (RT_FIRST_SUBROUTINE_0+0x1900)
#define IDT_RXS_PROGRAM_MC_MASKS_0                (RT_FIRST_SUBROUTINE_0+0x1A00)
#define IDT_RXS_READ_MC_MASKS_0                   (RT_FIRST_SUBROUTINE_0+0x1B00)
#define IDT_RXS_READ_RTE_ENTRIES_0                (RT_FIRST_SUBROUTINE_0+0x1C00)

#define IDT_RXS_PROGRAM_MC_MASKS(x)               (IDT_RXS_PROGRAM_MC_MASKS_0+x)
#define IDT_RXS_PROGRAM_RTE_ENTRIES(x)            (IDT_RXS_PROGRAM_RTE_ENTRIES_0+x)
#define IDT_RXS_RTE_SET_COMMON(x)                 (IDT_RXS_RTE_SET_COMMON_0+x)
#define IDT_RXS_READ_MC_MASKS(x)                  (IDT_RXS_READ_MC_MASKS_0+x)
#define IDT_RXS_READ_RTE_ENTRIES(x)               (IDT_RXS_READ_RTE_ENTRIES_0+x)

#define IDT_RXS_SET_ALL                           true
#define IDT_RXS_SET_CHANGED                       false

/* initializes the routing table hardware and/or routing table state structure.
 */

//#define IDT_RXS_RT_INITIALIZE(x)                  (RT_INITIALIZE_0+x)

/*uint32_t idt_rxs_rt_initialize(
    DAR_DEV_INFO_t           *dev_info,
    idt_rt_initialize_in_t   *in_parms,
    idt_rt_initialize_out_t  *out_parms
);*/

/* This function probes the hardware status of a routing table entry for 
 * the specified port and destination ID
 */

//#define IDT_RXS_RT_PROBE(x)                       (RT_PROBE_0+x)

/*uint32_t idt_rxs_rt_probe(
    DAR_DEV_INFO_t      *dev_info,
    idt_rt_probe_in_t   *in_parms,
    idt_rt_probe_out_t  *out_parms
);*/

/* This function returns the complete hardware state of packet routing
 * in a routing table state structure.
 * 
 * The routing table hardware must be initialized using idt_rt_initialize() 
 * before calling this routine.
 */

//#define IDT_RXS_RT_PROBE_ALL(x)                   (RT_PROBE_ALL_0+x)

/*uint32_t idt_rxs_rt_probe_all(
    DAR_DEV_INFO_t          *dev_info,
    idt_rt_probe_all_in_t   *in_parms,
    idt_rt_probe_all_out_t  *out_parms
);*/

/* This function sets the routing table hardware to match every entry
 * in the routing table state structure. 
 * After idt_rt_set_all is called, no entries are marked as changed in
 * the routing table state structure.
 */

//#define IDT_RXS_RT_SET_ALL(x)                     (RT_SET_ALL_0+x)

/*uint32_t idt_rxs_rt_set_all (
    DAR_DEV_INFO_t        *dev_info, 
    idt_rt_set_all_in_t   *in_parms, 
    idt_rt_set_all_out_t  *out_parms
);*/

/* This function sets the the routing table hardware to match every entry
 * that has been changed in the routing table state structure. 
 * Changes must be made using idt_rt_alloc_mc_mask, idt_rt_deallocate_mc_mask,
 * idt_rt_change_rte, and idt_rt_change_mc.
 * After idt_rt_set_changed is called, no entries are marked as changed in
 * the routing table state structure.
 */

//#define IDT_RXS_RT_SET_CHANGED(x)                 (RT_SET_CHANGED_0+x)

/*uint32_t idt_rxs_rt_set_changed (
    DAR_DEV_INFO_t            *dev_info, 
    idt_rt_set_changed_in_t   *in_parms, 
    idt_rt_set_changed_out_t  *out_parms
);*/

/* This function updates an idt_rt_state_t structure to
 * find the first previously unused multicast mask.  
 * Can be called consecutively to allocate multiple 
 * multicast masks.
 */

//#define IDT_RXS_RT_ALLOC_MC_MASK(x)               (RT_ALLOC_MC_MASK_0+x)

/*uint32_t idt_rxs_rt_alloc_mc_mask(
    DAR_DEV_INFO_t              *dev_info, 
    idt_rt_alloc_mc_mask_in_t   *in_parms, 
    idt_rt_alloc_mc_mask_out_t  *out_parms
);*/

/* This function updates an idt_rt_state_t structure to
 * deallocate a specified multicast mask.  Routing tables
 * are updated to remove all references to the multicast mask.
 * After deallocation, the hardware state must be updated by
 * calling idt_rt_set_all() or idt_rt_set_changed().
 */

//#define IDT_RXS_RT_DEALLOC_MC_MASK(x)             (RT_DEALLOC_MC_MASK_0+x)

/*uint32_t idt_rxs_rt_dealloc_mc_mask(
    DAR_DEV_INFO_t                *dev_info, 
    idt_rt_dealloc_mc_mask_in_t   *in_parms, 
    idt_rt_dealloc_mc_mask_out_t  *out_parms
);*/

/* This function updates an idt_rt_state_t structure to
 * change a routing table entry, and tracks changes.
 */

//#define IDT_RXS_RT_CHANGE_RTE(x)                  (RT_CHANGE_RTE_0+x)

/*uint32_t idt_rxs_rt_change_rte(
    DAR_DEV_INFO_t           *dev_info, 
    idt_rt_change_rte_in_t   *in_parms, 
    idt_rt_change_rte_out_t  *out_parms
);*/

/* This function updates an idt_rt_state_t structure to
 * change a multicast mask value, and tracks changes.
 */

//#define IDT_RXS_CHANGE_MC_MASK(x)                 (RT_CHANGE_MC_MASK_0+x)

/*uint32_t idt_rxs_rt_change_mc_mask(
    DAR_DEV_INFO_t               *dev_info, 
    idt_rt_change_mc_mask_in_t   *in_parms, 
    idt_rt_change_mc_mask_out_t  *out_parms
);*/

#ifdef __cplusplus
}
#endif

#endif /* __IDT_RXS_ROUTING_TABLE_CONFIG_API_H__ */

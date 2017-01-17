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

#ifndef __IDT_STATISTICS_COUNTER_API_H__
#define __IDT_STATISTICS_COUNTER_API_H__

#include <IDT_Common.h>
#include "rio_ecosystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions of parameter structures for IDT Port Configuration routines.

   Generic structure which contains the parameters which describe the
   configuration of a port.
*/

typedef enum idt_sc_ctr_t_TAG
{
    idt_sc_disabled,        // The counter is disabled.
    idt_sc_enabled,         // The counter is enabled.
			// START OF TSI57X SPECIFIC COUNTERS
    idt_sc_uc_req_pkts,     // Tsi57x Unicast request packets
                            //   Excludes response packets, maintenance packets,
			    //   maintenance packets with hop count of 0,
			    //   and packets which are multicast.
    idt_sc_uc_pkts,         // Tsi57x Unicast packets.
                            //   Excludes all packets which are multicast.
    idt_sc_retries,         // Count retry control symbols              
    idt_sc_all_cs,          // Excludes retries, and Status+NOP control symbols
    idt_sc_uc_4b_data,      // Count of multiple of words (4 bytes) of packet data
                            //    for unicast packets.  Excludes multicast packets.
    idt_sc_mc_pkts,         // Count of multicast packets.
                            //   Excludes all packets which are unicast.  
    idt_sc_mecs,            // Count of Multicast Event Control Symbols         
    idt_sc_mc_4b_data,      // Count of multiple of words (4 bytes) of packet data
                            //    for multicast packets.  Excludes unicast packets.
			// END OF TSI57X SPECIFIC COUNTERS
    idt_sc_pa,              // CPS Packet acknowledgements count (TX or RX)
    idt_sc_pkt,             // CPS Packets count (TX or RX)
    idt_sc_pna,             // CPS Packet negative acknowledgements count (RX only)
    idt_sc_pkt_drop,        // CPS Packets dropped count (TX or RX)
    idt_sc_pkt_drop_ttl,    // CPS RX Packets dropped count due to TTL (TX only)

			// START OF RXS2448 SPECIFIC PERFORMANCE COUNTERS
    idt_sc_fab_pkt,         // Number of packets received/transmitted on the fabric
    idt_sc_rio_pload,       // Count of packet payload received/transmitted on the RapidIO interface
    idt_sc_fab_pload,       // Count of packet payload received/transmitted on the fabric
    idt_sc_rio_bwidth,   // Count of the total number of code-groups/codewords
			// transmitted on the RapidIO interface per lane
			// END OF RXS2448 SPECIFIC PERFORMANCE COUNTERS

			// START OF TSI721 PERFORMANCE COUNTERS
    idt_sc_pcie_msg_rx,        // Received Completion Count for Messaging Engine Register.
    idt_sc_pcie_msg_tx,        // Sent TLP Count of Messaging Engine Register
    idt_sc_pcie_dma_rx,        // Received Completion Count for Block DMA Engine Register
    idt_sc_pcie_dma_tx,        // Sent TLP Count of Block DMA Engine Register
    idt_sc_pcie_brg_rx,        // Received Bridging TLP Count Register
    idt_sc_pcie_brg_tx,        // Sent Bridging TLP Count Register
    idt_sc_rio_nwr_tx,         // NWRITE_R Total Count Register
    idt_sc_rio_nwr_ok_rx,	// NWRITE_R RX OK Count Register
    idt_sc_rio_dbel_tx,        // Total outbound doorbells total sent
    idt_sc_rio_dbel_ok_rx,     // Total outbound doorbells responses OK
    idt_sc_rio_msg_tx,         // Sent Packet Count of Messaging Engine Register
    idt_sc_rio_msg_rx,         // Received Packet Count for Messaging Engine Register
    idt_sc_rio_msg_tx_rty,     // Generated Message Segment Retry Count Register
    idt_sc_rio_msg_rx_rty,     // Received Retry Message Response Count Register
    idt_sc_rio_dma_tx,         // Sent Packet Count of Block DMA Engine Register
    idt_sc_rio_dma_rx,         // Received Response Count for Block DMA Engine Register
    idt_sc_rio_brg_tx,          // Sent Bridging Packet Count Register
    idt_sc_rio_brg_rx,          // Received Bridging Packet Count Register
    idt_sc_rio_brg_rx_err,      // Received Bridging Packet Error Count Register
    idt_sc_rio_mwr_tx,        // Maintenance Write Total Count Register 
    idt_sc_rio_mwr_ok_rx,         // Maintenance Write OK Count Register
			// END OF TSI721 PERFORMANCE COUNTERS

    idt_sc_last             // Last index for enumerated type
} idt_sc_ctr_t;

typedef enum idt_sc_ctr_flag_t_TAG {
	sc_f_DROP = 0,
	sc_f_ERR  = 1,
	sc_f_RTY  = 2,
	sc_f_CS   = 3,
	sc_f_PKT  = 4,
	sc_f_DATA = 5,
	sc_f_LAST = 6,
} idt_sc_ctr_flag_t;

#define SC_FLAG_NAMES "DROP ERR RTY CS PKT DATA "

#define SC_F_DROP (1 << (uint32_t)(sc_f_DROP))
#define SC_F_ERR  (1 << (uint32_t)(sc_f_ERR ))
#define SC_F_RTY  (1 << (uint32_t)(sc_f_RTY ))
#define SC_F_CS   (1 << (uint32_t)(sc_f_CS  ))
#define SC_F_PKT  (1 << (uint32_t)(sc_f_PKT ))
#define SC_F_DATA (1 << (uint32_t)(sc_f_DATA))

typedef struct idt_sc_info_t_TAG {
	char *name;
	uint32_t flags;
} sc_info_t;

extern sc_info_t sc_info[(uint8_t)(idt_sc_last)+2];
#define SC_NAME(x) ((x<=idt_sc_last)? \
		sc_info[x].name : sc_info[(uint8_t)(idt_sc_last)+1].name)
#define SC_FLAG(x) ((x<=idt_sc_last)? \
		sc_info[x].flags : sc_info[(uint8_t)(idt_sc_last)+1].flags)

extern char *sc_flag_names[(uint8_t)(sc_f_LAST)+2];
#define SC_FLAG_NAME(x) ((x<=sc_f_LAST)? \
		sc_flag_names[x]:sc_flag_names[(uint8_t)(sc_f_LAST)+1])

#define SC_GEN_FLAG_NAMES "TX RX SRIO OTH "
#define SC_F_TX   0
#define SC_F_RX   1
#define SC_F_SRIO 2
#define SC_F_OTH  3

extern uint32_t idt_sc_other_if_names(DAR_DEV_INFO_t *dev_h, const char **name);

#define DIR_TX true
#define DIR_RX !DIR_TX
#define DIR_SRIO true
#define DIR_FAB  !DIR_SRIO

typedef uint8_t prio_mask_g1_t;
// Gen1 definitions for priority
#define SC_PRIO_MASK_G1_0	((prio_mask_g1_t)(0x01))
#define SC_PRIO_MASK_G1_1	((prio_mask_g1_t)(0x02))
#define SC_PRIO_MASK_G1_2	((prio_mask_g1_t)(0x04))
#define SC_PRIO_MASK_G1_3	((prio_mask_g1_t)(0x08))
#define SC_PRIO_MASK_G1_ALL	((prio_mask_g1_t)(0x0F))

typedef uint8_t prio_mask_t;
// Gen2 and beyond definitions for priority
#define SC_PRIO_MASK_0	((prio_mask_t)(0x01))
#define SC_PRIO_MASK_0C	((prio_mask_t)(0x02))
#define SC_PRIO_MASK_1	((prio_mask_t)(0x04))
#define SC_PRIO_MASK_1C	((prio_mask_t)(0x08))
#define SC_PRIO_MASK_2	((prio_mask_t)(0x10))
#define SC_PRIO_MASK_2C	((prio_mask_t)(0x20))
#define SC_PRIO_MASK_3	((prio_mask_t)(0x40))
#define SC_PRIO_MASK_3C	((prio_mask_t)(0x80))
#define SC_PRIO_MASK_ALL ((prio_mask_t)(0xFF))

typedef struct idt_sc_ctr_val_t_TAG
{
    long long    total;    // Accumulated counter value since counter was 
                           //   enabled/configured
    uint32_t       last_inc; // Value the counter increased since previous read.
    idt_sc_ctr_t sc;       // What is being counted
                           //    May be modified by device specific configuration routines,
			   //    as counters are configured/enabled/disabled
			   //    The fields "total" and "last_inc" are 0 when sc == idt_sc_dir
    bool         tx;       // true : transmitted "sc" are being counted.
                           // false: received    "sc" are being counted.
    bool         srio;     // true : Counter type reflects information on the RapidIO interface
                           // false: Counter type reflects information on the internal fabric interface
} idt_sc_ctr_val_t;

#define INIT_IDT_SC_CTR_VAL {0, 0, idt_sc_disabled, false, true} 

typedef struct idt_sc_p_ctrs_val_t_TAG
{
    uint8_t        pnum;      // Port number for these counters
    uint8_t        ctrs_cnt;  // Number of valid entries in ctrs
                            //    Device specific.
    idt_sc_ctr_val_t ctrs[RIO_MAX_SC];  // Counter values for the device
} idt_sc_p_ctrs_val_t;

typedef struct idt_sc_dev_ctrs_t_TAG
{
    uint8_t                num_p_ctrs;    // Number of allocated entries in p_ctrs[], 
                                        //    Maximum value is IDT_MAX_PORTS
	uint8_t                valid_p_ctrs;  // Number of valid entries in p_ctrs[],
	                                    //    Maximum value is num_p_ctrs;
	                                    // Initialized by idt_sc_init_dev_ctrs()...
    idt_sc_p_ctrs_val_t *p_ctrs;        // Location of performance counters structure array
} idt_sc_dev_ctrs_t;

typedef struct idt_sc_init_dev_ctrs_in_t_TAG
{
   struct DAR_ptl       ptl;       // Port list
   idt_sc_dev_ctrs_t   *dev_ctrs;  // Device performance counters state
} idt_sc_init_dev_ctrs_in_t;

typedef struct idt_sc_init_dev_ctrs_out_t_TAG 
{
   uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_init_dev_ctrs_out_t;

typedef struct idt_sc_read_ctrs_in_t_TAG
{
  struct DAR_ptl        ptl;       // Port list
   idt_sc_dev_ctrs_t   *dev_ctrs;  // Device performance counters.
} idt_sc_read_ctrs_in_t;

typedef struct idt_sc_read_ctrs_out_t_TAG
{
   uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_read_ctrs_out_t;

typedef struct idt_sc_cfg_tsi57x_ctr_in_t_TAG 
{
   struct DAR_ptl         ptl;        // Port list
   uint8_t                  ctr_idx;    // Index of the Tsi57x counter to be configured.  Range 0-5.
   prio_mask_g1_t        prio_mask;  // Priority of packets to be counted.
					// Not used for control symbol counters.
                                      // Uses SC_PRIO_MASK_G1_x constant
                                      // definitions.
   bool                   tx;         // Determines direction for the counter.  !tx = rx.
   idt_sc_ctr_t           ctr_type;   // Valid counter type, valid range from idt_sc_disabled to idt_sc_uc_4b_data
   idt_sc_dev_ctrs_t     *dev_ctrs;   // Device counters data type, initialized by idt_sc_init_dev_ctrs
} idt_sc_cfg_tsi57x_ctr_in_t;

typedef struct idt_sc_cfg_tsi57x_ctr_out_t_TAG
{
   uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_cfg_tsi57x_ctr_out_t;

typedef struct idt_sc_cfg_cps_ctrs_in_t_TAG
{
   struct DAR_ptl         ptl;       // Port list
   bool                   enable_ctrs; // true - enable all counters, false - disable all counters
   idt_sc_dev_ctrs_t     *dev_ctrs;    // Device counters data type, initialized by idt_sc_init_dev_ctrs
} idt_sc_cfg_cps_ctrs_in_t;

typedef struct idt_sc_cfg_cps_ctrs_out_t_TAG
{
   uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_cfg_cps_ctrs_out_t;

typedef struct idt_sc_cfg_cps_trace_in_t_TAG
{
  struct DAR_ptl          ptl;       // Port list
   uint8_t                  trace_idx;   // Index of the CPS trace/filter counter to be configured.  Range 0-3.
   uint32_t                 pkt_mask[5]; // Mask of packet fields to be checked.
   uint32_t                 pkt_val[5];  // Packet field values to match.
   bool                   count;       // Count packets which are traced or dropped.      
                                       //    If false, no action occurs. 
				       //    If true , all counters will be enabled on the port.
   bool                   trace;       // Send a copy of this packet to the trace port.   
   bool                   drop;        // Drop this packet.  This is independent of trace behavior.
   idt_sc_dev_ctrs_t     *dev_ctrs;    // Device counters data type, initialized by idt_sc_init_dev_ctrs
                                       //    May be set to NULL if counters are not of interest.
} idt_sc_cfg_cps_trace_in_t;

typedef struct idt_sc_cfg_cps_trace_out_t_TAG
{
   uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_cfg_cps_trace_out_t;

typedef struct idt_sc_cfg_rxs_ctr_in_t_TAG
{
	struct DAR_ptl ptl; // Port list.  
	uint8_t ctr_idx; // Index of the RXS counter [0..7] to be configured.
	prio_mask_t  prio_mask; // Packet priority, use SC_PRIO_MASK_x consts
        bool	ctr_en; // Enable/disable port counters
	bool	tx; // Determines direction for the counter.  !tx = rx.
        idt_sc_ctr_t ctr_type; // What to count
        idt_sc_dev_ctrs_t *dev_ctrs; // Initialized by idt_sc_init_dev_ctrs
} idt_sc_cfg_rxs_ctr_in_t;

typedef struct idt_sc_cfg_rxs_ctr_out_t_TAG
{
	uint32_t      imp_rc;     // Implementation specific return code information.
} idt_sc_cfg_rxs_ctr_out_t;


// Implementation specific return codes for Statistics Counter routines

#define SC_INIT_DEV_CTRS_0    (DAR_FIRST_IMP_SPEC_ERROR+0x0100)
#define SC_READ_CTRS_0        (DAR_FIRST_IMP_SPEC_ERROR+0x0200)
#define SC_CFG_TSI57X_CTR_0   (DAR_FIRST_IMP_SPEC_ERROR+0x0300)
#define SC_CFG_CPS_CTRS_0     (DAR_FIRST_IMP_SPEC_ERROR+0x0400)
#define SC_CFG_CPS_TRACE_0    (DAR_FIRST_IMP_SPEC_ERROR+0x0500)

#define SC_INIT_RXS_CTRS_0    (DAR_FIRST_IMP_SPEC_ERROR+0x0600)
#define SC_READ_RXS_CTRS_0    (DAR_FIRST_IMP_SPEC_ERROR+0x0700)
#define SC_CFG_RXS_CTR_0      (DAR_FIRST_IMP_SPEC_ERROR+0x0800)

/* The following functions are implemented to support the above structures
   Refer to the above structures for the implementation detail 
*/
/* This function initializes an idt_sc_dev_ctrs structure based
 * on input parameters and the current hardware state.
*/
#define SC_INIT_DEV_CTRS(x) (SC_INIT_DEV_CTRS_0+x)

uint32_t idt_sc_init_dev_ctrs (
    DAR_DEV_INFO_t             *dev_info,
    idt_sc_init_dev_ctrs_in_t  *in_parms,
    idt_sc_init_dev_ctrs_out_t *out_parms
);

/* Reads enabled/configured counters on selected ports   
*/
#define SC_READ_CTRS(x) (SC_READ_CTRS_0+x)

uint32_t idt_sc_read_ctrs(
    DAR_DEV_INFO_t           *dev_info,
    idt_sc_read_ctrs_in_t    *in_parms,
    idt_sc_read_ctrs_out_t   *out_parms
);

/* Configure counters on selected ports of a 
 * Tsi device.
 */

#define SC_CFG_TSI57X_CTR(x) (SC_CFG_TSI57X_CTR_0+x)
extern uint32_t idt_sc_cfg_tsi57x_ctr (
    DAR_DEV_INFO_t              *dev_info,
    idt_sc_cfg_tsi57x_ctr_in_t  *in_parms,
    idt_sc_cfg_tsi57x_ctr_out_t *out_parms
);

/* Configure counters on selected ports   
 *    of a CPS Gen2 device.
*/
#define SC_CFG_CPS_CTRS(x) (SC_CFG_CPS_CTRS_0+x)

extern uint32_t idt_sc_cfg_cps_ctrs(
    DAR_DEV_INFO_t            *dev_info,
    idt_sc_cfg_cps_ctrs_in_t  *in_parms,
    idt_sc_cfg_cps_ctrs_out_t *out_parms
);

extern uint32_t idt_sc_cfg_rxs_ctr(
    DAR_DEV_INFO_t            *dev_info,
    idt_sc_cfg_rxs_ctr_in_t  *in_parms,
    idt_sc_cfg_rxs_ctr_out_t *out_parms
);

#define SC_INIT_RXS_CTRS(x) (SC_INIT_RXS_CTRS_0+x)
#define SC_READ_RXS_CTRS(x) (SC_READ_RXS_CTRS_0+x)
#define SC_CFG_RXS_CTRS(x)  (SC_CFG_RXS_CTR_0+x)

#ifdef __cplusplus
}
#endif

#endif /* __IDT_STATISTICS_COUNTER_API_H__ */

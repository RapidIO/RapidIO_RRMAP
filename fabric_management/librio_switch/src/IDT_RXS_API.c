/*
 * ****************************************************************************
 * Copyright (c) 2016, Integrated Device Technology Inc.
 * Copyright (c) 2016, RapidIO Trade Association
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * *************************************************************************
 * */

#include "IDT_RXS_API.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "rio_ecosystem.h"

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_RXS_PORTS(x) ((NUM_PORTS(x) > RXS2448_MAX_PORTS) ? \
				RXS2448_MAX_PORTS : NUM_PORTS(x))

static DSF_Handle_t RXS_driver_handle;
static uint32_t num_RXS_driver_instances;




/* Initials counters on selected ports
 */
uint32_t idt_rxs_sc_init_dev_ctrs( DAR_DEV_INFO_t             *dev_info,
                                   idt_sc_init_dev_ctrs_in_t  *in_parms,
                                   idt_sc_init_dev_ctrs_out_t *out_parms )
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t idx, cntr_i;
	idt_sc_ctr_val_t init_val = INIT_IDT_SC_CTR_VAL;
	struct DAR_ptl good_ptl;

	out_parms->imp_rc = RIO_SUCCESS;

	if (NULL == in_parms->dev_ctrs) {
		out_parms->imp_rc = SC_INIT_RXS_CTRS(0x01);
		goto idt_sc_init_rxs_ctr_exit;
	};

	if (NULL == in_parms->dev_ctrs->p_ctrs) {
		out_parms->imp_rc = SC_INIT_RXS_CTRS(0x02);
		goto idt_sc_init_rxs_ctr_exit;
	};

	if (!in_parms->dev_ctrs->num_p_ctrs ||
		(in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		(in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
		out_parms->imp_rc = SC_INIT_RXS_CTRS(0x03);
		goto idt_sc_init_rxs_ctr_exit;
	};

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = SC_INIT_RXS_CTRS(0x10);
		goto idt_sc_init_rxs_ctr_exit;
	};

	in_parms->dev_ctrs->valid_p_ctrs = good_ptl.num_ports;
	for (idx = 0; idx < good_ptl.num_ports; idx++) {
		in_parms->dev_ctrs->p_ctrs[idx].pnum = good_ptl.pnums[idx];
		in_parms->dev_ctrs->p_ctrs[idx].ctrs_cnt = RXS2448_MAX_SC;
		for (cntr_i = 0; cntr_i < RXS2448_MAX_SC; cntr_i++) {
			in_parms->dev_ctrs->p_ctrs[idx].ctrs[cntr_i] = init_val;
		};
	};

	rc = RIO_SUCCESS;

idt_sc_init_rxs_ctr_exit:
	return rc;
}

/* Reads enabled counters on selected ports
 */
uint32_t rxs_read_ctrs(DAR_DEV_INFO_t           *dev_info,
			idt_sc_read_ctrs_in_t    *in_parms,
			idt_sc_read_ctrs_out_t   *out_parms,
			int			srch_i,
			rio_port_t		port_num)
{
	int cntr;
	uint32_t count;
	uint64_t l_c; // last counter value
	uint64_t c_c; // current counter value
	uint64_t tot; // new total counter value
	idt_sc_ctr_val_t *counter;
	uint32_t rc = !RIO_SUCCESS;

	for (cntr = 0; cntr < RXS2448_MAX_SC; cntr++) {
		counter = &in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[cntr];
		if (idt_sc_disabled == counter->sc) {
			continue;
		}

		rc = DARRegRead(dev_info,
			RXS_RIO_SPX_PCNTR_CNT(port_num, cntr), &count);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = SC_READ_RXS_CTRS(0x71 + cntr);
			goto exit;
		}
		c_c = count;
		l_c = counter->total & (uint64_t)0x00000000FFFFFFFF;
		tot = counter->total & (uint64_t)0xFFFFFFFF00000000;
		tot |= c_c;

		// If the counter has wrapped, increment tot by 1.
		//
		// Note: if c_c and l_c are equal, assume that the
		// counter has not changed since it was last read.
		if (l_c > c_c) {
			tot += (uint64_t)0x0000000100000000;
			c_c |= (uint64_t)0x0000000100000000;
		}
		counter->last_inc = c_c - l_c;
		counter->total = tot;
	}
	rc = RIO_SUCCESS;
exit:
	return rc;
};

uint32_t idt_rxs_sc_read_ctrs( DAR_DEV_INFO_t           *dev_info,
                               idt_sc_read_ctrs_in_t    *in_parms,
                               idt_sc_read_ctrs_out_t   *out_parms )
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t srch_i, srch_p, port_num;
	bool found;
	struct DAR_ptl good_ptl;

	out_parms->imp_rc = RIO_SUCCESS;

	if (NULL == in_parms->dev_ctrs) {
		out_parms->imp_rc = SC_READ_RXS_CTRS(0x01);
		goto exit;
	}

	if (NULL == in_parms->dev_ctrs->p_ctrs) {
		out_parms->imp_rc = SC_READ_RXS_CTRS(0x02);
		goto exit;
	}

	if (!in_parms->dev_ctrs->num_p_ctrs ||
		(in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		(in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
		out_parms->imp_rc = SC_READ_RXS_CTRS(0x03);
		goto exit;
	}

	if (((RIO_ALL_PORTS == in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs != NUM_RXS_PORTS(dev_info))) ||
		((RIO_ALL_PORTS != in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < in_parms->ptl.num_ports))) {
		out_parms->imp_rc = SC_READ_RXS_CTRS(0x04);
		goto exit;
	}

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = SC_READ_RXS_CTRS(0x10);
		goto exit;
	}

	for (srch_p = 0; srch_p < good_ptl.num_ports; srch_p++) {
		port_num = good_ptl.pnums[srch_p];
		found = false;
		for (srch_i = 0; srch_i < in_parms->dev_ctrs->valid_p_ctrs; srch_i++) {
			if (in_parms->dev_ctrs->p_ctrs[srch_i].pnum == port_num) {
				found = true;
				rc = rxs_read_ctrs(dev_info, in_parms,
					out_parms, srch_i, port_num);
				if (RIO_SUCCESS != rc) {
					goto exit;
				}
			}
		}
		if (!found) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_READ_RXS_CTRS(0x90 + srch_p);
			goto exit;
		}
	}
	rc = RIO_SUCCESS;

exit:
	return rc;
}

/* Configure counters on selected ports of a
 * RXS device.
 */
uint32_t idt_sc_cfg_rxs_ctr( DAR_DEV_INFO_t           *dev_info,
                             idt_sc_cfg_rxs_ctr_in_t  *in_parms,
                             idt_sc_cfg_rxs_ctr_out_t *out_parms )
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint32_t new_ctl = 0, ctl_reg;
	rio_port_t s_i, srch_p, pt;
	bool found;
	struct DAR_ptl good_ptl;
	bool srio = true;
	uint8_t c_i = in_parms->ctr_idx;

	out_parms->imp_rc = RIO_SUCCESS;

	if (NULL == in_parms->dev_ctrs) {
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x01);
		goto exit;
	}

	if (NULL == in_parms->dev_ctrs->p_ctrs) {
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x02);
		goto exit;
	}

	if (!in_parms->dev_ctrs->num_p_ctrs ||
		(in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		(in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x03);
		goto exit;
	}

	if (((RIO_ALL_PORTS == in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < NUM_RXS_PORTS(dev_info))) ||
		((RIO_ALL_PORTS != in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < in_parms->ptl.num_ports))) {
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x04);
		goto exit;
	}

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x10);
		goto exit;
	}

	if ((in_parms->dev_ctrs->num_p_ctrs   < good_ptl.num_ports) ||
		(in_parms->dev_ctrs->valid_p_ctrs < good_ptl.num_ports)) {
		rc = RIO_ERR_INVALID_PARAMETER;
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x05);
		goto exit;
	}

	// Create SC_CTL
	new_ctl = ((uint32_t)(in_parms->prio_mask) << 8);
	new_ctl &= RXS_RIO_SPC_PCNTR_CTL_PRIO;
	new_ctl |= (in_parms->tx) ? RXS_RIO_SPX_PCNTR_CTL_TX : 0;
		
	switch (in_parms->ctr_type) {
	case idt_sc_pkt:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_PKT;
		break;
	case idt_sc_fab_pkt:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_FAB_PKT;
		srio = false;
		// Fabric packet counts are prioirty specific.
		// Report a programming error if the priority mask is 0.
		if (!in_parms->prio_mask) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_CFG_RXS_CTRS(0x31);
			goto exit;
		}
		break;
	case idt_sc_rio_pload:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_PAYLOAD;
		break;
	case idt_sc_fab_pload:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_FAB_PAYLOAD;
		srio = false;
		// Fabric packet data counts are prioirty specific.
		// Report a programming error if the priority mask is 0.
		if (!in_parms->prio_mask) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_CFG_RXS_CTRS(0x32);
			goto exit;
		}
		break;
	case idt_sc_rio_bwidth:
		// Count of the total number of code-groups/codewords 
		// transmitted on the RapidIO interface per lane.
		// Hardware does not support count for RX (!TX).
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_TTL_PKTCNTR;
		if (!in_parms->tx) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_CFG_RXS_CTRS(0x33);
			goto exit;
		}
		break;
	case idt_sc_retries:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_RETRIES;
		break;
	case idt_sc_pna:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_PNA;
		break;
	case idt_sc_pkt_drop:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_PKT_DROP;
		// Packet drop counts are prioirty specific.
		// Report a programming error if the priority mask is 0.
		if (!in_parms->prio_mask) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_CFG_RXS_CTRS(0x34);
			goto exit;
		}
		break;
	case idt_sc_disabled:
		new_ctl |= RXS_RIO_SPX_PCNTR_CTL_SEL_DISABLED;
		break;
	default: rc = RIO_ERR_INVALID_PARAMETER;
		out_parms->imp_rc = SC_CFG_RXS_CTRS(0x35);
		goto exit;
	}	

	for (srch_p = 0; srch_p < good_ptl.num_ports; srch_p++) {
		pt = good_ptl.pnums[srch_p];
		found = false;

		// Enable counters for each port before programming the counter
		// control value.
		if (in_parms->ctr_en) {
			rc = DARRegWrite(dev_info,
					RXS_RIO_SPX_PCNTR_EN(pt),
					RXS_RIO_SPX_PCNTR_EN_ENABLE);
			if (RIO_SUCCESS != rc) {
				out_parms->imp_rc = SC_CFG_RXS_CTRS(0x40);
				goto exit;
			}
		}
	
		for (s_i = 0; s_i < in_parms->dev_ctrs->valid_p_ctrs; s_i++) {
			if (in_parms->dev_ctrs->p_ctrs[s_i].pnum != pt) {
				continue;
			};
			idt_sc_ctr_val_t *ctr_p;
			ctr_p = &in_parms->dev_ctrs->p_ctrs[s_i].ctrs[c_i];
			found = true;
			// Always program the control value...
			rc = DARRegRead(dev_info,
					RXS_RIO_SPX_PCNTR_CTL(pt, c_i),
					&ctl_reg);
			if (RIO_SUCCESS != rc) {
				out_parms->imp_rc = SC_CFG_RXS_CTRS(0x41);
				goto exit;
			}

			rc = DARRegWrite(dev_info,
					RXS_RIO_SPX_PCNTR_CTL(pt, c_i),
					new_ctl);
			if (RIO_SUCCESS != rc) {
				out_parms->imp_rc = SC_CFG_RXS_CTRS(0x42);
				goto exit;
			};

			if (ctl_reg != new_ctl) {
				// If the counted value has changed,
				// update the control structure and
				// zero the hw & sw  counters
				ctr_p->sc = in_parms->ctr_type;
				ctr_p->tx = in_parms->tx;
				ctr_p->srio = srio;
				ctr_p->total = 0;
				ctr_p->last_inc = 0;
				rc = DARRegWrite(dev_info,
						RXS_RIO_SPX_PCNTR_CNT(pt, c_i),
						0);
				if (RIO_SUCCESS != rc) {
					out_parms->imp_rc =
						SC_CFG_RXS_CTRS(0x43);
					goto exit;
				}
			}
		}
		if (!found) {
			rc = RIO_ERR_INVALID_PARAMETER;
			out_parms->imp_rc = SC_CFG_RXS_CTRS(0x44);
			goto exit;
		}
		// Disable counters for each port after programming the counter
		// control value.
		if (!in_parms->ctr_en) {
			idt_sc_ctr_val_t init_val = INIT_IDT_SC_CTR_VAL;
			int cntr_i;

			rc = DARRegWrite(dev_info, RXS_RIO_SPX_PCNTR_EN(pt), 0);
			if (RIO_SUCCESS != rc) {
				out_parms->imp_rc = SC_CFG_RXS_CTRS(0x60);
				goto exit;
			}
			for (cntr_i = 0; cntr_i < RXS2448_MAX_SC; cntr_i++) {
				in_parms->dev_ctrs->p_ctrs[s_i].ctrs[cntr_i] =
					init_val;
			}
		}
	}

	rc = RIO_SUCCESS;
exit:
	return rc;
}

#define MC_MASK_ADDR(b,m)     ((b)+(8*m))

uint32_t idt_rxs_program_mc_masks ( DAR_DEV_INFO_t        *dev_info, 
                                    idt_rt_set_all_in_t   *in_parms,
                                    bool                   set_all, // true if all entries should be set
                                    uint32_t              *imp_rc )
{
    uint32_t rc = RIO_SUCCESS;
    // Note that the base address for RXS2448 and RXS1632
    // are all the same.
    uint8_t  mask_num;
    uint32_t base_addr, mask_mask;
   

    switch (DEV_CODE(dev_info)) {
       case IDT_RXS2448_RIO_DEVICE_ID:
          mask_mask = RXS2448_RIO_BC_MC_X_S_CSR_SET;
       break;
       case IDT_RXS1632_RIO_DEVICE_ID:
          mask_mask = RXS1632_RIO_BC_MC_X_S_CSR_SET;
       break;
       default:
          *imp_rc = IDT_RXS_PROGRAM_MC_MASKS(0x01);
          goto exit;
    }

    if (RIO_ALL_PORTS == in_parms->set_on_port) {
       base_addr = RXS_RIO_SPX_MC_Y_S_CSR(0, 0);
    } else {
       base_addr = RXS_RIO_SPX_MC_Y_S_CSR(in_parms->set_on_port, 0);
    }

    for (mask_num = 0; mask_num < NUM_MC_MASKS(dev_info); mask_num++) {
       if (in_parms->rt->mc_masks[mask_num].changed || set_all) {
	  if ( in_parms->rt->mc_masks[mask_num].mc_mask & ~mask_mask ) {
             rc = RIO_ERR_INVALID_PARAMETER;
             *imp_rc = IDT_RXS_PROGRAM_MC_MASKS(3);
             goto exit; 
          }
          rc = DARRegWrite( dev_info, MC_MASK_ADDR(base_addr, mask_num), 
                            in_parms->rt->mc_masks[mask_num].mc_mask & mask_mask );
          if (RIO_SUCCESS != rc) {
             *imp_rc = IDT_RXS_PROGRAM_MC_MASKS(4);
             goto exit; 
          }
	  in_parms->rt->mc_masks[mask_num].changed = false;
       }
    }
        
exit: 
    return rc;
}

#define DEV_RTE_ADDR(b,n) ((b)+(4*n))
#define DOM_RTE_ADDR(b,n) ((b)+(4*n))

uint32_t idt_rxs_program_rte_entries ( DAR_DEV_INFO_t        *dev_info, 
                                       idt_rt_set_all_in_t   *in_parms, 
                                       bool                   set_all, // true if all entries should be set
                                       uint32_t              *imp_rc ) 
{
    uint32_t rc = RIO_SUCCESS;
    // Note that the base address for RXS2448 and RXS1632
    // are all the same.
    uint16_t rte_num;
    uint32_t dev_rte_base, dom_rte_base;

    rc = DARRegWrite( dev_info, RXS_RIO_ROUTE_DFLT_PORT, in_parms->rt->default_route );
    if (RIO_SUCCESS != rc) {
       *imp_rc = IDT_RXS_PROGRAM_RTE_ENTRIES(0x10);
       goto exit; 
    }

    if (RIO_ALL_PORTS == in_parms->set_on_port) {
       dev_rte_base = RXS_RIO_BC_L2_GX_ENTRYY_CSR(0, 0);
       dom_rte_base = RXS_RIO_BC_L1_GX_ENTRYY_CSR(0, 0);
    } else {
       dev_rte_base = RXS_RIO_SPX_L2_GY_ENTRYZ_CSR(in_parms->set_on_port, 0, 0);
       dom_rte_base = RXS_RIO_SPX_L1_GY_ENTRYZ_CSR(in_parms->set_on_port, 0, 0);
    } 
    
    for (rte_num = 0; rte_num < IDT_DAR_RT_DOM_TABLE_SIZE; rte_num++) {
       if (in_parms->rt->dom_table[rte_num].changed || set_all) {
          // Validate value to be programmed.
          if (in_parms->rt->dom_table[rte_num].rte_val >= NUM_RXS_PORTS(dev_info)) {
	     // Domain table can be a port number, use device table, use default route, or drop.
	     if ((in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_USE_DEVICE_TABLE    ) &&
	         (in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_USE_DEFAULT_ROUTE   ) &&
	         (in_parms->rt->dom_table[rte_num].rte_val != IDT_DSF_RT_NO_ROUTE            ) ) {
                rc = RIO_ERR_INVALID_PARAMETER;
                *imp_rc = IDT_RXS_PROGRAM_RTE_ENTRIES(1);
                goto exit; 
             } 
          } 

          rc = DARRegWrite( dev_info, DOM_RTE_ADDR(dom_rte_base, rte_num), 
                            (uint32_t)(in_parms->rt->dom_table[rte_num].rte_val) );
          if (RIO_SUCCESS != rc) {
             *imp_rc = IDT_RXS_PROGRAM_RTE_ENTRIES(2);
             goto exit; 
          } 
	  in_parms->rt->dom_table[rte_num].changed = false;
       } 
    } 
        
    for (rte_num = 0; rte_num < IDT_DAR_RT_DEV_TABLE_SIZE; rte_num++) {
       if (in_parms->rt->dev_table[rte_num].changed || set_all) {
	  // Validate value to be programmed.
          if (in_parms->rt->dev_table[rte_num].rte_val >= NUM_RXS_PORTS(dev_info)) {
	     // Device table can be a port number, a multicast mask, use default route, or drop.
	     if ((MC_MASK_IDX_FROM_ROUTE(in_parms->rt->dev_table[rte_num].rte_val)
				                           == IDT_DSF_BAD_MC_MASK         ) &&
	         (in_parms->rt->dev_table[rte_num].rte_val != IDT_DSF_RT_USE_DEFAULT_ROUTE) &&
	         (in_parms->rt->dev_table[rte_num].rte_val != IDT_DSF_RT_NO_ROUTE         ) ) {
                rc = RIO_ERR_INVALID_PARAMETER;
                *imp_rc = IDT_RXS_PROGRAM_RTE_ENTRIES(3);
                goto exit; 
             } 
          } 

          rc = DARRegWrite( dev_info, DEV_RTE_ADDR(dev_rte_base, rte_num),
                            (uint32_t)(in_parms->rt->dev_table[rte_num].rte_val) );
          if (RIO_SUCCESS != rc) {
             *imp_rc = IDT_RXS_PROGRAM_RTE_ENTRIES(4);
             goto exit; 
          } 
          in_parms->rt->dev_table[rte_num].changed = false;
       } 
    } 
        
exit: 
    return rc;
}

uint32_t idt_rxs_rt_set_common( DAR_DEV_INFO_t        *dev_info, 
                                idt_rt_set_all_in_t   *in_parms, 
                                idt_rt_set_all_out_t  *out_parms,
                                bool                   set_all  ) // true if all entries should be set
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;

    out_parms->imp_rc = RIO_SUCCESS;

    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->set_on_port ) && 
           ( in_parms->set_on_port >= NUM_RXS_PORTS(dev_info)    ) ) ||
         ( !in_parms->rt) ) 
    {
        out_parms->imp_rc = IDT_RXS_RTE_SET_COMMON(1);
        goto exit;
    }

    if ((NUM_RXS_PORTS(dev_info) <= in_parms->rt->default_route) &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->rt->default_route))   {
        out_parms->imp_rc = IDT_RXS_RTE_SET_COMMON(2);
        goto exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    rc = idt_rxs_program_mc_masks( dev_info, in_parms, set_all, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto exit;
    }

    rc = idt_rxs_program_rte_entries( dev_info, in_parms, set_all, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) 
       goto exit;

exit:

    return rc;
}

/* This function sets the routing table hardware to match every entry
 * in the routing table state structure. 
 * After idt_rt_set_all is called, no entries are marked as changed in
 * the routing table state structure.
 */
uint32_t idt_rxs_rt_set_all( DAR_DEV_INFO_t        *dev_info, 
                             idt_rt_set_all_in_t   *in_parms, 
                             idt_rt_set_all_out_t  *out_parms )
{
    return idt_rxs_rt_set_common(dev_info, in_parms, out_parms, IDT_RXS_SET_ALL);
}

/* This function sets the the routing table hardware to match every entry
 * that has been changed in the routing table state structure. 
 * Changes must be made using idt_rt_alloc_mc_mask, idt_rt_deallocate_mc_mask,
 * idt_rt_change_rte, and idt_rt_change_mc.
 * After idt_rt_set_changed is called, no entries are marked as changed in
 * the routing table state structure.
 */
uint32_t idt_rxs_rt_set_changed( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_changed_in_t   *in_parms, 
                                 idt_rt_set_changed_out_t  *out_parms ) 
{
    return idt_rxs_rt_set_common(dev_info, in_parms, out_parms, IDT_RXS_SET_CHANGED);
}

/* This function updates an idt_rt_state_t structure to
 * change a routing table entry, and tracks changes.
 */
uint32_t idt_rxs_rt_change_rte( DAR_DEV_INFO_t           *dev_info, 
                                idt_rt_change_rte_in_t   *in_parms, 
                                idt_rt_change_rte_out_t  *out_parms ) 
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;

   out_parms->imp_rc = RIO_SUCCESS;

   if (!in_parms->rt) {
      out_parms->imp_rc = RT_CHANGE_RTE(1);
      goto exit;
   } 

   // Validate rte_value 
   if ( (IDT_DSF_RT_USE_DEVICE_TABLE  != in_parms->rte_value) &&
        (IDT_DSF_RT_USE_DEFAULT_ROUTE != in_parms->rte_value) &&
        (IDT_DSF_RT_NO_ROUTE         != in_parms->rte_value) &&
        (in_parms->rte_value         >= NUM_RXS_PORTS(dev_info))) {
      out_parms->imp_rc = RT_CHANGE_RTE(2);
      goto exit;
   }

   if ( (IDT_DSF_RT_USE_DEVICE_TABLE == in_parms->rte_value) && (!in_parms->dom_entry)) {
      out_parms->imp_rc = RT_CHANGE_RTE(3);
      goto exit;
   } 

   rc = RIO_SUCCESS;

   // Do not allow any changes to index 0 of the domain table.
   // This must be set to "IDT_RXS_DSF_RT_USE_PACKET_ROUTE" at all times,
   // as this is the behavior required by the RXS RIO Domain register.

   if (in_parms->dom_entry && !in_parms->idx)
      goto exit;

   // If the entry has not already been changed, see if it is being changed
   if (in_parms->dom_entry) {
      if (  !in_parms->rt->dom_table[in_parms->idx].changed ) {
         if (in_parms->rt->dom_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dom_table[in_parms->idx].changed = true;
      } 
      in_parms->rt->dom_table[in_parms->idx].rte_val = in_parms->rte_value;
   } else {
      if (  !in_parms->rt->dev_table[in_parms->idx].changed ) {
         if (in_parms->rt->dev_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dev_table[in_parms->idx].changed = true;
      } 
      in_parms->rt->dev_table[in_parms->idx].rte_val = in_parms->rte_value;
   } 

exit:
   return rc;
}

uint32_t idt_rxs_rt_initialize( DAR_DEV_INFO_t           *dev_info,
                                idt_rt_initialize_in_t   *in_parms,
                                idt_rt_initialize_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t destID;
    uint32_t mc_idx;
    idt_rt_set_changed_in_t  all_in;
    idt_rt_set_changed_out_t all_out;
    idt_rt_state_t           rt_state;
    // Validate parameters

   
    if (  (in_parms->default_route      >= NUM_RXS_PORTS(dev_info))  &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->default_route)  )
    {
        out_parms->imp_rc = RT_INITIALIZE(1);
        goto exit;
    }

    if ( (in_parms->default_route_table_port >= NUM_RXS_PORTS(dev_info)) &&
         !((IDT_DSF_RT_USE_DEFAULT_ROUTE == in_parms->default_route_table_port) ||
           (IDT_DSF_RT_NO_ROUTE              == in_parms->default_route_table_port)) )
    {
        out_parms->imp_rc = RT_INITIALIZE(2);
        goto exit;
    }

    if ( (in_parms->set_on_port >= NUM_RXS_PORTS(dev_info)  )  &&
        !(RIO_ALL_PORTS         == in_parms->set_on_port))
    {
        out_parms->imp_rc = RT_INITIALIZE(3);
        goto exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    all_in.set_on_port = in_parms->set_on_port;

    if (!in_parms->rt)
    {
       all_in.rt = &rt_state;
    }
    else
    {
       all_in.rt = in_parms->rt;
    }

    all_in.rt->default_route = in_parms->default_route;

    // Configure initialization of all of the routing table entries
    for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
    {
        all_in.rt->dev_table[destID].changed = true ;
        all_in.rt->dev_table[destID].rte_val = in_parms->default_route_table_port;
    } 
    
    all_in.rt->dom_table[0].changed = true ;
    all_in.rt->dom_table[0].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;

    for (destID = 1; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
    {
        all_in.rt->dom_table[destID].changed = true ;
        all_in.rt->dom_table[destID].rte_val = in_parms->default_route_table_port;
    } 
    
    // Configure initialization of multicast masks and associations as necessary. 
    for (mc_idx = 0; mc_idx < IDT_DSF_MAX_MC_MASK; mc_idx++) 
    {
       all_in.rt->mc_masks[mc_idx].mc_destID = 0;
       all_in.rt->mc_masks[mc_idx].tt        = tt_dev8;
       all_in.rt->mc_masks[mc_idx].mc_mask   = 0;
       all_in.rt->mc_masks[mc_idx].in_use    = false;
       all_in.rt->mc_masks[mc_idx].allocd    = false;
       if ((mc_idx < IDT_RXS_MAX_MC_MASKS) && (mc_idx < IDT_DSF_MAX_MC_MASK)) {
          all_in.rt->mc_masks[mc_idx].changed   = true ;
       } else {
          all_in.rt->mc_masks[mc_idx].changed   = false ;
       } 
    } 

    if (in_parms->update_hw) {
       rc = idt_rxs_rt_set_changed(dev_info, &all_in, &all_out );
    } else {
       rc = RIO_SUCCESS;
    }
     
    if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = all_out.imp_rc;
    }

exit:

    return rc;
}

typedef struct spx_ctl2_ls_check_info_t_TAG {
	uint32_t      ls_en_val;
	uint32_t      ls_sup_val;
	idt_pc_ls_t   ls;
	uint32_t      prescalar_srv_clk;
} spx_ctl2_ls_check_info_t;

spx_ctl2_ls_check_info_t rxs_ls_check[] = {
	{ RIO_SPX_CTL2_GB_1p25_EN , RIO_SPX_CTL2_GB_1p25 , idt_pc_ls_1p25 , 13 },
	{ RIO_SPX_CTL2_GB_2p5_EN  , RIO_SPX_CTL2_GB_2p5  , idt_pc_ls_2p5  , 13 },
	{ RIO_SPX_CTL2_GB_3p125_EN, RIO_SPX_CTL2_GB_3p125, idt_pc_ls_3p125, 16 },
	{ RIO_SPX_CTL2_GB_5p0_EN  , RIO_SPX_CTL2_GB_5p0  , idt_pc_ls_5p0  , 25 },
	{ RIO_SPX_CTL2_GB_6p25_EN , RIO_SPX_CTL2_GB_6p25 , idt_pc_ls_6p25 , 31 },
	{ RIO_SPX_CTL2_GB_10p3_EN , RIO_SPX_CTL2_GB_10p3 , idt_pc_ls_10p3 ,  0 },/*TODO: prescalar_srv_clk:?*/
	{ RIO_SPX_CTL2_GB_12p5_EN , RIO_SPX_CTL2_GB_12p5 , idt_pc_ls_12p5 ,  0 },/*TODO: prescalar_srv_clk:?*/
	{ 0x00000000              , 0x00000000           , idt_pc_ls_last ,  0 }
};
//TODO: Maybe it needs to add lane to port mapping for this routine.
uint32_t idt_rxs_pc_get_config( DAR_DEV_INFO_t           *dev_info,
                                idt_pc_get_config_in_t   *in_parms,
                                idt_pc_get_config_out_t  *out_parms )
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint32_t port_idx, idx;
	bool   misconfigured = false;
	uint32_t plmCtl, spxCtl, spxCtl2, plmPol, errStat;
	int32_t  lane_num;
	struct DAR_ptl good_ptl;

	out_parms->num_ports = 0;
	out_parms->imp_rc = 0;

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_SET_CONFIG(0x1);
		goto exit;
	} 

	out_parms->num_ports = good_ptl.num_ports;
	for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
		out_parms->pc[port_idx].pnum = good_ptl.pnums[port_idx];

	// Always get LRTO
	{ uint32_t lrto;
	rc = DARRegRead(dev_info, RXS_RIO_SP_LT_CTL, &lrto);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_SET_CONFIG(0x2);
		goto exit;
	}
	out_parms->lrto = lrto >> 8;
	} 

	for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
	{
		out_parms->pc[port_idx].port_available = true;
		out_parms->pc[port_idx].pw = idt_pc_pw_last;
		out_parms->pc[port_idx].ls = idt_pc_ls_last;
		out_parms->pc[port_idx].iseq = idt_pc_is_one;
		out_parms->pc[port_idx].fc = idt_pc_fc_rx;
		out_parms->pc[port_idx].xmitter_disable = false;
		out_parms->pc[port_idx].port_lockout = false;
		out_parms->pc[port_idx].nmtc_xfer_enable = false;
		out_parms->pc[port_idx].rx_lswap = false;
		out_parms->pc[port_idx].tx_lswap = false;
		for (lane_num = 0; lane_num < IDT_PC_MAX_LANES; lane_num++) {
			out_parms->pc[port_idx].tx_linvert[lane_num] = false;
			out_parms->pc[port_idx].rx_linvert[lane_num] = false;
		} 

		// Check that RapidIO transmitter is enabled...
		rc = DARRegRead(dev_info, RXS_RIO_SPX_CTL(port_idx), &spxCtl);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = PC_GET_CONFIG(8);
			goto exit;
		} 

		out_parms->pc[port_idx].xmitter_disable =
			(spxCtl & RXS_RIO_SPX_CTL_PORT_DIS) ? true : false;

		// OK, port is enabled so it can train.
		// Check for port width overrides...
		rc = DARRegRead(dev_info, RXS_RIO_SPX_CTL(port_idx), &spxCtl);
		if (RIO_SUCCESS != rc)
		{
			out_parms->imp_rc = PC_GET_CONFIG(0x10);
			goto exit;
		} 
		switch (spxCtl & RIO_SPX_CTL_PTW_OVER) {
		case RIO_SPX_CTL_PTW_OVER_4x_NO_2X:
		case RIO_SPX_CTL_PTW_OVER_NONE_2:
		case RIO_SPX_CTL_PTW_OVER_NONE: out_parms->pc[port_idx].pw = idt_pc_pw_4x;
			break;
		case RIO_SPX_CTL_PTW_OVER_1x_L0: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l0;
			break;
		case RIO_SPX_CTL_PTW_OVER_1x_LR: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l2;
			break;
		case RIO_SPX_CTL_PTW_OVER_2x_NO_4X: out_parms->pc[port_idx].pw = idt_pc_pw_2x;
			break;
		default: out_parms->pc[port_idx].pw = idt_pc_pw_last;
		} 

		// Determine configured port speed...
		rc = DARRegRead(dev_info, RXS_RIO_SPX_CTL2(port_idx), &spxCtl2);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = PC_GET_CONFIG(0x11);
			goto exit;
		} 

		out_parms->pc[port_idx].ls = idt_pc_ls_last;
		misconfigured = false;

		for (idx = 0; (rxs_ls_check[idx].ls_en_val) && !misconfigured; idx++) {
			if (rxs_ls_check[idx].ls_en_val & spxCtl2) {
				if (!(rxs_ls_check[idx].ls_sup_val & spxCtl2)) {
					misconfigured = true;
					out_parms->pc[port_idx].ls = idt_pc_ls_last;
				}
				else {
					if (idt_pc_ls_last != out_parms->pc[port_idx].ls) {
						misconfigured = true;
						out_parms->pc[port_idx].ls = idt_pc_ls_last;
					}
					else {
						out_parms->pc[port_idx].ls = rxs_ls_check[idx].ls;
					} 
				} 
			} 
		} 

		out_parms->pc[port_idx].port_lockout =
			(spxCtl & RXS_RIO_SPX_CTL_PORT_LOCKOUT) ? true : false;

		out_parms->pc[port_idx].nmtc_xfer_enable =
			((spxCtl & (RXS_RIO_SPX_CTL_INP_EN | RXS_RIO_SPX_CTL_OTP_EN))
				== (RXS_RIO_SPX_CTL_INP_EN | RXS_RIO_SPX_CTL_OTP_EN));

		// Check for lane swapping & inversion
		// LANE SWAPPING AND INVERSION NOT SUPPORTED
		rc = DARRegRead(dev_info, RXS_RIO_PLM_SPX_IMP_SPEC_CTL(port_idx), &plmCtl);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = PC_GET_CONFIG(0x20);
			goto exit;
		} 

		if (plmCtl & RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX) {
			out_parms->pc[port_idx].rx_lswap = true;
		} 

		if (plmCtl & RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_TX) {
			out_parms->pc[port_idx].tx_lswap = true;
		}

                rc = DARRegRead(dev_info, RXS_RIO_SPX_ERR_STAT(port_idx), &errStat);
                if (RIO_SUCCESS != rc) {
                        out_parms->imp_rc = PC_GET_CONFIG(0x30);
                        goto exit;
                }

                out_parms->pc[port_idx].port_available = (errStat & RXS_RIO_SPX_ERR_STAT_PORT_UNAVL ? true : false);
                out_parms->pc[port_idx].port_available = (errStat & RXS_RIO_SPX_ERR_STAT_PORT_W_DIS ? true : false);

                rc = DARRegRead(dev_info, RXS_PLM_SPX_POL_CTL(port_idx), &plmPol);
                if (RIO_SUCCESS != rc) {
                        out_parms->imp_rc = PC_GET_CONFIG(0x40);
                        goto exit;
                } 

		out_parms->pc[port_idx].tx_linvert[3] = (plmPol & RXS_PLM_SPX_POL_CTL_TX3_POL ? true : false);
		out_parms->pc[port_idx].rx_linvert[3] = (plmPol & RXS_PLM_SPX_POL_CTL_RX3_POL ? true : false);

                out_parms->pc[port_idx].tx_linvert[2] = (plmPol & RXS_PLM_SPX_POL_CTL_TX2_POL ? true : false);
                out_parms->pc[port_idx].rx_linvert[2] = (plmPol & RXS_PLM_SPX_POL_CTL_RX2_POL ? true : false);

                out_parms->pc[port_idx].tx_linvert[1] = (plmPol & RXS_PLM_SPX_POL_CTL_TX1_POL ? true : false);
                out_parms->pc[port_idx].rx_linvert[1] = (plmPol & RXS_PLM_SPX_POL_CTL_RX1_POL ? true : false);

                out_parms->pc[port_idx].tx_linvert[0] = (plmPol & RXS_PLM_SPX_POL_CTL_TX0_POL ? true : false);
                out_parms->pc[port_idx].rx_linvert[0] = (plmPol & RXS_PLM_SPX_POL_CTL_RX0_POL ? true : false);		
	} 

exit:
	return rc;
}

uint32_t idt_rxs_pc_get_status( DAR_DEV_INFO_t           *dev_info,
                                idt_pc_get_status_in_t   *in_parms,
                                idt_pc_get_status_out_t  *out_parms )
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t  port_idx;
	uint32_t errStat, spxCtl;
	struct DAR_ptl good_ptl;

	out_parms->num_ports = 0;
	out_parms->imp_rc = RIO_SUCCESS;

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_GET_uint32_t(1);
		goto exit;
	}

	out_parms->num_ports = good_ptl.num_ports;
	for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
		out_parms->ps[port_idx].pnum = good_ptl.pnums[port_idx];

	for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
	{
		out_parms->ps[port_idx].pw = idt_pc_pw_last;
		out_parms->ps[port_idx].port_error = false;
		out_parms->ps[port_idx].input_stopped = false;
		out_parms->ps[port_idx].output_stopped = false;

		// Port is available and powered up, so let's figure out the status...
		rc = DARRegRead(dev_info, RXS_RIO_SPX_ERR_STAT(port_idx), &errStat);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = PC_GET_uint32_t(0x30 + port_idx);
			goto exit;
		}

		rc = DARRegRead(dev_info, RXS_RIO_SPX_CTL(port_idx), &spxCtl);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = PC_GET_uint32_t(0x40 + port_idx);
			goto exit;
		}

		out_parms->ps[port_idx].port_ok =
			(errStat & RXS_RIO_SPX_ERR_STAT_PORT_OK) ? true : false;
		out_parms->ps[port_idx].input_stopped =
			(errStat & RXS_RIO_SPX_ERR_STAT_INPUT_ERR_STOP) ? true : false;
		out_parms->ps[port_idx].output_stopped =
			(errStat & RXS_RIO_SPX_ERR_STAT_OUTPUT_ERR_STOP) ? true : false;

		// Port Error is true if a PORT_ERR is present, OR
		// if a OUTPUT_FAIL is present when STOP_FAIL_EN is set.
		out_parms->ps[port_idx].port_error =
			((errStat & RXS_RIO_SPX_ERR_STAT_PORT_ERR) ||
				((spxCtl  & RXS_RIO_SPX_CTL_STOP_FAIL_EN) &&
					(errStat & RXS_RIO_SPX_ERR_STAT_OUTPUT_FAIL)));

		// Baudrate and portwidth status are only defined when
		// PORT_OK is asserted... 
		if (out_parms->ps[port_idx].port_ok) {
			switch (spxCtl & RXS_RIO_SPX_CTL_INIT_PWIDTH) {
			case RIO_SPX_CTL_PTW_INIT_1x_L0: out_parms->ps[port_idx].pw = idt_pc_pw_1x_l0;
				break;
			case RIO_SPX_CTL_PTW_INIT_1x_LR: out_parms->ps[port_idx].pw = idt_pc_pw_1x_l2;
				break;
			case RIO_SPX_CTL_PTW_INIT_2x: out_parms->ps[port_idx].pw = idt_pc_pw_2x;
				break;
			case RIO_SPX_CTL_PTW_INIT_4x: out_parms->ps[port_idx].pw = idt_pc_pw_4x;
				break;
			default:  out_parms->ps[port_idx].pw = idt_pc_pw_last;
			}
		}
	}

exit:
	return rc;
}

uint32_t idt_rxs_check_port_for_discard( DAR_DEV_INFO_t     *dev_info, 
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

   if (NUM_RXS_PORTS(dev_info) <= port) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(1);
      goto exit;
   }

   cfg_in.ptl.num_ports = 1;
   cfg_in.ptl.pnums[0] = port;
   rc = idt_rxs_pc_get_config( dev_info, &cfg_in, &cfg_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(2);
      goto exit;
   }

   stat_in.ptl.num_ports = 1;
   stat_in.ptl.pnums[0] = port;
   rc = idt_rxs_pc_get_status( dev_info, &stat_in, &stat_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(3);
      goto exit;
   }

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
	     }
	  } else {
             if (stat_out.ps[0].port_error) {
                out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_fail:idt_rt_disc_port_fail;
             } else {
                if (cfg_out.pc[0].port_lockout) {
                   out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_lkout_or_dis:
			                                 idt_rt_disc_port_lkout_or_dis;
		} else {
                   rc = DARRegRead( dev_info, RXS_RIO_SPX_CTL(port), &ctlData );
                   if (RIO_SUCCESS != rc) {
                      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
                      out_parms->imp_rc = RT_PROBE(4);
                      goto exit;
                   }
            
                  if ( (RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) != 
                      ((RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) & ctlData)) {
                     out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_in_out_dis:idt_rt_disc_port_in_out_dis;
                  }
               }
            }
         }
      }
   }

   rc = RIO_SUCCESS;

exit:

    if (idt_rt_disc_not != out_parms->reason_for_discard)
       out_parms->valid_route = false;

    return rc;
}

void idt_rxs_check_multicast_routing ( DAR_DEV_INFO_t     *dev_info, 
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
	 }

         if ((in_parms->destID & mc_mask) == (in_parms->rt->mc_masks[mc_idx].mc_destID & mc_mask)) {
            if (found) {
               out_parms->reason_for_discard = idt_rt_disc_mc_mult_masks;
               out_parms->valid_route        = false;
	       break;
	    } else {
	       found = true;
               out_parms->routing_table_value = mc_idx + IDT_DSF_FIRST_MC_MASK;
	       for (bit = 0; bit < NUM_RXS_PORTS(dev_info); bit++)
	          out_parms->mcast_ports[bit] = ((uint32_t)(1 << bit) & in_parms->rt->mc_masks[mc_idx].mc_mask)?true:false;
	       if (in_parms->rt->mc_masks[mc_idx].mc_mask) {
                  if ((uint32_t)((uint32_t)(1) << in_parms->probe_on_port) == in_parms->rt->mc_masks[mc_idx].mc_mask) {
                     out_parms->reason_for_discard = idt_rt_disc_mc_one_bit;
	          } else {
                     out_parms->reason_for_discard = idt_rt_disc_not;
		     out_parms->valid_route        = true;
		  }
	       } else {
                  out_parms->reason_for_discard     = idt_rt_disc_mc_empty;
	       }
	    }
         }
      }
   }

   return;
}

void idt_rxs_check_unicast_routing( DAR_DEV_INFO_t     *dev_info, 
                                    idt_rt_probe_in_t  *in_parms, 
                                    idt_rt_probe_out_t *out_parms ) 
{
    uint8_t idx;
    uint32_t rte = 0;

	if (NULL == dev_info)
		return;

    if (tt_dev16 == in_parms->tt) {
       idx = (uint8_t)((in_parms->destID & (uint16_t)(0xFF00)) >> 8);
       rte = in_parms->rt->dom_table[idx].rte_val;
    }

    if ((tt_dev8 == in_parms->tt) || (IDT_DSF_RT_USE_DEVICE_TABLE == rte)) {
       idx = (uint8_t)(in_parms->destID & 0x00FF) ;
       rte = in_parms->rt->dev_table[idx].rte_val;
    }

    out_parms->routing_table_value = rte;
    out_parms->valid_route         = true;
    out_parms->reason_for_discard  = idt_rt_disc_not;

    if (in_parms->rt->default_route >= NUM_RXS_PORTS(dev_info)) {
        out_parms->valid_route         = false;
        out_parms->reason_for_discard  = idt_rt_disc_dflt_pt_invalid;
    }
   
    return;
}

uint32_t idt_rxs_rt_probe( DAR_DEV_INFO_t      *dev_info,
                           idt_rt_probe_in_t   *in_parms,
                           idt_rt_probe_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t bit;
    uint32_t regVal;

    out_parms->imp_rc                 = RIO_SUCCESS;
    out_parms->valid_route            = false;
    out_parms->routing_table_value    = RIO_ALL_PORTS;
    out_parms->filter_function_active = false; /* not supported on RXS */
    out_parms->trace_function_active  = false; /* not supported on RXS */

    for (bit = 0; bit < NUM_RXS_PORTS(dev_info); bit++)
        out_parms->mcast_ports[bit] = false;
    out_parms->reason_for_discard     = idt_rt_disc_probe_abort;

    if (   ((NUM_RXS_PORTS(dev_info) <= in_parms->probe_on_port) &&
            (RIO_ALL_PORTS       != in_parms->probe_on_port))  ||
           ( !in_parms->rt           ) ) {
       out_parms->imp_rc = RT_PROBE(0x11);
       goto exit;
    }
        
    rc = DARRegRead( dev_info, RXS_RIO_PKT_TIME_LIVE, &regVal ) ;
    if ( RIO_SUCCESS != rc ) {
       out_parms->imp_rc = RT_PROBE(0x12);
       goto exit;
    }
    out_parms->time_to_live_active = (regVal & RXS_RIO_PKT_TIME_LIVE_PKT_TIME_LIVE)?true:false;

    rc = RIO_SUCCESS;

    // Note, no failure possible...
    idt_rxs_check_multicast_routing(dev_info, in_parms, out_parms);

    /* Done if hit in multicast masks. */
    if (RIO_ALL_PORTS != out_parms->routing_table_value) 
       goto exit; 

    /*  Determine routing table value for the specified destination ID.
     *  If out_parms->valid_route is true 
     *  the valid values for out_parms->routing_table_value are
     *  - a valid port number, OR
     *  - IDT_DSF_RT_USE_DEFAULT_ROUTE
     *  When out_parms->routing_table_value is IDT_DSF_RT_USE_DEFAULT_ROUTE, the
     *  default route is a valid switch port number.
     */

    idt_rxs_check_unicast_routing( dev_info, in_parms, out_parms );

    if (out_parms->valid_route) {
       rc = idt_rxs_check_port_for_discard( dev_info, in_parms, out_parms );
    }
    
exit:
    return rc;
}

uint32_t idt_rxs_read_mc_masks( DAR_DEV_INFO_t            *dev_info,
                                uint8_t                    pnum,
                                idt_rt_state_t            *rt,
                                uint32_t                  *imp_rc )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mask_idx;
   uint32_t reg_val, port_mask;
   idt_rt_dealloc_mc_mask_in_t  d_in_parm;
   idt_rt_dealloc_mc_mask_out_t d_out_parm;

   uint32_t vend_id = dev_info->devID & RXS_RIO_DEV_IDENT_VEND;
   uint32_t dev_id = (dev_info->devID & RXS_RIO_DEV_IDENT_DEVI) >> 16;

   if (RXS_RIO_DEVICE_VENDOR != vend_id)
	   goto exit;

   switch (dev_id) {
   case IDT_RXS2448_RIO_DEVICE_ID:
	   port_mask = RXS2448_RIO_SPX_MC_Y_S_CSR_SET;
	   break;
   case IDT_RXS1632_RIO_DEVICE_ID:
	   port_mask = RXS1632_RIO_SPX_MC_Y_S_CSR_SET;
	   break;
   default:
	   goto exit;
   }

   d_in_parm.rt = rt;
   for (mask_idx = NUM_MC_MASKS(dev_info); mask_idx < IDT_DSF_MAX_MC_MASK; mask_idx++ )
   {
      d_in_parm.mc_mask_rte = IDT_DSF_FIRST_MC_MASK + mask_idx;
      rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &d_in_parm, &d_out_parm );
      if (RIO_SUCCESS != rc) 
      {
         *imp_rc = d_out_parm.imp_rc;
         goto exit;
      }
   }

   for (mask_idx = 0; mask_idx < NUM_MC_MASKS(dev_info); mask_idx++)  {
      rc = DARRegRead(dev_info, RXS_RIO_SPX_MC_Y_S_CSR(pnum, mask_idx), &reg_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = IDT_RXS_READ_MC_MASKS(1);
         goto exit;
      }

      rt->mc_masks[mask_idx].allocd    = false;
      rt->mc_masks[mask_idx].changed   = false;
      rt->mc_masks[mask_idx].tt        = tt_dev8;
      rt->mc_masks[mask_idx].in_use    = false;
      rt->mc_masks[mask_idx].mc_destID = 0x0;
      rt->mc_masks[mask_idx].mc_mask   = reg_val & port_mask;
   }

exit:
   return rc;
}

// Make sure that we're not orphaning a multicast mask...

uint32_t idt_rxs_tidy_routing_table( DAR_DEV_INFO_t  *dev_info, 
                                     uint8_t          idx, 
                                     idt_rt_state_t  *rt,
                                     uint32_t        *fail_pt,
                                     bool             is_dev_table )
{
   uint32_t rc = RIO_SUCCESS;
   uint16_t srch_idx, dev_idx, dom_idx;
   bool found_one = false;

   if (is_dev_table) {
      dev_idx = idx;
      if ((rt->dev_table[dev_idx].rte_val >= IDT_DSF_FIRST_MC_MASK) &&
          (rt->dev_table[dev_idx].rte_val < IDT_DSF_BAD_MC_MASK)) {
         for (srch_idx = 0; (srch_idx < IDT_DAR_RT_DEV_TABLE_SIZE) && !found_one; srch_idx++) {
             if (dev_idx == srch_idx)
                continue;
             if (rt->dev_table[dev_idx].rte_val == rt->dev_table[srch_idx].rte_val)
                found_one = true;
         }

         if (!found_one) {
            idt_rt_dealloc_mc_mask_in_t  in_parms;
            idt_rt_dealloc_mc_mask_out_t out_parms;
            in_parms.rt = rt;
            in_parms.mc_mask_rte = rt->dev_table[dev_idx].rte_val;
            rc = IDT_DSF_rt_dealloc_mc_mask(dev_info, &in_parms, &out_parms);
            if (RIO_SUCCESS != rc) {
               *fail_pt = out_parms.imp_rc;
            }
         }
      }
   }
   else {
      dom_idx = idx;
      if ((rt->dom_table[dom_idx].rte_val >= IDT_DSF_FIRST_MC_MASK) &&
          (rt->dom_table[dom_idx].rte_val < IDT_DSF_BAD_MC_MASK)) {
         for (srch_idx = 0; (srch_idx < IDT_DAR_RT_DEV_TABLE_SIZE) && !found_one; srch_idx++) {
             if (dom_idx == srch_idx)
                continue;
             if (rt->dom_table[dom_idx].rte_val == rt->dom_table[srch_idx].rte_val)
                found_one = true;
         }

         if (!found_one) {
            idt_rt_dealloc_mc_mask_in_t  in_parms;
            idt_rt_dealloc_mc_mask_out_t out_parms;
            in_parms.rt = rt;
            in_parms.mc_mask_rte = rt->dom_table[dom_idx].rte_val;
            rc = IDT_DSF_rt_dealloc_mc_mask(dev_info, &in_parms, &out_parms);
            if (RIO_SUCCESS != rc) {
               *fail_pt = out_parms.imp_rc;
            }
         }
      }
   }
   return rc;
}

/* This function updates an idt_rt_state_t structure to
 * change a multicast mask value, and tracks changes.
 */
uint32_t idt_rxs_rt_change_mc_mask( DAR_DEV_INFO_t               *dev_info,
                                    idt_rt_change_mc_mask_in_t   *in_parms, 
                                    idt_rt_change_mc_mask_out_t  *out_parms )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  chg_idx, dom_idx, dev_idx;
   uint32_t illegal_ports   = ~((1 << IDT_MAX_PORTS      ) - 1);
   uint32_t avail_ports     =   (1 << NUM_RXS_PORTS(dev_info)) - 1;

   out_parms->imp_rc = RIO_SUCCESS;

   if (!in_parms->rt) {
      out_parms->imp_rc = CHANGE_MC_MASK(1);
      goto exit;
   }

   // Check destination ID value against tt, and that the multicast mask
   // does not select ports which do not exist on the RXS device.
   if ( (in_parms->mc_info.mc_destID > IDT_LAST_DEV16_DESTID   ) ||
        ((in_parms->mc_info.mc_destID > IDT_LAST_DEV8_DESTID) &&
         (tt_dev8 == in_parms->mc_info.tt                     )) ||
        (in_parms->mc_info.mc_mask & illegal_ports             )) {
      out_parms->imp_rc = CHANGE_MC_MASK(2);
      goto exit;
   }
 
   if (!in_parms->mc_info.in_use) {
      idt_rt_dealloc_mc_mask_in_t  d_in_parm;
      idt_rt_dealloc_mc_mask_out_t d_out_parm;

      d_in_parm.mc_mask_rte = in_parms->mc_mask_rte;
      d_in_parm.rt          = in_parms->rt;

      rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &d_in_parm, &d_out_parm );
      if (RIO_SUCCESS != rc) {
	 out_parms->imp_rc = d_out_parm.imp_rc;
      }
      goto exit;
   }

   // Allow requests to change masks not supported by RXS family
   // but there's nothing to do...

   chg_idx = MC_MASK_IDX_FROM_ROUTE(in_parms->mc_mask_rte);
   
   if (chg_idx >= NUM_MC_MASKS(dev_info)) {
      rc = RIO_ERR_INVALID_PARAMETER;
      out_parms->imp_rc = CHANGE_MC_MASK(3);
      goto exit;
   }

   // If entry has not already been changed, see if it is being changed
   if ( !in_parms->rt->mc_masks[chg_idx].changed ) {
      if ((in_parms->rt->mc_masks[chg_idx].mc_mask != in_parms->mc_info.mc_mask) ||
          (in_parms->rt->mc_masks[chg_idx].in_use  != in_parms->mc_info.in_use ))  {
         in_parms->rt->mc_masks[chg_idx].changed = true;
      }
   }

   // Note: The multicast mask must be in use now.  We must make sure that
   // the routing tables are set appropriately.
   dom_idx = (in_parms->mc_info.mc_destID & 0xFF00) >> 8;
   if ((tt_dev16 == in_parms->mc_info.tt) && (dom_idx) 
       && (in_parms->mc_mask_rte != in_parms->rt->dom_table[dom_idx].rte_val)) {
      rc = idt_rxs_tidy_routing_table(dev_info, dom_idx, in_parms->rt, &out_parms->imp_rc, false);
      if (RIO_SUCCESS != rc)
         goto exit; 

      in_parms->rt->dom_table[dom_idx].rte_val = in_parms->mc_mask_rte;
      in_parms->rt->dom_table[dom_idx].changed = true;
   }

   dev_idx = (in_parms->mc_info.mc_destID & 0x00FF);
   if (in_parms->mc_mask_rte != in_parms->rt->dev_table[dev_idx].rte_val) {
      rc = idt_rxs_tidy_routing_table(dev_info, dev_idx, in_parms->rt, &out_parms->imp_rc, true);
      if (RIO_SUCCESS != rc) 
         goto exit;

      in_parms->rt->dev_table[dev_idx].rte_val = in_parms->mc_mask_rte;
      in_parms->rt->dev_table[dev_idx].changed = true;
   }
	 
   in_parms->rt->mc_masks[chg_idx].in_use    = true                       ;
   in_parms->rt->mc_masks[chg_idx].mc_destID = in_parms->mc_info.mc_destID;
   in_parms->rt->mc_masks[chg_idx].tt        = in_parms->mc_info.tt       ;
   in_parms->rt->mc_masks[chg_idx].mc_mask   = (in_parms->mc_info.mc_mask & avail_ports);

   rc = RIO_SUCCESS;
exit:
   return rc;
}

uint32_t idt_rxs_read_rte_entries( DAR_DEV_INFO_t            *dev_info,
                                   uint8_t                    pnum,
                                   idt_rt_state_t            *rt,
                                   uint32_t                  *imp_rc )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t destID, rte_val, first_mc_destID;
   bool  found_one = false;

   // Fill in default route value
   
   rc = DARRegRead(dev_info, RXS_RIO_ROUTE_DFLT_PORT, &rte_val);
   if (RIO_SUCCESS != rc) {
      *imp_rc = IDT_RXS_READ_RTE_ENTRIES(1);
      goto exit;
   }

   rt->default_route = (uint8_t)(rte_val & RXS_RIO_ROUTE_DFLT_PORT_DEFAULT_OUT_PORT);
   if ( rt->default_route >= NUM_RXS_PORTS(dev_info)) {
      rt->default_route = IDT_DSF_RT_NO_ROUTE;
   }

   // Read all of the domain routing table entries.
   rt->dom_table[0].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;
   rt->dom_table[0].changed = false;
   first_mc_destID = 0;

   for (destID = 1; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
   {
      rt->dom_table[destID].changed = false;

      // Read routing table entry for deviceID
      rc = DARRegRead(dev_info, RXS_RIO_SPX_L1_GY_ENTRYZ_CSR(pnum, 0, destID), &rte_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = IDT_RXS_READ_RTE_ENTRIES(4);
         goto exit;
      }
      rte_val &= RXS_RIO_BC_L1_GX_ENTRYY_CSR_ROUTING_VALUE;
      rt->dom_table[destID].rte_val = (uint8_t)(rte_val);

      if (IDT_DSF_RT_USE_DEVICE_TABLE == rte_val) {
         if (!found_one) {
            first_mc_destID = (uint16_t)(destID) << 8;
            found_one = true;
         }
      } else {
        if ((IDT_DSF_RT_USE_DEFAULT_ROUTE != rte_val) &&
            (IDT_DSF_RT_NO_ROUTE          != rte_val) &&
            (NUM_RXS_PORTS(dev_info)          <= rte_val) ) { 
            rt->dom_table[destID].rte_val = IDT_DSF_RT_NO_ROUTE;
        }
      }
   }
   
   // Read all of the device routing table entries.
   // Update multicast entries as we go...
   for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
   {
      uint32_t mask_idx;

      rt->dev_table[destID].changed = false;
      rc = DARRegRead(dev_info, RXS_RIO_BC_L2_GX_ENTRYY_CSR(pnum, destID), &rte_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = IDT_RXS_READ_RTE_ENTRIES(8);
         goto exit;
      }

      rte_val &= RXS_RIO_BC_L2_GX_ENTRYY_CSR_ROUTING_VALUE;

      rt->dev_table[destID].rte_val = (uint32_t)(rte_val);

      mask_idx = MC_MASK_IDX_FROM_ROUTE(rte_val);
      if ((IDT_DSF_BAD_MC_MASK != mask_idx) && !(rt->mc_masks[mask_idx].in_use)) {
         rt->mc_masks[mask_idx].tt        = tt_dev16;
         rt->mc_masks[mask_idx].in_use    = true;
         rt->mc_masks[mask_idx].mc_destID = first_mc_destID + destID;
      }
         
      if (  ((rte_val >= NUM_RXS_PORTS(dev_info)) && (rte_val < IDT_DSF_FIRST_MC_MASK))    ||
            ((rte_val >= IDT_DSF_BAD_MC_MASK) && (IDT_DSF_RT_NO_ROUTE         != rte_val) 
                                              && (IDT_DSF_RT_USE_DEFAULT_ROUTE != rte_val)) ) {
         rt->dev_table[destID].rte_val = IDT_DSF_RT_NO_ROUTE;
      }
   }
   
exit:
   return rc;
}

/* This function returns the complete hardware state of packet routing
 * in a routing table state structure.
 *
 * The routing table hardware must be initialized using idt_rt_initialize() 
 * before calling this routine.
 */
uint32_t idt_rxs_rt_probe_all( DAR_DEV_INFO_t          *dev_info,
                               idt_rt_probe_all_in_t   *in_parms,
                               idt_rt_probe_all_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  probe_port;

    out_parms->imp_rc = RIO_SUCCESS;
    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->probe_on_port ) && 
           ( in_parms->probe_on_port >= NUM_RXS_PORTS(dev_info)    ) ) ||
         ( !in_parms->rt) ) 
    {
        out_parms->imp_rc = RT_PROBE_ALL(1);
        goto exit;
    }

    probe_port = (RIO_ALL_PORTS == in_parms->probe_on_port)?0:in_parms->probe_on_port;

    rc = idt_rxs_read_mc_masks( dev_info, probe_port, in_parms->rt, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc)
       goto exit;
    
    rc = idt_rxs_read_rte_entries( dev_info, probe_port, in_parms->rt, &out_parms->imp_rc );

exit:
    return rc;
}

uint32_t idt_rxs_rioSetEnumBound( DAR_DEV_INFO_t *dev_info,
                                  struct DAR_ptl *ptl,
			          int             enum_bnd_val )
{
	if (NULL != dev_info || !ptl || enum_bnd_val)
		return RIO_SUCCESS;

	return RIO_SUCCESS;
}

uint32_t idt_rxs_pc_dev_reset_config( DAR_DEV_INFO_t                 *dev_info,
	                              idt_pc_dev_reset_config_in_t   *in_parms,
	                              idt_pc_dev_reset_config_out_t  *out_parms )
{
	if (NULL != dev_info)
		out_parms->rst = in_parms->rst;

	return RIO_SUCCESS;
}

uint32_t idt_rxs_em_cfg_pw( DAR_DEV_INFO_t       *dev_info,
	                    idt_em_cfg_pw_in_t   *in_parms,
	                    idt_em_cfg_pw_out_t  *out_parms )
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->imp_rc;

	return RIO_SUCCESS;
}

uint32_t idt_rxs_em_dev_rpt_ctl( DAR_DEV_INFO_t            *dev_info,
	                         idt_em_dev_rpt_ctl_in_t   *in_parms,
	                         idt_em_dev_rpt_ctl_out_t  *out_parms )
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->ptl.num_ports;

	return RIO_SUCCESS;
}

uint32_t idt_rxs_DeviceSupported( DAR_DEV_INFO_t *DAR_info )
{
	uint32_t rc = DAR_DB_NO_DRIVER;

	if (RXS_RIO_DEVICE_VENDOR == (DAR_info->devID & RIO_DEV_IDENT_VEND))
	{
		if ((IDT_RXS2448_RIO_DEVICE_ID) == ((DAR_info->devID & RIO_DEV_IDENT_DEVI) >> 16))
		{
			/* Now fill out the DAR_info structure... */
			rc = DARDB_rioDeviceSupportedDefault(DAR_info);

			/* Index and information for DSF is the same as the DAR handle */
			DAR_info->dsf_h = RXS_driver_handle;

			if (rc == RIO_SUCCESS) {
				num_RXS_driver_instances++;
				strncpy(DAR_info->name, "RXS2448", sizeof(DAR_info->name));
			}
		}
		else if ((IDT_RXS1632_RIO_DEVICE_ID) == ((DAR_info->devID & RIO_DEV_IDENT_DEVI) >> 16))
                {
                        /* Now fill out the DAR_info structure... */
                        rc = DARDB_rioDeviceSupportedDefault(DAR_info);

                        /* Index and information for DSF is the same as the DAR handle */
                        DAR_info->dsf_h = RXS_driver_handle;

                        if (rc == RIO_SUCCESS) {
                                num_RXS_driver_instances++;
                                strncpy(DAR_info->name, "RXS1632", sizeof(DAR_info->name));
                        }
                }
	}
	return rc;
}

uint32_t bind_rxs_DAR_support(void)
{
	DAR_DB_Driver_t DAR_info;

	DARDB_Init_Driver_Info(IDT_TSI_VENDOR_ID, &DAR_info);

	DAR_info.rioDeviceSupported = idt_rxs_DeviceSupported;

	DAR_info.rioSetEnumBound = idt_rxs_rioSetEnumBound;

	DARDB_Bind_Driver(&DAR_info);

	return RIO_SUCCESS;
}

/* Routine to bind in all RXSs specific Device Specific Function routines.
 * Supports RXSs
 */

uint32_t bind_rxs_DSF_support(void)
{
	IDT_DSF_DB_t idt_driver;

	IDT_DSF_init_driver(&idt_driver);

	idt_driver.dev_type = IDT_RXSx_RIO_DEVICE_ID;

	idt_driver.idt_pc_get_config = idt_rxs_pc_get_config;
	idt_driver.idt_pc_get_status = idt_rxs_pc_get_status;
	idt_driver.idt_pc_dev_reset_config = idt_rxs_pc_dev_reset_config;

	idt_driver.idt_rt_initialize = idt_rxs_rt_initialize;
	idt_driver.idt_rt_probe = idt_rxs_rt_probe;
	idt_driver.idt_rt_probe_all = idt_rxs_rt_probe_all;
	idt_driver.idt_rt_set_all = idt_rxs_rt_set_all;
        idt_driver.idt_rt_set_changed = idt_rxs_rt_set_changed;
        idt_driver.idt_rt_change_rte = idt_rxs_rt_change_rte;
        idt_driver.idt_rt_alloc_mc_mask = IDT_DSF_rt_alloc_mc_mask;
        idt_driver.idt_rt_dealloc_mc_mask = IDT_DSF_rt_dealloc_mc_mask;
        idt_driver.idt_rt_change_mc_mask = idt_rxs_rt_change_mc_mask;

        idt_driver.idt_sc_init_dev_ctrs = idt_rxs_sc_init_dev_ctrs;
	idt_driver.idt_sc_read_ctrs = idt_rxs_sc_read_ctrs;

	idt_driver.idt_em_dev_rpt_ctl = idt_rxs_em_dev_rpt_ctl;
	idt_driver.idt_em_cfg_pw = idt_rxs_em_cfg_pw;

	IDT_DSF_bind_driver(&idt_driver, &RXS_driver_handle);

	return RIO_SUCCESS;
}


#ifdef __cplusplus
}
#endif

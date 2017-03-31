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

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "rio_misc.h"
#include "RapidIO_Device_Access_Routines_API.h"
#include "RapidIO_Error_Management_API.h"
#include "RapidIO_Port_Config_API.h"
#include "RapidIO_Routing_Table_API.h"
#include "RapidIO_Statistics_Counter_API.h"
#include "RapidIO_Utilities_API.h"

#include "DAR_DB_Private.h"
#include "DSF_DB_Private.h"

#include "Tsi57x_API.h"
#include "Tsi721_API.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t DARDB_ReadRegNoDriver(DAR_DEV_INFO_t *dev_info, uint32_t UNUSED(offset),
		uint32_t *UNUSED(readdata))
{
	if (!VALIDATE_DEV_INFO(dev_info)) {
		return DAR_DB_INVALID_HANDLE;
	}
	return DAR_DB_NO_DRIVER;
}

uint32_t DARDB_WriteRegNoDriver(DAR_DEV_INFO_t *dev_info, uint32_t UNUSED(offset),
		uint32_t UNUSED(writedata))
{
	if (!VALIDATE_DEV_INFO(dev_info)) {
		return DAR_DB_INVALID_HANDLE;
	}
	return DAR_DB_NO_DRIVER;
}

// Error Management
//
uint32_t DSF_rio_em_cfg_pw(DAR_DEV_INFO_t *dev_info,
		rio_em_cfg_pw_in_t *in_parms, rio_em_cfg_pw_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_cfg_set(DAR_DEV_INFO_t *dev_info,
		rio_em_cfg_set_in_t *in_parms, rio_em_cfg_set_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_cfg_get(DAR_DEV_INFO_t *dev_info,
		rio_em_cfg_get_in_t *in_parms, rio_em_cfg_get_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_dev_rpt_ctl(DAR_DEV_INFO_t *dev_info,
		rio_em_dev_rpt_ctl_in_t *in_parms,
		rio_em_dev_rpt_ctl_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_parse_pw(DAR_DEV_INFO_t *dev_info,
		rio_em_parse_pw_in_t *in_parms,
		rio_em_parse_pw_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_get_int_stat(DAR_DEV_INFO_t *dev_info,
		rio_em_get_int_stat_in_t *in_parms,
		rio_em_get_int_stat_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_get_pw_stat(DAR_DEV_INFO_t *dev_info,
		rio_em_get_pw_stat_in_t *in_parms,
		rio_em_get_pw_stat_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_clr_events(DAR_DEV_INFO_t *dev_info,
		rio_em_clr_events_in_t *in_parms,
		rio_em_clr_events_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_em_create_events(DAR_DEV_INFO_t *dev_info,
		rio_em_create_events_in_t *in_parms,
		rio_em_create_events_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

// Port Config
//
uint32_t DSF_rio_pc_get_config(DAR_DEV_INFO_t *dev_info,
		rio_pc_get_config_in_t *in_parms,
		rio_pc_get_config_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_set_config(DAR_DEV_INFO_t *dev_info,
		rio_pc_set_config_in_t *in_parms,
		rio_pc_set_config_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_get_status(DAR_DEV_INFO_t *dev_info,
		rio_pc_get_status_in_t *in_parms,
		rio_pc_get_status_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_reset_port(DAR_DEV_INFO_t *dev_info,
		rio_pc_reset_port_in_t *in_parms,
		rio_pc_reset_port_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_reset_link_partner(DAR_DEV_INFO_t *dev_info,
		rio_pc_reset_link_partner_in_t *in_parms,
		rio_pc_reset_link_partner_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_clr_errs(DAR_DEV_INFO_t *dev_info,
		rio_pc_clr_errs_in_t *in_parms,
		rio_pc_clr_errs_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_secure_port(DAR_DEV_INFO_t *dev_info,
		rio_pc_secure_port_in_t *in_parms,
		rio_pc_secure_port_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_dev_reset_config(DAR_DEV_INFO_t *dev_info,
		rio_pc_dev_reset_config_in_t *in_parms,
		rio_pc_dev_reset_config_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_pc_probe(DAR_DEV_INFO_t *dev_info,
		rio_pc_probe_in_t *in_parms, rio_pc_probe_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t default_rio_pc_probe(DAR_DEV_INFO_t *dev_info,
		rio_pc_probe_in_t *in_parms, rio_pc_probe_out_t *out_parms)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint32_t regVal, regVal2;

	rio_pc_get_status_in_t stat_in;
	rio_pc_get_status_out_t stat_out;
	rio_pc_get_config_in_t cfg_in;
	rio_pc_get_config_out_t cfg_out;

	out_parms->status = port_los;

	NULL_CHECK;

	if (in_parms->port >= NUM_PORTS(dev_info)) {
		out_parms->imp_rc = PC_PROBE(1);
		goto exit;
	}

	out_parms->imp_rc = RIO_SUCCESS;

	stat_in.ptl.num_ports = 1;
	stat_in.ptl.pnums[0] = in_parms->port;
	rc = rio_pc_get_status(dev_info, &stat_in, &stat_out);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = stat_out.imp_rc;
		goto exit;
	}

	if (!(stat_out.ps[0].port_ok)) {
		out_parms->imp_rc = PC_PROBE(8);
		goto exit;
	}

	cfg_in.ptl.num_ports = 1;
	cfg_in.ptl.pnums[0] = in_parms->port;
	rc = rio_pc_get_config(dev_info, &cfg_in, &cfg_out);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = cfg_out.imp_rc;
		goto exit;
	}

	if (cfg_out.pc[0].pw == stat_out.ps[0].pw) {
		out_parms->imp_rc = PC_PROBE(0x11);
		out_parms->status = port_ok;
	} else {
		out_parms->imp_rc = PC_PROBE(0x12);
		out_parms->status = port_degr;
	}

	rc = DARRegRead(dev_info,
			RIO_SPX_ERR_STAT(dev_info->extFPtrForPort,
					dev_info->extFPtrPortType,
					in_parms->port), &regVal);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_PROBE(0x19);
		goto exit;
	}

	if (regVal & (RIO_SPX_ERR_STAT_ERR |
	RIO_SPX_ERR_STAT_IES |
	RIO_SPX_ERR_STAT_IRS |
	RIO_SPX_ERR_STAT_OES |
	RIO_SPX_ERR_STAT_ORS)) {
		out_parms->imp_rc = PC_PROBE(0x20);
		out_parms->status = port_err;
		goto exit;
	}

	if (regVal & (RIO_SPX_ERR_STAT_FAIL |
	RIO_SPX_ERR_STAT_DROP)) {
		out_parms->imp_rc = PC_PROBE(0x30);
		out_parms->status = port_err;
		goto exit;
	}

	rc = DARRegRead(dev_info,
			RIO_SPX_ACKID_ST(dev_info->extFPtrForPort,
					dev_info->extFPtrPortType,
					in_parms->port), &regVal);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_PROBE(0x40);
		goto exit;
	}

	// NOTE: If ackIDs aren't in sync, querying the link partner's ackID
	// will cause an output error-stopped condition and a PORT_FAIL condition
	// on CPS devices.

	rc = DARRegWrite(dev_info,
			RIO_SPX_LM_REQ(dev_info->extFPtrForPort,
					dev_info->extFPtrPortType,
					in_parms->port),
			RIO_SPX_LM_REQ_CMD_LR_IS);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_PROBE(0x50);
		goto exit;
	}

	rc = DARRegRead(dev_info,
			RIO_SPX_LM_RESP(dev_info->extFPtrForPort,
					dev_info->extFPtrPortType,
					in_parms->port), &regVal2);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_PROBE(0x51);
		goto exit;
	}

	if (!(regVal2 & RIO_SPX_LM_RESP_VLD)) {
		rc = RIO_ERR_SW_FAILURE;
		out_parms->imp_rc = PC_PROBE(0x52);
		goto exit;
	}

	if (((regVal2 & RIO_SPX_LM_RESP_ACK_ID3) >> 5)
			!= (regVal & RIO_SPX_ACKID_ST_OUTB)) {
		out_parms->imp_rc = PC_PROBE(0x70);
		out_parms->status = port_err;
		goto exit;
	}

exit:
	return rc;
}


// Routing Table
//
uint32_t DSF_rio_rt_initialize(DAR_DEV_INFO_t *dev_info,
		rio_rt_initialize_in_t *in_parms,
		rio_rt_initialize_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_probe(DAR_DEV_INFO_t *dev_info, rio_rt_probe_in_t *in_parms,
		rio_rt_probe_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_probe_all(DAR_DEV_INFO_t *dev_info,
		rio_rt_probe_all_in_t *in_parms,
		rio_rt_probe_all_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_set_all(DAR_DEV_INFO_t *dev_info,
		rio_rt_set_all_in_t *in_parms, rio_rt_set_all_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_set_changed(DAR_DEV_INFO_t *dev_info,
		rio_rt_set_changed_in_t *in_parms,
		rio_rt_set_changed_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_default_alloc_mc_mask(DAR_DEV_INFO_t *dev_info,
		rio_rt_alloc_mc_mask_in_t *in_parms,
		rio_rt_alloc_mc_mask_out_t *out_parms,
		uint32_t mc_mask_cnt)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint32_t mc_idx;

	NULL_CHECK;

	if (!in_parms->rt) {
		out_parms->imp_rc = RT_ALLOC_MC_MASK(1);
		goto exit;
	}

	for (mc_idx = 0; mc_idx < mc_mask_cnt; mc_idx++) {
		if (!in_parms->rt->mc_masks[mc_idx].in_use
				&& !in_parms->rt->mc_masks[mc_idx].allocd) {
			out_parms->mc_mask_rte = RIO_RTV_MC_MSK(mc_idx);
			out_parms->imp_rc = RIO_SUCCESS;
			in_parms->rt->mc_masks[mc_idx].allocd = true;
			rc = RIO_SUCCESS;
			break;
		}
	}

	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = RT_ALLOC_MC_MASK(2);
		out_parms->mc_mask_rte = RIO_RTE_BAD;
		rc = RIO_ERR_INSUFFICIENT_RESOURCES;
	}

exit:
	return rc;
}

uint32_t DSF_rio_rt_alloc_mc_mask(DAR_DEV_INFO_t *dev_info,
		rio_rt_alloc_mc_mask_in_t *in_parms,
		rio_rt_alloc_mc_mask_out_t *out_parms)
{
	return DSF_rio_rt_default_alloc_mc_mask(dev_info, in_parms, out_parms,
				NUM_MC_MASKS(dev_info));
}

uint32_t DSF_rio_rt_dealloc_mc_mask(DAR_DEV_INFO_t *dev_info,
		rio_rt_dealloc_mc_mask_in_t *in_parms,
		rio_rt_dealloc_mc_mask_out_t *out_parms)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	pe_rt_val mc_idx;
	pe_rt_val dev_rte, dom_rte;
	rio_rt_mc_info_t *mc_mask;

	out_parms->imp_rc = RIO_SUCCESS;

	NULL_CHECK;

	if (!in_parms->rt) {
		out_parms->imp_rc = RT_DEALLOC_MC_MASK(1);
		goto exit;
	}

	mc_idx = RIO_RTV_GET_MC_MSK(in_parms->mc_mask_rte);

	if (mc_idx >= RIO_MAX_MC_MASKS) {
		out_parms->imp_rc = RT_DEALLOC_MC_MASK(2);
		goto exit;
	}

	rc = RIO_SUCCESS;

	for (dev_rte = 0; dev_rte < RIO_RT_GRP_SZ; dev_rte++) {
		if (in_parms->rt->dev_table[dev_rte].rte_val
				== in_parms->mc_mask_rte) {
			in_parms->rt->dev_table[dev_rte].changed = true;
			in_parms->rt->dev_table[dev_rte].rte_val = RIO_RTE_DROP;
		}
	}

	for (dom_rte = 0; dom_rte < RIO_RT_GRP_SZ; dom_rte++) {
		if (in_parms->rt->dom_table[dom_rte].rte_val
				== in_parms->mc_mask_rte) {
			in_parms->rt->dom_table[dom_rte].changed = true;
			in_parms->rt->dom_table[dom_rte].rte_val = RIO_RTE_DROP;
		}
	}

	if (in_parms->rt->mc_masks[mc_idx].in_use) {
		dev_rte = in_parms->rt->mc_masks[mc_idx].mc_destID & 0x00FF;
		in_parms->rt->dev_table[dev_rte].changed = true;
		in_parms->rt->dev_table[dev_rte].rte_val = RIO_RTE_DROP;
	}

	mc_mask = &in_parms->rt->mc_masks[mc_idx];

	if (mc_mask->in_use || mc_mask->allocd) {
		in_parms->rt->mc_masks[mc_idx].changed = mc_mask->in_use;
		in_parms->rt->mc_masks[mc_idx].mc_destID = 0;
		in_parms->rt->mc_masks[mc_idx].tt = tt_dev8;
		in_parms->rt->mc_masks[mc_idx].mc_mask = 0;
		in_parms->rt->mc_masks[mc_idx].in_use = false;
		in_parms->rt->mc_masks[mc_idx].allocd = false;
	}

exit:
	return rc;
}

uint32_t DSF_rio_rt_change_rte(DAR_DEV_INFO_t *dev_info,
		rio_rt_change_rte_in_t *in_parms,
		rio_rt_change_rte_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_rt_change_mc_mask(DAR_DEV_INFO_t *dev_info,
		rio_rt_change_mc_mask_in_t *in_parms,
		rio_rt_change_mc_mask_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

// Statistics Counter
//
uint32_t DSF_rio_sc_init_dev_ctrs(DAR_DEV_INFO_t *dev_info,
		rio_sc_init_dev_ctrs_in_t *in_parms,
		rio_sc_init_dev_ctrs_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t DSF_rio_sc_read_ctrs(DAR_DEV_INFO_t *dev_info,
		rio_sc_read_ctrs_in_t *in_parms,
		rio_sc_read_ctrs_out_t *out_parms)
{
	NULL_CHECK;
	return RIO_STUBBED;
}

uint32_t RIO_bind_procs(
		uint32_t (*ReadRegCall)(DAR_DEV_INFO_t *dev_info,
				uint32_t offset, uint32_t *readdata),
		uint32_t (*WriteRegCall)(DAR_DEV_INFO_t *dev_info,
				uint32_t offset, uint32_t writedata),
		void (*WaitSecCall)(uint32_t delay_nsec, uint32_t delay_sec))
{
	DAR_proc_ptr_init(ReadRegCall, WriteRegCall, WaitSecCall);
	return RIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

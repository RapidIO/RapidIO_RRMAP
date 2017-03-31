/* Implementation for libriocp_pe register read/write driver  and	 */
/* libriocp_pe PE driver based on librio and libmport.	      */
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>

#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "string_util.h"
#include "rio_ecosystem.h"
#include "rio_standard.h"
#include "liblog.h"
#include "pe_mpdrv_private.h"
#include "riocp_pe_internal.h"
#include "comptag.h"
#include "RapidIO_Statistics_Counter_API.h"
#include "RapidIO_Device_Access_Routines_API.h"
#include "RapidIO_Routing_Table_API.h"
#include "cfg.h"
#include "Tsi578.h"
#include "RXS2448.h"
#include "rapidio_mport_mgmt.h"
#include "Tsi721.h"
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

int RIOCP_WU mpsw_drv_reg_rd(struct riocp_pe *pe, uint32_t offset, uint32_t *val)
{
	int ret;
	struct mpsw_drv_private_data *priv_ptr;

	DBG("ENTRY: offset 0x%x val 0x%x\n", offset, *val);

	ret = riocp_pe_handle_get_private(pe, (void **)&priv_ptr);
	if (ret) {
		ERR("Could not get private data: rc %d:%s\n", ret,
				strerror(ret));
		return ret;
	}

	ret = DARRegRead(&priv_ptr->dev_h, offset, val);
	if (ret) {
		ERR("Read Failed: offset 0x%x rc 0x%x\n", offset, ret);
		ret = EIO;
	}

	DBG("EXIT\n");
	return ret;
}

int RIOCP_WU mpsw_drv_reg_wr(struct riocp_pe *pe, uint32_t offset, uint32_t val)
{
	int ret;
	struct mpsw_drv_private_data *priv_ptr = NULL;

	DBG("ENTRY: offset 0x%x val 0x%x\n", offset, val);

	ret = riocp_pe_handle_get_private(pe, (void **)&priv_ptr);
	if (ret) {
		ERR("Could not get private data: rc %d:%s\n", -ret,
				strerror(-ret));
		return -ret;
	}

	if ((RIO_DEVID == offset) && RIOCP_PE_IS_MPORT(pe)) {
		struct mpsw_drv_pe_acc_info *p_acc;
		uint16_t dev8_did = ((val >> 16) & 0xFF);
		p_acc =
				(struct mpsw_drv_pe_acc_info *)priv_ptr->dev_h.accessInfo;
		ret = riomp_mgmt_destid_set(p_acc->maint, dev8_did);
		if (ret) {
			return -ret;
		}
	}

	ret = DARRegWrite(&priv_ptr->dev_h, offset, val);
	if (ret) {
		ERR("Write Failed: offset 0x%x rc 0x%x\n", offset, ret);
	}

	DBG("EXIT\n");
	return 0;
}

int RIOCP_WU mpsw_drv_raw_reg_wr(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t addr, uint32_t val)
{
	int rc;
	struct mpsw_drv_pe_acc_info *p_acc = NULL;

	if (!RIOCP_PE_IS_HOST(pe)) {
		return -ENOSYS;
	}

	p_acc = (struct mpsw_drv_pe_acc_info *)pe->mport->minfo->private_data;
	if (!p_acc->maint_valid) {
		return -EINVAL;
	}

	// if (RIOCP_PE_IS_MPORT(pe) && (did == pe->destid))
	if (RIOCP_PE_IS_MPORT(pe)) {
		rc = riomp_mgmt_lcfg_write(p_acc->maint, addr, 4, val);
	} else {
		rc = riomp_mgmt_rcfg_write(p_acc->maint, did_get_value(did), hc,
				addr, 4, val);
	}
	return rc;
}

int RIOCP_WU mpsw_drv_raw_reg_rd(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t addr, uint32_t *val)
{
	int rc;
	struct mpsw_drv_pe_acc_info *p_acc = NULL;

	p_acc = (struct mpsw_drv_pe_acc_info *)pe->mport->minfo->private_data;
	if (!p_acc->maint_valid) {
		return -EINVAL;
	}

	// if (RIOCP_PE_IS_MPORT(pe) && (did == pe->destid))
	if (RIOCP_PE_IS_MPORT(pe)) {
		rc = riomp_mgmt_lcfg_read(p_acc->maint, addr, 4, val);
	} else {
		rc = riomp_mgmt_rcfg_read(p_acc->maint, did_get_value(did), hc,
				addr, 4, val);
	}
	return rc;
}

#define MPSW_MAX_DEV_FN 255
#define MPSW_DFLT_DEV_DIR "/sys/bus/rapidio/devices/"

int mpsw_mport_dev_add(struct riocp_pe *pe, char *name)
{
	int rc = 0;
	struct mpsw_drv_pe_acc_info *p_acc = NULL;
	struct mpsw_drv_private_data *p_dat = NULL;
	char dev_fn[MPSW_MAX_DEV_FN + 1] = {0};

	if ((NULL == pe) || (NULL == name)) {
		return -EINVAL;
	}

	SAFE_STRNCPY(pe->sysfs_name, name, sizeof(pe->sysfs_name));

	if (RIOCP_PE_IS_MPORT(pe)) {
		return 0;
	}

	p_dat = (struct mpsw_drv_private_data *)pe->private_data;
	if (SWITCH(&p_dat->dev_h))
		return 0;

	p_acc = (struct mpsw_drv_pe_acc_info *)pe->mport->minfo->private_data;

	if (!p_acc->maint_valid) {
		return -EINVAL;
	}

	memset(dev_fn, 0, sizeof(dev_fn));
	snprintf(dev_fn, sizeof(dev_fn) - 1, "%s%s", MPSW_DFLT_DEV_DIR, name);

	if (access(dev_fn, F_OK) != -1) {
		INFO("\nFMD: device \"%s\" exists...\n", name);
	} else {
		rc = riomp_mgmt_device_add(p_acc->maint, pe->did_reg_val,
				pe->hopcount, pe->comptag, pe->sysfs_name);
		if (rc) {
			ERR("riomp_mgmt_device_add, rc %d errno %d\n", rc,
					errno);
		}
	}

	return rc;
}

int mpsw_destroy_priv_data(struct riocp_pe *pe);

int mpsw_alloc_priv_data(struct riocp_pe *pe, void **p_dat,
		struct riocp_pe *peer)
{
	struct mpsw_drv_private_data *priv_ptr = NULL;
	int ret = 1;

	DBG("ENTRY\n");

	*p_dat = calloc(1, sizeof(struct mpsw_drv_private_data));
	if (NULL == *p_dat) {
		ERR(MALLOC_FAIL);
		ret = -ENOMEM;
		goto err;
	}

	pe->private_data = *p_dat;
	priv_ptr = (struct mpsw_drv_private_data *)*p_dat;

	priv_ptr->is_mport = RIOCP_PE_IS_MPORT(pe) ? 1 : 0;
	priv_ptr->dev_h_valid = 0;
	priv_ptr->dev_h.privateData = (void *)pe;
	priv_ptr->dev_h.accessInfo = NULL;

	if (priv_ptr->is_mport) {
		struct mpsw_drv_pe_acc_info *acc_p;

		priv_ptr->dev_h.accessInfo = calloc(1,
				sizeof(struct mpsw_drv_pe_acc_info));
		if (NULL == priv_ptr->dev_h.accessInfo) {
			ERR(MALLOC_FAIL);
			ret = ENOMEM;
			goto err;
		}

		acc_p =
				(struct mpsw_drv_pe_acc_info *)priv_ptr->dev_h.accessInfo;
		acc_p->local = 1;
		acc_p->maint_valid = 0;

		ret = riomp_mgmt_mport_create_handle(pe->minfo->id, 0,
				&acc_p->maint);
		if (ret) {
			ERR("Unable to open mport %d %d:%s\n", pe->minfo->id,
					errno, strerror(errno));
			goto exit;
		}
		acc_p->maint_valid = 1;
		DBG("Successfully openned mport did %d ct %x\n", pe->did_reg_val,
				pe->comptag);

		ret = riomp_mgmt_query(acc_p->maint, &acc_p->props);
		if (ret < 0) {
			ERR("Unable to query mport %d properties %d:%s\n",
					pe->mport, errno, strerror(errno));
			goto err;
		}
		pe->mport->minfo->private_data = (void *)acc_p;
		ret = 0;
		goto exit;
	}

	if (NULL == peer) {
		goto err;
	}

	pe->mport = peer->mport;

	ret = 0;
	goto exit;

err:
	mpsw_destroy_priv_data(pe);
exit:
	DBG("EXIT\n");
	return ret;
}

int mpsw_destroy_priv_data(struct riocp_pe *pe)
{
	struct mpsw_drv_private_data *priv_ptr = NULL;

	if (NULL == pe->private_data)
		return 0;

	priv_ptr = (struct mpsw_drv_private_data *)pe->private_data;
	if (NULL != priv_ptr->dev_h.accessInfo) {
		struct mpsw_drv_pe_acc_info *acc_p;

		acc_p =
				(struct mpsw_drv_pe_acc_info *)(priv_ptr->dev_h.accessInfo);
		if (acc_p->maint_valid) {
			if (riomp_mgmt_mport_destroy_handle(&acc_p->maint))
				ERR("Unable to close mport %d %d:%s\n",
						pe->mport, errno,
						strerror(errno));
		}
		free(priv_ptr->dev_h.accessInfo);
	}
	free(pe->private_data);

	return 0;
}

int mpdrv_auto_init_rt(struct riocp_pe *pe, DAR_DEV_INFO_t *dh,
		struct mpsw_drv_private_data *priv, pe_port_t acc_port)
{
	rio_rt_set_all_in_t set_in;
	rio_rt_set_all_out_t set_out;
	rio_rt_initialize_in_t init_in;
	rio_rt_initialize_out_t init_out;
	rio_rt_change_rte_in_t rte_in;
	rio_rt_change_rte_out_t rte_out;
	uint32_t rc;

	init_in.set_on_port = RIO_ALL_PORTS;
	init_in.default_route = RIO_RTE_DROP;
	init_in.default_route_table_port = RIO_RTE_DROP;
	init_in.update_hw = false;
	init_in.rt = &priv->st.g_rt;

	rc = rio_rt_initialize(dh, &init_in, &init_out);
	if (rc) {
		goto fail;
	}

	rte_in.dom_entry = false;
	rte_in.idx = pe->mport->did_reg_val;
	rte_in.rte_value = acc_port;
	rte_in.rt = &priv->st.g_rt;

	rc = rio_rt_change_rte(dh, &rte_in, &rte_out);
	if (rc) {
		goto fail;
	}

	set_in.set_on_port = RIO_ALL_PORTS;
	set_in.rt = &priv->st.g_rt;

	rc = rio_rt_set_all(dh, &set_in, &set_out);
	if (rc) {
		goto fail;
	}
	return 0;
fail:
	return 1;
}

int mpdrv_init_rt(struct riocp_pe *pe, DAR_DEV_INFO_t *dh,
		struct mpsw_drv_private_data *priv, pe_port_t acc_port)
{
	struct cfg_dev sw;
	rio_rt_set_all_in_t set_in;
	rio_rt_set_all_out_t set_out;
	pe_port_t port;
	int rc;

	rc = cfg_find_dev_by_ct(pe->comptag, &sw);
	if (rc) {
		/* device not found in config file
		 * continue with other initialization
		 */
		if (cfg_auto()) {
			return mpdrv_auto_init_rt(pe, dh, priv, acc_port);
		}
		return 0;
	}

	if (NULL != sw.sw_info.rt[CFG_DEV08]) {
		set_in.set_on_port = RIO_ALL_PORTS;
		set_in.rt = sw.sw_info.rt[CFG_DEV08];

		rc = rio_rt_set_all(dh, &set_in, &set_out);
		if (RIO_SUCCESS != rc) {
			ERR("Error programming global rt on ct 0x%x rc %d\n",
					sw.ct, rc);
			goto fail;
		}
	}

	for (port = 0; port < NUM_PORTS(dh); port++) {
		if (NULL == sw.sw_info.sw_pt[port].rt[CFG_DEV08])
			continue;

		set_in.set_on_port = port;
		set_in.rt = sw.sw_info.sw_pt[port].rt[CFG_DEV08];

		rc = rio_rt_set_all(dh, &set_in, &set_out);
		if (RIO_SUCCESS != rc) {
			ERR("Error programming port %d rt on ct 0x%x rc %d\n",
					port, pe->comptag, rc);
			goto fail;
		}
	}
	return 0;
fail:
	return 1;
}

struct sc_cfg_t {
	rio_sc_ctr_t ctr_t;
	bool tx;
};

struct sc_cfg_t tsi_sc_cfg[TSI578_NUM_PERF_CTRS] = { //
		{rio_sc_uc_pkts, true},		//
		{rio_sc_uc_pkts, false},	//
		{rio_sc_uc_4b_data, true},	//
		{rio_sc_uc_4b_data, false},	//
		{rio_sc_retries, true},		//
		{rio_sc_retries, false},	//
};

struct sc_cfg_t rxs_sc_cfg[RXS2448_MAX_SC] = { //
		{rio_sc_fab_pkt, true},		//
		{rio_sc_fab_pkt, false},	//
		{rio_sc_fab_pload, true},	//
		{rio_sc_fab_pload, false},	//
		{rio_sc_retries, true},		//
		{rio_sc_retries, false},	//
		{rio_sc_pkt_drop, false},	//
		{rio_sc_rio_bwidth, true},	//
};

uint32_t rio_sc_config_dev_ctrs(DAR_DEV_INFO_t *dev_h,
		struct mpsw_drv_private_data *priv)
{
	uint32_t rc = RIO_SUCCESS;
	unsigned int idx;
	rio_sc_cfg_tsi57x_ctr_in_t tsi_in;
	rio_sc_cfg_tsi57x_ctr_out_t tsi_out;
	rio_sc_cfg_cps_ctrs_in_t cps_in;
	rio_sc_cfg_cps_ctrs_out_t cps_out;
	rio_sc_cfg_rxs_ctr_in_t rxs_in;
	rio_sc_cfg_rxs_ctr_out_t rxs_out;

	//@sonar:off - c:S1871
	switch (dev_h->driver_family) {
	case RIO_CPS_DEVICE:
		// Enable counters on all ports.
		cps_in.ptl.num_ports = RIO_ALL_PORTS;
		cps_in.enable_ctrs = true;
		cps_in.dev_ctrs = &priv->st.sc_dev;
		rc = rio_sc_cfg_cps_ctrs(dev_h, &cps_in, &cps_out);
		break;

	case RIO_RXS_DEVICE:
		rxs_in.ptl.num_ports = RIO_ALL_PORTS;
		rxs_in.prio_mask = SC_PRIO_MASK_ALL;
		rxs_in.ctr_en = true;
		rxs_in.dev_ctrs = &priv->st.sc_dev;
		for (idx = 0; idx < RXS2448_MAX_SC; idx++) {
			rxs_in.ctr_idx = idx;
			rxs_in.tx = rxs_sc_cfg[idx].tx;
			rxs_in.ctr_type = rxs_sc_cfg[idx].ctr_t;
			rc = rio_sc_cfg_rxs_ctr(dev_h, &rxs_in, &rxs_out);
			if (rc) {
				break;
			}
		}
		break;


	case RIO_TSI721_DEVICE:
		// No configuration required.
		break;

	case RIO_TSI57X_DEVICE:
		tsi_in.ptl.num_ports = RIO_ALL_PORTS;
		tsi_in.prio_mask = SC_PRIO_MASK_G1_ALL;
		tsi_in.dev_ctrs = &priv->st.sc_dev;
		for (idx = 0; idx < TSI578_NUM_PERF_CTRS; idx++) {
			tsi_in.tx = tsi_sc_cfg[idx].tx;
			tsi_in.ctr_type = tsi_sc_cfg[idx].ctr_t;
			rc = rio_sc_cfg_tsi57x_ctr(dev_h, &tsi_in, &tsi_out);
			if (rc) {
				break;
			}
		}
		break;

	default:

		break;
	}
	//@sonar:on

	return rc;
}

static int probe_all_rt(DAR_DEV_INFO_t *dev_h, struct mpsw_drv_private_data *priv)
{
	rio_rt_probe_all_in_t rt_in;
	rio_rt_probe_all_out_t rt_out;
	pe_port_t port;
	int rc;

	rt_in.probe_on_port = RIO_ALL_PORTS;
	rt_in.rt = &priv->st.g_rt;

	rc = rio_rt_probe_all(dev_h, &rt_in, &rt_out);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	for (port = 0; port < NUM_PORTS(&priv->dev_h); port++) {
		rt_in.probe_on_port = port;
		rt_in.rt = &priv->st.pprt[port];
		rc = rio_rt_probe_all(dev_h, &rt_in, &rt_out);
		if (RIO_SUCCESS != rc) {
			goto exit;
		}
	}

exit:
	return rc;
}

// NOTE: generic_device_init is called when the
// pe->cap structure has not yet been filled in.
//
// Only librio macros for checking register/capability
// values work at this point.

int generic_device_init(struct riocp_pe *pe)
{
	uint32_t port_info;
	struct mpsw_drv_private_data *priv = NULL;
	DAR_DEV_INFO_t *dev_h = NULL;
	struct DAR_ptl ptl;
	rio_pc_set_config_in_t set_pc_in;
	rio_pc_dev_reset_config_in_t rst_in = {rio_pc_rst_port};
	rio_pc_dev_reset_config_out_t rst_out;
	rio_pc_get_status_in_t ps_in;
	rio_pc_get_config_in_t pc_in;
	rio_sc_init_dev_ctrs_in_t sc_in;
	rio_sc_init_dev_ctrs_out_t sc_out;
	rio_em_dev_rpt_ctl_in_t rpt_in;
	did_t did;
	rio_port_t port;
	struct cfg_dev sw;
	int rc = 1;

	DBG("ENTRY\n");
	priv = (struct mpsw_drv_private_data *)(pe->private_data);

	if (NULL == priv) {
		ERR("Private Data is NULL, exiting\n");
		goto exit;
	}

	pe->dev_name = (const char *)priv->dev_h.name;
	dev_h = &priv->dev_h;

	// ensure destID/comptag/addrmode is configured correctly
	rc = riocp_pe_update_comptag(pe, 1);
	if (rc) {
		ERR("Could not update comptag\n");
		goto exit;
	}

	if (!RIOCP_PE_IS_HOST(pe)) {
		rc = 0;
		goto exit;
	}

	// Enable all ports on switches to allow continued exploration.
	// Disable ports on all other devices, until enumeration is complete.

	ptl.num_ports = RIO_ALL_PORTS;
	if (SWITCH(dev_h) || RIOCP_PE_IS_MPORT(pe)) {
		rc = DARrioPortEnable(dev_h, &ptl, true, false, true);
	} else {
		rc = DARrioPortEnable(dev_h, &ptl, true, false, false);
	}

	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	rc = DARrioSetEnumBound(dev_h, &ptl, 0);
	if (rc) {
		ERR("Could not clear enumeration indication\n");
		goto exit;
	}

	if (MEMORY(dev_h)) {
		struct mport_regs regs;
		uint8_t memsz;
		uint32_t reg_val;

		if (RIOCP_PE_IS_MPORT(pe)) {
			if (cfg_get_mp_mem_sz(pe->minfo->id, &memsz)) {
				rc = -1;
				ERR("Could not get mport.");
				goto exit;
			}

			switch (memsz) {
			case CFG_MEM_SZ_34 :
				reg_val = RIO_PE_LL_CTL_34BIT;
				break;
			case CFG_MEM_SZ_50 :
				reg_val = RIO_PE_LL_CTL_50BIT;
				break;
			case CFG_MEM_SZ_66 :
				reg_val = RIO_PE_LL_CTL_66BIT;
				break;
			default:
				rc = -1;
				ERR("Could not get mport.");
				goto exit;
			}
		} else {
			rc = mpsw_get_mport_regs(pe->mport->minfo->id, &regs);
			if (rc) {
				ERR("Could not read mport registers.");
				goto exit;
			}
			reg_val = regs.memaddr_sz;
		}

		rc = DARrioSetAddrMode(dev_h, reg_val);
		if (RIO_SUCCESS != rc) {
			ERR("CT 0x%0x DARrioSetAddrMode rc 0x%x\n", pe->comptag, rc);
			goto exit;
		}
	}

	/* Query port configuration and status */
	pc_in.ptl.num_ports = RIO_ALL_PORTS;
	rc = rio_pc_get_config(dev_h, &pc_in, &priv->st.pc);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	set_pc_in.lrto = 50; /* 5 usec link timeout */
	set_pc_in.log_rto = 500; /* 50 usec logical response timeout */
	set_pc_in.oob_reg_acc = false;
	set_pc_in.num_ports = priv->st.pc.num_ports;
	memcpy(set_pc_in.pc, priv->st.pc.pc, sizeof(set_pc_in.pc));

	/* device not found in config file
	 * continue with other initialization
	 */
	rc = did_get(&did, pe->did_reg_val);
	if (rc) {
		ERR("Device ID 0x%08x does not exist\n", pe->did_reg_val);
		goto exit;
	}

	rc = mpsw_drv_raw_reg_rd(pe, did, pe->hopcount, RIO_SW_PORT_INF,
			&port_info);
	if (rc) {
		ERR("Unable to get port info %d:%s\n", rc, strerror(rc));
		goto exit;
	}

	set_pc_in.reg_acc_port = RIO_ACCESS_PORT(port_info);
	for (port = 0; port < set_pc_in.num_ports; port++) {
		set_pc_in.pc[port].port_lockout = false;
		set_pc_in.pc[port].nmtc_xfer_enable = false;
	}

	if (!cfg_find_dev_by_ct(pe->comptag, &sw)) {
		if (sw.is_sw) {
			for (port = 0; port < sw.sw_info.num_ports; port++) {
				if (!sw.sw_info.sw_pt[port].valid) {
					set_pc_in.pc[port].port_available
									= false;
					set_pc_in.pc[port].powered_up = false;
					continue;
				}
				if (sw.sw_info.sw_pt[port].port != port) {
					ERR("Port numbers unequal %d %d\n",
						port,
						sw.sw_info.sw_pt[port].port);
					goto exit;
				}
				
				set_pc_in.pc[port].ls =
						sw.sw_info.sw_pt[port].ls;
				set_pc_in.pc[port].pw =
						sw.sw_info.sw_pt[port].op_pw;
			}
		} else {
			set_pc_in.pc[0].ls = sw.ep_pt.ls;
			set_pc_in.pc[0].pw = sw.ep_pt.op_pw;
		}
	}

	rc = rio_pc_set_config(dev_h, &set_pc_in, &priv->st.pc);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	ps_in.ptl.num_ports = RIO_ALL_PORTS;
	rc = rio_pc_get_status(dev_h, &ps_in, &priv->st.ps);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	if (SWITCH(dev_h)) {
		// initialize the status of the routing tables
		rc = probe_all_rt(dev_h, priv);
		if (rc) {
			goto exit;
		}

		// initialize the routing tables based on the configuration file
		rc = mpdrv_init_rt(pe, dev_h, priv,
				RIO_ACCESS_PORT(port_info));
		if (rc) {
			goto exit;
		}

		// update the routing tables
		rc = probe_all_rt(dev_h, priv);
		if (rc) {
			goto exit;
		}
	}

	// Initialize performance counter structure, and 
	// set default hardware performance counter configuration
	sc_in.ptl.num_ports = RIO_ALL_PORTS;
	sc_in.dev_ctrs = &priv->st.sc_dev;
	priv->st.sc_dev.num_p_ctrs = RIO_MAX_PORTS;
	priv->st.sc_dev.valid_p_ctrs = 0;
	priv->st.sc_dev.p_ctrs = priv->st.sc;

	rc = rio_sc_init_dev_ctrs(dev_h, &sc_in, &sc_out);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	rc = rio_sc_config_dev_ctrs(dev_h, priv);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	if (RIOCP_PE_IS_MPORT(pe)) {
		rst_in.rst = rio_pc_rst_ignore;
	} else {
		/* Set device reset handling to "per port" if possible */
		rst_in.rst = rio_pc_rst_port;
	}

	rc = rio_pc_dev_reset_config(dev_h, &rst_in, &rst_out);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	if (RIOCP_PE_IS_MPORT(pe)) {
		return 0;
	}

	rpt_in.ptl.num_ports = RIO_ALL_PORTS;
	rpt_in.notfn = rio_em_notfn_none;
	rc = rio_em_dev_rpt_ctl(dev_h, &rpt_in, &priv->st.em_notfn);
	if (RIO_SUCCESS != rc) {
		goto exit;
	}

	// Set up port-write reporting.
	// Note that this tells the endpoints the destination ID of the
	// system Host.

	priv->st.em_pw_cfg.deviceID_tt = tt_dev8;
	priv->st.em_pw_cfg.port_write_destID = pe->mport->did_reg_val;
	priv->st.em_pw_cfg.srcID_valid = true;
	priv->st.em_pw_cfg.port_write_srcID = pe->did_reg_val;
	priv->st.em_pw_cfg.priority = 3;
	priv->st.em_pw_cfg.CRF = true;
	priv->st.em_pw_cfg.port_write_re_tx = 0;

	rc = rio_em_cfg_pw(dev_h, &priv->st.em_pw_cfg, &priv->st.em_pw_cfg);
	if (rc) {
		goto exit;
	}
	return 0;

exit:
	return rc;
}

int RIOCP_WU mpsw_drv_init_pe(struct riocp_pe *pe, struct riocp_pe *peer,
		char *name)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret = 1;
	uint32_t temp_devid;

	DBG("ENTRY\n");

	p_dat = (struct mpsw_drv_private_data *)pe->private_data;

	/* If private data has been allocated, device was found previously */
	if (NULL != p_dat) {
		DBG("Private Data exists!\n");
		goto exit;
	}

	ret = mpsw_alloc_priv_data(pe, (void **)&p_dat, peer);
	if (ret) {
		ERR("Unable to allocate private data.\n");
		goto exit;
	}

	/* Select a driver for the device */
	ret = mpsw_drv_raw_reg_rd(pe, DID_ANY_DEV8_ID, pe->hopcount, RIO_DEV_IDENT,
			&temp_devid);
	if (ret) {
		ERR("Unable to read device ID %d:%s\n", ret, strerror(ret));
		goto exit;
	}

	p_dat->dev_h.devID = temp_devid;
	p_dat->dev_h.driver_family = rio_get_driver_family(temp_devid);
	ret = DAR_Find_Driver_for_Device(1, &p_dat->dev_h);
	if (RIO_SUCCESS != ret) {
		ERR("Unable to find driver for device, type 0x%x\n, ret 0x%x",
				ret, p_dat->dev_h.devID);
		ret = errno = EOPNOTSUPP;
		goto exit;
	}
	p_dat->dev_h_valid = 1;

	ret = generic_device_init(pe);
	if (ret) {
		ERR("Generic device init failed: %d (0x%x)\n", ret, ret);
		goto exit;
	}

	ret = mpsw_mport_dev_add(pe, name);
	if (ret) {
		ERR("Adding device to mport failed: %d (0x%x)\n", ret, ret);
		goto exit;
	}

exit:
	DBG("EXIT\n");
	return ret;
}

int RIOCP_WU mpsw_drv_destroy_pe(struct riocp_pe *pe)
{
	DBG("ENTRY\n");
	mpsw_destroy_priv_data(pe);
	DBG("EXIT\n");
	return 0;
}

int RIOCP_WU mpsw_drv_recover_port(struct riocp_pe *pe, pe_port_t port,
		pe_port_t lp_port)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret;
	rio_pc_clr_errs_in_t clr_errs_in;
	rio_pc_clr_errs_out_t clr_errs_out;

	DBG("ENTRY\n");

	if (riocp_pe_handle_get_private(pe, (void **)&p_dat)) {
		DBG("Private Data does not exist EXITING!\n");
		goto fail;
	}

	if (!p_dat->dev_h_valid) {
		DBG("Device handle not valid EXITING!\n");
		goto fail;
	}

	clr_errs_in.port_num = port;
	clr_errs_in.clr_lp_port_err = 1;
	clr_errs_in.lp_dev_info = NULL;
	clr_errs_in.num_lp_ports = 1;
	clr_errs_in.lp_port_list[0] = lp_port;

	ret = rio_pc_clr_errs(&p_dat->dev_h, &clr_errs_in, &clr_errs_out);
	if (ret) {
		DBG("Failed clearing %s Port %d ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, ret, clr_errs_out.imp_rc);
		goto fail;
	}

	DBG("EXIT\n");
	return 0;
fail:
	DBG("FAIL\n");
	return 1;
}

int RIOCP_WU mpsw_drv_get_route_entry(struct riocp_pe *pe, pe_port_t port,
		did_t did, pe_rt_val *rt_val)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret;
	rio_rt_probe_in_t probe_in;
	rio_rt_probe_out_t probe_out;
	did_reg_t destID;

	DBG("ENTRY\n");

	if (riocp_pe_handle_get_private(pe, (void **)&p_dat)) {
		DBG("Private Data does not exist EXITING!\n");
		goto fail;
	}

	if (!p_dat->dev_h_valid) {
		DBG("Device handle not valid EXITING!\n");
		goto fail;
	}

	destID = (did_reg_t)did_get_value(did);
	probe_in.probe_on_port = port;
	probe_in.tt = tt_dev8;
	probe_in.destID = destID;
	probe_in.rt = &p_dat->st.g_rt;

	ret = rio_rt_probe(&p_dat->dev_h, &probe_in, &probe_out);
	if (ret) {
		DBG("Failed probing %s port %d did %d ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, destID, ret,
				probe_out.imp_rc);
		goto fail;
	}

	*rt_val = probe_out.routing_table_value;

	DBG("EXIT\n");
	return 0;
fail:
	DBG("FAIL\n");
	return 1;
}

int RIOCP_WU mpsw_drv_set_route_entry(struct riocp_pe *pe, pe_port_t port,
		did_t did, pe_rt_val rt_val)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret;
	rio_rt_change_rte_in_t chg_in;
	rio_rt_change_rte_out_t chg_out;
	rio_rt_set_changed_in_t set_in;
	rio_rt_set_changed_out_t set_out;
	did_val_t did_val;

	DBG("ENTRY\n");
	if (riocp_pe_handle_get_private(pe, (void **)&p_dat)) {
		DBG("Private Data does not exist EXITING!\n");
		goto fail;
	}

	if (!p_dat->dev_h_valid) {
		DBG("Device handle not valid EXITING!\n");
		goto fail;
	}

	did_val = did_get_value(did);
	chg_in.dom_entry = 0;
	chg_in.idx = did_val;
	chg_in.rte_value = rt_val;
	if (RIO_ALL_PORTS == port) {
		chg_in.rt = &p_dat->st.g_rt;
	} else {
		if (port >= RIO_MAX_PORTS) {
			DBG("Invalid port #, max is %d", RIO_MAX_PORTS);
			goto fail;
		}
		chg_in.rt = &p_dat->st.pprt[port];
	}

	ret = rio_rt_change_rte(&p_dat->dev_h, &chg_in, &chg_out);
	if (ret) {
		DBG("RT_CHG %s port %d did %d rte %x ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, did_val, rt_val, ret,
				chg_out.imp_rc);
		goto fail;
	}

	set_in.set_on_port = port;
	set_in.rt = chg_in.rt;

	ret = rio_rt_set_changed(&p_dat->dev_h, &set_in, &set_out);
	if (ret) {
		DBG("RT_SET %s port %d did %d rte 0x%x ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, did_val, rt_val, ret,
				set_out.imp_rc);
		goto fail;
	}

	DBG("EXIT\n");
	return 0;
fail:
	DBG("FAIL\n");
	return 1;
}

int RIOCP_WU mpsw_drv_reset_port(struct riocp_pe *pe, pe_port_t port,
bool reset_lp)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret;
	rio_pc_reset_port_in_t reset_in;
	rio_pc_reset_port_out_t reset_out;

	DBG("ENTRY\n");
	if (riocp_pe_handle_get_private(pe, (void **)&p_dat)) {
		DBG("Private Data does not exist EXITING!\n");
		goto fail;
	}

	if (!p_dat->dev_h_valid) {
		DBG("Device handle not valid EXITING!\n");
		goto fail;
	}

	if (port >= NUM_PORTS(&p_dat->dev_h)) {
		DBG("Port illegal, EXITING!\n");
		goto fail;
	}

	reset_in.port_num = port;
	reset_in.oob_reg_acc = false;
	reset_in.reg_acc_port = RIOCP_PE_SW_PORT(pe->cap);
	reset_in.reset_lp = reset_lp;
	reset_in.preserve_config = true;

	ret = rio_pc_reset_port(&p_dat->dev_h, &reset_in, &reset_out);
	if (ret) {
		DBG("PC_reset_port %s port %d ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, ret, reset_out.imp_rc);
		goto fail;
	}

	DBG("EXIT\n");
	return 0;
fail:
	DBG("FAIL\n");
	return 1;
}

int RIOCP_WU mpsw_drv_get_port_state(struct riocp_pe *pe, pe_port_t port,
		struct riocp_pe_port_state_t *state)
{
	struct mpsw_drv_private_data *p_dat = NULL;
	int ret;
	rio_pc_get_status_in_t st_in;

	DBG("ENTRY\n");
	if (riocp_pe_handle_get_private(pe, (void **)&p_dat)) {
		DBG("Private Data does not exist EXITING!\n");
		goto fail;
	}

	if (!p_dat->dev_h_valid) {
		DBG("Device handle not valid EXITING!\n");
		goto fail;
	}

	if (port >= NUM_PORTS(&p_dat->dev_h)) {
		DBG("Port illegal, EXITING!\n");
		goto fail;
	}

	if ((NULL == state) || (port >= NUM_PORTS(&p_dat->dev_h))) {
		DBG("State or port illegal, EXITING!\n");
		goto fail;
	}

	st_in.ptl.num_ports = RIO_ALL_PORTS;
	ret = rio_pc_get_status(&p_dat->dev_h, &st_in, &p_dat->st.ps);
	if (ret) {
		DBG("PC_Status %s port %d ret 0x%x imp_rc 0x%x\n",
				pe->sysfs_name, port, ret, p_dat->st.ps.imp_rc);
		goto fail;
	}

	state->port_ok = p_dat->st.ps.ps[port].port_ok;
	state->port_max_width = PW_TO_LANES(p_dat->st.pc.pc[port].pw);
	if (state->port_ok) {
		state->port_cur_width = PW_TO_LANES(p_dat->st.ps.ps[port].pw);
	} else {
		state->port_cur_width = 0;
	}

	switch (p_dat->st.pc.pc[port].ls) {
	case rio_pc_ls_1p25:
		state->port_lane_speed = 1250;
		break;
	case rio_pc_ls_2p5:
		state->port_lane_speed = 2500;
		break;
	case rio_pc_ls_3p125:
		state->port_lane_speed = 3125;
		break;
	case rio_pc_ls_5p0:
		state->port_lane_speed = 5000;
		break;
	case rio_pc_ls_6p25:
		state->port_lane_speed = 6250;
		break;
	default:
		state->port_lane_speed = 0;
	}

	DBG("EXIT\n");
	return 0;
fail:
	DBG("FAIL\n");
	return 1;
}

int RIOCP_WU mpsw_get_mport_regs(int mp_num, struct mport_regs *regs)
{
	riomp_mport_t mp_h;
	int rc = -1;

	if (riomp_mgmt_mport_create_handle(mp_num, 0, &mp_h)) {
		goto fail;
	}

	if (riomp_mgmt_lcfg_read(mp_h, RIO_PE_LL_CTL, 4, &regs->memaddr_sz)) {
		goto close;
	}

	if (riomp_mgmt_lcfg_read(mp_h, RIO_DEVID, 4, &regs->my_did_reg_val)) {
		goto close;
	}

	if (riomp_mgmt_lcfg_read(mp_h, RIO_COMPTAG, 4, &regs->comptag)) {
		goto close;
	}

	// For now, only works with Tsi721.

	if (riomp_mgmt_lcfg_read(mp_h, TSI721_PW_TGT_ID, 4,
			(uint32_t *)&regs->host_did_reg_val)) {
		goto close;
	}

	// For now, works with Tsi721.

	if (riomp_mgmt_lcfg_read(mp_h, TSI721_SP_GEN_CTL, 4, &regs->disc)) {
		goto close;
	}

	if (riomp_mgmt_lcfg_read(mp_h, TSI721_SP_ERR_STAT, 4,
			&regs->p_err_stat)) {
		goto close;
	}

	if (riomp_mgmt_lcfg_read(mp_h, TSI721_SP_CTL, 4, &regs->p_ctl1)) {
		goto close;
	}

	if (riomp_mgmt_lcfg_read(mp_h, TSI721_WHITEBOARD, 4,
			&regs->scratch_cm_sock)) {
		goto close;
	}

	rc = 0;
close:
	riomp_mgmt_mport_destroy_handle(&mp_h);
fail:
	return rc;
}

int RIOCP_WU mpsw_enable_pe(struct riocp_pe *pe, pe_port_t port)
{
	// Enabling a PE requires two things:
	//
	// - Set the DISC and MAST_EN bits in the RIO_SP_GEN_CTL
	// - Set the INP_EN and OTP_EN bits in the RIO_SPX_CTL for each port

	struct mpsw_drv_private_data *priv;
	uint32_t oset;
	uint32_t reg_val;
	uint32_t port_info;
	uint32_t rc;
	did_t did;
	rio_port_t st_port = port;
	rio_port_t end_port = port;
	rio_pc_set_config_in_t set_pc_in;
	DAR_DEV_INFO_t *dev_h = NULL;

	DBG("ENTRY\n");
	priv = (struct mpsw_drv_private_data *)(pe->private_data);

	if (NULL == priv) {
		ERR("Private Data is NULL, exiting\n");
		goto fail;
	}

	dev_h = &priv->dev_h;
	if (!priv->dev_h.extFPtrForPort) {
		ERR("DevID 0x%x extFPtrForPort is 0", priv->dev_h.devID);
		goto fail;
	}

	// 5 usec link timeout, may need to increase for FPGA
	set_pc_in.lrto = 50;
	// 1 msec logical response timeout
	// Required for riosocket operation.
	set_pc_in.log_rto = 10000;
	set_pc_in.oob_reg_acc = false;
	set_pc_in.num_ports = priv->st.pc.num_ports;
	memcpy(set_pc_in.pc, priv->st.pc.pc, sizeof(set_pc_in.pc));

	/* device not found in config file
	 * continue with other initialization
	 */
	rc = did_get(&did, pe->did_reg_val);
	if (rc) {
		ERR("Device ID 0x%08x does not exist\n", pe->did_reg_val);
		goto fail;
	}

	rc = mpsw_drv_raw_reg_rd(pe, did, pe->hopcount, RIO_SW_PORT_INF,
			&port_info);
	if (rc) {
		ERR("Unable to get port info %d:%s\n", rc, strerror(rc));
		goto fail;
	}
	set_pc_in.reg_acc_port = RIO_ACCESS_PORT(port_info);

	if (RIOCP_PE_ANY_PORT == port) {
		st_port = 0;
		end_port = RIOCP_PE_PORT_COUNT(pe->cap) - 1;
	} else {
		if (port >= RIO_MAX_PORTS) {
			ERR("Illegal port number %d max is %d", port,
					RIO_MAX_PORTS -1);
			goto fail;
		}
	}

	for (port = st_port; port <= end_port; port++) {
		set_pc_in.pc[port].port_lockout = false;
		set_pc_in.pc[port].nmtc_xfer_enable = true;
	}

	rc = rio_pc_set_config(dev_h, &set_pc_in, &priv->st.pc);
	if (RIO_SUCCESS != rc) {
		goto fail;
	}

	oset = RIO_SP_GEN_CTL(priv->dev_h.extFPtrForPort);

	if (riocp_pe_maint_read(pe, oset, &reg_val)) {
		ERR("PE %s failed to read @ 0x%x.", pe->sysfs_name, oset);
		goto fail;
	}
	reg_val |= RIO_SP_GEN_CTL_MAST_EN | RIO_SP_GEN_CTL_DISC;
	if (riocp_pe_maint_write(pe, oset, reg_val)) {
		ERR("PE %s failed to write @ 0x%x.", pe->sysfs_name, oset);
		goto fail;
	}

	return 0;
fail:
	return -1;
}

#ifdef __cplusplus
}
#endif

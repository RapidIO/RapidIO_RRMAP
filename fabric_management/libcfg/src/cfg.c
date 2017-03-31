
/* Fabric Management Daemon Configuration file and options parsing support */
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
#include <string.h>
#include <stdlib.h>
#include <limits.h>
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

#include "rio_route.h"
#include "rio_standard.h"
#include "tok_parse.h"
#include "did.h"
#include "ct.h"
#include "fmd_dd.h"
#include "cfg.h"
#include "cfg_private.h"
#include "libcli.h"
#include "liblog.h"

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct int_cfg_parms *cfg = NULL;
FILE *cfg_fd = NULL;
const char *DEV_TYPE = "ENDPOINT";

static void init_rt(rio_rt_state_t *rt)
{ 
	int k;

	memset(rt, 0, sizeof(rio_rt_state_t));
	rt->default_route = RIO_RTE_DROP;

	for (k = 0; k < RIO_RT_GRP_SZ; k++) {
		rt->dev_table[k].rte_val = RIO_RTE_DROP;
		rt->dom_table[k].rte_val = RIO_RTE_DROP;
	}

	for (k = 0; k < RIO_MAX_MC_MASKS; k++) {
		rt->mc_masks[k].mc_destID = (did_reg_t)0xFF;
		rt->mc_masks[k].tt = tt_dev8;
	}
}

static int init_cfg_ptr()
{
	int i, j;

	cfg = (struct int_cfg_parms *)calloc(1, sizeof(struct int_cfg_parms));
	if (NULL == cfg) {
		return 1;
	}

	cfg->mast_idx = CFG_SLAVE;

	for (i = 0; i < CFG_MAX_MPORTS; i++) {
		cfg->mport_info[i].num = -1;
		cfg->mport_info[i].op_mode = -1;
		cfg->mport_info[i].mem_sz = CFG_MEM_SZ_DEFAULT;
		for (j = 0; j < CFG_DEVID_MAX; j++) {
			cfg->mport_info[i].devids[j].hc = HC_MP;
		}
		cfg->mport_info[i].ep_pnum = -1;
	}
	cfg->mast_did_sz = CFG_DFLT_MAST_DEVID_SZ;
	cfg->mast_did_val = CFG_DFLT_MAST_DEVID;
	cfg->mast_cm_port = FMD_DFLT_MAST_CM_PORT;

	for (i = 0; i < CFG_MAX_EP; i++) {
		for (j = 0; j < CFG_MAX_EP_PORT; j++) {
			int k;
			cfg->eps[i].ports[j].rio.max_pw = rio_pc_pw_last;
			cfg->eps[i].ports[j].rio.op_pw = rio_pc_pw_last;
			cfg->eps[i].ports[j].rio.ls = rio_pc_ls_last;
			for (k = 0; k < CFG_DEVID_MAX; k++) {
				cfg->eps[i].ports[j].devids[k].hc = HC_MP;
			}
			cfg->eps[i].ports[j].conn_end = -1;
		}
	}

	for (i = 0; i < CFG_MAX_SW; i++) {
		for (j = 0; j < CFG_MAX_SW_PORT; j++) {
			cfg->sws[i].ports[j].rio.max_pw = rio_pc_pw_last;
			cfg->sws[i].ports[j].rio.op_pw = rio_pc_pw_last;
			cfg->sws[i].ports[j].rio.ls = rio_pc_ls_last;
			cfg->sws[i].ports[j].conn_end = -1;
		}
		for (j = 0; j < CFG_DEVID_MAX; j++) {
			init_rt(&cfg->sws[i].rt[j]);
			for (int k = 0; k < CFG_MAX_SW_PORT; k++) {
				init_rt(&cfg->sws[i].ports[k].rt[j]);
			}
		}
	}

	for (i = 0; i < CFG_MAX_CONN; i++) {
		int e;
		cfg->cons[i].valid = 0;
		for (e = 0; e < 2; e++) {
			cfg->cons[i].ends[e].port_num = -1;
			cfg->cons[i].ends[e].ep = -1;
		}
	}

	cfg->auto_config = false;

	return 0;
}

static void strip_crlf(char *tok)
{
	char *temp = NULL;

	if (NULL == tok) {
		return;
	}

	temp = strchr(tok, '\n');
	if (NULL != temp) {
		temp[0] = '\0';
	}

	temp = strchr(tok, '\r');
	if(NULL != temp) {
		temp[0] = '\0';
	}
}

const char *delim = " 	";
char *save_ptr = NULL;

static void flush_comment(char *tok)
{
	while (NULL != tok) {
		DBG("%s\n", tok);
		tok = strtok_r(NULL, delim, &save_ptr);
		strip_crlf(tok);
	}
}

#define LINE_SIZE 256
char *line;

static char *try_get_next_token(struct int_cfg_parms *cfg)
{
	char *rc = NULL;
	size_t byte_cnt = 1;
	int done = 0;

	if (cfg->init_err)
		goto fail;

	if (NULL == line) {
		line = (char *)calloc(1, LINE_SIZE);
		rc = NULL;
	} else {
		rc = strtok_r(NULL, delim, &save_ptr);
	}

	while (!done) {
		while ((NULL == rc) && (byte_cnt > 0)) {
			DBG("\n");
			byte_cnt = LINE_SIZE;
			byte_cnt = getline(&line, &byte_cnt, cfg_fd);
			strip_crlf(line);
			rc = strtok_r(line, delim, &save_ptr);
		}

		if (byte_cnt <= 0) {
			rc = NULL;
			break;
		}

		if (NULL != rc) {
			done = strncmp(rc, "//", 2);
			if (!done) {
				flush_comment(rc);
				rc = NULL;
			}
		}
	}

	if (NULL != rc) {
		DBG("%s\n", rc);
	}

fail:
	return rc;
}

static void parse_err(struct int_cfg_parms *cfg, char *err_msg)
{
	
	if (!cfg->init_err) {
		ERR("\n%s\n", err_msg);
	}
	cfg->init_err = 1;
}

static int get_next_token(struct int_cfg_parms *cfg, char **token)
{
	if (cfg->init_err) {
		*token = NULL;
		return 1;
	}

	*token = try_get_next_token(cfg);
	if (NULL == *token) {
		parse_err(cfg, (char *)"Unexpected end of file.");
	}

	return (NULL == *token);
}

#define DEVID_SZ_TOKENS "dev08 dev16 dev32"
#define DEVID_SZ_TOKENS_END "dev08 dev16 dev32 END"

static int get_devid_sz(struct int_cfg_parms *cfg, uint32_t *devID_sz)
{
	char *tok = NULL;

	if (cfg->init_err) {
		goto fail;
	}

	if (get_next_token(cfg, &tok)) {
		goto fail;
	}

	*devID_sz = (uint32_t)parm_idx(tok, (char *)DEVID_SZ_TOKENS);
	switch (*devID_sz) {
	case CFG_DEV08: // 0
	case CFG_DEV16: // 1
	case CFG_DEV32: // 2
		break;
	default:
		parse_err(cfg, (char *)"Unknown devID size.");
		goto fail;
	}

	return 0;

fail:
	parse_err(cfg, (char *)"Premature EOF.");
	return 1;
}

static int get_dec_int(struct int_cfg_parms *cfg, uint32_t *dec_int)
{
	char *tok = NULL;

	if (cfg->init_err || get_next_token(cfg, &tok)) {
		goto fail;
	}

	if (tok_parse_ul(tok, dec_int, 10)) {
		goto fail;
	}
	return 0;

fail:
	parse_err(cfg, (char *)"get_dec_int error.");
	return 1;
}

static int get_hex_int(struct int_cfg_parms *cfg, uint32_t *hex_int)
{
	char *tok = NULL;

	if (cfg->init_err || get_next_token(cfg, &tok)) {
		goto fail;
	}

	if (tok_parse_ul(tok, hex_int, 16)) {
		goto fail;
	}
	return 0;

fail:
	parse_err(cfg, (char *)"get_hex_int error.");
	return 1;
}

static int get_parm_idx(struct int_cfg_parms *cfg, char *parm_list)
{
	char *tok = NULL;

	if (!get_next_token(cfg, &tok))
		return parm_idx(tok, parm_list);
	parse_err(cfg, (char *)"Premature EOF.");
	return -1;
}

static int get_string(struct int_cfg_parms *cfg, char **parm)
{
	char *tok = NULL;

	if (!get_next_token(cfg, &tok)) {
		return update_string(parm, tok, strlen(tok));
	}
	parse_err(cfg, (char *)"Premature EOF.");
	return 1;
}

static int get_rt_v(struct int_cfg_parms *cfg, uint32_t *rt_val)
{
	char *tok = NULL;
	pe_rt_val val = 0;

	if (get_next_token(cfg, &tok)) {
		goto fail;
	}

	switch (parm_idx(tok, (char *)"MC NEXT_BYTE DEFAULT DROP")) {
	case 0: // MC
		if (get_dec_int(cfg, &val)) {
			goto fail;
		}

		*rt_val = (val<RIO_MAX_MC_MASKS)?
			(val + RIO_MAX_MC_MASKS):RIO_RTE_DROP;
		if (RIO_RTE_DROP == *rt_val) {
			parse_err(cfg, (char *)"Illegal MC Mask number.");
			goto fail;
		}
		break;
	case 1: // NEXT_BYTE
		*rt_val = RIO_RTE_LVL_G0;
		break;
	case 2: // DEFAULT
		*rt_val = RIO_RTE_DFLT_PORT;
		break;
	case 3: // DROP
		*rt_val = RIO_RTE_DROP;
		break;
	default:
		if (tok_parse_port_num(tok, &val, 0)) {
			parse_err(cfg, (char *)"Illegal port index.");
			goto fail;
		}
		*rt_val = val;
	}
	return 0;
fail:
	parse_err(cfg, (char *)"Premature EOF.");
	return 1;
}

static int find_ep_name(struct int_cfg_parms *cfg, char *name, struct int_cfg_ep **ep)
{
	uint32_t i;

	for (i = 0; i < cfg->ep_cnt; i++) {
		if (!strcmp(name, cfg->eps[i].name) && 
			(strlen(name) == strlen(cfg->eps[i].name))) {
			*ep = &cfg->eps[i];
			return 0;
		}
	}
	*ep = NULL;
	return 1;
}

static int find_sw_name(struct int_cfg_parms *cfg, char *name, struct int_cfg_sw **sw)
{
	uint32_t i;

	for (i = 0; i < cfg->sw_cnt; i++) {
		if (!strcmp(name, cfg->sws[i].name) && 
			(strlen(name) == strlen(cfg->sws[i].name))) {
			*sw = &cfg->sws[i];
			return 0;
		}
	}
	*sw = NULL;
	return 1;
}

static int find_ep_and_port(struct int_cfg_parms *cfg, char *tok,
			struct int_cfg_ep **ep, int *port)
{
	char *temp = NULL;
	uint32_t tmp;

	*port = 0;
	*ep = NULL;

	temp = strchr(tok, '.');
	if (NULL == temp) {
		goto fail;
	}

	if ('.' == temp[0]) {
		temp[0] = '\0';
		if (tok_parse_ulong(&temp[1], &tmp, 0, CFG_MAX_EP_PORT, 0)) {
			parse_err(cfg, (char *)"Illegal port index.");
			goto fail;
		}
		*port = (int)tmp;
	}

	if (find_ep_name(cfg, tok, ep)) {
		goto fail;
	}

	if (!(*ep)->ports[*port].valid) {
		parse_err(cfg, (char *)"Invalid port selected.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int get_ep_sw_and_port(struct int_cfg_parms *cfg, struct int_cfg_conn *conn,
			int idx)
{
	char *temp = NULL, *tok = NULL;
	uint32_t tmp;

	conn->ends[idx].port_num = 0;
	conn->ends[idx].ep_h = NULL;
	conn->ends[idx].sw_h = NULL;

	if (get_next_token(cfg, &tok)) {
		parse_err(cfg, (char *)"No tokens.");
		goto fail;
	}

	temp = strchr(tok, '.');
	if (NULL == temp) {
		parse_err(cfg, (char *)"Illegal token.");
		goto fail;
	}

	if ('.' == temp[0]) {
		temp[0] = '\0';
		if (tok_parse_port_num(&temp[1], &tmp, 0)) {
			parse_err(cfg, (char *)"Illegal port index.");
			goto fail;
		}
		conn->ends[idx].port_num = (int)tmp;
	}

	if (!find_ep_name(cfg, tok, &conn->ends[idx].ep_h)) {
		if (conn->ends[idx].port_num >= CFG_MAX_EP_PORT) {
			parse_err(cfg, (char *)"Illegal port index.");
			goto fail;
		}
		if (!conn->ends[idx].ep_h->ports[conn->ends[idx].port_num].valid) {
			parse_err(cfg, (char *)"Invalid port selected.");
			goto fail;
		}
		conn->ends[idx].ep = 1;
		conn->ends[idx].ep_h->ports[conn->ends[idx].port_num].conn
			= conn;
		conn->ends[idx].ep_h->ports[conn->ends[idx].port_num].conn_end
			= idx;
		return 0;
	}

	if (!find_sw_name(cfg, tok, &conn->ends[idx].sw_h)) {
		if (conn->ends[idx].port_num >= CFG_MAX_SW_PORT) {
			parse_err(cfg, (char *)"Illegal port index.");
			goto fail;
		}
		if (!conn->ends[idx].sw_h->ports[conn->ends[idx].port_num].valid) {
			parse_err(cfg, (char *)"Invalid port selected.");
			goto fail;
		}
		conn->ends[idx].ep = 0;
		conn->ends[idx].sw_h->ports[conn->ends[idx].port_num].conn 
			= conn;
		conn->ends[idx].sw_h->ports[conn->ends[idx].port_num].conn_end 
			= idx;
		return 0;
	}
	parse_err(cfg, (char *)"Unknown device.");

fail:
	return 1;
}

static int cfg_get_destid(struct int_cfg_parms *cfg, did_val_t *did_val, uint32_t devid_sz)
{
	int port = 0;
	char *tok;
	struct int_cfg_ep *ep;

	if (get_next_token(cfg, &tok)) {
		goto fail;
	}

	if (find_ep_and_port(cfg, tok, &ep, &port)) {
		if (cfg->init_err) {
			goto fail;
		}
		if (tok_parse_did(tok, did_val, 16)) {
			goto fail;
		}
		return 0;
	}


	if (!ep->ports[port].devids[devid_sz].valid) {
		parse_err(cfg, (char *)"Unconfigured devid selected.");
		goto fail;
	}
	*did_val = ep->ports[port].devids[devid_sz].did_val;
	return 0;

fail:
	parse_err(cfg, (char *)"cfg_get_destid error.");
	return 1;
}

#define MEM_SZ_TOKENS "mem34 mem50 mem66"

static int parse_mport_mem_size(struct int_cfg_parms *cfg, uint8_t *mem_sz)
{
	uint32_t idx;

	idx = get_parm_idx(cfg, (char *)MEM_SZ_TOKENS);
	switch (idx) {
	case 0: // mem34
		*mem_sz = CFG_MEM_SZ_34;
		break;
	case 1: // mem50
		*mem_sz = CFG_MEM_SZ_50;
		break;
	case 2: // mem66
		*mem_sz = CFG_MEM_SZ_66;
		break;
	default: // Unknown
		goto fail;
	}
	return 0;

fail:
	parse_err(cfg, (char *)"parse_mport_mem_size error.");
	return 1;
}

static int parse_ep_devids(struct int_cfg_parms *cfg, struct dev_id *devids)
{
	uint32_t devid_sz, done = 0;
	uint32_t tmp;

	for (devid_sz = 0; devid_sz < 3; devid_sz++) {
		devids[devid_sz].valid = 0;
		devids[devid_sz].hc = HC_MP;
		devids[devid_sz].did_val = 0;
	}

	while (!done) {
		devid_sz = get_parm_idx(cfg, (char *)DEVID_SZ_TOKENS_END);
		switch (devid_sz) {
			case 0: // dev08
			case 1: // dev16
			case 2: // dev32
				// only one of each size
				if (devids[devid_sz].valid) {
					goto fail;
				}

				if (get_hex_int(cfg, &devids[devid_sz].did_val)) {
					goto fail;
				}

				if (get_dec_int(cfg, &tmp)) {
					goto fail;
				}

				if (tmp > HC_MP) {
					goto fail;
				}

				devids[devid_sz].hc = (hc_t)tmp;
				devids[devid_sz].valid = 1;
				break;
			case 3: // END
				done = 1;
				break;
			default:
				goto fail;
		}
	}
	return 0;

fail:
	parse_err(cfg, (char *)"parse_ep_devids error.");
	return 1;
}

static int check_match (struct dev_id *mp_did, struct dev_id *ep_did,
		struct int_mport_info *mpi, struct int_cfg_parms *cfg, 
		struct int_cfg_ep *ep, int pnum)
{
	if (!mp_did->valid) {
		goto exit;
	}

	if (!ep_did->valid) {
		goto exit;
	}

	if (mp_did->did_val != ep_did->did_val) {
		goto exit;
	}

	if (mp_did->hc != ep_did->hc) {
		goto exit;
	}

	if (mpi->ep != NULL) {
		parse_err(cfg, (char *)"Duplicate MPORT definitions");
		goto fail;
	}

	mpi->ep = ep;
	mpi->ep_pnum = pnum;
	mpi->ct = ep->ports[mpi->ep_pnum].ct;

exit:
	return 0;
fail:
	return -1;
}

static int match_ep_to_mports(struct int_cfg_parms *cfg, struct int_cfg_ep_port *ep_p,
			int pt_i, struct int_cfg_ep *ep)
{
	uint32_t mp_i, did_sz;
	struct dev_id *mp_did = NULL;
	struct dev_id *ep_did = ep_p->devids;

	for (mp_i = 0; mp_i < cfg->max_mport_info_idx; mp_i++) {
		mp_did = cfg->mport_info[mp_i].devids;
		for (did_sz = 0; did_sz < CFG_DEVID_MAX; did_sz++) {
			if (check_match( &mp_did[did_sz], &ep_did[did_sz],
					&cfg->mport_info[mp_i], cfg, ep, pt_i)) {
				return -1;
			}
		}
	}
	return 0;
}

static int parse_mport_info(struct int_cfg_parms *cfg)
{
	int idx, i;

	if (cfg->max_mport_info_idx >= CFG_MAX_MPORTS) {
		parse_err(cfg, (char *)"Too many MPORTs.");
		goto fail;
	}

	idx = cfg->max_mport_info_idx;
	if (get_dec_int(cfg, &cfg->mport_info[idx].num))
		goto fail;

	cfg->max_mport_info_idx++;

	for (i = 0; i < idx; i++) {
		if (cfg->mport_info[i].num == cfg->mport_info[idx].num) {
			parse_err(cfg, (char *)"Duplicate mport number.");
			goto fail;
		}
	}

	switch (get_parm_idx(cfg, (char *)"master slave")) {
	case 0: // "master"
		if (CFG_SLAVE != cfg->mast_idx) {
			parse_err(cfg, 
			(char *)"Only one MPORT can be master for now.");
			goto fail;
		}
		cfg->mport_info[idx].op_mode = CFG_OP_MODE_MASTER;
		cfg->mast_idx = idx;
		break;
	case 1: // "slave"
		cfg->mport_info[idx].op_mode = CFG_OP_MODE_SLAVE;
		break;
	default:
		parse_err(cfg, (char *)"Unknown operating mode.");
		goto fail;
	}

	if (parse_mport_mem_size(cfg, &cfg->mport_info[idx].mem_sz)) {
		goto fail;
	}

	return parse_ep_devids(cfg, cfg->mport_info[idx].devids);

fail:
	parse_err(cfg, (char *)"parse_mport_info error.");
	return 1;
}

static int parse_master_info(struct int_cfg_parms *cfg)
{
	if (get_devid_sz(cfg, &cfg->mast_did_sz)) {
		goto fail;
	}

	if (get_hex_int(cfg, &cfg->mast_did_val)) {
		goto fail;
	}

	if (get_dec_int(cfg, &cfg->mast_cm_port)) {
		goto fail;
	}
	return 0;

fail:
	parse_err(cfg, (char *)"parse_master_info error.");
	return 1;
}

static int parse_mc_mask(struct int_cfg_parms *cfg, rio_rt_mc_info_t *mc_info)
{
	uint32_t mc_mask_idx, done = 0, pnum;
	char *tok = NULL;

	if (get_dec_int(cfg, &mc_mask_idx)) {
		goto fail;
	}

	if (mc_mask_idx >= RIO_MAX_MC_MASKS) {
		parse_err(cfg, (char *)"Illegal multicast mask index.");
		goto fail;
	}

	mc_info[mc_mask_idx].mc_destID = 0;
	mc_info[mc_mask_idx].tt = tt_dev8;
	mc_info[mc_mask_idx].mc_mask = 0;

	while (!done) {
		if (get_next_token(cfg, &tok)) {
			goto fail;
		}

		switch (parm_idx(tok, (char *)"END")) {
		case 0: // END
			done = 1;
			break;
		default: 
			if (tok_parse_port_num(tok, &pnum, 0)) {
				parse_err(cfg, (char *)"Illegal multicast port.");
				goto fail;
			}
			mc_info[mc_mask_idx].mc_mask |= (1 << pnum);
			break;
		}
	}

	mc_info[mc_mask_idx].in_use = 1;
	mc_info[mc_mask_idx].allocd = 1;
	mc_info[mc_mask_idx].changed = 1;
	return 0;

fail:
	parse_err(cfg, (char *)"parse_mc_mask error.");
	return 1;
}

static int get_lane_speed(struct int_cfg_parms *cfg, rio_pc_ls_t *ls)
{
	switch (get_parm_idx(cfg, (char *)"1p25 2p5 3p125 5p0 6p25")) {
	case 0: // 1p25
		*ls = rio_pc_ls_1p25;
		break;
	case 1: // 2p5
		*ls = rio_pc_ls_2p5;
		break;
	case 2: // 3p125
		*ls = rio_pc_ls_3p125;
		break;
	case 3: // 5p0
		*ls = rio_pc_ls_5p0;
		break;
	case 4: // 6p25
		*ls = rio_pc_ls_6p25;
		break;
	default:
		parse_err(cfg, (char *)"Unknown lane speed.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int get_port_width(struct int_cfg_parms *cfg, rio_pc_pw_t *pw)
{
	switch (get_parm_idx(cfg, (char *)"1x 2x 4x 1x_l0 1x_l1 1x_l2")) {
	case 0: // 1x
		*pw = rio_pc_pw_1x;
		break;
	case 1: // 2x
		*pw = rio_pc_pw_2x;
		break;
	case 2: // 4x
		*pw = rio_pc_pw_4x;
		break;
	case 3: // 1x lane 0
		*pw = rio_pc_pw_1x_l0;
		break;
	case 4: // 1x lane 1
		*pw = rio_pc_pw_1x_l1;
		break;
	case 5: // 1x lane 2
		*pw = rio_pc_pw_1x_l2;
		break;
	default:
		parse_err(cfg, (char *)"Unknown port width.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int get_idle_seq(struct int_cfg_parms *cfg, int *idle)
{
	*idle = get_parm_idx(cfg, (char *)"IDLE1 IDLE2");
	if ((*idle < 0) || (*idle > 1)) {
		parse_err(cfg, (char *)"Unknown idle sequence.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int parse_rapidio(struct int_cfg_parms *cfg, struct int_cfg_rapidio *rio)
{
	if (get_port_width(cfg, &rio->max_pw)) {
		goto fail;
	}

	if (get_port_width(cfg, &rio->op_pw)) {
		goto fail;
	}

	if (get_lane_speed(cfg, &rio->ls)) {
		goto fail;
	}

	if (get_idle_seq(cfg, &rio->idle2)) {
		goto fail;
	}

	switch (get_parm_idx(cfg, (char *)"EM_OFF EM_ON")) {
	case 0: // "OFF" 
		rio->em = 0;
		break;
	case 1: // "OFF" 
		rio->em = 1;
		break;
	default:
		parse_err(cfg, (char *)"Unknown error management config.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int parse_ep_port(struct int_cfg_parms *cfg, struct int_cfg_ep_port *prt)
{

	if (get_dec_int(cfg, &prt->port)) {
		goto fail;
	}

	if (get_hex_int(cfg, &prt->ct)) {
		goto fail;
	}

	if (parse_rapidio(cfg, &prt->rio)) {
		goto fail;
	}

	if (parse_ep_devids(cfg, prt->devids)) {
		goto fail;
	}
	prt->valid = 1;
	return 0;

fail:
	return 1;
}

static int parse_endpoint(struct int_cfg_parms *cfg)
{
	int i = cfg->ep_cnt;
	int done = 0;

	if (i >= CFG_MAX_EP) {
		parse_err(cfg, (char *)"Too many endpoints.");
		goto fail;
	}

	if (get_string(cfg, &cfg->eps[i].name)) {
		goto fail;
	}

	cfg->eps[i].port_cnt = 0;
	while (!done && (cfg->eps[i].port_cnt < CFG_MAX_EP_PORT + 1)) {
		int pt_i;
		switch (get_parm_idx(cfg, (char *)"PORT PEND")) {
		case 0: // "PORT"
			pt_i = cfg->eps[i].port_cnt;
			if (cfg->eps[i].port_cnt >= CFG_MAX_EP_PORT) {
				parse_err(cfg, (char *)"Too many ports!");
				goto fail;
			}

			if (parse_ep_port(cfg, &cfg->eps[i].ports[pt_i])) {
				goto fail;
			}

			cfg->eps[i].port_cnt++;
			if (match_ep_to_mports(cfg, &cfg->eps[i].ports[pt_i], 
						pt_i, &cfg->eps[i])) {
				goto fail;
			}
			break;
		case 1: // "PEND"
			done = 1;
			break;
		default:
			parse_err(cfg, (char *)"Unknown parameter.");
			goto fail;
		}
	}
				
	cfg->eps[i].valid = 1;
	cfg->ep_cnt++;
	return 0;

fail:
	return 1;
}

static int assign_rt_v(int rt_sz, did_val_t st_did_val, did_val_t end_did_val, pe_rt_val rtv,
			rio_rt_state_t *rt, struct int_cfg_parms *cfg)
{
	did_val_t i;

	switch (rt_sz) {
	case 0: // dev08
		if ((st_did_val >= RIO_RT_GRP_SZ)
				|| (end_did_val >= RIO_RT_GRP_SZ)) {
			parse_err(cfg, (char *)"DestID value too large.");
			goto fail;
		}

		if (st_did_val > end_did_val) {
			did_val_t temp = end_did_val;
			end_did_val = st_did_val;
			st_did_val = temp;
		}

		for (i = st_did_val; i <= end_did_val; i++) {
			if (rt->dev_table[i].rte_val != rtv) {
				rt->dev_table[i].rte_val = rtv;
				rt->dev_table[i].changed = 1;
			}
		}
		break;
	case 1: // dev16
		parse_err(cfg, (char *)"Dev16 not supported yet.");
		goto fail;
	case 2: // dev32
		parse_err(cfg, (char *)"Dev32 not supported yet.");
		goto fail;
	default:
		parse_err(cfg, (char *)"Unknown rt size.");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

static int parse_sw_port(struct int_cfg_parms *cfg)
{
	uint32_t idx = cfg->sw_cnt;
	uint32_t port;

	if (get_dec_int(cfg, &port)) {
		goto fail;
	}

	if (parse_rapidio(cfg, &cfg->sws[idx].ports[port].rio)) {
		goto fail;
	}

	cfg->sws[idx].ports[port].valid = 1;
	cfg->sws[idx].ports[port].port  = port;

	return 0;

fail:
	return 1;
}

static int parse_switch(struct int_cfg_parms *cfg)
{
	uint32_t i, done = 0;
	uint32_t rt_sz = 0;
	did_val_t st_did_val;
	did_val_t end_did_val;
	uint32_t rtv;
	uint32_t tmp;
	rio_rt_state_t *rt = NULL;

	if (cfg->sw_cnt >= CFG_MAX_SW) {
		parse_err(cfg, (char *)"Too many switches.");
		goto fail;
	}
	i = cfg->sw_cnt;

	if (get_string(cfg, &cfg->sws[i].dev_type))
		goto fail;
	if (get_string(cfg, &cfg->sws[i].name))
		goto fail;
	if (get_devid_sz(cfg, &cfg->sws[i].did_sz))
		goto fail;
	if (get_hex_int(cfg, &cfg->sws[i].did_val))
		goto fail;
	if (get_dec_int(cfg, &tmp))
		goto fail;
	if (tmp > HC_MP)
		goto fail;

	cfg->sws[i].hc = tmp;
	if (get_hex_int(cfg, &cfg->sws[i].ct)) {
		goto fail;
	}

	while (!done) {
		switch(get_parm_idx(cfg, (char *)
		"PORT ROUTING_TABLE DFLTPORT DESTID RANGE MCMASK END")) {
		case 0: // PORT
			if (parse_sw_port(cfg)) {
				goto fail;
			}
			break;
		case 1: // ROUTING_TABLE
			char *token;

			rt_sz = get_parm_idx(cfg, (char *)DEVID_SZ_TOKENS);
			if (rt_sz > 2) {
				parse_err(cfg, (char *)"Unknown devID size.");
				goto fail;
			}

			if (get_next_token(cfg, &token)) {
				goto fail;
			}

			switch (parm_idx(token, (char *)"GLOBAL")) {
			case 0:
				rt = &cfg->sws[i].rt[rt_sz];
				cfg->sws[i].rt_valid[rt_sz] = true;
				break;
			default:
				uint32_t port;
				if (tok_parse_port_num(token, &port, 0)) {
					parse_err(cfg, (char *)"Illegal port.");
					goto fail;
				}

				rt = &cfg->sws[i].ports[port].rt[rt_sz];
				cfg->sws[i].ports[port].rt_valid[rt_sz] = true;
				if (cfg->sws[i].rt_valid[rt_sz]) {
					memcpy(rt, &cfg->sws[i].rt[rt_sz],
							sizeof(rio_rt_state_t));
				}
				break;
			}
				
			break;
		case 2: // DFLTPORT
			if (get_rt_v(cfg, &rtv)) {
				goto fail;
			}

			// klocwork sees rt as null, but...
			// rt is set whenever ROUTING_TABLE option (above) is hit
			if (NULL == rt) {
				parse_err(cfg, (char *)"DFLTPORT: rt not set.");
				goto fail;
			}
			rt->default_route = rtv;
			break;
		case 3: // DESTID
			if (cfg_get_destid(cfg, &st_did_val, rt_sz)) {
				goto fail;
			}

			if (get_rt_v(cfg, &rtv)) {
				goto fail;
			}

			// klocwork sees rt as null, but...
			// rt is set whenever ROUTING_TABLE option (above) is hit
			//
			// That option, if successfull, will set
			// cfg->sws[i].rt_valid[rt_sz] = true; or
			// cfg->sws[i].ports[port].rt_valid[rt_sz] = true;
			//
			if (NULL == rt) {
				parse_err(cfg, (char *)"DESTID: rt not set.");
				goto fail;
			}

			if (assign_rt_v(rt_sz, st_did_val, st_did_val, rtv, rt, cfg)) {
				parse_err(cfg, (char *)"Illegal destID/rtv.");
				goto fail;
			}
			break;
		case 4: // RANGE
			if (cfg_get_destid(cfg, &st_did_val, rt_sz)) {
				goto fail;
			}

			if (cfg_get_destid(cfg, &end_did_val, rt_sz)) {
				goto fail;
			}

			if (get_rt_v(cfg, &rtv)) {
				goto fail;
			}

			// klocwork - see DESTID comment about rt
			if (NULL == rt) {
				parse_err(cfg, (char *)"rt not set.");
				goto fail;
			}

			if (assign_rt_v(rt_sz, st_did_val, end_did_val, rtv, rt, cfg)) {
				parse_err(cfg, (char *)"RANGE: Illegal destID/rtv.");
				goto fail;
			}
			break;
		case 5: // MCMASK
			// klocwork - see DESTID comment about rt
			if (NULL == rt) {
				parse_err(cfg, (char *)"MCMASK: rt not set.");
				goto fail;
			}

			if (parse_mc_mask(cfg, rt->mc_masks)) {
				goto fail;
			}
			break;
		case 6: // END
			done = 1;
			break;
		default:
			parse_err(cfg, (char *)"Unknown parameter.");
			goto fail;
		}
	}

	cfg->sws[i].valid = 1;
	cfg->sw_cnt++;
	return 0;

fail:
	return 1;
}

static int parse_connect(struct int_cfg_parms *cfg)
{
	int idx = cfg->conn_cnt;

	if (cfg->conn_cnt >= CFG_MAX_CONN) {
		parse_err(cfg, (char *)"Too many connections.");
		goto fail;
	}

	if (get_ep_sw_and_port(cfg, &cfg->cons[idx], 0)) {
		goto fail;
	}

	if (get_ep_sw_and_port(cfg, &cfg->cons[idx], 1)) {
		goto fail;
	}

	cfg->conn_cnt++;
	cfg->cons[idx].valid = 1;

	return 0;
fail:
	return 1;
}

static int fmd_parse_cfg(struct int_cfg_parms *cfg)
{
	char *tok;
	// size_t byte_cnt = LINE_SIZE;

	tok = try_get_next_token(cfg);

	while ((NULL != tok) && !cfg->init_err) {
		switch (parm_idx(tok, (char *)
	"// DEV_DIR DEV_DIR_MTX MPORT MASTER_INFO ENDPOINT SWITCH CONNECT AUTO EOF")) {
		case 0: // "//"
			flush_comment(tok);
			break;
		case 1: // "DEV_DIR"
			if (get_next_token(cfg, &tok)) {
				break;
			}

			if (get_v_str(&cfg->dd_fn, tok, 1)) {
				parse_err(cfg, (char *)"Bad directory name.");
			}
			break;
		case 2: // "DEV_DIR_MTX"
			if (get_next_token(cfg, &tok)) {
				break;
			}

			if (get_v_str(&cfg->dd_mtx_fn, tok, 1)) {
				parse_err(cfg, (char *)"Bad directory name.");
			}
			break;
		case 3: // "MPORT"
			parse_mport_info(cfg);
			break;
		case 4: // "MASTER_INFO"
			parse_master_info(cfg);
			break;
		case 5: // "ENDPOINT"
			parse_endpoint(cfg);
			break;
		case 6: // "SWITCH"
			parse_switch(cfg);
			break;
		case 7: // "CONNECT"
			parse_connect(cfg);
			break;
		case 8: // "AUTO"
			cfg->auto_config = true;
			break;
		case 9: // "EOF"
			DBG((char *)"\n");
			goto exit;
			break;
		default:
			parse_err(cfg, (char *)"Unknown parameter.");
			goto exit;
		}
		tok = try_get_next_token(cfg);
	}

exit:
	free(line);
	line = NULL;
	return cfg->init_err;
}

int cfg_parse_file(char *cfg_fn, char **dd_mtx_fn, char **dd_fn,
		did_t *m_did, uint32_t *m_cm_port, uint32_t *m_mode)
{
	int j,k;
	uint32_t i;

	ct_t ct;
	ct_nr_t nr;

	did_t did;
	did_val_t did_val;
	did_sz_t did_sz;

	struct int_cfg_ep ep;
	struct int_cfg_ep_port port;
	struct int_cfg_sw sw;

	if (init_cfg_ptr()) {
		goto fail;
	}

	INFO("\nCFG: Opening configuration file \"%s\"...\n", cfg_fn);
	cfg_fd = fopen(cfg_fn, "r");
	if (NULL == cfg_fd) {
		WARN("CFG: Config file open failed, errno %d : %s\n",
				errno, strerror(errno));
		goto fail;
	}

	DBG("\nCFG: Config file contents:");
	fmd_parse_cfg(cfg);
	
	if (fclose(cfg_fd)) {
		ERR("CFG: Config file close failed, errno %d : %s\n",
				errno, strerror(errno));
	}

	cfg_fd = NULL;

	//@sonar:off - Collapsible "if" statements should be merged
	if (cfg->dd_mtx_fn && strlen(cfg->dd_mtx_fn)) {
		if (update_string(dd_mtx_fn, cfg->dd_mtx_fn, strlen(cfg->dd_mtx_fn))) {
			goto fail;
		}
	}

	if (cfg->dd_fn && strlen(cfg->dd_fn)) {
		if (update_string(dd_fn, cfg->dd_fn, strlen(cfg->dd_fn))) {
			goto fail;
		}
	}
	//@sonar:on

	if (cfg->init_err) {
		goto fail;
	}

	// mark the endpoint ct and devIds from the configuration as in use
	for(i = 0; i < cfg->ep_cnt; i++) {
		ep = cfg->eps[i];
		if (ep.valid) {
			for (j = 0; j < ep.port_cnt; j++) {
				port = ep.ports[j];
				if (port.valid) {
					if (ct_get_nr(&nr, (ct_t)port.ct)) {
						goto fail;
					}

					for (k = 0; k < CFG_DEVID_MAX; k++) {
						// Continue for unsupported sizes
						if (did_size_from_int(&did_sz, k)) {
							continue;
						}

						did_val = port.devids[k].did_val;
						if (did_val && ct_create_from_data(&ct, &did, nr, did_val, did_sz)) {
							goto fail;
						}
					}
				}
			}
		}
	}

	// mark the switch ct and devIds from the configuration as in use
	for(i=0; i < cfg->sw_cnt; i++) {
		sw = cfg->sws[i];
		if (sw.valid) {
			if (ct_get_nr(&nr, sw.ct)) {
				goto fail;
			}

			// Continue for unsupported sizes
			if (did_size_from_int(&did_sz, sw.did_sz)) {
				continue;
			}

			if (ct_create_from_data(&ct, &did, nr, sw.did_val, did_sz)) {
				goto fail;
			}
		}
	}

	// update parameters
	if (CFG_SLAVE == cfg->mast_idx) {
		// fake out the creation of the master did
		did_size_from_int(&did_sz, cfg->mast_did_sz);
		*m_did = (did_t){cfg->mast_did_val, did_sz};
	} else {
		if (cfg_auto()) {
			// fake out the creation of the master did
			did_size_from_int(&did_sz, cfg->mast_did_sz);
			*m_did = (did_t){cfg->mast_did_val, did_sz};
		} else {
			// ensure the master did was created
			if (did_from_value(m_did, cfg->mast_did_val,
					cfg->mast_did_sz)) {
				goto fail;
			}
		}
	}
	*m_cm_port = cfg->mast_cm_port;
	*m_mode = !(CFG_SLAVE == cfg->mast_idx);

	return 0;

fail:
	return 1;
}

struct int_cfg_sw *find_cfg_sw_by_ct(ct_t ct, struct int_cfg_parms *cfg)
{
	struct int_cfg_sw *ret = NULL;
	uint32_t i;

	for (i = 0; i < cfg->sw_cnt; i++) {
		if (cfg->sws[i].ct == ct) {
			ret = &cfg->sws[i];
			break;
		}
	}
	return ret;
}

struct int_cfg_ep *find_cfg_ep_by_ct(ct_t ct, struct int_cfg_parms *cfg)
{
	struct int_cfg_ep *ret = NULL;
	uint32_t i, p;

	for (i = 0; (i < cfg->ep_cnt) && (NULL == ret); i++) {
		if (!cfg->eps[i].valid) {
			continue;
		}
		for (p = 0; (p < CFG_MAX_EP_PORT) && (NULL == ret); p++) {
			if (!cfg->eps[i].ports[p].valid) {
				continue;
			}
			if (cfg->eps[i].ports[p].ct == ct) {
				ret = &cfg->eps[i];
				break;
			}
		}
	}

	for (i = 0; (i < cfg->max_mport_info_idx) && (NULL == ret); i++) {
		if (cfg->mport_info[i].ct == ct) {
			ret = cfg->mport_info[i].ep;
		}
	}

	return ret;
}

int cfg_find_mport(uint32_t mport, struct cfg_mport_info *mp)
{
	unsigned int i;

	for (i = 0; i < cfg->max_mport_info_idx; i++) {
		if (mport != cfg->mport_info[i].num) {
			continue;
		}

		mp->num = cfg->mport_info[i].num;
		mp->ct = cfg->mport_info[i].ct;
		memcpy(mp->devids, cfg->mport_info[i].devids,
			sizeof(mp->devids));
		return 0;
	}
	return 1;
}

int cfg_get_mp_mem_sz(uint32_t mport, uint8_t *mem_sz)
{
	unsigned int i;

	for (i = 0; i < cfg->max_mport_info_idx; i++) {
		if (mport != cfg->mport_info[i].num) {
			continue;
		}

		*mem_sz = cfg->mport_info[i].mem_sz;
		return 0;
	}
	return 1;
}

static int fill_in_dev_from_ep(struct cfg_dev *dev, struct int_cfg_ep *ep)
{
	if (!ep->ports[0].valid) {
		goto fail;
	}

	memset(dev, 0, sizeof(struct cfg_dev));
	dev->name = ep->name;
	dev->is_sw = 0;
	dev->ct = ep->ports[0].ct;
	dev->ep_pt.valid = 1;
	dev->ep_pt.op_pw = ep->ports[0].rio.op_pw;
	dev->ep_pt.ls = ep->ports[0].rio.ls;
	memcpy(dev->ep_pt.devids, ep->ports[0].devids,
		sizeof(dev->ep_pt.devids));

	return 0;

fail:
	return 1;
}

static int fill_in_dev_from_sw(struct cfg_dev *dev, struct int_cfg_sw *sw)
{
	int i;

	memset(dev, 0, sizeof(struct cfg_dev));

	dev->name = sw->name;
	dev->ct = sw->ct;
	dev->is_sw = 1;
	dev->sw_info.num_ports = CFG_MAX_SW_PORT;

	for (i = 0; i < CFG_MAX_SW_PORT; i++) {
		dev->sw_info.sw_pt[i].valid = sw->ports[i].valid;
		dev->sw_info.sw_pt[i].port = sw->ports[i].port;
		dev->sw_info.sw_pt[i].op_pw = sw->ports[i].rio.op_pw;
		dev->sw_info.sw_pt[i].ls = sw->ports[i].rio.ls;

		for (int sz = 0; sz < CFG_DEVID_MAX; sz++) {
			if (sw->ports[i].rt_valid[sz]) {
				dev->sw_info.sw_pt[i].rt[sz] =
					&sw->ports[i].rt[sz];
			} else {
				dev->sw_info.sw_pt[i].rt[sz] = NULL;
			}
		}
	}

	for (int sz = 0; sz < CFG_DEVID_MAX; sz++) {
		if (sw->rt_valid[sz]) {
			dev->sw_info.rt[sz] = &sw->rt[sz];
		} else {
			dev->sw_info.rt[sz] = NULL;
		}
	}

	return 0;
}

int cfg_find_dev_by_ct(ct_t ct, struct cfg_dev *dev)
{
	struct int_cfg_ep *ep = NULL;
	struct int_cfg_sw *sw = NULL;

	ep = find_cfg_ep_by_ct(ct, cfg);
	if (NULL != ep) {
		return fill_in_dev_from_ep(dev, ep);
	}

	sw = find_cfg_sw_by_ct(ct, cfg);
	if (NULL != sw) {
		return fill_in_dev_from_sw(dev, sw);
	}

	return 1;
}

int cfg_get_conn_dev(ct_t ct, int pt,
		struct cfg_dev *dev, int *conn_pt)
{
	struct int_cfg_conn *conn = NULL;
	int conn_end = -1, oe;
	struct int_cfg_ep *ep;

	ep = find_cfg_ep_by_ct(ct, cfg);

	if (NULL != ep) {
		if (pt) {
			goto fail;
		}
		conn = ep->ports[0].conn;
		conn_end = ep->ports[0].conn_end;
	}

	if (NULL == ep) {
		struct int_cfg_sw *sw;

		sw = find_cfg_sw_by_ct(ct, cfg);
		if (NULL != sw) {
			if (pt >= CFG_MAX_SW_PORT) {
				goto fail;
			}

			if (!sw->ports[pt].valid) {
				goto fail;
			}

			if (sw->ports[pt].port != pt) {
				goto fail;
			}
			conn = sw->ports[pt].conn;
			conn_end = sw->ports[pt].conn_end;
		}
	}

	if (NULL == conn) {
		goto fail;
	}

	if (!conn->valid) {
		goto fail;
	}

	oe = OTHER_END(conn_end);
	if ((conn_end > 1) || (oe > 1)) {
		goto fail;
	}

	*conn_pt = conn->ends[oe].port_num;
	if (conn->ends[oe].ep) {
		return fill_in_dev_from_ep(dev, conn->ends[oe].ep_h);
	} else {
		return fill_in_dev_from_sw(dev, conn->ends[oe].sw_h);
	}

fail:
	return 1;
}

bool cfg_auto(void)
{
	return cfg->auto_config;
}

#ifdef __cplusplus
}
#endif


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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>

#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#include "cfg.h"
#include "cfg_private.h"
#include "libcli.h"
#include "liblog.h"
#include "ct_test.h"
#include "did_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MASTER_SUCCESS (char *)("test/master_success.cfg")
#define SLAVE_SUCCESS (char *)("test/slave_success.cfg")
#define TOR_SUCCESS   (char *)("test/tor_success.cfg")
#define RXS_SUCCESS (char *)("test/rxs_success.cfg")

int test_case_1(void)
{
	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	char *test_dd_mtx_fn = (char *)FMD_DFLT_DD_MTX_FN;
	char *test_dd_fn  = (char *)FMD_DFLT_DD_FN;
	uint8_t mem_sz;

	uint32_t m_did, m_cm_port, m_mode;

	if (cfg_parse_file(MASTER_SUCCESS, &dd_mtx_fn, &dd_fn, &m_did,
			&m_cm_port, &m_mode))
		goto fail;

	if (strncmp(dd_mtx_fn, test_dd_mtx_fn, strlen(dd_mtx_fn)))
		goto fail;

	if (strncmp(dd_fn, test_dd_fn, strlen(test_dd_fn)))
		goto fail;

	if (5 != m_did) 
		goto fail;

	if (FMD_DFLT_MAST_CM_PORT != m_cm_port)
		goto fail;

	if (cfg_get_mp_mem_sz(0, &mem_sz))
		goto fail;
	if (mem_sz != CFG_MEM_SZ_34)
		goto fail;

	if (cfg_get_mp_mem_sz(1, &mem_sz))
		goto fail;
	if (mem_sz != CFG_MEM_SZ_50)
		goto fail;

	if (cfg_get_mp_mem_sz(2, &mem_sz))
		goto fail;
	if (mem_sz != CFG_MEM_SZ_66)
		goto fail;

	if (!m_mode)
		goto fail;

	return 0;
fail:
	return 1;

};

int test_case_2(void)
{
	int i;
	int rc = 1;
	char fn[90];

	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	uint32_t m_did, m_cm_port, m_mode;
	

	for (i = 0; (i < 10) && rc; i++) {
		snprintf(fn, 90, "test/parse_fail_%d.cfg", i);
	
		rc = cfg_parse_file(fn, &dd_mtx_fn, &dd_fn, &m_did,
			&m_cm_port, &m_mode);
		free(cfg);
		if (rc)
			printf("\nTest case 2 test %d passed", i);
		else
			printf("\nTest case 2 test %d FAILED", i);
	};

	return !rc;
};

int test_case_3(void)
{
	struct cfg_mport_info mp;
	struct cfg_dev dev;
	int conn_pt;
	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	uint32_t m_did, m_cm_port, m_mode;

	did_reset();
	ct_reset();
	memset(&dev, 0, sizeof(dev));
	if (cfg_parse_file(MASTER_SUCCESS, &dd_mtx_fn, &dd_fn, &m_did,
			&m_cm_port, &m_mode))
		goto fail;

	if (cfg_find_mport(0, &mp))
		goto fail;

	if (mp.num != 0)
		goto fail;
	if (mp.ct != 0x10005)
		goto fail;
	if (mp.op_mode != 1)
		goto fail;

	if (!cfg_find_mport(3, &mp))
		goto fail;

	if (cfg_find_dev_by_ct(0x10005, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_5p0)
		goto fail;

	if (cfg_find_dev_by_ct(0x20006, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_6p25)
		goto fail;

	if (cfg_find_dev_by_ct(0x30007, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_1p25)
		goto fail;

	if (cfg_find_dev_by_ct(0x40008, &dev))
		goto fail;

	if (cfg_find_dev_by_ct(0x70009, &dev))
		goto fail;

	if (dev.sw_info.sw_pt[0].max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.sw_info.sw_pt[0].op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.sw_info.sw_pt[0].ls != idt_pc_ls_6p25)
		goto fail;

	if (dev.sw_info.sw_pt[1].max_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.sw_info.sw_pt[1].op_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.sw_info.sw_pt[1].ls != idt_pc_ls_5p0)
		goto fail;

	if (dev.sw_info.sw_pt[2].max_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.sw_info.sw_pt[2].op_pw != idt_pc_pw_1x)
		goto fail;
	if (dev.sw_info.sw_pt[2].ls != idt_pc_ls_3p125)
		goto fail;

	if (dev.sw_info.sw_pt[3].max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.sw_info.sw_pt[3].op_pw != idt_pc_pw_1x)
		goto fail;
	if (dev.sw_info.sw_pt[3].ls != idt_pc_ls_2p5)
		goto fail;

	if (dev.sw_info.sw_pt[4].max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.sw_info.sw_pt[4].op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.sw_info.sw_pt[4].ls != idt_pc_ls_1p25)
		goto fail;

	if (cfg_get_conn_dev(0x70009, 0, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x70009, 1, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x70009, 4, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x70009, 5, &dev, &conn_pt))
		goto fail;

	if (!cfg_get_conn_dev(0x70009, 7, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x10005, 0, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x20006, 0, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x30007, 0, &dev, &conn_pt))
		goto fail;

	if (cfg_get_conn_dev(0x40008, 0, &dev, &conn_pt))
		goto fail;

	if (!cfg_get_conn_dev(0x40008, 1, &dev, &conn_pt))
		goto fail;

	if (!cfg_find_dev_by_ct(0x99999, &dev))
		goto fail;

	return 0;
fail:
	return 1;
};
	
int test_case_4(void)
{
	struct cfg_mport_info mp;
	struct cfg_dev dev;
	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	char *test_dd_mtx_fn = (char *)FMD_DFLT_DD_MTX_FN;
	char *test_dd_fn = (char *)FMD_DFLT_DD_FN;
	uint32_t m_did, m_cm_port, m_mode;

	did_reset();
	ct_reset();
	if (cfg_parse_file(SLAVE_SUCCESS, &dd_mtx_fn, &dd_fn, &m_did,
			&m_cm_port, &m_mode))
		goto fail;

	if (strncmp(dd_mtx_fn, test_dd_mtx_fn, strlen(dd_mtx_fn)))
		goto fail;

	if (strncmp(dd_fn, test_dd_fn, strlen(test_dd_fn)))
		goto fail;

	if (5 != m_did)
		goto fail;

	if (FMD_DFLT_MAST_CM_PORT != m_cm_port)
		goto fail;

	if (m_mode)
		goto fail;

	if (cfg_find_mport(0, &mp))
		goto fail;

	if (!cfg_find_mport(3, &mp))
		goto fail;

	if (cfg_find_dev_by_ct(0x20006, &dev))
		goto fail;

	if (!cfg_find_dev_by_ct(0x70000, &dev))
		goto fail;

	return 0;
fail:
	return 1;
};
	
int test_case_5(void)
{
	struct cfg_mport_info mp;
	struct cfg_dev dev;
	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	char *test_dd_mtx_fn = (char *)FMD_DFLT_DD_MTX_FN;
	char *test_dd_fn = (char *)FMD_DFLT_DD_FN;
	uint32_t m_did, m_cm_port, m_mode;
	int p_idx, idx;
	int pnums[6] = {2, 3, 5, 6, 10, 11};
	uint8_t chk_pnum[6] = {0, 1, 4, 7, 8, 9};
	int x = 1;

	did_reset();
	ct_reset();
	if (cfg_parse_file(TOR_SUCCESS, &dd_mtx_fn, &dd_fn, &m_did,
			&m_cm_port, &m_mode))
		goto fail;

	if (strncmp(dd_mtx_fn, test_dd_mtx_fn, strlen(dd_mtx_fn)))
		goto fail;

	if (strncmp(dd_fn, test_dd_fn, strlen(test_dd_fn)))
		goto fail;

	if (0x1A != m_did)
		goto fail;

	if (FMD_DFLT_MAST_CM_PORT != m_cm_port)
		goto fail;

	if (!m_mode)
		goto fail;

	if (cfg_find_mport(0, &mp))
		goto fail;

	if (!cfg_find_mport(3, &mp))
		goto fail;

	if (cfg_find_dev_by_ct(0x21001A, &dev))
		goto fail;

	if (cfg_find_dev_by_ct(0x700F7, &dev))
		goto fail;

	/* Check out the switch routing table parsing in detail. */
	if (cfg_find_dev_by_ct(0x100f1, &dev))
		goto fail;

	if (!dev.is_sw)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->default_route != IDT_DSF_RT_NO_ROUTE)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x12].rte_val  != 2)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x13].rte_val  != 3)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x15].rte_val  != 5)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x16].rte_val  != 6)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x1A].rte_val  != 10)
		goto fail;

	if (dev.sw_info.rt[CFG_DEV08]->dev_table[0x1B].rte_val  != 11)
		goto fail;

	for (p_idx = 0; p_idx < 6; p_idx++) {
		int pnum = pnums[p_idx];
		for (idx = 0; idx <= RIO_LAST_DEV8; idx++) {
			if (dev.sw_info.sw_pt[pnum].rt[CFG_DEV08]->dev_table[idx].rte_val  != chk_pnum[p_idx]) {
				if ((2 == pnum) && ((idx >= 0xf7) && (idx <= 0xfc))) 
					continue;
				goto fail;
			};
		};
	};

	/* Check out connection parsing between endpoints & switches,
	* and between switches.
	*/

	for (int idx = 0; idx < 4; idx++) {
		struct cfg_dev ep, sw, rev_ep;
		int sw_pt, rev_pt;
		uint32_t ct[4] = { 0x21001A, 0x220015, 0x230012, 0x240013 };

		if (cfg_find_dev_by_ct(ct[idx], &ep)) {
			x = 5;
			goto fail;
		}

		if (cfg_get_conn_dev(ct[idx], 0, &sw, &sw_pt)) {
			x = 6;
			goto fail;
		}

		if (cfg_get_conn_dev(sw.ct, sw_pt, &rev_ep, &rev_pt)) {
			x = 7;
			goto fail;
		}

		if (memcmp(&ep, &rev_ep, sizeof(ep))) {
			x = 8;
			goto fail;
		}
		if (0 != rev_pt) {
			x = 9;
			goto fail;
		}
	};

	for (int sw = 1; sw < 7; sw++) {
		struct cfg_dev l0_sw, l1_sw, rev_dev;
		uint32_t ct = 0x000f0 + (0x10001 * sw);
		int port_list[6] = {0, 1, 4, 7, 8, 9};
		int pt_idx;
		int l0_pt, l1_pt, rev_pt;

		if (cfg_find_dev_by_ct(ct, &l0_sw))
			goto fail;
		for (pt_idx = 0; pt_idx < 6; pt_idx++) {
			l0_pt = port_list[pt_idx];

			if (cfg_get_conn_dev(ct, l0_pt, &l1_sw, &l1_pt))
				goto fail;

			if (cfg_get_conn_dev(l1_sw.ct, l1_pt, &rev_dev, &rev_pt))
				goto fail;
			if (memcmp(&l0_sw, &rev_dev, sizeof(l0_sw)))
				goto fail;
			if (rev_pt != l0_pt)
				goto fail;
		};
	};


	return 0;
fail:
	return x;
};

int test_case_6(void)
{
	struct cfg_dev dev;
	char *dd_mtx_fn = NULL, *dd_fn = NULL;
	char *test_dd_mtx_fn = (char *)FMD_DFLT_DD_MTX_FN;
	char *test_dd_fn = (char *)FMD_DFLT_DD_FN;
	uint32_t m_did, m_cm_port, m_mode;
	int conn_pt;
	int x = 1;

	if (cfg_parse_file(RXS_SUCCESS, &dd_mtx_fn, &dd_fn, &m_did,
		&m_cm_port, &m_mode))
		goto fail;

	if (strncmp(dd_mtx_fn, test_dd_mtx_fn, strlen(dd_mtx_fn)))
		goto fail;

	if (strncmp(dd_fn, test_dd_fn, strlen(test_dd_fn)))
		goto fail;

	if (5 != m_did)
		goto fail;

	if (FMD_DFLT_MAST_CM_PORT != m_cm_port)
		goto fail;

	if (cfg_find_dev_by_ct(0x10005, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_5p0)
		goto fail;

	if (cfg_find_dev_by_ct(0x20006, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_2x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_6p25)
		goto fail;

	if (cfg_find_dev_by_ct(0x30007, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_1p25)
		goto fail;

	if (cfg_find_dev_by_ct(0x40008, &dev))
		goto fail;

	if (dev.ep_pt.max_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.op_pw != idt_pc_pw_4x)
		goto fail;
	if (dev.ep_pt.ls != idt_pc_ls_2p5)
		goto fail;

	if (cfg_find_dev_by_ct(0x70000, &dev)) 
		goto fail;

	if (memcmp("RXS2448", dev.dev_type, sizeof("RXS2448")))
		goto fail;

        if (cfg_get_conn_dev(0x70000, 0, &dev, &conn_pt))
		goto fail;

	return 0;
fail:
	return x;
}
	
int main(int argc, char *argv[])
{
	int rc = EXIT_FAILURE;

	if (0)
		argv[0][0] = argc;

	rdma_log_init("cfg_test.log", 1);

	g_level = RDMA_LL_OFF;

	if (test_case_1()) {
		printf("\nTest_case_1 FAILED.");
		goto fail;
	};
	printf("\nTest_case_1 passed.");
	free(cfg);

	if (test_case_2()) {
		printf("\nTest_case_2 FAILED.");
		goto fail;
	};
	printf("\nTest_case_2 passed.");

	if (test_case_3()) {
		printf("\nTest_case_3 FAILED.");
		goto fail;
	};
	printf("\nTest_case_3 passed.");
	free(cfg);

	if (test_case_4()) {
		printf("\nTest_case_4 FAILED.");
		goto fail;
	};
	printf("\nTest_case_4 passed.");
	free(cfg);

	if (test_case_5()) {
		printf("\nTest_case_5 FAILED.");
		goto fail;
	};
	printf("\nTest_case_5 passed.");
	free(cfg);

	if (test_case_6()) {
		printf("\nTest_case_6 FAILED.");
		goto fail;
        }
	printf("\nTest_case_6 passed.");
	free(cfg);

        rc = EXIT_SUCCESS;
fail:
	printf("\n");
	if (rc != EXIT_SUCCESS)
		rdma_log_dump();
	printf("\n");
	rdma_log_close();
	exit(rc);
};

#ifdef __cplusplus
}
#endif


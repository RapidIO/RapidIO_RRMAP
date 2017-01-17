/*
 ************************************************************************ 
Copyright (c) 2016, Integrated Device Technology Inc.
Copyright (c) 2016, RapidIO Trade Association
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
l of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this l of conditions and the following disclaimer in the documentation
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
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"

#include "DAR_DB.h"
#include "DAR_DevDriver.h"
#include "rio_standard.h"
#include "rio_ecosystem.h"

#include "src/IDT_Tsi721_API.c"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t tsi721_regs[] = {
	TSI721_RIO_SP_LT_CTL,
	TSI721_RIO_SR_RSP_TO,
	TSI721_RIO_SP_GEN_CTL,
	TSI721_RIO_SP_LM_REQ,
	TSI721_RIO_SP_LM_RESP,
	TSI721_RIO_SP_ACKID_STAT,
	TSI721_RIO_SP_CTL2,
	TSI721_RIO_SP_ERR_STAT,
	TSI721_RIO_SP_CTL,
};

#define TSI721_NUM_MOCK_REGS (sizeof(tsi721_regs)/sizeof(tsi721_regs[0]))
	
typedef struct mock_dar_reg_t_TAG
{
	uint32_t offset;
	uint32_t data;
} mock_dar_reg_t;

#define NUM_DAR_REG (TSI721_NUM_MOCK_REGS + TSI721_NUM_PERF_CTRS)

#define UPB_DAR_REG (NUM_DAR_REG+1)

mock_dar_reg_t mock_dar_reg[UPB_DAR_REG];

static DAR_DEV_INFO_t mock_dev_info;
static idt_sc_dev_ctrs_t *mock_dev_ctrs = (idt_sc_dev_ctrs_t *)malloc(sizeof(idt_sc_dev_ctrs_t));
static idt_sc_p_ctrs_val_t *pp_ctrs = (idt_sc_p_ctrs_val_t *)malloc((MAX_DAR_PORTS) * sizeof(idt_sc_p_ctrs_val_t));

/* Create a mock dev_info.
 */
static void tsi721_test_setup(void)
{
        uint8_t idx, pnum;
	idt_sc_ctr_val_t init = INIT_IDT_SC_CTR_VAL;

	mock_dev_info.db_h = 3670020;
	mock_dev_info.privateData = 0x0;
        mock_dev_info.accessInfo = 0x0;
	strcpy(mock_dev_info.name, "Tsi721");
	mock_dev_info.dsf_h = 0x80E50005;
        mock_dev_info.extFPtrForPort = TSI721_RIO_SP_MB_HEAD;
	mock_dev_info.extFPtrPortType = RIO_EFB_T_SP_EP_SAER;
	mock_dev_info.extFPtrForLane = TSI721_RIO_PER_LANE_BH;
	mock_dev_info.extFPtrForErr = TSI721_RIO_ERR_RPT_BH;
	mock_dev_info.extFPtrForVC = 0;
	mock_dev_info.extFPtrForVOQ = 0;
	mock_dev_info.devID = 0x80AB0038;
	mock_dev_info.devInfo = 0;
	mock_dev_info.assyInfo = 0;
	mock_dev_info.features = 0xC000003F;
	mock_dev_info.swPortInfo = 0x00000100;
	mock_dev_info.swRtInfo = 0;
	mock_dev_info.srcOps = 0x0000FC04;
	mock_dev_info.dstOps = 0x0000FC04;
	mock_dev_info.swMcastInfo = 0;
	for (idx = 0; idx < MAX_DAR_PORTS; idx++) {
		mock_dev_info.ctl1_reg[idx] = 0;
        }

	for (idx = 0; idx < MAX_DAR_SCRPAD_IDX; idx++) {
		mock_dev_info.scratchpad[idx] = 0;
        }

	mock_dev_ctrs->num_p_ctrs = MAX_DAR_PORTS;
	mock_dev_ctrs->valid_p_ctrs = 0;

        for (pnum = 0; pnum < MAX_DAR_PORTS; pnum++) { 
            pp_ctrs[pnum].pnum = pnum;
            pp_ctrs[pnum].ctrs_cnt = TSI721_NUM_PERF_CTRS;
            for (idx = 0; idx < RIO_MAX_SC; idx++) {
                pp_ctrs[pnum].ctrs[idx] = init;
           }
       }
       mock_dev_ctrs->p_ctrs = pp_ctrs;
}

/* Initialize the mock register structure for different registers.
 */
static void init_mock_tsi721_reg(void) 
{
	// idx is always should be less than UPB_DAR_REG.
	uint32_t cntr, idx;

	for (idx = 0; idx < TSI721_NUM_MOCK_REGS; idx++) {
        	mock_dar_reg[idx].offset = tsi721_regs[idx];
        	mock_dar_reg[idx].data = 0x00;
	}
	// initialize performance counters
	for (cntr = 0; cntr < TSI721_NUM_PERF_CTRS; cntr++) {
		if (tsi721_dev_ctrs[cntr].split && !tsi721_dev_ctrs[cntr].os) {
			continue;
		};
		mock_dar_reg[idx].offset = tsi721_dev_ctrs[cntr].os;
		mock_dar_reg[idx].data = 0;
                idx++;
        }
}

/* The function tries to find the index of the offset in the dar_reg array and returns the idx,
 * otherwise it returns UPB_DAR_REG.
 */
uint32_t find_offset(uint32_t offset)
{
	uint32_t idx;
	for (idx = 0; idx < NUM_DAR_REG; idx++) {
            if (mock_dar_reg[idx].offset == offset) {
               return idx;
            }
	}
	return UPB_DAR_REG;
}

/* The function reads the value data of offset from the dar_reg array.
 * If the function finds the offset, it returns SUCCESS otherwise ERR_ACCESS.
 */
uint32_t __wrap_DARRegRead(DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t *readdata)
{
	uint32_t idx = UPB_DAR_REG;

        if (NULL != dev_info && *readdata) {
            mock_dev_info = *dev_info;
        }
        else {
            *dev_info = mock_dev_info;
        }

	idx = find_offset(offset);
	if (idx == UPB_DAR_REG) {
           return RIO_ERR_ACCESS;
        }

	*readdata = mock_dar_reg[idx].data;
	return RIO_SUCCESS;
}

/* The function updates the value data of offset in the dar_reg array.
 * If the function finds the offset, it returns SUCCESS otherwise ERR_ACCESS.
 */
uint32_t __wrap_DARRegWrite(DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t writedata)
{
	uint32_t idx = UPB_DAR_REG;

        if (NULL != dev_info) {
            mock_dev_info = *dev_info;
        }
        else {
            *dev_info = mock_dev_info;
        }

	idx = find_offset(offset);
	if (idx == UPB_DAR_REG) {
           return RIO_ERR_ACCESS;
        }

	mock_dar_reg[idx].data = writedata;
        return RIO_SUCCESS;
}

/* The setup function which should be called before any unit tests that need to be executed.
 */
static int tsi721_setup(void **state)
{
        memset(&mock_dev_info, 0x00, sizeof(idt_sc_dev_ctrs_t));
        DARDB_init();
	tsi721_test_setup();
        init_mock_tsi721_reg();

        (void)state; // unused
        return 0;
}

/* The teardown function to be called after any tests have finished.
 */
static int tsi721_teardown(void **state) 
{
        free(mock_dev_ctrs);

        (void)state; //unused
        return 0;
}

void tsi721_assumptions_test(void **state)
{
	// Verify constants
	assert_int_equal(1, TSI721_MAX_PORTS);
	assert_int_equal(4, TSI721_MAX_LANES);
	
	// Verify assumptions in the code
	assert_int_equal(0xFFFF0000, TSI721_ODB_CNTX_ODB_TOT_CNT);
	assert_int_equal(0xFFFF0000, TSI721_NWR_CNT_NW_TOT_CNT);
	assert_int_equal(0xFFFF0000, TSI721_MWR_CNT_MW_TOT_CNT);
	assert_int_equal(0x0000FFFF, TSI721_ODB_CNTX_ODB_OK_CNT);
	assert_int_equal(0x0000FFFF, TSI721_NWR_CNT_NW_OK_CNT);
	assert_int_equal(0x0000FFFF, TSI721_MWR_CNT_MW_OK_CNT);

	// Verify ranges are sane
	assert_in_range(TSI721_NUM_PERF_CTRS, 0, RIO_MAX_SC);

	(void)state; // unused
}

static void tsi721_init_ctrs(idt_sc_init_dev_ctrs_in_t *parms_in)
{
	uint8_t pnum;

	parms_in->ptl.num_ports = RIO_ALL_PORTS;
	for (pnum = 0; pnum < RIO_MAX_DEV_PORT; pnum++) {
		parms_in->ptl.pnums[pnum] = 0x00;
	}

	parms_in->dev_ctrs = mock_dev_ctrs;
}

void tsi721_init_dev_ctrs_test_success(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;
	idt_sc_p_ctrs_val_t *p_ctrs;
	unsigned int j;

	// Success case, all ports
        tsi721_init_ctrs(&mock_sc_in);

        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        assert_int_equal(1, mock_sc_in.dev_ctrs->valid_p_ctrs);

	p_ctrs = mock_sc_in.dev_ctrs->p_ctrs;
	assert_int_equal(0, p_ctrs->pnum);
	assert_int_equal(TSI721_NUM_PERF_CTRS, p_ctrs->ctrs_cnt);
	for (j = 0; j < TSI721_NUM_PERF_CTRS; j++) {
		assert_int_equal(0, p_ctrs->ctrs[j].total);
		assert_int_equal(0, p_ctrs->ctrs[j].last_inc);
		assert_int_equal(tsi721_dev_ctrs[j].ctr_t, p_ctrs->ctrs[j].sc);
		assert_int_equal(tsi721_dev_ctrs[j].tx, p_ctrs->ctrs[j].tx);
		assert_int_equal(tsi721_dev_ctrs[j].srio, p_ctrs->ctrs[j].srio);
	}
        (void)state; // unused
}

void tsi721_init_dev_ctrs_test_bad_ptrs(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;

	// Test invalid dev_ctrs pointer
	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.dev_ctrs = NULL;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	// Test invalid dev_ctrs->p_ctrs pointer
	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.dev_ctrs->p_ctrs = NULL;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        (void)state; // unused
};

void tsi721_init_dev_ctrs_test_bad_p_ctrs(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;

	// Test invalid number of p_ctrs
	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.dev_ctrs->num_p_ctrs = 0;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.dev_ctrs->num_p_ctrs = IDT_MAX_PORTS + 1;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.dev_ctrs->valid_p_ctrs = TSI721_MAX_PORTS + 1;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        (void)state; // unused
}

void tsi721_init_dev_ctrs_test_bad_ptl_1(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;

	// Test that a bad Port list is reported correctly.
	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.ptl.num_ports = 1;
	mock_sc_in.ptl.pnums[0] = 1;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        (void)state; // unused
};

void tsi721_init_dev_ctrs_test_bad_ptl_2(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;

	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.ptl.num_ports = 1;
	mock_sc_in.ptl.pnums[0] = -1;
        assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        (void)state; // unused
};

void tsi721_init_dev_ctrs_test_good_ptl(void **state)
{
        idt_sc_init_dev_ctrs_in_t      mock_sc_in;
        idt_sc_init_dev_ctrs_out_t     mock_sc_out;
	idt_sc_p_ctrs_val_t *p_ctrs;
	unsigned int j;

	// Test Port list with a few good entries...
	tsi721_init_ctrs(&mock_sc_in);
	mock_sc_in.ptl.num_ports = RIO_ALL_PORTS;
        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
			&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
        assert_int_equal(1, mock_sc_in.dev_ctrs->valid_p_ctrs);
        assert_int_equal(0, mock_sc_in.dev_ctrs->p_ctrs[0].pnum);
        assert_int_equal(TSI721_NUM_PERF_CTRS,
			mock_sc_in.dev_ctrs->p_ctrs[0].ctrs_cnt);

	p_ctrs = mock_sc_in.dev_ctrs->p_ctrs;
	for (j = 0; j < TSI721_NUM_PERF_CTRS; j++) {
		assert_int_equal(0, p_ctrs->ctrs[j].total);
		assert_int_equal(0, p_ctrs->ctrs[j].last_inc);
		assert_int_equal(tsi721_dev_ctrs[j].ctr_t, p_ctrs->ctrs[j].sc);
		assert_int_equal(tsi721_dev_ctrs[j].tx, p_ctrs->ctrs[j].tx);
		assert_int_equal(tsi721_dev_ctrs[j].srio, p_ctrs->ctrs[j].srio);
	}
        (void)state; // unused
}

static void tsi721_init_read_ctrs(idt_sc_read_ctrs_in_t *parms_in)
{
	parms_in->ptl.num_ports = RIO_ALL_PORTS;
	parms_in->dev_ctrs = mock_dev_ctrs;
}

void tsi721_read_dev_ctrs_test(void **state)
{
	idt_sc_read_ctrs_in_t      mock_sc_in;
	idt_sc_read_ctrs_out_t     mock_sc_out;
        idt_sc_init_dev_ctrs_in_t      init_in;
        idt_sc_init_dev_ctrs_out_t     init_out;
	unsigned int idx, ridx;
	idt_sc_ctr_val_t *ctrs;
	uint64_t wrap_base = 0x00000000FFFFFFFF;

	// Initialize counters structure
        tsi721_init_ctrs(&init_in);
        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
					&mock_dev_info, &init_in, &init_out));
        assert_int_equal(RIO_SUCCESS, init_out.imp_rc);

	// Set up counters 
	tsi721_init_read_ctrs(&mock_sc_in);

	// Set up counter registers
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		// Set non-zero counter value for the port
		if(tsi721_dev_ctrs[idx].split && !tsi721_dev_ctrs[idx].os) {
			continue;
		};
		ridx = find_offset(tsi721_dev_ctrs[idx].os);
		assert_int_not_equal(ridx, UPB_DAR_REG);

		if (tsi721_dev_ctrs[idx].split) {
			mock_dar_reg[ridx].data = (ridx << 17) + ridx;
		} else {
			mock_dar_reg[ridx].data = ridx;
		};
	};

	// Check for successful reads...
	assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	// Check counter values... 
	ctrs = &mock_sc_in.dev_ctrs->p_ctrs[0].ctrs[0];
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		// Check the counter value for the port...
		if(tsi721_dev_ctrs[idx].split && !tsi721_dev_ctrs[idx].os) {
			continue;
		};
		ridx = find_offset(tsi721_dev_ctrs[idx].os);
		assert_int_not_equal(ridx, UPB_DAR_REG);

		if (!tsi721_dev_ctrs[idx].split) {
			assert_int_equal(ridx, ctrs[idx].total);
			assert_int_equal(ridx, ctrs[idx].last_inc);
			continue;
		};
		assert_int_equal(2 * ridx, ctrs[idx].total);
		assert_int_equal(2 * ridx, ctrs[idx].last_inc);
		assert_int_equal(ridx, ctrs[idx + 1].total);
		assert_int_equal(ridx, ctrs[idx + 1].last_inc);
	};

	// Change counter registers
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		// Set non-zero counter value for the port
		if(tsi721_dev_ctrs[idx].split && !tsi721_dev_ctrs[idx].os) {
			continue;
		};
		ridx = find_offset(tsi721_dev_ctrs[idx].os);
		assert_int_not_equal(ridx, UPB_DAR_REG);

		if (tsi721_dev_ctrs[idx].split) {
			mock_dar_reg[ridx].data = (ridx << 18) + (3 * ridx);
		} else {
			mock_dar_reg[ridx].data = 3 * ridx;
		};
	};

	// Check for successful reads...
	assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(&mock_dev_info,
						&mock_sc_in, &mock_sc_out));
        assert_int_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	// Check counter values... 
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		// Check the counter value for the port...
		if(tsi721_dev_ctrs[idx].split && !tsi721_dev_ctrs[idx].os) {
			continue;
		};
		ridx = find_offset(tsi721_dev_ctrs[idx].os);
		assert_int_not_equal(ridx, UPB_DAR_REG);

		if (!tsi721_dev_ctrs[idx].split) {
			assert_int_equal(4 * ridx, ctrs[idx].total);
			assert_int_equal(3 * ridx, ctrs[idx].last_inc);
			continue;
		};
		assert_int_equal(6 * ridx, ctrs[idx].total);
		assert_int_equal(4 * ridx, ctrs[idx].last_inc);
		assert_int_equal(4 * ridx, ctrs[idx + 1].total);
		assert_int_equal(3 * ridx, ctrs[idx + 1].last_inc);
	};

	// Set all totals registers to wrap over 32 bit boundary...
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		// Check the counter value for the port...
		ctrs[idx].total = wrap_base;
	};

	// Read the same values again...
	assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	// Check counter values... 
	for (idx = 0; idx < TSI721_NUM_PERF_CTRS; idx++) {
		uint64_t tot;
		// Check the counter value for the port...
		if(tsi721_dev_ctrs[idx].split && !tsi721_dev_ctrs[idx].os) {
			continue;
		};
		ridx = find_offset(tsi721_dev_ctrs[idx].os);
		assert_int_not_equal(ridx, UPB_DAR_REG);

		if (!tsi721_dev_ctrs[idx].split) {
			tot = wrap_base + (3 * ridx);
			assert_int_equal(tot, ctrs[idx].total);
			assert_int_equal(3 * ridx, ctrs[idx].last_inc);
			continue;
		};
		tot = wrap_base + (4 * ridx);
		assert_int_equal(tot, ctrs[idx].total);
		assert_int_equal(4 * ridx, ctrs[idx].last_inc);
		tot = wrap_base + (3 * ridx);
		assert_int_equal(tot, ctrs[idx + 1].total);
		assert_int_equal(3 * ridx, ctrs[idx + 1].last_inc);
	};
	(void)state; // unused
}

void tsi721_read_dev_ctrs_test_bad_parms1(void **state)
{
	idt_sc_read_ctrs_in_t      mock_sc_in;
	idt_sc_read_ctrs_out_t     mock_sc_out;
        idt_sc_init_dev_ctrs_in_t      init_in;
        idt_sc_init_dev_ctrs_out_t     init_out;

	// Initialize counters structure
        tsi721_init_ctrs(&init_in);
        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &init_in, &init_out));
        assert_int_equal(RIO_SUCCESS, init_out.imp_rc);

	// Set up counters 
	tsi721_init_read_ctrs(&mock_sc_in);

	// Now try some bad parameters/failure test cases
	mock_sc_in.ptl.num_ports = TSI721_MAX_PORTS + 1;
	mock_sc_out.imp_rc = RIO_SUCCESS;
	assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
	
	mock_sc_in.ptl.num_ports = 1;
	mock_sc_in.ptl.pnums[0] = TSI721_MAX_PORTS + 1;
	mock_sc_out.imp_rc = RIO_SUCCESS;
	assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	mock_sc_in.ptl.num_ports = RIO_ALL_PORTS;
	mock_sc_in.dev_ctrs->p_ctrs = NULL;
	mock_sc_out.imp_rc = RIO_SUCCESS;
	assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);

	mock_sc_in.dev_ctrs = NULL;
	mock_sc_out.imp_rc = RIO_SUCCESS;
	assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(
				&mock_dev_info, &mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
	(void)state; // unused
}

void tsi721_read_dev_ctrs_test_bad_parms2(void **state)
{
	idt_sc_read_ctrs_in_t      mock_sc_in;
	idt_sc_read_ctrs_out_t     mock_sc_out;
        idt_sc_init_dev_ctrs_in_t      init_in;
        idt_sc_init_dev_ctrs_out_t     init_out;

	// Initialize counters structure
        tsi721_init_ctrs(&init_in);
        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(&mock_dev_info,
						&init_in, &init_out));
        assert_int_equal(RIO_SUCCESS, init_out.imp_rc);

	// Set up counters 
	tsi721_init_read_ctrs(&mock_sc_in);

	// Try to read a port that is not in the port list.
        tsi721_init_ctrs(&init_in);
	init_in.ptl.num_ports = RIO_ALL_PORTS;
        assert_int_equal(RIO_SUCCESS, idt_tsi721_sc_init_dev_ctrs(
				&mock_dev_info, &init_in, &init_out));
        assert_int_equal(RIO_SUCCESS, init_out.imp_rc);
	tsi721_init_read_ctrs(&mock_sc_in);

	mock_sc_in.ptl.num_ports = 1;
	mock_sc_in.ptl.pnums[0] = TSI721_MAX_PORTS;
	mock_sc_out.imp_rc = RIO_SUCCESS;
	assert_int_not_equal(RIO_SUCCESS, idt_tsi721_sc_read_ctrs(&mock_dev_info,
						&mock_sc_in, &mock_sc_out));
        assert_int_not_equal(RIO_SUCCESS, mock_sc_out.imp_rc);
	
	(void)state; // unused
}

void tsi721_init_read_dev_ctrs_test(void **state)
{
        tsi721_init_dev_ctrs_test_success(state);
        tsi721_read_dev_ctrs_test(state);

        (void)state; // unused
}

int main(int argc, char** argv)
{
	(void)argv; // not used
	argc++; // not used

	const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
			tsi721_assumptions_test, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_success, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_bad_ptrs, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_bad_p_ctrs, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_bad_ptl_1, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_bad_ptl_2, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_dev_ctrs_test_good_ptl, tsi721_setup, NULL),

                cmocka_unit_test_setup_teardown(
			tsi721_read_dev_ctrs_test, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_read_dev_ctrs_test_bad_parms1, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_read_dev_ctrs_test_bad_parms2, tsi721_setup, NULL),
                cmocka_unit_test_setup_teardown(
			tsi721_init_read_dev_ctrs_test, tsi721_setup, tsi721_teardown),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

#ifdef __cplusplus
}
#endif


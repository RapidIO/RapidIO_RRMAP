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
#include "IDT_Statistics_Counter_API.h"
#include "src/IDT_Statistics_Counter_API.c"
#include "rio_standard.h"
#include "rio_ecosystem.h"
#include "libcli.h"

#ifdef __cplusplus
extern "C" {
#endif

static DAR_DEV_INFO_t mock_dev_info;

static void test_setup(void)
{
        uint8_t idx;

        mock_dev_info.db_h = 3670020;
        mock_dev_info.privateData = 0x0;
        mock_dev_info.accessInfo = 0x0;
        strcpy(mock_dev_info.name, "FASTER_EP");
        mock_dev_info.dsf_h = 0x80E50005;
        mock_dev_info.extFPtrForPort = 0x100;
        mock_dev_info.extFPtrPortType = 0x1000;
        mock_dev_info.extFPtrForLane = 0x2000;
        mock_dev_info.extFPtrForErr = 0x3000;
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
}

typedef struct mock_dar_reg_t_TAG
{
        uint32_t offset;
        uint32_t data;
} mock_dar_reg_t;

#define NUM_DAR_REG (100)

#define UPB_DAR_REG (NUM_DAR_REG+1)

mock_dar_reg_t mock_dar_reg[UPB_DAR_REG];

static void init_mock_regs(void)
{
        // idx is always should be less than UPB_DAR_REG.
       uint32_t idx;

        for (idx = 0; idx < UPB_DAR_REG; idx++) {
                mock_dar_reg[idx].offset = idx << 2;
                mock_dar_reg[idx].data = 0x00;
        }
}

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

uint32_t __wrap_DARRegRead(DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t *readdata)
{
        uint32_t idx = UPB_DAR_REG;

        if (NULL != dev_info && *readdata) {
            mock_dev_info = *dev_info;
        } else {
            *dev_info = mock_dev_info;
        }

        idx = find_offset(offset);
        if (idx == UPB_DAR_REG) {
           return RIO_ERR_ACCESS;
        }

        *readdata = mock_dar_reg[idx].data;
        return RIO_SUCCESS;
}

uint32_t __wrap_DARRegWrite(DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t writedata)
{
        uint32_t idx = UPB_DAR_REG;

        if (NULL != dev_info) {
            mock_dev_info = *dev_info;
        } else {
            *dev_info = mock_dev_info;
        }

        idx = find_offset(offset);
        if (idx == UPB_DAR_REG) {
           return RIO_ERR_ACCESS;
	}

        mock_dar_reg[idx].data = writedata;
        return RIO_SUCCESS;
}
void assumptions_test(void **state)
{
	assert_string_equal((char *)"PCIExp", (char *)sc_other_if_names_PCIe);
	assert_string_equal((char *)"FABRIC", (char *)sc_other_if_names_FABRIC);
	assert_string_equal((char *)"INVALID", (char *)sc_other_if_names_Invalid);
	assert_string_equal((char *)"UNKNOWN", (char *)sc_other_if_names_UNKNOWN);
	(void)state; // unused
}

void test_sc_info(idt_sc_ctr_t stctr, char *name, uint32_t flags, bool *chk_a)
{
        assert_string_equal(name, SC_NAME(stctr));
	assert_int_equal(flags, SC_FLAG(stctr));
	assert_false(chk_a[stctr]);
	chk_a[stctr] = true;
}

void test_sc_flag_info(idt_sc_ctr_flag_t sc_f, uint32_t flag, char *name, bool *chk_a)
{
	assert_string_equal(name, SC_FLAG_NAME(sc_f));
	if (sc_f < sc_f_LAST) {
		assert_int_equal(1 << sc_f, flag);
		assert_int_equal(sc_f,
			parm_idx(SC_FLAG_NAME(sc_f), (char *)SC_FLAG_NAMES));
	};

	assert_false(chk_a[sc_f]);
	chk_a[sc_f] = true;
}

void sc_info_test(void **state)
{
	bool chk_a[(uint8_t)(idt_sc_last)+2] = {false};
	unsigned int i;

        // Verify that sc_info array is correctly defined
        assert_int_equal(sizeof(chk_a) / sizeof(chk_a[0]),
			sizeof(sc_info) / sizeof (sc_info[0]));

        test_sc_info(idt_sc_disabled, (char *)"Disabled__", 0, chk_a);
        test_sc_info(idt_sc_enabled, (char *)"Enabled___", 0, chk_a);

	test_sc_info(idt_sc_uc_req_pkts, (char *)"UC_REQ_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_uc_pkts, (char *)"UC_ALL_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_retries, (char *)"Retry___CS", SC_F_CS | SC_F_RTY, chk_a);
	test_sc_info(idt_sc_all_cs, (char *)"All_____CS", SC_F_CS, chk_a);
	test_sc_info(idt_sc_uc_4b_data, (char *)"UC_4B_Data", SC_F_DATA, chk_a);
	test_sc_info(idt_sc_mc_pkts, (char *)"MCast__PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_mecs, (char *)"MECS____CS", SC_F_CS, chk_a);
	test_sc_info(idt_sc_mc_4b_data, (char *)"MC_4B_Data", SC_F_DATA, chk_a);

	test_sc_info(idt_sc_pa, (char *)"PktAcc__CS", SC_F_CS, chk_a);
	test_sc_info(idt_sc_pkt, (char *)"ALL____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pna, (char *)"PktNotA_CS", SC_F_CS | SC_F_ERR, chk_a);
	test_sc_info(idt_sc_pkt_drop, (char *)"Drop___PKT", SC_F_PKT | SC_F_DROP | SC_F_ERR, chk_a);
	test_sc_info(idt_sc_pkt_drop_ttl, (char *)"DropTTLPKT", SC_F_PKT | SC_F_DROP | SC_F_ERR, chk_a);

	test_sc_info(idt_sc_fab_pkt, (char *)"FAB____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_pload, (char *)"8B_DAT_PKT", SC_F_DATA, chk_a);
	test_sc_info(idt_sc_fab_pload, (char *)"8B_DAT_PKT", SC_F_DATA, chk_a);
	test_sc_info(idt_sc_rio_bwidth, (char *)"RAW_BWIDTH", 0, chk_a);

	test_sc_info(idt_sc_pcie_msg_rx, (char *)"PCI_M__PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pcie_msg_tx, (char *)"PCI_M__PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pcie_dma_rx, (char *)"PCI__D_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pcie_dma_tx, (char *)"PCI__D_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pcie_brg_rx, (char *)"PCI_BG_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_pcie_brg_tx, (char *)"PCI_BG_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_nwr_tx, (char *)"NWR____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_nwr_ok_rx, (char *)"NWR_OK_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_dbel_tx, (char *)"DB_____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_dbel_ok_rx, (char *)"DB__OK_PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_msg_tx, (char *)"MSG____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_msg_rx, (char *)"MSG____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_msg_tx_rty, (char *)"MSG_RTYPKT", SC_F_PKT | SC_F_RTY, chk_a);
	test_sc_info(idt_sc_rio_msg_rx_rty, (char *)"MSG_RTYPKT", SC_F_PKT | SC_F_RTY, chk_a);
	test_sc_info(idt_sc_rio_dma_tx, (char *)"DMA____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_dma_rx, (char *)"DMA____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_brg_tx, (char *)"BRG____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_brg_rx, (char *)"BRG____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_brg_rx_err, (char *)"BRG_ERRPKT", SC_F_PKT | SC_F_ERR, chk_a);
	test_sc_info(idt_sc_rio_mwr_tx, (char *)"MWR____PKT", SC_F_PKT, chk_a);
	test_sc_info(idt_sc_rio_mwr_ok_rx, (char *)"MWR_OK_PKT", SC_F_PKT, chk_a);

        test_sc_info(idt_sc_last, (char *)"Last______", 0, chk_a);
        test_sc_info((idt_sc_ctr_t)(idt_sc_last + 1), (char *)"Invalid___", 0, chk_a);

	// Confirm that all elements of the sc_info array were tested.
	for (i = 0; i < sizeof(sc_info) / sizeof (sc_info[0]); i++) {
		assert_true(chk_a[i]);
	}
	(void)state; // unused
}

void sc_ctr_flag_test(void **state)
{
	bool chk_f_a[(uint8_t)(sc_f_LAST) + 2] = {false};
	unsigned int i;

	// Check flag names info
        assert_int_equal(sizeof(chk_f_a) / sizeof(chk_f_a[0]),
			sizeof(sc_flag_names) / sizeof (sc_flag_names[0]));

	test_sc_flag_info(sc_f_DROP, SC_F_DROP, (char *)"DROP", chk_f_a);
	test_sc_flag_info(sc_f_ERR, SC_F_ERR, (char *)"ERR", chk_f_a);
	test_sc_flag_info(sc_f_RTY, SC_F_RTY, (char *)"RTY", chk_f_a);
	test_sc_flag_info(sc_f_CS, SC_F_CS, (char *)"CS", chk_f_a);
	test_sc_flag_info(sc_f_PKT, SC_F_PKT, (char *)"PKT", chk_f_a);
	test_sc_flag_info(sc_f_DATA, SC_F_DATA, (char *)"DATA", chk_f_a);
	test_sc_flag_info(sc_f_LAST, 0, (char *)"Last", chk_f_a);
	test_sc_flag_info((idt_sc_ctr_flag_t)(sc_f_LAST + 1), 0, (char *)"Ivld", chk_f_a);
	
	// Confirm that all elements of the sc_flag_names array were tested.
	for (i = 0; i < sizeof(sc_flag_names)/sizeof (sc_flag_names[0]); i++) {
		assert_true(chk_f_a[i]);
	}
	(void)state; // unused
}

void sc_gen_flag_test(void **state)
{
	assert_int_equal(SC_F_TX, parm_idx((char *)"TX", (char *)SC_GEN_FLAG_NAMES));
	assert_int_equal(SC_F_RX, parm_idx((char *)"RX", (char *)SC_GEN_FLAG_NAMES));
	assert_int_equal(SC_F_SRIO, parm_idx((char *)"SRIO", (char *)SC_GEN_FLAG_NAMES));
	assert_int_equal(SC_F_OTH, parm_idx((char *)"OTH", (char *)SC_GEN_FLAG_NAMES));

	(void)state; // unused
}

void idt_sc_other_if_names_test(void **state)
{
	const char *name;
	unsigned int i;
	uint32_t IDT_switches[] = {
		((uint32_t)RIO_DEVI_IDT_CPS1848 << 16) + RIO_VEND_IDT,
		((uint32_t)RIO_DEVI_IDT_CPS1432 << 16) + RIO_VEND_IDT,
		((uint32_t)RIO_DEVI_IDT_CPS1616 << 16) + RIO_VEND_IDT,
		((uint32_t)RIO_DEVI_IDT_SPS1616 << 16) + RIO_VEND_IDT,
		((uint32_t)RIO_DEVI_IDT_RXS2448 << 16) + RIO_VEND_IDT,
		((uint32_t)RIO_DEVI_IDT_RXS1632 << 16) + RIO_VEND_IDT
	};

	init_mock_regs();
	test_setup();

        mock_dev_info.devID = (RIO_DEVI_IDT_CPS8 << 16) + RIO_VEND_TI;
	assert_int_equal(RIO_ERR_NO_DEVICE_SUPPORT,
				idt_sc_other_if_names(&mock_dev_info, &name));
	assert_string_equal(sc_other_if_names_UNKNOWN, name);
	
        mock_dev_info.devID = (RIO_DEVI_IDT_CPS8 << 16) + RIO_VEND_TUNDRA;
	assert_int_equal(RIO_ERR_NO_DEVICE_SUPPORT,
				idt_sc_other_if_names(&mock_dev_info, &name));
	assert_string_equal(sc_other_if_names_Invalid, name);
	
        mock_dev_info.devID = (RIO_DEVI_IDT_CPS8 << 16) + RIO_VEND_IDT;
	assert_int_equal(RIO_ERR_NO_DEVICE_SUPPORT,
			idt_sc_other_if_names(&mock_dev_info, &name));
	assert_string_equal(sc_other_if_names_UNKNOWN, name);
	
        mock_dev_info.devID = (RIO_DEVI_IDT_TSI721 << 16) + RIO_VEND_IDT;
        assert_int_equal(RIO_SUCCESS,
			idt_sc_other_if_names(&mock_dev_info, &name));
        assert_string_equal(sc_other_if_names_PCIe, name);
	
	for (i = 0; i < sizeof(IDT_switches)/sizeof(IDT_switches[0]); i++) {
        	mock_dev_info.devID = IDT_switches[i];
        	assert_int_equal(RIO_SUCCESS,
			idt_sc_other_if_names(&mock_dev_info, &name));
        	assert_string_equal(sc_other_if_names_FABRIC, name);
	}
	
	(void)state; // unused
}

int main(int argc, char** argv)
{
	(void)argv; // not used
	argc++; // not used

	const struct CMUnitTest tests[] = {
                cmocka_unit_test(assumptions_test),
                cmocka_unit_test(sc_info_test),
                cmocka_unit_test(sc_ctr_flag_test),
                cmocka_unit_test(sc_gen_flag_test),
                cmocka_unit_test(idt_sc_other_if_names_test)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

#ifdef __cplusplus
}
#endif


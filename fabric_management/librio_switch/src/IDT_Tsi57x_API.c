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
#include <IDT_Tsi57x_API_Private.h>
#include "tsi578.h"
#include "Tsi575.h"
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define TSI57X_NUM_PORTS(x) ((NUM_PORTS(x) > Tsi578_MAX_PORTS) ? \
	Tsi578_MAX_PORTS : NUM_PORTS(x))

// CHANGES
//
// Check port width computation for get_config and set_config
// Check set_config with "all ports" as an input parameter.  Endless loop?
//

static DSF_Handle_t Tsi57x_driver_handle;
static uint32_t num_Tsi57x_driver_instances;

// Routing table entry value to use when requesting
// default route or packet discard (no route)
#define HW_DFLT_RT 0xFF

#define PC_DET_PORTS_2_SKIP_0 (PC_FIRST_SUBROUTINE_0+0x0900) // 001900
#define TSI57X_LP_0           (PC_FIRST_SUBROUTINE_0+0x0A00) // 001A00

#define PROGRAM_RTE_ENTRIES_0 (RT_FIRST_SUBROUTINE_0+0xA000) // 10A000
#define PROGRAM_MC_MASKS_0    (RT_FIRST_SUBROUTINE_0+0xB000) // 10B000
#define READ_MC_MASKS_0       (RT_FIRST_SUBROUTINE_0+0xC000) // 10C000
#define READ_RTE_ENTRIES_0    (RT_FIRST_SUBROUTINE_0+0xD000) // 10D000

#define EM_SET_EVENT_PW_0     (EM_FIRST_SUBROUTINE_0+0x2900) // 202900
#define EM_SET_EVENT_INT_0    (EM_FIRST_SUBROUTINE_0+0x2A00) // 202A00
#define EM_EN_ERR_CTR_0       (EM_FIRST_SUBROUTINE_0+0x2B00) // 202B00
#define EM_SET_EVENT_EN_0     (EM_FIRST_SUBROUTINE_0+0x2C00) // 202C00
#define EM_UPDATE_RESET_0     (EM_FIRST_SUBROUTINE_0+0x2D00) // 202D00
#define EM_DET_NOTFN_0        (EM_FIRST_SUBROUTINE_0+0x2E00) // 202E00
#define EM_CREATE_RATE_0      (EM_FIRST_SUBROUTINE_0+0x2F00) // 202F00

#define END_PMR_ARRAY {RIO_ALL_PORTS,0, 0, 0, 0, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, {RIO_ALL_PORTS, RIO_ALL_PORTS, RIO_ALL_PORTS} }
//          4x 1x PWR
//  P  M L1 LC LC  M     Power Down Mask                      Reset Mask                           Other Mac Ports
port_mac_relations_t tsi578_pmr[] = {
  { 0, 0, 0, 4, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 1, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 1, 0, 1, 0, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 0, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 2, 1, 0, 4, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 3, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 3, 1, 1, 0, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 2, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 4, 2, 0, 4, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 5, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 5, 2, 1, 0, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 4, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 6, 3, 0, 4, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 7, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 7, 3, 1, 0, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 6, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 8, 4, 0, 4, 1, 4, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 9, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 9, 4, 1, 0, 1, 4, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 8, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {10, 5, 0, 4, 1, 5, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, {11, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {11, 5, 1, 0, 1, 5, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, {10, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {12, 6, 0, 4, 1, 6, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, {13, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {13, 6, 1, 0, 1, 6, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, {12, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {14, 7, 0, 4, 1, 7, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, {15, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  {15, 7, 1, 0, 1, 7, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, {14, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  END_PMR_ARRAY
};

//          4x 1x PWR
//  P  M L1 LC LC  M     Power Down Mask                      REset Mask                           Other Mac Ports
port_mac_relations_t tsi577_pmr[] = {
  { 0, 0, 0, 4, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 1,  8,  9} },
  { 1, 0, 1, 0, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 0,  8,  9} },
  { 2, 1, 0, 4, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 3, 10, 11} },
  { 3, 1, 1, 0, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 2, 10, 11} },
  { 4, 2, 0, 4, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 5, 12, 13} },
  { 5, 2, 1, 0, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 4, 12, 13} },
  { 6, 3, 0, 4, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 7, 14, 15} },
  { 7, 3, 1, 0, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 6, 14, 15} },
  { 8, 0, 2, 0, 1, 4, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 0,  1,  9} },
  { 9, 0, 3, 0, 1, 4, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 0,  1,  8} },
  {10, 1, 2, 0, 1, 5, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 2,  3, 11} },
  {11, 1, 3, 0, 1, 5, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 2,  3, 10} },
  {12, 2, 2, 0, 1, 6, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 4,  5, 13} },
  {13, 2, 3, 0, 1, 6, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 4,  5, 12} },
  {14, 3, 2, 0, 1, 7, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 6,  7, 15} },
  {15, 3, 3, 0, 1, 7, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 6,  7, 14} },
  END_PMR_ARRAY
};

port_mac_relations_t tsi574_pmr[] = {
  { 0, 0, 0, 4, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 1, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 1, 0, 1, 0, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 0, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 2, 1, 0, 4, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 3, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 3, 1, 1, 0, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 2, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 4, 2, 0, 4, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 5, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 5, 2, 1, 0, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 4, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 6, 3, 0, 4, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 7, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 7, 3, 1, 0, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 6, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  END_PMR_ARRAY
};

port_mac_relations_t tsi572_pmr[] = {
  { 0, 0, 0, 4, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 1, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 1, 0, 1, 0, 1, 0, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 0, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 2, 1, 0, 4, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 3, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 3, 1, 1, 0, 1, 1, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 2, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 4, 2, 0, 4, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 5, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 5, 2, 1, 0, 1, 2, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 4, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 6, 3, 0, 4, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4, { 7, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  { 7, 3, 1, 0, 1, 3, Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1, Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1, { 6, RIO_ALL_PORTS, RIO_ALL_PORTS} },
  END_PMR_ARRAY
};

struct {
        const char   *name;  /* Constant name string */
        const uint32_t devID;  /* Vendor + Device ID   */
} device_names[] = {
{"Tsi572", ((uint32_t)(Tsi572_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
{"Tsi574", ((uint32_t)(Tsi574_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
{"Tsi578", ((uint32_t)(Tsi578_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
{"Tsi576", ((uint32_t)(Tsi576_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
{"Tsi620", ((uint32_t)(Tsi575_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
{"Tsi577", ((uint32_t)(Tsi577_RIO_DEVID_VAL) << 16) + IDT_TSI_VENDOR_ID },
};

void getTsiName(DAR_DEV_INFO_t *dev_info)
{
        uint32_t i;

        for (i = 0; i < (sizeof(device_names)/sizeof(device_names[0])); i++) {
                if (device_names[i].devID == dev_info->devID) {
                        strcpy(dev_info->name, device_names[i].name);
                        break;
                };
        };
};

#define SCRPAD_EOF_OFFSET 0xFFFFFFFF
#define SCRPAD_FLAGS_IDX    0
#define SCRPAD_FIRST_IDX    0
#define SCRPAD_MASK_IDX     (SCRPAD_FIRST_IDX+Tsi578_MAX_MC_MASKS)

#define ALL_BITS ((uint32_t)(0xFFFFFFFF))
#define MC_IDX_MASK (Tsi578_RIO_MC_IDX_MC_ID| \
		Tsi578_RIO_MC_IDX_LARGE_SYS| \
		Tsi578_RIO_MC_IDX_MC_EN)
#define PW_MASK (Tsi578_RIO_PW_DESTID_LARGE_DESTID | \
		Tsi578_RIO_PW_DESTID_DESTID_LSB | \
		Tsi578_RIO_PW_DESTID_DESTID_MSB)
#define ERR_DET_MASK (Tsi578_RIO_LOG_ERR_DET_EN_UNSUP_TRANS_EN | \
		Tsi578_RIO_LOG_ERR_DET_EN_ILL_RESP_EN | \
		Tsi578_RIO_LOG_ERR_DET_EN_ILL_TRANS_EN)
#define MC_MASK_CFG_MASK ((uint32_t)(Tsi578_RIO_MC_MASK_CFG_PORT_PRESENT | \
			  Tsi578_RIO_MC_MASK_CFG_MASK_CMD | \
			Tsi578_RIO_MC_MASK_CFG_EG_PORT_NUM | \
			Tsi578_RIO_MC_MASK_CFG_MC_MASK_NUM))
#define MC_DESTID_MASK ((uint32_t)(Tsi578_RIO_MC_DESTID_CFG_DESTID_BASE | \
			Tsi578_RIO_MC_DESTID_CFG_DESTID_BASE_LT | \
			Tsi578_RIO_MC_DESTID_CFG_MASK_NUM_BASE))
#define MC_ASSOC_MASK ((uint32_t)(Tsi578_RIO_MC_DESTID_ASSOC_ASSOC_PRESENT | \
			Tsi578_RIO_MC_DESTID_ASSOC_CMD | \
			Tsi578_RIO_MC_DESTID_ASSOC_LARGE))

const struct scrpad_info scratchpad_const[MAX_DAR_SCRPAD_IDX] = {
	{Tsi578_RIO_MC_IDX(0) , MC_IDX_MASK},  
	{Tsi578_RIO_MC_IDX(1) , MC_IDX_MASK}, 
	{Tsi578_RIO_MC_IDX(2) , MC_IDX_MASK},
	{Tsi578_RIO_MC_IDX(3) , MC_IDX_MASK},
	{Tsi578_RIO_MC_IDX(4) , MC_IDX_MASK},
	{Tsi578_RIO_MC_IDX(5) , MC_IDX_MASK},
	{Tsi578_RIO_MC_IDX(6) , MC_IDX_MASK},
	{Tsi578_RIO_MC_IDX(7) , MC_IDX_MASK},
	{Tsi578_RIO_MC_MSKX(0), Tsi578_RIO_MC_MSKX_MC_MSK},/* SCRPAD_MASK_IDX for offsets to preserve/track */
	{Tsi578_RIO_MC_MSKX(1), Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(2), Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(3),Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(4), Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(5), Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(6), Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_MC_MSKX(7),Tsi578_RIO_MC_MSKX_MC_MSK},
	{Tsi578_RIO_COMP_TAG   , Tsi578_RIO_COMP_TAG_CTAG },
	{Tsi575_RIO_LUT_ATTR   , Tsi578_RIO_LUT_ATTR_DEFAULT_PORT},
		{Tsi578_RIO_SW_LT_CTL  , Tsi578_RIO_SW_LT_CTL_TVAL},
		{Tsi578_RIO_PW_DESTID  , PW_MASK},
		{Tsi578_RIO_LOG_ERR_DET_EN, ERR_DET_MASK},
		{Tsi578_RIO_PKT_TTL   ,  Tsi578_RIO_PKT_TTL_TTL },
		{Tsi578_RIO_MC_MASK_CFG, MC_MASK_CFG_MASK},
		{Tsi578_RIO_MC_DESTID_CFG,   MC_DESTID_MASK},  /* Code expects that this 
								* is the register immediately before 
								* Tsi578_RIO_MC_DESTID_ASSOC.
								*/
		{Tsi578_RIO_MC_DESTID_ASSOC, MC_ASSOC_MASK},
		{SCRPAD_EOF_OFFSET, ALL_BITS}
};

const struct scrpad_info *get_scrpad_info( void ) 
{
	return scratchpad_const;
};


uint32_t IDT_tsi57xWriteReg( DAR_DEV_INFO_t *dev_info,
                                uint32_t  offset,
                                uint32_t  writedata )
{
	uint32_t rc = WriteReg( dev_info, offset, writedata );

	if (RIO_SUCCESS == rc) {
		uint8_t idx = 0;
		for (idx = SCRPAD_FIRST_IDX; idx < MAX_DAR_SCRPAD_IDX; idx++) {
			if (scratchpad_const[idx].offset == offset) {
				writedata &= scratchpad_const[idx].rw_mask;
				dev_info->scratchpad[idx] = writedata;

				switch (offset) {
				case Tsi578_RIO_MC_MASK_CFG    : 
				{
					uint32_t mask = (writedata & Tsi578_RIO_MC_MASK_CFG_MC_MASK_NUM) >> 16;
					uint8_t port = (writedata & RIO_MC_MSK_CFG_PT_NUM) >> 8;
					uint32_t cmd  = (writedata & RIO_MC_MSK_CFG_CMD);
					/* Write to Tsi578_RIO_MC_MASK_CFG can update mask registers.
					 * Emulate effect on mask registers, as we can't trust reading the
					 * global mask registers if Port 0 is powered down.
					 */

					switch (cmd) {
					case RIO_MC_MSK_CFG_CMD_ADD:
						dev_info->scratchpad[mask+SCRPAD_MASK_IDX] |= ((uint32_t)(1) << (port + 16));
						break;
					case RIO_MC_MSK_CFG_CMD_DEL:
						dev_info->scratchpad[mask+SCRPAD_MASK_IDX] &= ~((uint32_t)(1) << (port + 16));
						break;
					case RIO_MC_MSK_CFG_CMD_DEL_ALL:
						dev_info->scratchpad[mask+SCRPAD_MASK_IDX] &= ~Tsi578_RIO_MC_MSKX_MC_MSK;
						break;
					case RIO_MC_MSK_CFG_CMD_ADD_ALL:
						dev_info->scratchpad[mask+SCRPAD_MASK_IDX] |= Tsi578_RIO_MC_MSKX_MC_MSK;
						break;
					default:
						break;
					};
					break;
				}

				case Tsi578_RIO_MC_DESTID_ASSOC:
				{
					uint8_t mask;
					uint32_t destid;
					bool large = (dev_info->scratchpad[idx] & RIO_MC_CON_OP_DEV16M);
					uint32_t cmd = (dev_info->scratchpad[idx] & RIO_MC_CON_OP_CMD);
					
					if (!idx) {
						rc = RIO_ERR_SW_FAILURE;
						break;
					}

					mask = (dev_info->scratchpad[idx - 1] & RIO_MC_CON_SEL_MASK);
					destid = dev_info->scratchpad[idx - 1] & 
						(RIO_MC_CON_SEL_DEV8 | RIO_MC_CON_SEL_DEV16);

					/* Write to Tsi578_RIO_MC_DESTID_ASSOC can update destID registers.
					 * Must emulate the effect, as it is not possible to trust the value
					 * of the destID register selected when port 0 is powered down.
					 */
					switch (cmd) {
					case RIO_MC_CON_OP_CMD_DEL:
						dev_info->scratchpad[mask] = 0;
						break;
					case RIO_MC_CON_OP_CMD_ADD:
						dev_info->scratchpad[mask] = (destid >> 16) |
							Tsi578_RIO_MC_IDX_MC_EN | ((large)?(Tsi578_RIO_MC_IDX_LARGE_SYS):0);
						break;
					default:
						break;
					};
					break;
				};
				default: break;
				};
				break;
			};
		};
	}
	return rc;
}

uint32_t IDT_tsi57xReadReg( DAR_DEV_INFO_t *dev_info,
                                uint32_t  offset,
                                uint32_t  *readdata )
{
	uint32_t rc = RIO_SUCCESS;
	bool found_one = false;
	uint8_t idx = 0;

	for (idx = SCRPAD_FIRST_IDX; idx < MAX_DAR_SCRPAD_IDX; idx++) {
		if (scratchpad_const[idx].offset == offset) {
			switch (offset) {
			case Tsi578_RIO_MC_DESTID_ASSOC:
			case Tsi578_RIO_MC_MASK_CFG    : 
			case Tsi578_RIO_MC_DESTID_CFG  : 
				continue;
			default:
				*readdata = dev_info->scratchpad[idx];
				found_one = true;
				continue;
			};
		};
	};

	if (!found_one)
		rc = ReadReg( dev_info, offset, readdata );

	return rc;
}


uint32_t init_scratchpad( DAR_DEV_INFO_t *DAR_info )
{
	uint32_t rc;
	uint8_t idx;

	for (idx = SCRPAD_FIRST_IDX; idx < MAX_DAR_SCRPAD_IDX; idx++) {
		if (SCRPAD_EOF_OFFSET == scratchpad_const[idx].offset) {
			rc = RIO_ERR_REG_ACCESS_FAIL;
			break;
		};

		rc = ReadReg( DAR_info, scratchpad_const[idx].offset, &DAR_info->scratchpad[idx]);
		if (RIO_SUCCESS != rc)
			break;
	};
	return rc;
};

uint32_t IDT_tsi57xDeviceSupported( DAR_DEV_INFO_t *DAR_info )
{
    uint32_t rc = DAR_DB_NO_DRIVER;

    if ( IDT_TSI_VENDOR_ID ==  ( DAR_info->devID & RIO_DEV_IDENT_VEND ) )
    {
        if ( (IDT_TSI57x_DEV_ID >> 4) == ( (DAR_info->devID & RIO_DEV_IDENT_DEVI) >> 20) )
        {
            /* Now fill out the DAR_info structure... */
            rc = DARDB_rioDeviceSupportedDefault( DAR_info );

            /* Index and information for DSF is the same as the DAR handle */
            DAR_info->dsf_h = Tsi57x_driver_handle;
	rc = init_scratchpad( DAR_info );

            if ( rc == RIO_SUCCESS ) {
                num_Tsi57x_driver_instances++ ;
                getTsiName( DAR_info );
            };
        }
    }
    return rc;
}

uint32_t bind_tsi57x_DAR_support( void )
{
    DAR_DB_Driver_t DAR_info;

    DARDB_Init_Driver_Info( IDT_TSI_VENDOR_ID, &DAR_info );

	DAR_info.WriteReg = IDT_tsi57xWriteReg;
	DAR_info.ReadReg = IDT_tsi57xReadReg;
    DAR_info.rioDeviceSupported = IDT_tsi57xDeviceSupported;

    DARDB_Bind_Driver( &DAR_info );
    
    return RIO_SUCCESS;
}

#define TSI57X_HIDDEN_SERDES_REG(xx,yy) ((uint32_t)(0x1E00C+(2*0x100*xx)+(0x40*yy)))
#define TSI57X_HIDDEN_SERDES_TX_INV 0x00080000
#define TSI57X_HIDDEN_SERDES_RX_INV 0x00040000
#define MAX(a,b) ((a>b)?a:b)
#define MAC(x) (x/2)
#define EVEN_PORT(x) (x & ~1)

uint32_t init_sw_pmr(DAR_DEV_INFO_t        *dev_info, 
                 port_mac_relations_t **sw_pmr   ) {
   uint32_t rc = RIO_SUCCESS;

   switch ( (dev_info->devID & Tsi578_RIO_DEV_IDENT_DEVI) >> 16 ) {
           case Tsi578_RIO_DEVID_VAL:  *sw_pmr = tsi578_pmr;
                                        break;
           case Tsi574_RIO_DEVID_VAL:  *sw_pmr = tsi574_pmr;
                                        break;
           case Tsi572_RIO_DEVID_VAL:  *sw_pmr = tsi572_pmr;
                                        break;
           case Tsi577_RIO_DEVID_VAL:  *sw_pmr = tsi577_pmr;
                                        break;
           default: rc = RIO_ERR_NO_FUNCTION_SUPPORT;
   };

   return rc;
};

uint32_t tsi57x_set_lrto( DAR_DEV_INFO_t           *dev_info , 
                        idt_pc_set_config_in_t   *in_parms )
{
 uint32_t rc;
 uint32_t lrto;

 // LRTO register has a granularity of 320 nsec.
 
 lrto = (in_parms->lrto*10)/32;

 if (lrto > 0xFFFFFF) {
    lrto = 0xFFFFFF;
 };

 if (!lrto)
	 lrto = 1;

 rc = DARRegWrite( dev_info, Tsi578_RIO_SW_LT_CTL, lrto << 8 );

 return rc;   
}

uint32_t tsi57x_get_lrto( DAR_DEV_INFO_t           *dev_info , 
                        idt_pc_set_config_out_t  *out_parms )
{
 uint32_t rc;
 uint32_t lrto;

 rc = DARRegRead( dev_info, Tsi578_RIO_SW_LT_CTL, &lrto );
 if (RIO_SUCCESS != rc)
    return rc;

 // LRTO granularity is 320 nanoseconds.
 lrto = ((lrto >> 8) * 32)/10;

 out_parms->lrto = lrto;

 return rc;   
}


uint32_t idt_tsi57x_pc_get_config  ( DAR_DEV_INFO_t           *dev_info, 
                                   idt_pc_get_config_in_t   *in_parms, 
                                   idt_pc_get_config_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  port_num;
    uint32_t port_idx;
    uint32_t dloopRegVal, SerDesRegVal, spxCtl;
    int32_t  first_lane, num_lanes = 4, lane_num;
    port_mac_relations_t *sw_pmr;
	struct DAR_ptl good_ptl;

    out_parms->num_ports = 0;
    out_parms->lrto = 0;
    out_parms->imp_rc        = 0;

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_GET_CONFIG(1);
        goto idt_tsi57x_pc_get_config_exit;       
    };

	out_parms->num_ports = good_ptl.num_ports;
    for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
       out_parms->pc[port_idx].pnum = good_ptl.pnums[port_idx];

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_GET_CONFIG(3);
       goto idt_tsi57x_pc_get_config_exit;       
    };

    rc = tsi57x_get_lrto( dev_info, out_parms );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_GET_CONFIG(4);
       goto idt_tsi57x_pc_get_config_exit;       
    };

    for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
    {
        port_num = out_parms->pc[port_idx].pnum;
	out_parms->pc[port_idx].pw = idt_pc_pw_last;
        out_parms->pc[port_idx].ls = idt_pc_ls_last;
        out_parms->pc[port_idx].fc = idt_pc_fc_rx;
        out_parms->pc[port_idx].iseq = idt_pc_is_one;
        out_parms->pc[port_idx].nmtc_xfer_enable = false;
        out_parms->pc[port_idx].xmitter_disable = false;
        out_parms->pc[port_idx].port_lockout = false;
        out_parms->pc[port_idx].rx_lswap = false;
        out_parms->pc[port_idx].tx_lswap = false;
        for (lane_num = 0; lane_num < IDT_PC_MAX_LANES;lane_num++) {
           out_parms->pc[port_idx].tx_linvert[lane_num] = false;
           out_parms->pc[port_idx].rx_linvert[lane_num] = false;
        };
        
        /* 1x ports are not available when the 4x port is configured 
               to be a 4x port.              
        */
        rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].mac_num * 2),
                        &dloopRegVal );
        if (RIO_SUCCESS != rc) {
            out_parms->imp_rc = PC_GET_CONFIG(4);
            goto idt_tsi57x_pc_get_config_exit;
       }

        first_lane = sw_pmr[port_num].first_mac_lane;

        /* Deal with 1x port configuration. */
        if (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_MAC_MODE) {
            num_lanes  = sw_pmr[port_num].lane_count_1x;
        } else {
            num_lanes  = sw_pmr[port_num].lane_count_4x;
        }

       switch (num_lanes) {
           case 0: // If the port is not available, we still need to
                   // check if the port is powered up/down.
                   out_parms->pc[port_idx].port_available = false;
                   break;
           case 1: out_parms->pc[port_idx].port_available = true;
                   out_parms->pc[port_idx].pw             = idt_pc_pw_1x;
                   // Get the real swap values for 4x ports, or for a 1x 
                   // port that could be a 4x port.
                   if (4 == sw_pmr[port_num].lane_count_4x) {
                      out_parms->pc[port_idx].rx_lswap =
                          (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX)?true:false;
                      out_parms->pc[port_idx].tx_lswap = 
                          (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX)?true:false;
                   };
                   break;
           case 4: out_parms->pc[port_idx].port_available = true;
                   // Check for port width overrides...
                   rc = DARRegRead( dev_info, Tsi578_SPX_CTL(port_num), &spxCtl );
                   if (RIO_SUCCESS != rc) 
                   {
                      out_parms->imp_rc = PC_GET_CONFIG(5);
                      goto idt_tsi57x_pc_get_config_exit;
                   };
                   switch (spxCtl & RIO_SPX_CTL_PTW_OVER) {
                       case RIO_SPX_CTL_PTW_OVER_NONE : out_parms->pc[port_idx].pw = idt_pc_pw_4x;
                                                           break;
                       case RIO_SPX_CTL_PTW_OVER_1x_L0: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l0;
                                                           break;
                       case RIO_SPX_CTL_PTW_OVER_1x_LR: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l2;
                                                           break;
                       default                           : out_parms->pc[port_idx].pw = idt_pc_pw_last;
                   };
                   out_parms->pc[port_idx].rx_lswap =
                       (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX)?true:false;
                   out_parms->pc[port_idx].tx_lswap = 
                       (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX)?true:false;
                 break;
           default: rc = RIO_ERR_NOT_EXPECTED_RETURN_VALUE;
                    out_parms->imp_rc = PC_GET_CONFIG(6);
                    goto idt_tsi57x_pc_get_config_exit;
        };

        switch (dloopRegVal & Tsi578_SMACX_DLOOP_CLK_SEL_CLK_SEL)
        {
            case 0:  out_parms->pc[port_idx].ls = idt_pc_ls_1p25;
                     break;
            case 1:  out_parms->pc[port_idx].ls = idt_pc_ls_2p5;
                     break;
            default: out_parms->pc[port_idx].ls = idt_pc_ls_3p125;
                     break;
        }

       if (sw_pmr[port_num].mac_num != sw_pmr[port_num].pwr_mac_num) {
           rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].pwr_mac_num * 2),
                           &dloopRegVal );
           if (RIO_SUCCESS != rc) {
               out_parms->imp_rc = PC_GET_CONFIG(7);
               goto idt_tsi57x_pc_get_config_exit;
          }
       };

       out_parms->pc[port_idx].powered_up = (dloopRegVal & sw_pmr[port_num].pwr_down_mask)?false:true;
        
       if (out_parms->pc[port_idx].powered_up) {
           rc = DARRegRead( dev_info, Tsi578_SPX_CTL(port_num), &spxCtl );
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_GET_CONFIG(8);
              goto idt_tsi57x_pc_get_config_exit;       
          };

          out_parms->pc[port_idx].xmitter_disable = 
             (spxCtl & Tsi578_SPX_CTL_PORT_DIS)?true:false;
          out_parms->pc[port_idx].port_lockout = 
             (spxCtl & Tsi578_SPX_CTL_PORT_LOCKOUT)?true:false;
		  out_parms->pc[port_idx].nmtc_xfer_enable =
             ((spxCtl & Tsi578_SPX_CTL_INPUT_EN) && 
			  (spxCtl & Tsi578_SPX_CTL_OUTPUT_EN))?true:false;

          for (lane_num = first_lane; lane_num < (first_lane + num_lanes); lane_num++)
          {
              rc = DARRegRead( dev_info, TSI57X_HIDDEN_SERDES_REG((sw_pmr[port_num].mac_num),lane_num), &SerDesRegVal );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = PC_GET_CONFIG(9);
                 goto idt_tsi57x_pc_get_config_exit;       
              };
                
              out_parms->pc[port_idx].rx_linvert[lane_num - first_lane] = (SerDesRegVal & TSI57X_HIDDEN_SERDES_RX_INV)?true:false;
              out_parms->pc[port_idx].tx_linvert[lane_num - first_lane] = (SerDesRegVal & TSI57X_HIDDEN_SERDES_TX_INV)?true:false;
          }
       }
    }
idt_tsi57x_pc_get_config_exit:       
    return rc;
}

typedef enum { Tsi57x_none, Tsi57x_4x, Tsi57x_2_1x } Tsi57x_MAC_MODE;
typedef enum { Tsi57x_no_ch, Tsi57x_1p25, Tsi57x_2p5, Tsi57x_3p125 } Tsi57x_LS;

typedef struct mac_indexes_t_TAG
{
    int32_t port_idx[2]; 
} mac_indexes_t;

typedef struct idt_lane_diffs_per_port_t_TAG
{
    int32_t first_lane;
    int32_t num_lanes;
    int32_t change_mask;
    int32_t invert_mask;
} idt_lane_diffs_per_port_t;

typedef struct idt_lane_diffs_per_mac_t_TAG
{
    idt_lane_diffs_per_port_t lane_diffs[2];
} idt_lane_diffs_per_mac_t;

typedef struct port_cfg_chg_t_TAG {
   bool   valid[IDT_MAX_PORTS];
   bool   laneRegsChanged[Tsi578_MAX_MAC][Tsi578_MAX_LANES];
   uint32_t laneRegs[Tsi578_MAX_MAC][Tsi578_MAX_LANES];
   uint32_t dloopCtl[Tsi578_MAX_MAC];
   uint32_t dloopCtlOrig[Tsi578_MAX_MAC];
   uint32_t spxCtl[IDT_MAX_PORTS];
   uint32_t spxCtlOrig[IDT_MAX_PORTS];
} port_cfg_chg_t;

void compute_lane_reg( port_mac_relations_t     *sw_pmr         ,
                       port_cfg_chg_t           *chg            ,
                       uint8_t                     port_num       ,
                       uint8_t                     lane_num       ,
                       bool                      tx_linvert     ,
                       bool                      rx_linvert      )
{
   uint8_t  mac     = sw_pmr[port_num].mac_num;
   uint32_t reg_val = chg->laneRegs[mac][lane_num];

   if (tx_linvert)  {
      reg_val |=  TSI57X_HIDDEN_SERDES_TX_INV;
   } else {
      reg_val &= ~TSI57X_HIDDEN_SERDES_TX_INV;
   };
   if (rx_linvert) {
      reg_val |=  TSI57X_HIDDEN_SERDES_RX_INV;
   } else {
      reg_val &= ~TSI57X_HIDDEN_SERDES_RX_INV;
   };

   if (reg_val != chg->laneRegs[mac][lane_num]) {
      chg->laneRegs[mac][lane_num]= reg_val;
      chg->laneRegsChanged[mac][lane_num]= true;
   };
};

uint32_t compute_port_config_changes( DAR_DEV_INFO_t           *dev_info       , 
                                    port_cfg_chg_t           *chg            ,
                                    port_mac_relations_t     *sw_pmr         ,
                                    idt_pc_one_port_config_t *in_parms_sorted,
                                    idt_pc_set_config_out_t  *out_parms      )
{
    uint32_t rc = RIO_SUCCESS;
    idt_pc_get_config_in_t  curr_cfg_in;
    idt_pc_get_config_out_t curr_cfg_out;
    uint8_t  mac, pnum, port_num, lane_idx, change_requested;

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(0x21);
       goto compute_port_config_changes_exit;
    };

    /* First, get current operating mode of ALL ports 
     * Curr_cfg_out will have information about all ports in
     * ascending order, contiguously, for all ports on the device.
    */

	curr_cfg_in.ptl.num_ports = RIO_ALL_PORTS;
    rc = idt_pc_get_config( dev_info, &curr_cfg_in, &curr_cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = curr_cfg_out.imp_rc;
       rc = PC_SET_CONFIG(0x22);
       goto compute_port_config_changes_exit;
    };

    // Get DLOOP values
    for (mac = 0; mac < Tsi578_MAX_MAC; mac++) {
        rc = DARRegRead( dev_info,
              Tsi578_SMACX_DLOOP_CLK_SEL(mac*2),
              &chg->dloopCtl[mac] );

        if (RIO_SUCCESS != rc) {
            out_parms->imp_rc = PC_SET_CONFIG(0x23);
            goto compute_port_config_changes_exit;
       };

        chg->dloopCtlOrig[mac] = chg->dloopCtl[mac];
    };

    // Get SPXCTL values
    for (pnum = 0; pnum < TSI57X_NUM_PORTS(dev_info); pnum++ ) {
        rc = DARRegRead( dev_info, Tsi578_SPX_CTL(pnum), &chg->spxCtl[pnum] );
        if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = rc;
           rc = PC_SET_CONFIG(0x24);
           goto compute_port_config_changes_exit;
       };
       chg->spxCtlOrig[pnum] = chg->spxCtl[pnum];
    };

    for (pnum = 0; pnum < TSI57X_NUM_PORTS(dev_info); pnum++) {
        // If this entry is valid, and there's a change in configuration,
       // then deal with it.
       change_requested = ( chg->valid[pnum] && memcmp( (void *)(&curr_cfg_out.pc[pnum]), 
                                                        (void *)(&in_parms_sorted[pnum]),
                                                         sizeof(idt_pc_one_port_config_t )));
         
       if (change_requested) {
          uint32_t start_dloop;
          // Change the DLOOP CONTROL VALUE 
          mac = sw_pmr[pnum].mac_num;

          start_dloop = chg->dloopCtl[mac];
          // Set the powerup state
          if (in_parms_sorted[pnum].powered_up) {
             chg->dloopCtl[mac] &= ~sw_pmr[pnum].pwr_down_mask;
          } else {
             chg->dloopCtl[mac] |=  sw_pmr[pnum].pwr_down_mask;
          }

          // If we're powering up a port, the starting value for SPx_CTL is wrong.
          // Correct it.
          if ((chg->dloopCtl[mac] != start_dloop) && in_parms_sorted[pnum].powered_up) {
             chg->spxCtl[pnum] = 0x00600001; 
          }

          if (sw_pmr[pnum].lane_count_4x) {
             // If port is powered up, change configuration as necessary
             // Port width - 4x or 1x
             if (in_parms_sorted[pnum].powered_up) {
                 if (idt_pc_pw_1x == in_parms_sorted[pnum].pw) {
                   chg->dloopCtl[mac] |=  Tsi578_SMACX_DLOOP_CLK_SEL_MAC_MODE;
                 } else {
                   chg->dloopCtl[mac] &= ~Tsi578_SMACX_DLOOP_CLK_SEL_MAC_MODE;
                 }

                 if (idt_pc_pw_4x == in_parms_sorted[pnum].pw) {
                    // Clear any overrides in the SPx_CTL register
                    chg->spxCtl[pnum] = (chg->spxCtl[pnum] & ~Tsi578_SPX_CTL_OVER_PWIDTH) | RIO_SPX_CTL_PTW_OVER_NONE;
                     
                    // Apply the requested RX and TX swap values
                    if (in_parms_sorted[pnum].tx_lswap) {
                      chg->dloopCtl[mac] |=  Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX;
                    } else {
                      chg->dloopCtl[mac] &= ~Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX;
                    }
                    
                    if (in_parms_sorted[pnum].rx_lswap) {
                      chg->dloopCtl[mac] |=  Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX;
                    } else {
                      chg->dloopCtl[mac] &= ~Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX;
                    }
                 } else {
                    // Lane swapping is not supported in any 1x mode.
                    chg->dloopCtl[mac] &= ~(Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX | Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX  );

                    if (idt_pc_pw_1x == in_parms_sorted[pnum].pw) {
                       // Clear any overrides in the SPx_CTL register
                       chg->spxCtl[pnum] = (chg->spxCtl[pnum] & ~Tsi578_SPX_CTL_OVER_PWIDTH) | RIO_SPX_CTL_PTW_OVER_NONE;
                    } else {
                       // Need to set overrides in the port width control register.
                       if (idt_pc_pw_1x_l0 == in_parms_sorted[pnum].pw) {
                          chg->spxCtl[pnum] = (chg->spxCtl[pnum] & ~Tsi578_SPX_CTL_OVER_PWIDTH) | RIO_SPX_CTL_PTW_OVER_1x_L0;
                       } else {
                          chg->spxCtl[pnum] = (chg->spxCtl[pnum] & ~Tsi578_SPX_CTL_OVER_PWIDTH) | RIO_SPX_CTL_PTW_OVER_1x_LR;
                       }
                    }
                 };

                 // Set link speed
                 chg->dloopCtl[mac] = (chg->dloopCtl[mac] & ~(Tsi578_SMACX_DLOOP_CLK_SEL_CLK_SEL)) | (uint32_t)(in_parms_sorted[pnum].ls);
             }; // Powered up
          }; // 4x port

          // Change the SPX_CTL value
          if (in_parms_sorted[pnum].xmitter_disable) {
             chg->spxCtl[pnum] |=  Tsi578_SPX_CTL_PORT_DIS;
          } else {
             chg->spxCtl[pnum] &= ~Tsi578_SPX_CTL_PORT_DIS;
          };

          if (in_parms_sorted[pnum].port_lockout) {
             chg->spxCtl[pnum] |=  Tsi578_SPX_CTL_PORT_LOCKOUT;
          } else {
             chg->spxCtl[pnum] &= ~Tsi578_SPX_CTL_PORT_LOCKOUT;
          };
		           
		  if (in_parms_sorted[pnum].nmtc_xfer_enable) {
             chg->spxCtl[pnum] |=  Tsi578_SPX_CTL_INPUT_EN | Tsi578_SPX_CTL_OUTPUT_EN;
          } else {
             chg->spxCtl[pnum] &= ~(Tsi578_SPX_CTL_INPUT_EN | Tsi578_SPX_CTL_OUTPUT_EN);
          };
       };
    };
        
    // Update lane register values for the 4x ports.
    // If the port is operating in 4x mode, take the lane inversion values from the port.
    // Otherwise, take the lane inversion values from the dependent port(s).

    for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num++) {
       if (!chg->valid[port_num] || !in_parms_sorted[port_num].port_available || !in_parms_sorted[port_num].powered_up) 
          continue;

       if (idt_pc_pw_4x == in_parms_sorted[port_num].pw)  {
          for (lane_idx = 0; lane_idx < IDT_MAX_LANES; lane_idx++) {
             compute_lane_reg( sw_pmr, chg, port_num, lane_idx, in_parms_sorted[port_num].tx_linvert[lane_idx], 
                                                                in_parms_sorted[port_num].rx_linvert[lane_idx] );
           }
       } else {
           //  1x ports always have only one lane to check for inversions :)
           uint8_t  dep_lnum = sw_pmr[port_num].first_mac_lane;

           compute_lane_reg( sw_pmr, chg, port_num, dep_lnum, in_parms_sorted[port_num].tx_linvert[0], 
                                                              in_parms_sorted[port_num].rx_linvert[0] );

       };
    };

compute_port_config_changes_exit:
    return rc;
}

#define PC_DET_PORTS_2_SKIP(x) (PC_DET_PORTS_2_SKIP_0+x)
#define MAX_PORTS_TO_SKIP      2

uint32_t idt_tsi57x_pc_get_status  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_pc_get_status_in_t   *in_parms, 
                                 idt_pc_get_status_out_t  *out_parms );

#define RESET_PORT_SKIP   true
#define CONFIG_PORT_SKIP  false

uint32_t determine_ports_to_skip ( DAR_DEV_INFO_t          *dev_info, 
                                 bool                     oob_reg_acc,
                                 uint8_t                   *ports_to_skip,
                                 uint32_t                  *imp_rc,      
                                 port_mac_relations_t    *sw_pmr,
                                 bool                    reset_port ) 
{
   uint32_t rc = RIO_SUCCESS;
   uint8_t  idx;
   uint8_t  conn_port;
   idt_pc_get_status_in_t  stat_in;
   idt_pc_get_status_out_t stat_out;
   uint32_t                  sw_port;
   
   for (idx = 0; idx < MAX_PORTS_TO_SKIP; idx++) {
      ports_to_skip[idx] = RIO_ALL_PORTS;
   };

   // If resetting the connected port is OK, or
   // if we're running on the Tsi577, there are no
   // dependencies between powering down one port 
   // and another.  Just return.
   if (oob_reg_acc || (Tsi577_RIO_DEVID_VAL == DEV_CODE(dev_info))) {
      return rc;
   };

   // Determine the connected port.
   rc = DARRegRead( dev_info, Tsi578_RIO_SW_PORT, &sw_port );
   if (RIO_SUCCESS != rc) {
      *imp_rc = PC_DET_PORTS_2_SKIP(1);
      goto determine_ports_to_skip_exit;
   }

   conn_port = (uint8_t)(sw_port & Tsi578_RIO_SW_PORT_PORT_NUM);

   // Check status:  If conn_port is not PORT_OK, then
   // it can't be in use to access this device.  Reset this port.
   stat_in.ptl.num_ports   = 1;
   stat_in.ptl.pnums[0] = conn_port;

   rc = idt_tsi57x_pc_get_status( dev_info, &stat_in, &stat_out );
   if (RIO_SUCCESS != rc) {
      *imp_rc = stat_out.imp_rc;
      goto determine_ports_to_skip_exit;
   }

   if (!stat_out.ps[0].port_ok) {
      *imp_rc = PC_DET_PORTS_2_SKIP(2);
      goto determine_ports_to_skip_exit;
   }

   // The connected port is in use.
   // Initialize ports_to_skip based on port_in and conn_port relationship
   
   if (reset_port) {
      if (sw_pmr[conn_port].lane_count_4x) {
         // If the connected port is a 4x port, then don't reset it.
         ports_to_skip[0] = conn_port;
         *imp_rc = PC_DET_PORTS_2_SKIP(3);
      } else {
         // If the connected port is not a 4x port, then don't reset it 
         // and don't reset the 4x port on the same MAC.
         ports_to_skip[0] = sw_pmr[conn_port].other_mac_ports[0];
         ports_to_skip[1] = conn_port;
         *imp_rc = PC_DET_PORTS_2_SKIP(4);
      };
   } else {
      // Must skip reconfiguration of all ports on the connected port's MAC.
      ports_to_skip[0] = conn_port;
      ports_to_skip[1] = sw_pmr[conn_port].other_mac_ports[0];
      *imp_rc = PC_DET_PORTS_2_SKIP(5);
   };


determine_ports_to_skip_exit:
   return rc;
};

typedef struct powerup_reg_offsets_t_TAG
{
   uint32_t offset;          // Base register address
   uint32_t per_port_offset; // If there is a per-port offset
   uint32_t mask;            // Mask applied to zero out fields 
                           //    that should not be preserved.
} powerup_reg_offsets_t;

powerup_reg_offsets_t reg_offsets[]=
// Start of global registers with per-port copies that must be preserved
{ 
// Start of per-port registers that must be preserved
  { (uint32_t)Tsi578_SPX_CTL(0)             , (uint32_t)0x20, ALL_BITS  },
  { (uint32_t)Tsi578_SPX_RATE_EN(0)         , (uint32_t)0x40, ALL_BITS  },
  { (uint32_t)Tsi578_SPX_ERR_RATE(0)        , (uint32_t)0x40, (uint32_t)~Tsi578_SPX_ERR_RATE_ERR_RATE_CNT  },
  { (uint32_t)Tsi578_SPX_ERR_THRESH(0)      , (uint32_t)0x40, ALL_BITS  },
  { (uint32_t)Tsi578_SPX_DISCOVERY_TIMER(0) , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_MODE(0)            , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_RIO_WM(0)          , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_ROUTE_CFG_DESTID(0), (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_ROUTE_BASE(0)      , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_CTL_INDEP(0)       , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_SILENCE_TIMER(0)   , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_RIOX_MC_LAT_LIMIT(0)   , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_PSC0n1_CTRL(0)     , (uint32_t)0x100, ALL_BITS }, 
  { (uint32_t)Tsi578_SPX_PSC2n3_CTRL(0)     , (uint32_t)0x100, ALL_BITS }, 
  { (uint32_t)Tsi578_SPX_PSC4n5_CTRL(0)     , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_TX_Q_D_THRESH(0)   , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_TX_Q_uint32_t(0)     , (uint32_t)0x100, (uint32_t)Tsi578_SPX_TX_Q_uint32_t_CONG_THRESH }, //  MASK
  { (uint32_t)Tsi578_SPX_TX_Q_PERIOD(0)     , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_RX_Q_D_THRESH(0)   , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_RX_Q_uint32_t(0)     , (uint32_t)0x100, (uint32_t)Tsi578_SPX_RX_Q_uint32_t_CONG_THRESH }, //  MASK
  { (uint32_t)Tsi578_SPX_RX_Q_PERIOD(0)     , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_REORDER_CTR(0)     , (uint32_t)0x100, (uint32_t)Tsi578_SPX_REORDER_CTR_THRESH  }, //  MASK
  { (uint32_t)Tsi578_SPX_ISF_WM(0)          , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_WRR_0(0)           , (uint32_t)0x100, ALL_BITS }, 
  { (uint32_t)Tsi578_SPX_WRR_1(0)           , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_WRR_2(0)           , (uint32_t)0x100, ALL_BITS },
  { (uint32_t)Tsi578_SPX_WRR_3(0)           , (uint32_t)0x100, ALL_BITS } 
};

#define PRESERVED_REGS_SIZE ((uint32_t)(sizeof(reg_offsets)/sizeof(reg_offsets[0])))
typedef uint32_t preserved_regs[PRESERVED_REGS_SIZE];
                                   
uint32_t preserve_regs_for_powerup( DAR_DEV_INFO_t  *dev_info,
                                  uint8_t            port_num,
                                  preserved_regs  *saved_regs,
								  bool			   do_all_regs)
{
    uint32_t rc = RIO_SUCCESS;
    uint32_t  idx;

	if (!do_all_regs)
		goto preserve_regs_for_powerup_exit;

    for (idx = 0; idx < PRESERVED_REGS_SIZE; idx++)
    {
        rc = DARRegRead( dev_info, reg_offsets[idx].offset + (port_num * reg_offsets[idx].per_port_offset),
                        &((*saved_regs)[idx]) );
        if (RIO_SUCCESS != rc)
            goto preserve_regs_for_powerup_exit;
        (*saved_regs)[idx] &= reg_offsets[idx].mask;
    }
    
preserve_regs_for_powerup_exit:
    return rc;
}                            

uint32_t restore_regs_from_powerup( DAR_DEV_INFO_t  *dev_info,
                                  uint8_t            port_num,
                                  preserved_regs  *saved_regs,
								  bool			   do_all_regs )
{
    uint32_t rc = RIO_SUCCESS;
    uint32_t idx;

	for (idx = 0; idx < MAX_DAR_SCRPAD_IDX; idx++) {
		/* Multicast programming registers do not need to be restored. */
		if ((scratchpad_const[idx].offset == Tsi578_RIO_MC_MASK_CFG    ) ||
		    (scratchpad_const[idx].offset == Tsi578_RIO_MC_DESTID_CFG  ) ||
		    (scratchpad_const[idx].offset == Tsi578_RIO_MC_DESTID_ASSOC))
			continue;
        	rc = DARRegWrite( dev_info, scratchpad_const[idx].offset, dev_info->scratchpad[idx] );
        	if (RIO_SUCCESS != rc)
            		goto restore_regs_from_powerup_exit;
	};

	if (!do_all_regs)
		goto restore_regs_from_powerup_exit;

    for (idx = 0; idx < PRESERVED_REGS_SIZE; idx++)
    {
        (*saved_regs)[idx] &= reg_offsets[idx].mask;
        rc = DARRegWrite( dev_info, reg_offsets[idx].offset + 
                                    (port_num * reg_offsets[idx].per_port_offset),
                    (*saved_regs)[idx] );
        if (RIO_SUCCESS != rc)
            goto restore_regs_from_powerup_exit;
    }
    
restore_regs_from_powerup_exit:
    return rc;
}                     

uint32_t  update_lane_inversions( DAR_DEV_INFO_t       *dev_info     , 
                                uint8_t                 port_num     ,
                                port_cfg_chg_t       *chg          ,
                                port_mac_relations_t *sw_pmr       )
{
   uint32_t rc = RIO_SUCCESS;
   uint8_t mac_num = sw_pmr[port_num].mac_num, lane_idx;

   for (lane_idx = 0; lane_idx < Tsi578_MAX_LANES; lane_idx++) {
      if ( chg->laneRegsChanged[mac_num][lane_idx] ) {
         rc = DARRegWrite( dev_info, TSI57X_HIDDEN_SERDES_REG(mac_num,lane_idx),
                           chg->laneRegs[mac_num][lane_idx] );
         if (RIO_SUCCESS != rc)
            goto update_lane_inversions_exit;       
      }
   }

update_lane_inversions_exit:
   return rc;
}

uint32_t pc_set_config_init_parms_check_conflicts_all_ports (DAR_DEV_INFO_t          *dev_info, 
                                                           port_cfg_chg_t          *chg,
                                                           port_mac_relations_t    *sw_pmr,
                                                           idt_pc_set_config_in_t  *in_parms, 
                                                           idt_pc_set_config_in_t  *in_parms_sorted,
                                                           idt_pc_set_config_out_t *out_parms )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
    uint8_t  port_num;

    out_parms->num_ports = TSI57X_NUM_PORTS(dev_info);

    if ((in_parms->pc[0].pw >= idt_pc_pw_last) ||
        (in_parms->pc[0].pw == idt_pc_pw_2x  ) ||
        (in_parms->pc[0].pw == idt_pc_pw_1x_l1) ||
        (in_parms->pc[0].ls >= idt_pc_ls_5p0 )) {
       out_parms->imp_rc = PC_SET_CONFIG(0x50);
       goto pc_set_config_init_parms_check_conflicts_all_ports_exit; 
    };

    if (in_parms->pc[0].pw == idt_pc_pw_1x) {
       // Make sure that no lane swapping is attempted, 
       // and that no lane inversion is requested.
       if (in_parms->pc[0].tx_lswap || 
           in_parms->pc[0].rx_lswap || 
           in_parms->pc[0].tx_linvert[0] || 
           in_parms->pc[0].rx_linvert[0] ) {
          out_parms->imp_rc = PC_SET_CONFIG(0x51);
          goto pc_set_config_init_parms_check_conflicts_all_ports_exit; 
       };
    };
       
    for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num+=2) {
       chg->valid[port_num  ] = true;
       chg->valid[port_num+1] = true;
       in_parms_sorted->pc[port_num  ].pnum = port_num;
       in_parms_sorted->pc[port_num+1].pnum = port_num+1;

       if (sw_pmr[port_num].lane_count_4x) {
          // Usual 2 port 4 lane MAC.  Always set up the even port,
          // If the even port is set up for 1x, duplicate the setup
          // for the odd port.
          // If the even port is set up for 4x, make the odd port
          // unavailable and powered down.
          memcpy( (void *)(&in_parms_sorted->pc[port_num]), 
                      (void *)(&in_parms->pc[0]), 
                     sizeof(idt_pc_one_port_config_t) );

          if (in_parms->pc[0].pw == idt_pc_pw_1x) {
             memcpy( (void *)(&in_parms_sorted->pc[port_num+1]), 
                      (void *)(&in_parms->pc[0]), 
                      sizeof(idt_pc_one_port_config_t) );
          } else {
             // If even ports are being set to 4x, then 
             in_parms_sorted->pc[port_num+1].port_available = false;
             in_parms_sorted->pc[port_num+1].powered_up     = false;
          };
       } else {
          // Two 1x ports for a Tsi577 4 port 4 lane MAC.  
          // If the setup is for 1x, clone both ports to be 1x.
          // If the setup is for 4x, both of these ports must be 
          // unavailable and powered down.
          if (in_parms->pc[0].pw == idt_pc_pw_1x) {
             memcpy( (void *)(&in_parms_sorted->pc[port_num  ]), 
                     (void *)(&in_parms->pc[0]), 
                      sizeof(idt_pc_one_port_config_t) );
             memcpy( (void *)(&in_parms_sorted->pc[port_num+1]), 
                     (void *)(&in_parms->pc[0]), 
                      sizeof(idt_pc_one_port_config_t) );
          } else {
             // If even ports are being set to 4x, then 
             in_parms_sorted->pc[port_num  ].port_available = false;
             in_parms_sorted->pc[port_num+1].port_available = false;
             in_parms_sorted->pc[port_num  ].powered_up     = false;
             in_parms_sorted->pc[port_num+1].powered_up     = false;
          };
       };
    };

    rc = RIO_SUCCESS;
pc_set_config_init_parms_check_conflicts_all_ports_exit: 
    return rc;
};

uint32_t pc_set_config_init_parms_check_conflict( DAR_DEV_INFO_t          *dev_info, 
                                                port_cfg_chg_t          *chg,
                                                port_mac_relations_t    *sw_pmr,
                                                idt_pc_set_config_in_t  *in_parms, 
                                                idt_pc_set_config_in_t  *in_parms_sorted, 
                                                idt_pc_set_config_out_t *out_parms )
{
    uint32_t rc;
    uint8_t  port_num, pnum, port_idx, lane_num;

    // Have to read all of the lane registers to know if they have changed or not.
    
    for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num++) {
       uint8_t mac = sw_pmr[port_num].mac_num;
       chg->valid[port_num]         = false;
       for (lane_num = 0; lane_num < Tsi578_MAX_LANES; lane_num++ ) 
       {
           if (sw_pmr[port_num].lane_count_4x) {
              rc = DARRegRead( dev_info, TSI57X_HIDDEN_SERDES_REG(mac, lane_num), 
                                               &chg->laneRegs[mac][lane_num] );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = PC_SET_CONFIG(0x30);
                 goto pc_set_config_init_parms_check_conflict_exit;
              };
           };
           chg->laneRegsChanged[mac][lane_num] = false;
       };
    };

    if (RIO_ALL_PORTS == in_parms->num_ports) 
    {
       rc = pc_set_config_init_parms_check_conflicts_all_ports (dev_info, chg, sw_pmr, in_parms, in_parms_sorted, out_parms );
       goto pc_set_config_init_parms_check_conflict_exit;
    };
       // NOT ALL PORTS
       rc = RIO_ERR_INVALID_PARAMETER;

	if (in_parms->num_ports >= TSI57X_NUM_PORTS(dev_info)) {
                 out_parms->imp_rc = PC_SET_CONFIG(0x99);
       		goto pc_set_config_init_parms_check_conflict_exit;
	}
       out_parms->num_ports = in_parms->num_ports;
       for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++) 
       {
           port_num = in_parms->pc[port_idx].pnum;
           if (  (TSI57X_NUM_PORTS(dev_info) <= port_num)
             || (chg->valid[port_num]                )) 
           {
               rc = RIO_ERR_INVALID_PARAMETER;
               out_parms->imp_rc = PC_SET_CONFIG(0x31);
               goto pc_set_config_init_parms_check_conflict_exit;
           }
           chg->valid[port_num] = true;
           memcpy( (void *)(&in_parms_sorted->pc[port_num]), 
                   (void *)(&in_parms->pc[port_idx]), 
                  sizeof(idt_pc_one_port_config_t) );
       };
       // Only check for conflicts between the 4x port and the 1x ports.
       for (port_num = 0; port_num < (TSI57X_NUM_PORTS(dev_info)); port_num++ ) 
       {
          // Tsi57x family does not support lane speeds above 3.125 Gbaud
          // or 2 lane port widths.
          if (((in_parms_sorted->pc[port_num].ls > idt_pc_ls_3p125 ) ||
               (in_parms_sorted->pc[port_num].pw == idt_pc_pw_2x   ) ||
               (in_parms_sorted->pc[port_num].pw == idt_pc_pw_1x_l1)) && chg->valid[port_num]) {
             rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
             out_parms->imp_rc = PC_SET_CONFIG(0x32);
             goto pc_set_config_init_parms_check_conflict_exit;
          };

          if ((in_parms_sorted->pc[port_num].pw >= idt_pc_pw_last) && chg->valid[port_num]) {
             out_parms->imp_rc = PC_SET_CONFIG(0x33);
             goto pc_set_config_init_parms_check_conflict_exit;
          };

          if ((4 == sw_pmr[port_num].lane_count_4x) && chg->valid[port_num])  
          {
            port_idx = 0;
            while ((port_idx < MAX_OTHER_MAC_PORTS) &&                        
		(RIO_ALL_PORTS != sw_pmr[port_num].other_mac_ports[port_idx]))
            {
               // The dependent port is always 1x.
               pnum = sw_pmr[port_num].other_mac_ports[port_idx];
               if (chg->valid[pnum]) 
               {
                  // Check that if the 4x port uses 4 lanes and is available, then the 1x port is not available
                  if (in_parms_sorted->pc[port_num].port_available && (in_parms_sorted->pc[port_num].pw != idt_pc_pw_1x)  &&
                      in_parms_sorted->pc[pnum    ].port_available) {
                      rc = RIO_ERR_INVALID_PARAMETER;
                      out_parms->imp_rc = PC_SET_CONFIG(0x34);
                      goto pc_set_config_init_parms_check_conflict_exit;
                  };
                  // Check that if the 4x port is powered down (not on a Tsi577), then the 1x port is either 
                  // not available or powered down or both.
                  if (!in_parms_sorted->pc[port_num].powered_up && 
                       in_parms_sorted->pc[pnum].port_available && 
                       in_parms_sorted->pc[pnum].powered_up     && 
                      !(Tsi577_RIO_DEVID_VAL == DEV_CODE(dev_info))) {
                     rc = RIO_ERR_INVALID_PARAMETER;
                     out_parms->imp_rc = PC_SET_CONFIG(0x35);
                     goto pc_set_config_init_parms_check_conflict_exit;
                  };
                  // Check that the lane speeds of the ports on the same quad are all the same.                      
                  if ( in_parms_sorted->pc[port_num].port_available && in_parms_sorted->pc[pnum].port_available &&
                      (in_parms_sorted->pc[port_num].ls        != in_parms_sorted->pc[pnum].ls       )) {
                      rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
                      out_parms->imp_rc = PC_SET_CONFIG(0x36);
                      goto pc_set_config_init_parms_check_conflict_exit;
                  };
                  // Tsi57x family does not support lane swapping for downgraded 4x ports
                  if ( ( (idt_pc_pw_1x    == in_parms_sorted->pc[port_num].pw) ||
                         (idt_pc_pw_1x_l0 == in_parms_sorted->pc[port_num].pw) ||
                         (idt_pc_pw_1x_l2 == in_parms_sorted->pc[port_num].pw)   ) &&
                       (in_parms_sorted->pc[port_num].tx_lswap || 
                        in_parms_sorted->pc[port_num].rx_lswap                    )  ) {
                      rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
                      out_parms->imp_rc = PC_SET_CONFIG(0x37);
                      goto pc_set_config_init_parms_check_conflict_exit;
                  };
               }
               port_idx++;
            };
          };
       };

    rc = RIO_SUCCESS;
pc_set_config_init_parms_check_conflict_exit:
    return rc;
} 

uint32_t idt_tsi57x_pc_set_config  ( DAR_DEV_INFO_t           *dev_info, 
                                   idt_pc_set_config_in_t   *in_parms, 
                                   idt_pc_set_config_out_t  *out_parms )
{
    uint32_t rc = RIO_SUCCESS;
    idt_pc_get_config_in_t   curr_cfg_in;
    idt_pc_set_config_in_t   in_parms_sorted;
    port_mac_relations_t    *sw_pmr;
    port_cfg_chg_t           chg;
    uint8_t                    even_odd, port_num, idx;
    uint8_t                    ports_to_skip[MAX_PORTS_TO_SKIP];
    bool restore_all_4x = false, restore_all_1x = false;

    out_parms->imp_rc = RIO_SUCCESS;
    out_parms->num_ports = 0;
    out_parms->lrto = 0;

    if ( (TSI57X_NUM_PORTS(dev_info) < in_parms->num_ports) &&
        !(RIO_ALL_PORTS      == in_parms->num_ports))
    {
        rc = RIO_ERR_INVALID_PARAMETER;
        out_parms->imp_rc = PC_SET_CONFIG(0x40);
        goto idt_tsi57x_pc_set_config_exit;
    }

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(0x41);
       goto idt_tsi57x_pc_set_config_exit;
    };

    // Check for incorrect parameters and configuration conflicts
    rc = pc_set_config_init_parms_check_conflict( dev_info, &chg, sw_pmr, in_parms, &in_parms_sorted, out_parms );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_pc_set_config_exit;
    };

    // Make sure we don't reconfigure the port that we're connected to.
    // Bad things can happen, like permanent connectivity loss :)
    rc = determine_ports_to_skip ( dev_info, in_parms->oob_reg_acc, ports_to_skip, &out_parms->imp_rc, sw_pmr, CONFIG_PORT_SKIP ) ;
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_pc_set_config_exit;

    for (idx = 0; idx < MAX_PORTS_TO_SKIP; idx++) {
       if (RIO_ALL_PORTS != ports_to_skip[idx]) {
          chg.valid[ports_to_skip[idx]] = false;
       };
    };

    // Compute configuration register changes
    rc = compute_port_config_changes( dev_info, &chg, sw_pmr, &in_parms_sorted.pc[0], out_parms );
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_pc_set_config_exit;

    /* Implement the changes to the Dloop registers */
    for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num+=2)
    {       
       uint8_t mac = sw_pmr[port_num].mac_num;
       bool  lanes_changed = chg.laneRegsChanged[mac][0] || chg.laneRegsChanged[mac][1] || 
                             chg.laneRegsChanged[mac][2] || chg.laneRegsChanged[mac][3];

       if ((chg.dloopCtlOrig[mac] !=chg.dloopCtl[mac])  || lanes_changed)
       {
          /* Something changed.  Set up the powerdown mask, and preserve registers as necessary
          */
          uint32_t         powerdown_mask = 0;
          preserved_regs even_port_regs, odd_port_regs;
          uint32_t         reg_val = chg.dloopCtlOrig[mac];

          if ( ((chg.dloopCtlOrig[mac] ^ chg.dloopCtl[mac]) & ~Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1) || lanes_changed )
          {
              powerdown_mask = Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4 |
                               Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1;

          } else {
              powerdown_mask = Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1;
          }
	  if (!(chg.dloopCtlOrig[mac] & Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4)) { 
	     restore_all_4x = true;
             rc = preserve_regs_for_powerup( dev_info, port_num, &even_port_regs, true );
             if (RIO_SUCCESS != rc) {
                out_parms->imp_rc = PC_SET_CONFIG(0x43);
                goto idt_tsi57x_pc_set_config_exit;
             };
	  };

	  if (!(chg.dloopCtlOrig[mac] & Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1)) { 
	     restore_all_1x = true;
             rc = preserve_regs_for_powerup( dev_info, port_num+1, &odd_port_regs, true );
             if (RIO_SUCCESS != rc) {
                out_parms->imp_rc = PC_SET_CONFIG(0x44);
                goto idt_tsi57x_pc_set_config_exit;
             };
          }; 
            
          reg_val |= powerdown_mask;

          /* To change values in the DLOOP clock select register,
                it is necessary to power down the ports,
                then change the values, then power the ports up.
          */
          rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(port_num), reg_val );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x46);
             goto idt_tsi57x_pc_set_config_exit;
          };
            

          reg_val = chg.dloopCtl[mac] | powerdown_mask;

          rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(port_num), reg_val );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x47);
             goto idt_tsi57x_pc_set_config_exit;
          }

          rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(port_num), chg.dloopCtl[mac] );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x49);
             goto idt_tsi57x_pc_set_config_exit;
          };

	  // Only restore registers if the port was and is powered up...
	   if (!(chg.dloopCtl[mac] & Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4)) {
              rc = restore_regs_from_powerup( dev_info, port_num, &even_port_regs, restore_all_4x );
              if (RIO_SUCCESS != rc) 
              {
                 out_parms->imp_rc = PC_SET_CONFIG(0x4A);
                 goto idt_tsi57x_pc_set_config_exit;
              }
          }

          // Lane registers can only be written when the MAC is powered up.
          rc = update_lane_inversions( dev_info, port_num, &chg, sw_pmr );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x48);
             goto idt_tsi57x_pc_set_config_exit;
          };

	  if (!(chg.dloopCtl[mac] & Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1)) {
             rc = restore_regs_from_powerup( dev_info, port_num+1, &odd_port_regs, restore_all_1x );
             if (RIO_SUCCESS != rc) 
             {
                out_parms->imp_rc = PC_SET_CONFIG(0x4B);
                goto idt_tsi57x_pc_set_config_exit;
             };
          };
       }
       
       // Set PORT_DIS, PORT_LOCKOUT, and INPUT/OUTPUT_ENABLE as requested.
       for (even_odd = 0; even_odd < 2; even_odd++) 
       {
          uint8_t pnum = port_num + even_odd;
          if (chg.valid[pnum] && (chg.spxCtl[pnum] != chg.spxCtlOrig[pnum]))
          {
             rc = DARRegWrite( dev_info, Tsi578_SPX_CTL(pnum), chg.spxCtl[pnum] );
             if (RIO_SUCCESS != rc) 
             {
                out_parms->imp_rc = PC_SET_CONFIG(0x4C);
                goto idt_tsi57x_pc_set_config_exit;
             };
          };
       };
    };

    // Initialize input parameters to select ports for updating
    // current configuration.
    if (RIO_ALL_PORTS == in_parms->num_ports) 
    {
       curr_cfg_in.ptl.num_ports = RIO_ALL_PORTS;
    } else {
       curr_cfg_in.ptl.num_ports = 0;
       for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num++) {
          if (chg.valid[port_num]) {
             curr_cfg_in.ptl.pnums[ curr_cfg_in.ptl.num_ports ] = port_num;
             curr_cfg_in.ptl.num_ports++;
          };
       };
    };

    // Update link response timeout value as the last item.
    // This is the best place to do it, as it reflects the current
    // power up/power down state of Port 0...
    rc = tsi57x_set_lrto( dev_info, in_parms );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(0x42);
       goto idt_tsi57x_pc_set_config_exit;
    };

    rc = idt_tsi57x_pc_get_config( dev_info, &curr_cfg_in, out_parms );
    
idt_tsi57x_pc_set_config_exit:       
    return rc;
}

uint32_t idt_tsi57x_pc_get_status  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_pc_get_status_in_t   *in_parms, 
                                 idt_pc_get_status_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  port_idx, port_num;
    uint32_t dloop, err_n_stat, spx_ctl;
    port_mac_relations_t    *sw_pmr;
	struct DAR_ptl good_ptl;
          
    out_parms->num_ports = 0;
    out_parms->imp_rc        = RIO_SUCCESS;

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_GET_uint32_t(1);
        goto idt_tsi57x_pc_get_status_exit;       
    };

    out_parms->num_ports = good_ptl.num_ports;
    for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
        out_parms->ps[port_idx].pnum = good_ptl.pnums[port_idx];

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_GET_uint32_t(3);
       goto idt_tsi57x_pc_get_status_exit;
    };

    for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
    {
       port_num = out_parms->ps[port_idx].pnum;

       rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].mac_num*2), &dloop );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_uint32_t(0x10+port_idx);
          goto idt_tsi57x_pc_get_status_exit;
       };

       out_parms->ps[port_idx].first_lane = sw_pmr[port_num].first_mac_lane;
       out_parms->ps[port_idx].num_lanes  = 
             (uint8_t)((dloop & Tsi578_SMACX_DLOOP_CLK_SEL_MAC_MODE)?sw_pmr[port_num].lane_count_1x:sw_pmr[port_num].lane_count_4x);

       // If there are lanes assigned to the port, check to see if the power is powered down.
       if (out_parms->ps[port_idx].num_lanes) {
          if (sw_pmr[port_num].mac_num != sw_pmr[port_num].pwr_mac_num) {
             rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].pwr_mac_num*2),
                             &dloop );
             if (RIO_SUCCESS != rc) {
                out_parms->imp_rc = PC_GET_uint32_t(0x20+port_idx);
                goto idt_tsi57x_pc_get_status_exit;
             };
          };
          if (dloop & sw_pmr[port_num].pwr_down_mask) {
             out_parms->ps[port_idx].num_lanes = 0;
          };
       };

       out_parms->ps[port_idx].pw = idt_pc_pw_last;
       if (!out_parms->ps[port_idx].num_lanes) {
          out_parms->ps[port_idx].port_ok        = false;
          out_parms->ps[port_idx].port_error     = false;
          out_parms->ps[port_idx].input_stopped  = false;
          out_parms->ps[port_idx].output_stopped = false;
          continue;
       };

       // Port is available and powered up, so let's figure out the status...
       rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(port_num), &err_n_stat );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_uint32_t(0x30+port_idx);
          goto idt_tsi57x_pc_get_status_exit;
       };

       rc = DARRegRead( dev_info, Tsi578_SPX_CTL(port_num), &spx_ctl );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_uint32_t(0x40+port_idx);
          goto idt_tsi57x_pc_get_status_exit;
       };

       out_parms->ps[port_idx].port_ok        = 
           (err_n_stat & Tsi578_SPX_ERR_uint32_t_PORT_OK        ) ? true : false;
       out_parms->ps[port_idx].input_stopped  = 
           (err_n_stat & Tsi578_SPX_ERR_uint32_t_INPUT_ERR_STOP ) ? true : false;
       out_parms->ps[port_idx].output_stopped = 
           (err_n_stat & Tsi578_SPX_ERR_uint32_t_OUTPUT_ERR_STOP) ? true : false;

       if (out_parms->ps[port_idx].port_ok) {
          switch (spx_ctl & Tsi578_SPX_CTL_INIT_PWIDTH) {
             case RIO_SPX_CTL_PTW_INIT_1x_L0: if (1 == out_parms->ps[port_idx].num_lanes) {
                                                    out_parms->ps[port_idx].pw = idt_pc_pw_1x;
                                                 } else {
                                                    out_parms->ps[port_idx].pw = idt_pc_pw_1x_l0;
                                                 };
                                                 break;
             case RIO_SPX_CTL_PTW_INIT_1x_LR: out_parms->ps[port_idx].pw = idt_pc_pw_1x_l2;
                                                 break;
             case RIO_SPX_CTL_PTW_INIT_4x   : out_parms->ps[port_idx].pw = idt_pc_pw_4x;
                                                 break;
             default:  out_parms->ps[port_idx].pw = idt_pc_pw_last;
          };
       };

       // Port Error is true if a PORT_ERR is present, OR
       // if a OUTPUT_FAIL is present when STOP_FAIL_EN is set.
       out_parms->ps[port_idx].port_error     = 
           ((err_n_stat & Tsi578_SPX_ERR_uint32_t_PORT_ERR       ) |
           ((spx_ctl    & Tsi578_SPX_CTL_STOP_FAIL_EN ) && 
             (err_n_stat & Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL))) ? true : false;
    }

idt_tsi57x_pc_get_status_exit:
    return rc;
}

uint32_t idt_tsi57x_rt_probe_all   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_probe_all_in_t     *in_parms, 
                                 idt_rt_probe_all_out_t    *out_parms );

uint32_t idt_tsi57x_rt_set_all   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_all_in_t       *in_parms, 
                                 idt_rt_set_all_out_t      *out_parms );

uint32_t restore_luts( DAR_DEV_INFO_t         *dev_info, 
                   uint8_t                   port_num,
                     idt_rt_state_t         *lut_save_in ) 
{
    idt_rt_set_all_in_t    lut_restore_in;
    idt_rt_set_all_out_t   lut_restore_out;

    lut_restore_in.set_on_port = port_num;
    lut_restore_in.rt          = lut_save_in;

    return idt_tsi57x_rt_set_all( dev_info, &lut_restore_in, &lut_restore_out );
};

#define TSI57X_LP(x) (TSI57X_LP_0+x)

uint32_t reset_tsi57x_lp( DAR_DEV_INFO_t          *dev_info, 
                        uint8_t                    port_num, 
                        uint32_t                  *imp_rc )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t lr_cmd = STYPE1_LREQ_CMD_RST_DEV;

   if (port_num >= TSI57X_NUM_PORTS(dev_info)) {
      *imp_rc = TSI57X_LP(0x10 + port_num);
      goto reset_tsi57x_lp_exit;
   }

   rc = DARRegWrite( dev_info, Tsi578_SPX_LM_REQ(port_num), lr_cmd );
   if (RIO_SUCCESS != rc) {
      *imp_rc = TSI57X_LP(0x20 + port_num);
      goto reset_tsi57x_lp_exit;
   }

reset_tsi57x_lp_exit:
  return rc;
};

uint32_t idt_tsi57x_pc_reset_port  ( DAR_DEV_INFO_t          *dev_info, 
                                   idt_pc_reset_port_in_t  *in_parms, 
                                   idt_pc_reset_port_out_t *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    preserved_regs          saved_regs[2];
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    uint32_t dloop   , pwrdwn;
    uint8_t  port_idx, port_num, dep_port_idx, dep_port_num, ports_to_skip[MAX_PORTS_TO_SKIP];
    port_mac_relations_t *sw_pmr;
    bool   found;

    out_parms->imp_rc = RIO_SUCCESS;

    if ((RIO_ALL_PORTS      != in_parms->port_num) &&
        (in_parms->port_num >= TSI57X_NUM_PORTS(dev_info))) {
       out_parms->imp_rc = PC_RESET_PORT(1);
       goto idt_tsi57x_pc_reset_port_exit;
    };

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) { 
       out_parms->imp_rc = PC_RESET_PORT(2);
       goto idt_tsi57x_pc_reset_port_exit;
    };

    rc = determine_ports_to_skip( dev_info, in_parms->oob_reg_acc, ports_to_skip, &out_parms->imp_rc, sw_pmr, RESET_PORT_SKIP );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_pc_reset_port_exit;
    }
     out_parms->imp_rc = RIO_SUCCESS;

    if (RIO_ALL_PORTS == in_parms->port_num) {
       cfg_in.ptl.num_ports = RIO_ALL_PORTS;
    } else {
       cfg_in.ptl.num_ports = 1;
       cfg_in.ptl.pnums[0] = in_parms->port_num;
       if (sw_pmr[in_parms->port_num].lane_count_4x && !(Tsi577_RIO_DEVID_VAL == DEV_CODE(dev_info))) {
          cfg_in.ptl.num_ports = 2;
          cfg_in.ptl.pnums[1] = sw_pmr[in_parms->port_num].other_mac_ports[0];
       };
    };

    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out);
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_pc_reset_port_exit;
    }

    for (port_idx = 0; port_idx < cfg_out.num_ports; port_idx++) {
       port_num = cfg_out.pc[port_idx].pnum;
       dep_port_num = RIO_ALL_PORTS;

       // Do not reset ports required for connectivity.
       // Also skip ports that are not available or powered down.
       if ((port_num == ports_to_skip[0]        ) ||
           (port_num == ports_to_skip[1]        ) ||
           (!cfg_out.pc[port_idx].port_available) ||
           (!cfg_out.pc[port_idx].powered_up    ))
          continue;

       if (in_parms->preserve_config) {
          rc = preserve_regs_for_powerup( dev_info, port_num, &saved_regs[0], in_parms->preserve_config );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_RESET_PORT(4);
             goto idt_tsi57x_pc_reset_port_exit;
          };

         // If this is an even MAC on a non-Tsi577, check the 
         // current configuration to see if the odd MAC port is powered up and active.
         // If it is, then we're resetting 2 ports at a time, so save the state.
         if (sw_pmr[port_num].lane_count_4x && !(Tsi577_RIO_DEVID_VAL == DEV_CODE(dev_info))) {
            dep_port_num = port_num + 1;
            found = false;
            for (dep_port_idx = 0; (dep_port_idx < cfg_out.num_ports) && !found; dep_port_idx++) {
               if (cfg_out.pc[dep_port_idx].pnum == dep_port_num) {
                  found = true;
                  if ((cfg_out.pc[port_idx].port_available) &&
                      (cfg_out.pc[port_idx].powered_up    )) {
                     rc = preserve_regs_for_powerup( dev_info, dep_port_num, &saved_regs[1], in_parms->preserve_config );
                  } else {
                     dep_port_num = RIO_ALL_PORTS;
                  };
               };
            };
            if (!found) {
               rc = RIO_ERR_RETURN_NO_RESULT;
               out_parms->imp_rc = PC_RESET_PORT(5);
               goto idt_tsi57x_pc_reset_port_exit;
            };
         };
       };

       if (in_parms->reset_lp) {
          rc = reset_tsi57x_lp( dev_info, port_num, &out_parms->imp_rc );
          if (RIO_SUCCESS != rc) {
             goto idt_tsi57x_pc_reset_port_exit;
          }

          if (RIO_ALL_PORTS != dep_port_num) {
             rc = reset_tsi57x_lp( dev_info, dep_port_num, &out_parms->imp_rc );
             if (RIO_SUCCESS != rc) {
                goto idt_tsi57x_pc_reset_port_exit;
             }
          }
       }

       /* Now, reset the port. 
        * Blow away the configuration by power cycling the port.
        */
       rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].pwr_mac_num*2), &dloop );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(6);
          goto idt_tsi57x_pc_reset_port_exit;
       };

       pwrdwn = dloop | sw_pmr[port_num].pwr_down_mask;

       rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].pwr_mac_num*2), pwrdwn );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(7);
          goto idt_tsi57x_pc_reset_port_exit;
       };

       rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[port_num].pwr_mac_num*2), dloop );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(8);
          goto idt_tsi57x_pc_reset_port_exit;
       };

       /* If we preserved state, restore the state now. */
        rc = restore_regs_from_powerup( dev_info, port_num, &saved_regs[0], in_parms->preserve_config );
        if (RIO_SUCCESS != rc) {
            out_parms->imp_rc = PC_RESET_PORT(9);
            goto idt_tsi57x_pc_reset_port_exit;
        };
        if (RIO_ALL_PORTS != dep_port_num) {
            rc = restore_regs_from_powerup( dev_info, dep_port_num, &saved_regs[1], in_parms->preserve_config );
            if (RIO_SUCCESS != rc) {
            out_parms->imp_rc = PC_RESET_PORT(0xA);
            };
        };
    };

idt_tsi57x_pc_reset_port_exit:
    return rc;
}

uint32_t idt_tsi57x_pc_reset_link_partner(
    DAR_DEV_INFO_t                   *dev_info, 
    idt_pc_reset_link_partner_in_t   *in_parms, 
    idt_pc_reset_link_partner_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;;

    out_parms->imp_rc = RIO_SUCCESS;

    if (TSI57X_NUM_PORTS(dev_info) <= in_parms->port_num)
    {
       out_parms->imp_rc = PC_RESET_LP(1);
       goto idt_tsi57x_pc_reset_link_partner_exit;
    }

    rc = reset_tsi57x_lp( dev_info, in_parms->port_num, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_pc_reset_link_partner_exit;
    };

    if (in_parms->resync_ackids) {
        rc = DARRegWrite( dev_info, Tsi578_SPX_ACKID_STAT(in_parms->port_num), 
                                    Tsi578_SPX_ACKID_STAT_CLR_PKTS );  
       if (RIO_SUCCESS != rc) 
          out_parms->imp_rc = PC_RESET_LP(2);
    }
    
idt_tsi57x_pc_reset_link_partner_exit:
    return rc;
}

#define PC_CLR_ERRS(x) (PC_CLR_ERRS_0+x)

uint32_t idt_tsi57x_pc_clr_errs  ( DAR_DEV_INFO_t       *dev_info, 
                               idt_pc_clr_errs_in_t   *in_parms, 
                               idt_pc_clr_errs_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t port_idx, lp_port_num; 
    CS_field_t magic_cs;
    CS_bytes_t magic_cs_bytes;
    uint32_t cs_reg_val;
    uint32_t dlay;
    uint32_t lr_cmd = STYPE1_LREQ_CMD_PORT_STAT; /* Command for link-request/input-status. */
    uint32_t lresp = 0;
    uint32_t ackid_stat;
    uint32_t err_stat;
    idt_pc_get_status_in_t  status_in;
    idt_pc_get_status_out_t status_out;

    out_parms->imp_rc = RIO_SUCCESS;

    if (TSI57X_NUM_PORTS(dev_info) <= in_parms->port_num)
    {
       out_parms->imp_rc = PC_CLR_ERRS(1);
       goto idt_tsi57x_pc_clr_errs_exit;
    }
    
    if (in_parms->clr_lp_port_err) 
    {
       if (!in_parms->num_lp_ports                  ||
          ( in_parms->num_lp_ports > IDT_MAX_PORTS) ||
          (NULL == in_parms->lp_dev_info          )) 
       {
          out_parms->imp_rc = PC_CLR_ERRS(2);
          goto idt_tsi57x_pc_clr_errs_exit;
       };
       for (port_idx = 0; port_idx < in_parms->num_lp_ports; port_idx++) 
       {
          if (in_parms->lp_port_list[port_idx] >= TSI57X_NUM_PORTS(in_parms->lp_dev_info) ) 
          {
             out_parms->imp_rc = PC_CLR_ERRS(3);
             goto idt_tsi57x_pc_clr_errs_exit;
          };
       }
    }

    // If the port is not PORT_OK, it is not possible to clear error conditions.
    status_in.ptl.num_ports   = 1;
    status_in.ptl.pnums[0] = in_parms->port_num;
    rc = idt_tsi57x_pc_get_status( dev_info, &status_in, &status_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = status_out.imp_rc;
       goto idt_tsi57x_pc_clr_errs_exit;
    }

    if ((status_out.num_ports != 1) || (!status_out.ps[0].port_ok)) {
       rc = RIO_ERR_ERRS_NOT_CL;  
       out_parms->imp_rc = PC_CLR_ERRS(4);
       goto idt_tsi57x_pc_clr_errs_exit;
    }
    
    /* First, ensure input/output error-stopped conditions are cleared 
       on our port and the link partner by sending the magic control symbol.
    */

    magic_cs.cs_size   = cs_small;
    magic_cs.cs_t0     = stype0_pna;
    magic_cs.parm_0    = 0;
    magic_cs.parm_1    = PNA_GENERAL_ERR;
    magic_cs.cs_t1     = stype1_lreq;
    magic_cs.cs_t1_cmd = 0;

    CS_fields_to_bytes( &magic_cs, &magic_cs_bytes );

    cs_reg_val = ( (uint32_t)(magic_cs_bytes.cs_bytes[1]) << 24) ||
                 ( (uint32_t)(magic_cs_bytes.cs_bytes[2]) << 16) ||
                 (((uint32_t)(magic_cs_bytes.cs_bytes[3]) <<  8) & Tsi578_SPX_CS_TX_CMD);

    rc = DARRegWrite( dev_info, Tsi578_SPX_CS_TX(in_parms->port_num), cs_reg_val );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x10);
       goto idt_tsi57x_pc_clr_errs_exit;
    }

    //  Delay while waiting for control symbol magic to complete.
    //  Should be > 5 usec, just to allow some margin for different link speeds
    //  and link partners.

    DAR_WaitSec( 5000, 0 );

    // Prepare to clear any port-err conditions that may exist on this port.
    //     Send link-request/input-status to learn what link partners
    //     next expected ackID is.
    rc = DARRegWrite( dev_info, Tsi578_SPX_LM_REQ(in_parms->port_num), lr_cmd );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x11); 
       goto idt_tsi57x_pc_clr_errs_exit;
    };

    // Poll until we get a response.  Fail if no response is received.

    dlay = 10;
    while (!(lresp & Tsi578_SPX_LM_RESP_RESP_VLD) && dlay) {
       dlay--;
       rc = DARRegRead( dev_info, Tsi578_SPX_LM_RESP(in_parms->port_num), &lresp );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_CLR_ERRS(0x12); 
          goto idt_tsi57x_pc_clr_errs_exit;
       }
    }

    if (!(lresp & Tsi578_SPX_LM_RESP_RESP_VLD)) {
       rc = RIO_ERR_NOT_EXPECTED_RETURN_VALUE;
       out_parms->imp_rc = PC_CLR_ERRS(0x13); 
       goto idt_tsi57x_pc_clr_errs_exit;
    }

    // We have valid ackID information.  Update our local ackID status.
    // The act of updating our local ackID status will clear a local 
    // port-err condition.
    
    rc = DARRegRead( dev_info, Tsi578_SPX_ACKID_STAT(in_parms->port_num),
                    &ackid_stat );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x14); 
       goto idt_tsi57x_pc_clr_errs_exit;
    }

    lresp = (lresp & Tsi578_SPX_LM_RESP_ACK_ID_STAT) >> 5;
    ackid_stat = ackid_stat & Tsi578_SPX_ACKID_STAT_INBOUND;
    ackid_stat = ackid_stat | lresp | (lresp << 8);

    rc = DARRegWrite( dev_info, Tsi578_SPX_ACKID_STAT(in_parms->port_num),
                      ackid_stat );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x15); 
       goto idt_tsi57x_pc_clr_errs_exit;
    }

    if (in_parms->clr_lp_port_err) 
    {
       /* Update the link partners ackID status register to match that of
              the Tsi57x.
          Increment the expected inbound ackID to reflect the reception of
              the maintenance write packet.
          Link partners inbound value should be our outbound value, plus 1.
          Link partners outbound value should be our inbound value.
       */
       lresp = ( ackid_stat + 1 ) & Tsi578_SPX_ACKID_STAT_OUTBOUND;
       ackid_stat = (ackid_stat & Tsi578_SPX_ACKID_STAT_INBOUND) >> 24;
       ackid_stat |= (ackid_stat << 8) | (lresp << 24);

       for (port_idx = 0; port_idx < in_parms->num_lp_ports; port_idx++) 
       {
          lp_port_num = in_parms->lp_port_list[port_idx];
          rc = DARRegWrite( in_parms->lp_dev_info,
			RIO_SPX_ACKID_ST(in_parms->lp_dev_info->extFPtrForPort,
			RIO_EFB_T_FIXME, lp_port_num),
                            ackid_stat );
          if (RIO_SUCCESS != rc) 
          {
             // The write can fail because the incorrect port was selected.
             //    Call ourselves to clear errors on the local port, and then
             //    try the next link partner port.
             idt_pc_clr_errs_in_t  temp_in;
             idt_pc_clr_errs_out_t temp_out;
             uint32_t                temp_rc;

             memcpy(&temp_in, in_parms, sizeof(idt_pc_clr_errs_in_t));
             temp_in.clr_lp_port_err = false;

             temp_rc = idt_tsi57x_pc_clr_errs( dev_info, &temp_in, &temp_out );
             if (RIO_SUCCESS != temp_rc) 
             {
                rc = temp_rc;
                out_parms->imp_rc = PC_CLR_ERRS(0x16); 
                goto idt_tsi57x_pc_clr_errs_exit;
             };
          };
       };
    };

    // Lastly, clear physical layer error status indications for the port.
    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(in_parms->port_num), &err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_pc_clr_errs_exit;

    rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_uint32_t(in_parms->port_num), err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_pc_clr_errs_exit;

    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(in_parms->port_num), &err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_pc_clr_errs_exit;

    if (err_stat & (Tsi578_SPX_ERR_uint32_t_PORT_ERR        | 
                    Tsi578_SPX_ERR_uint32_t_INPUT_ERR_STOP  |  
                    Tsi578_SPX_ERR_uint32_t_OUTPUT_ERR_STOP |  
                    Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL     ) ) 
    {
       rc = RIO_ERR_ERRS_NOT_CL;  
       out_parms->imp_rc = PC_CLR_ERRS(0x20);
       goto idt_tsi57x_pc_clr_errs_exit;
    }; 
                         
idt_tsi57x_pc_clr_errs_exit:
    return rc;
}

void tsi57x_rst_policy_vals( idt_pc_rst_handling  rst_policy_in,
                             uint32_t              *spx_mode_val,
                             idt_pc_rst_handling *rst_policy_out )
{
    *spx_mode_val   = 0;
    *rst_policy_out = rst_policy_in;

    // Tsi57x devices support
    // - reset the device
    // - interrupt on reset 
    // - ignore 
    // Rst_pw and Rst_port both get translated to "ignore".
    if (idt_pc_rst_device == rst_policy_in) {
        *spx_mode_val = Tsi578_SPX_MODE_SELF_RST;
    } else {
        if (idt_pc_rst_int == rst_policy_in) {
            *spx_mode_val = Tsi578_SPX_MODE_RCS_INT_EN;
        } else {
            *rst_policy_out = idt_pc_rst_ignore;
        }
    }
}

#define UPDATE_RESET(x) (EM_UPDATE_RESET_0+x)

uint32_t update_reset_policy( DAR_DEV_INFO_t          *dev_info    , 
                            uint8_t                    pnum        ,
                            uint32_t                   spx_mode_val,
                            uint32_t                  *imp_rc      )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t ctl_indep;

    if (pnum >= TSI57X_NUM_PORTS(dev_info)) {
       *imp_rc = UPDATE_RESET(1);
       goto update_reset_policy_exit;
    }

    rc = DARRegWrite( dev_info, Tsi578_SPX_MODE(pnum), spx_mode_val );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(2);
       goto update_reset_policy_exit;
    }

    rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &ctl_indep );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(3);
       goto update_reset_policy_exit;
    }

    // Make a "note" of whether or not reset interrupts should be 
    // enabled in SPx_CTL_INDEP[30], used by error management routines
    // to know whether or not to enable/disable reset interrupts.
    if (spx_mode_val & Tsi578_SPX_MODE_RCS_INT_EN) {
       ctl_indep |=  Tsi578_SPX_CTL_INDEP_RSVD1;
    } else {
       ctl_indep &= ~Tsi578_SPX_CTL_INDEP_RSVD1;
    };

    rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), ctl_indep );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(4);
       goto update_reset_policy_exit;
    }

update_reset_policy_exit:
   return rc;
};

#define PC_SECURE_PORT(x) (PC_SECURE_PORT_0+x)

uint32_t idt_tsi57x_pc_secure_port  ( DAR_DEV_INFO_t          *dev_info, 
                                  idt_pc_secure_port_in_t   *in_parms, 
                                  idt_pc_secure_port_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t pnum;
    uint32_t port_mode = 0;
    uint32_t port_mode_mod, port_mode_mask, port_ctl, glob_int_en, port_idx;
	struct DAR_ptl good_ptl;

    out_parms->imp_rc = RIO_SUCCESS;

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if ((rc != RIO_SUCCESS) || !good_ptl.num_ports) {
        out_parms->imp_rc = PC_SECURE_PORT(1);
        return rc;
    }

    if (in_parms->rst >= idt_pc_rst_last)
    {
    	rc = RIO_ERR_INVALID_PARAMETER;
        out_parms->imp_rc = PC_SECURE_PORT(2);
        return rc;
    }

	for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++) {
		port_mode_mask = ~(Tsi578_SPX_MODE_MCS_INT_EN | Tsi578_SPX_MODE_RCS_INT_EN  | Tsi578_SPX_MODE_SELF_RST);
		pnum = good_ptl.pnums[port_idx];

		rc = DARRegRead( dev_info, Tsi578_SPX_MODE(pnum), &port_mode );
		if (RIO_SUCCESS != rc) {
		   out_parms->imp_rc = PC_SECURE_PORT(3);
		   goto idt_tsi57x_pc_secure_port_exit;
		}
 
		rc = DARRegRead( dev_info, Tsi578_SPX_CTL(pnum), &port_ctl );
		if (RIO_SUCCESS != rc) {
		   out_parms->imp_rc = PC_SECURE_PORT(4);
		   goto idt_tsi57x_pc_secure_port_exit;
		}

		tsi57x_rst_policy_vals( in_parms->rst, &port_mode_mod, &out_parms->rst);

		if (in_parms->MECS_acceptance) {
		   port_mode_mod |=   Tsi578_SPX_MODE_MCS_INT_EN;
		}

		if (in_parms->MECS_participant) {
		   port_ctl |=  Tsi578_SPX_CTL_MCS_EN;
		} else {
		   port_ctl &= ~Tsi578_SPX_CTL_MCS_EN;
		}

		out_parms->bc_mtc_pkts_allowed = true;
		out_parms->MECS_acceptance     = true;
		out_parms->MECS_participant    = in_parms->MECS_participant;

		port_mode = (port_mode & port_mode_mask) | port_mode_mod;

		rc = update_reset_policy( dev_info, pnum, port_mode, &out_parms->imp_rc );
		if (RIO_SUCCESS != rc) {
		   goto idt_tsi57x_pc_secure_port_exit;
		}

		rc = DARRegWrite( dev_info, Tsi578_SPX_CTL(pnum), port_ctl );
		if (RIO_SUCCESS != rc) {
		   out_parms->imp_rc = PC_SECURE_PORT(6);
		}
	}

    // Program reset interrupt control at the top level 
    if ((port_mode & Tsi578_SPX_MODE_MCS_INT_EN) ||
        (port_mode & Tsi578_SPX_MODE_RCS_INT_EN) ) {
       rc = DARRegRead( dev_info, Tsi578_GLOB_INT_ENABLE, &glob_int_en );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_SECURE_PORT(7);
          goto idt_tsi57x_pc_secure_port_exit;
       };
   
       if (port_mode & Tsi578_SPX_MODE_RCS_INT_EN) {
          glob_int_en |=  Tsi578_GLOB_INT_ENABLE_RCS_EN;
       };
   
       if (port_mode & Tsi578_SPX_MODE_MCS_INT_EN) {
          glob_int_en |=  Tsi578_GLOB_INT_ENABLE_MCS_EN;
       };
     
       rc = DARRegWrite( dev_info, Tsi578_GLOB_INT_ENABLE, glob_int_en );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_SECURE_PORT(8);
       };
    };
idt_tsi57x_pc_secure_port_exit:
    return rc;
}

#define PC_DEV_RESET_CONFIG(x) (PC_DEV_RESET_CONFIG_0+x)

uint32_t idt_tsi57x_pc_dev_reset_config(
    DAR_DEV_INFO_t                 *dev_info, 
    idt_pc_dev_reset_config_in_t   *in_parms, 
    idt_pc_dev_reset_config_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t port_num;
    uint32_t port_mode, port_mode_mask, port_mode_value, glob_int_en;

    out_parms->rst = in_parms->rst;
    out_parms->imp_rc  = RIO_SUCCESS;

    if ((uint8_t)(out_parms->rst) >= (uint8_t)(idt_pc_rst_last)) {
       out_parms->imp_rc = PC_DEV_RESET_CONFIG(1);
       goto idt_tsi57x_pc_dev_reset_config_exit;
    };

    port_mode_mask = ~(Tsi578_SPX_MODE_SELF_RST  | 
                       Tsi578_SPX_MODE_RCS_INT_EN);

    tsi57x_rst_policy_vals( in_parms->rst, &port_mode_value, &out_parms->rst );
    
    // Configure reset interrupt and reset response in SPx_MODE registers
    for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num++) {
        rc = DARRegRead( dev_info, Tsi578_SPX_MODE(port_num), &port_mode );
        if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = PC_DEV_RESET_CONFIG(2);
           goto idt_tsi57x_pc_dev_reset_config_exit;
        };

        port_mode = (port_mode & port_mode_mask) | port_mode_value;

        rc = update_reset_policy( dev_info, port_num, port_mode, &out_parms->imp_rc );
        if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = PC_DEV_RESET_CONFIG(3);
           goto idt_tsi57x_pc_dev_reset_config_exit;
        };
       
        rc = DARRegRead( dev_info, Tsi578_SPX_MODE(port_num), &port_mode );
        if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = PC_DEV_RESET_CONFIG(2);
           goto idt_tsi57x_pc_dev_reset_config_exit;
        };
    }

    // Program reset interrupt control at the top level 
    rc = DARRegRead( dev_info, Tsi578_GLOB_INT_ENABLE, &glob_int_en );
    if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = PC_DEV_RESET_CONFIG(4);
           goto idt_tsi57x_pc_dev_reset_config_exit;
    };

    if (idt_pc_rst_int == out_parms->rst) {
       glob_int_en |=  Tsi578_GLOB_INT_ENABLE_RCS_EN;
    } else {
       glob_int_en &= ~Tsi578_GLOB_INT_ENABLE_RCS_EN;
    };
  
    rc = DARRegWrite( dev_info, Tsi578_GLOB_INT_ENABLE, glob_int_en );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_DEV_RESET_CONFIG(5);
    };

idt_tsi57x_pc_dev_reset_config_exit:
    return rc;
}

uint32_t idt_tsi57x_rt_set_changed ( DAR_DEV_INFO_t            *dev_info, 
                                   idt_rt_set_changed_in_t   *in_parms, 
                                   idt_rt_set_changed_out_t  *out_parms );

uint32_t idt_tsi57x_rt_initialize  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_rt_initialize_in_t   *in_parms, 
                                 idt_rt_initialize_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t destID, spx_mode;
    uint32_t mc_idx;
    uint8_t  port, start_port, end_port;
    idt_rt_set_changed_in_t  all_in;
    idt_rt_set_changed_out_t all_out;
    idt_rt_state_t           rt_state; 

    // Validate parameters
    
    if (  ((in_parms->default_route      >= TSI57X_NUM_PORTS(dev_info))  &&
         !( (IDT_DSF_RT_USE_DEFAULT_ROUTE == in_parms->default_route) ||
            (IDT_DSF_RT_NO_ROUTE          == in_parms->default_route))) )
    {
        out_parms->imp_rc = RT_INITIALIZE(1);
        goto idt_tsi57x_rt_initialize_exit;
    }

    if ( (in_parms->default_route_table_port >= TSI57X_NUM_PORTS(dev_info)) &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->default_route_table_port)  )
    {
        out_parms->imp_rc = RT_INITIALIZE(2);
        goto idt_tsi57x_rt_initialize_exit;
    }

    if ( (in_parms->set_on_port >= TSI57X_NUM_PORTS(dev_info)  )  &&
        !(RIO_ALL_PORTS         == in_parms->set_on_port))
    {
        out_parms->imp_rc = RT_INITIALIZE(3);
        goto idt_tsi57x_rt_initialize_exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    all_in.set_on_port = in_parms->set_on_port;

    if (NULL == in_parms->rt)
       all_in.rt = &rt_state;
    else
       all_in.rt = in_parms->rt;

    all_in.rt->default_route = in_parms->default_route;

    // Ensure routing tables are operating in hierarchical mode

    if (in_parms->update_hw) {
       if (RIO_ALL_PORTS == in_parms->set_on_port) {
          start_port = 0;
          end_port   = TSI57X_NUM_PORTS(dev_info) - 1;
       } else {
          start_port = end_port = in_parms->set_on_port;
       };

       for (port = start_port; port <= end_port; port++) {
          rc = DARRegRead( dev_info, Tsi578_SPX_MODE(port), &spx_mode );
          if (RIO_SUCCESS != rc) {
               out_parms->imp_rc = RT_INITIALIZE(4);
               goto idt_tsi57x_rt_initialize_exit;
          };

          spx_mode &= ~Tsi578_SPX_MODE_LUT_512;

          rc = DARRegWrite( dev_info, Tsi578_SPX_MODE(port), spx_mode );
          if (RIO_SUCCESS != rc) {
               out_parms->imp_rc = RT_INITIALIZE(5);
               goto idt_tsi57x_rt_initialize_exit;
          };
       };
    };

    // Configure initialization of all of the routing table entries
    for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
    {
        all_in.rt->dev_table[destID].changed = true ;
        all_in.rt->dev_table[destID].rte_val = in_parms->default_route_table_port;
    };
    
    all_in.rt->dom_table[0].changed = true ;
    all_in.rt->dom_table[0].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;

    for (destID = 1; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
    {
        all_in.rt->dom_table[destID].changed = true ;
        all_in.rt->dom_table[destID].rte_val = in_parms->default_route_table_port;
    };
    
    // Configure initialization of multicast masks and associations as necessary. 
    for (mc_idx = 0; mc_idx < IDT_DSF_MAX_MC_MASK; mc_idx++) 
    {
       all_in.rt->mc_masks[mc_idx].mc_destID = 0;
       all_in.rt->mc_masks[mc_idx].tt        = tt_dev8;
       all_in.rt->mc_masks[mc_idx].mc_mask   = 0;
       all_in.rt->mc_masks[mc_idx].in_use    = false;
       all_in.rt->mc_masks[mc_idx].allocd    = false;
       // Only change multicast masks that exist AND
       // when all ports are being initialized.
       if ( (mc_idx < Tsi578_MAX_MC_MASKS )  &&
            (RIO_ALL_PORTS == in_parms->set_on_port) ) 
       {
          all_in.rt->mc_masks[mc_idx].changed   = true ;
       } else {
          all_in.rt->mc_masks[mc_idx].changed  = false;
       };
    };

    if (in_parms->update_hw)
       rc = idt_tsi57x_rt_set_changed(dev_info, &all_in, &all_out );
    else
       rc = RIO_SUCCESS;
     
    if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = all_out.imp_rc;
    }

    idt_tsi57x_rt_initialize_exit:

    return rc;
}

uint32_t idt_check_port_for_discard( DAR_DEV_INFO_t     *dev_info, 
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

   if (TSI57X_NUM_PORTS(dev_info) <= port) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(1);
      goto idt_check_port_for_discard_exit;
   };

   cfg_in.ptl.num_ports = 1;
   cfg_in.ptl.pnums[0] = port;
   rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(2);
      goto idt_check_port_for_discard_exit;
   };

   stat_in.ptl.num_ports = 1;
   stat_in.ptl.pnums[0] = port;
   rc = idt_tsi57x_pc_get_status( dev_info, &stat_in, &stat_out );
   if (RIO_SUCCESS != rc) {
      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
      out_parms->imp_rc = RT_PROBE(3);
      goto idt_check_port_for_discard_exit;
   };

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
             };
          } else {
             if (stat_out.ps[0].port_error) {
                out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_fail:idt_rt_disc_port_fail;
             } else {
                if (cfg_out.pc[0].port_lockout) {
                   out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_lkout_or_dis:
                                                         idt_rt_disc_port_lkout_or_dis;
                } else {
                   rc = DARRegRead( dev_info, Tsi578_SPX_CTL(port), &ctlData );
                   if (RIO_SUCCESS != rc) {
                      out_parms->reason_for_discard = idt_rt_disc_probe_abort;
                      out_parms->imp_rc = RT_PROBE(4);
                      goto idt_check_port_for_discard_exit;
                   };
            
                  if ( (RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) != 
                      ((RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN) & ctlData)) {
                     out_parms->reason_for_discard = (dflt_rt)?idt_rt_disc_dflt_pt_in_out_dis:idt_rt_disc_port_in_out_dis;
                  };
               }
            }
         }
      }
   }

   rc = RIO_SUCCESS;

idt_check_port_for_discard_exit:

    if (idt_rt_disc_not != out_parms->reason_for_discard)
       out_parms->valid_route = false;

    return rc;
}

uint32_t idt_tsi57x_rt_probe     ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_probe_in_t         *in_parms, 
                                 idt_rt_probe_out_t        *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t bit;

    out_parms->imp_rc                 = RIO_SUCCESS;
    out_parms->valid_route            = false;
    out_parms->routing_table_value    = RIO_ALL_PORTS;
    out_parms->filter_function_active = false; /* not supported on Tsi */
    out_parms->trace_function_active  = false; /* not supported on Tsi */
    out_parms->time_to_live_active    = false; /* not supported on Tsi */
    for (bit = 0; bit < TSI57X_NUM_PORTS(dev_info); bit++)
        out_parms->mcast_ports[bit] = false;
    out_parms->reason_for_discard     = idt_rt_disc_probe_abort;

    if (   ((TSI57X_NUM_PORTS(dev_info) <= in_parms->probe_on_port) &&
            (RIO_ALL_PORTS       != in_parms->probe_on_port))  ||
           ( NULL                == in_parms->rt           ) ) {
       out_parms->imp_rc = RT_PROBE(0x11);
       goto idt_tsi57x_rt_probe_exit;
    }
        
    rc = RIO_SUCCESS;

    // Note, no failure possible...
    check_multicast_routing( dev_info, in_parms, out_parms );

    /* Done if hit in multicast masks. */
    if (RIO_ALL_PORTS != out_parms->routing_table_value) 
       goto idt_tsi57x_rt_probe_exit;

    /*  Determine routing table value for the specified destination ID.
     *  If out_parms->valid_route is true 
     *  the valid values for out_parms->routing_table_value are
     *  - a valid port number, OR
     *  - IDT_DSF_RT_USE_DEFAULT_ROUTE
     *  When out_parms->routing_table_value is IDT_DSF_RT_USE_DEFAULT_ROUTE, the
     *  default route is a valid switch port number.
     */

    check_unicast_routing( dev_info, in_parms, out_parms );

    if (out_parms->valid_route) {
       rc = idt_check_port_for_discard( dev_info, in_parms, out_parms );
    }
    
idt_tsi57x_rt_probe_exit:
    return rc;
}

#define READ_MC_MASKS(x) (READ_MC_MASKS_0+x)

uint32_t tsi57x_read_mc_masks( DAR_DEV_INFO_t            *dev_info, 
                             idt_rt_state_t            *rt,
                             uint32_t                    *imp_rc  )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mask_idx;
   uint32_t reg_val;
   idt_rt_dealloc_mc_mask_in_t  d_in_parm;
   idt_rt_dealloc_mc_mask_out_t d_out_parm;

   d_in_parm.rt = rt;
   for (mask_idx = Tsi578_MAX_MC_MASKS; mask_idx < IDT_DSF_MAX_MC_MASK; mask_idx++ ) 
   {
      d_in_parm.mc_mask_rte = IDT_DSF_FIRST_MC_MASK + mask_idx;
      rc = IDT_DSF_rt_dealloc_mc_mask( dev_info, &d_in_parm, &d_out_parm );
      if (RIO_SUCCESS != rc) 
      {
         *imp_rc = d_out_parm.imp_rc;
         goto read_mc_masks_exit;
      };
   };

   for (mask_idx = 0; mask_idx < Tsi578_MAX_MC_MASKS; mask_idx++)  {
      rc = DARRegRead( dev_info, Tsi578_RIO_MC_IDX(mask_idx), &reg_val);
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_MC_MASKS(1);
         goto read_mc_masks_exit;
      };

      rt->mc_masks[mask_idx].allocd  = false;
      rt->mc_masks[mask_idx].changed = false;
      rt->mc_masks[mask_idx].tt = (reg_val & Tsi578_RIO_MC_IDX_LARGE_SYS) ?
                                   tt_dev16 : tt_dev8;
      rt->mc_masks[mask_idx].in_use = (reg_val & Tsi578_RIO_MC_IDX_MC_EN) ? 
                                   true : false;
      rt->mc_masks[mask_idx].mc_destID = (reg_val & ( (tt_dev16 == rt->mc_masks[mask_idx].tt) ? 
                                                       Tsi578_RIO_MC_IDX_MC_ID : (Tsi578_RIO_MC_IDX_MC_ID>>8)) );

      rc = DARRegRead( dev_info, Tsi578_RIO_MC_MSKX(mask_idx), &reg_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_MC_MASKS(2);
         goto read_mc_masks_exit;
      };

      if (reg_val & ~Tsi578_RIO_MC_MSKX_MC_MSK) {
         rc = RIO_ERR_RT_CORRUPTED;  
         *imp_rc = READ_MC_MASKS(3);
         goto read_mc_masks_exit;
      };

      rt->mc_masks[mask_idx].mc_mask = (reg_val & Tsi578_RIO_MC_MSKX_MC_MSK) >> 16;
   };

read_mc_masks_exit:
   return rc;
}

#define READ_RTE_ENTRIES(x) (READ_RTE_ENTRIES_0+x)

uint32_t tsi57x_read_rte_entries( DAR_DEV_INFO_t            *dev_info,
                                uint8_t                      pnum,
                                idt_rt_state_t            *rt,
                                uint32_t                    *imp_rc  )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t destID, idx_val, rte_val, base_reg;

   // Fill in default route value

   rc = DARRegRead( dev_info, Tsi578_RIO_LUT_ATTR, &rte_val );
   if (RIO_SUCCESS != rc) {
      *imp_rc = READ_RTE_ENTRIES(1);
      goto read_rte_entries_exit;
   };

   if ( HW_DFLT_RT == (rte_val & Tsi578_RIO_LUT_ATTR_DEFAULT_PORT))
      rt->default_route = IDT_DSF_RT_NO_ROUTE;
   else
      rt->default_route = rte_val & Tsi578_RIO_LUT_ATTR_DEFAULT_PORT;

   // Determine the base id for hierarchical mode.
   rc = DARRegRead( dev_info, Tsi578_SPX_ROUTE_BASE(pnum), &base_reg);
   if (RIO_SUCCESS != rc) {
      *imp_rc = READ_RTE_ENTRIES(10);
      goto read_rte_entries_exit;
   };

   // Read all of the domain routing table entries.
   //
   for (destID = 0; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
   {
      rt->dom_table[destID].changed = false;

      // Set deviceID, read routing table entry for deviceID
      idx_val = destID << 8;
      rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_DESTID(pnum), idx_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(7);
         goto read_rte_entries_exit;
      };

      rc = DARRegRead( dev_info, Tsi578_SPX_ROUTE_CFG_PORT(pnum), &rte_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(8);
         goto read_rte_entries_exit;
      }

      if (HW_DFLT_RT == rte_val) {
         rt->dom_table[destID].rte_val = IDT_DSF_RT_USE_DEFAULT_ROUTE;
      } else {
         rt->dom_table[destID].rte_val = (uint8_t)(rte_val & Tsi578_SPX_ROUTE_CFG_PORT_PORT);
      };
   };
   
   destID = (base_reg & Tsi578_SPX_ROUTE_BASE_BASE) >> 24;
   rt->dom_table[destID].rte_val = IDT_DSF_RT_USE_DEVICE_TABLE;
   base_reg = destID << 8;

   // Read all of the device routing table entries.
   //
   //
   for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
   {
      rt->dev_table[destID].changed = false;

      // Set deviceID, read routing table entry for deviceID,
      idx_val = base_reg + destID;

      rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_DESTID(pnum), idx_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(3);
         goto read_rte_entries_exit;
      };

      rc = DARRegRead( dev_info, Tsi578_SPX_ROUTE_CFG_PORT(pnum), &rte_val );
      if (RIO_SUCCESS != rc) {
         *imp_rc = READ_RTE_ENTRIES(4);
         goto read_rte_entries_exit;
      }

      if (HW_DFLT_RT == rte_val) {
         rt->dev_table[destID].rte_val = IDT_DSF_RT_USE_DEFAULT_ROUTE;
      } else {
         rt->dev_table[destID].rte_val = (uint8_t)(rte_val & Tsi578_SPX_ROUTE_CFG_PORT_PORT);
      };
   };
   
read_rte_entries_exit:
   return rc;
}

#define RT_PROBE_ALL(x) (RT_PROBE_ALL_0+x)

uint32_t idt_tsi57x_rt_probe_all  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_rt_probe_all_in_t     *in_parms, 
                                 idt_rt_probe_all_out_t    *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  probe_port;

    out_parms->imp_rc = RIO_SUCCESS;
    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->probe_on_port ) && 
           ( in_parms->probe_on_port >= TSI57X_NUM_PORTS(dev_info)    ) ) ||
         ( NULL == in_parms->rt) ) 
    {
        out_parms->imp_rc = RT_PROBE_ALL(1);
        goto idt_tsi57x_rt_probe_all_exit;
    }

    probe_port = (RIO_ALL_PORTS == in_parms->probe_on_port)?0:in_parms->probe_on_port;

    rc = tsi57x_read_mc_masks( dev_info, in_parms->rt, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc)
       goto idt_tsi57x_rt_probe_all_exit;
    
    rc = tsi57x_read_rte_entries( dev_info, probe_port, in_parms->rt, &out_parms->imp_rc );

idt_tsi57x_rt_probe_all_exit:
    return rc;
}

#define ALL_ENTRIES true
#define CHG_ENTRIES false
#define PROGRAM_RTE_ENTRIES(x) (PROGRAM_RTE_ENTRIES_0+x)

uint32_t program_rte_entries( DAR_DEV_INFO_t            *dev_info,
                            idt_rt_state_t            *rt,
                            uint8_t                      pnum,
                            bool                       prog_all, // Use ALL_ENTRIES/CHG_ENTRIES
                            uint32_t                    *imp_rc  )  
{

   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t destID, baseID = 0;
   uint32_t rte_val, idx_val;
   bool   set_base = false;
   uint8_t  port, start_port, end_port;

   if (RIO_ALL_PORTS == pnum) {
      start_port = 0;
      end_port   = TSI57X_NUM_PORTS(dev_info) - 1;
   } else {
      start_port = end_port = pnum;
   };

   // Set the default route output port
   
   if ( IDT_DSF_RT_NO_ROUTE  == rt->default_route )
      rte_val = HW_DFLT_RT & Tsi578_RIO_LUT_ATTR_DEFAULT_PORT;
   else
      rte_val = rt->default_route & Tsi578_RIO_LUT_ATTR_DEFAULT_PORT;

   rc = DARRegWrite( dev_info, Tsi578_RIO_LUT_ATTR, rte_val );
   if (RIO_SUCCESS != rc) {
      *imp_rc = PROGRAM_RTE_ENTRIES(1);
      goto program_rte_entries_exit;
   };

   // Find base ID, and set it.
   for (destID = 0; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
   {
      idx_val = destID << 8;
      rte_val = rt->dom_table[destID].rte_val;
      if (IDT_DSF_RT_USE_DEVICE_TABLE == rte_val) {
         if (set_base) {
               rc = RIO_ERR_INVALID_PARAMETER;
               *imp_rc = PROGRAM_RTE_ENTRIES(2);
               goto program_rte_entries_exit;
         } else {
               set_base = true;
               baseID   = idx_val;
         }
      };
   };

   for (port = start_port; port <= end_port; port++) {
      rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_BASE(port), baseID << 16 );
      if (RIO_SUCCESS != rc) {
         *imp_rc = PROGRAM_RTE_ENTRIES(6);
         goto program_rte_entries_exit;
      };
   };

   // Set all of the domain routing table entries
   for (destID = 0; destID < IDT_DAR_RT_DOM_TABLE_SIZE; destID++)
   {
      if (prog_all || rt->dom_table[destID].changed ) {
         idx_val = destID << 8;
         rte_val = rt->dom_table[destID].rte_val;

         if (IDT_DSF_RT_USE_DEVICE_TABLE != rte_val) {
            if (IDT_DSF_RT_USE_DEFAULT_ROUTE == rte_val) {
               rte_val = HW_DFLT_RT;
            } else {
               if (IDT_DSF_RT_NO_ROUTE == rte_val) {
                  rte_val = HW_DFLT_RT;
                  idx_val |= Tsi578_SPX_ROUTE_CFG_DESTID_PAR_INVERT;
               } else {
                  if (TSI57X_NUM_PORTS(dev_info) <= rte_val) {
                     rc = RIO_ERR_INVALID_PARAMETER;
                     *imp_rc = PROGRAM_RTE_ENTRIES(3);
                     goto program_rte_entries_exit;
                  };
               };
            };
            for (port = start_port; port <= end_port; port++) {
               rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_DESTID(port), idx_val );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = PROGRAM_RTE_ENTRIES(4);
                  goto program_rte_entries_exit;
               };
   
               rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_PORT(port), rte_val );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = PROGRAM_RTE_ENTRIES(5);
                  goto program_rte_entries_exit;
               };
            };
         };
         rt->dom_table[destID].changed = false;
     }
   }

   // Set all of the device routing table entries
   for (destID = 0; destID < IDT_DAR_RT_DEV_TABLE_SIZE; destID++)
   {
      if (prog_all || rt->dev_table[destID].changed ) {
         idx_val = baseID + destID;
         rte_val = rt->dev_table[destID].rte_val;
         if (IDT_DSF_RT_USE_DEFAULT_ROUTE == rte_val) {
            rte_val = HW_DFLT_RT;
         } else {
            if (IDT_DSF_RT_NO_ROUTE == rte_val) {
               rte_val = HW_DFLT_RT;
               idx_val |= Tsi578_SPX_ROUTE_CFG_DESTID_PAR_INVERT;
            } else {
               if ((IDT_DSF_RT_USE_DEVICE_TABLE == rte_val) ||
                   (TSI57X_NUM_PORTS(dev_info)        <= rte_val)) {
                  rc = RIO_ERR_INVALID_PARAMETER;
                  *imp_rc = PROGRAM_RTE_ENTRIES(7);
                  goto program_rte_entries_exit;
               };
            };
         };

         for (port = start_port; port <= end_port; port++) {
            rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_DESTID(port), idx_val );
            if (RIO_SUCCESS != rc) {
               *imp_rc = PROGRAM_RTE_ENTRIES(8);
               goto program_rte_entries_exit;
            };

            rc = DARRegWrite( dev_info, Tsi578_SPX_ROUTE_CFG_PORT(port), rte_val );
            if (RIO_SUCCESS != rc) {
               *imp_rc = PROGRAM_RTE_ENTRIES(9);
               goto program_rte_entries_exit;
            }
         }
         rt->dev_table[destID].changed = false;
      }
   }
   
   rc      = RIO_SUCCESS;
   *imp_rc = RIO_SUCCESS;

program_rte_entries_exit:

   return rc;
}


#define ALL_MASKS true
#define CHG_MASKS false
#define PROGRAM_MC_MASKS(x) (PROGRAM_MC_MASKS_0+x)

uint32_t program_mc_masks( DAR_DEV_INFO_t            *dev_info, 
                         idt_rt_state_t            *rt,
                         bool                      prog_all,  // Use ALL_MASKS or CHG_MASKS
                         uint32_t                    *imp_rc  )  
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mask_idx;
   uint32_t reg_val;
   uint32_t invalid_mc_mask = ~(uint32_t)((1 << TSI57X_NUM_PORTS(dev_info)) - 1);

   for (mask_idx = Tsi578_MAX_MC_MASKS; mask_idx < IDT_DSF_MAX_MC_MASK; mask_idx++ ) {
      if (rt->mc_masks[mask_idx].in_use || rt->mc_masks[mask_idx].changed || rt->mc_masks[mask_idx].allocd)  {
         *imp_rc = PROGRAM_MC_MASKS(1);
         goto program_mc_masks_exit;
      };
   };

   for (mask_idx = 0; mask_idx < Tsi578_MAX_MC_MASKS; mask_idx++)  {
      if  (invalid_mc_mask & rt->mc_masks[mask_idx].mc_mask) {
         *imp_rc = PROGRAM_MC_MASKS(2);
         goto program_mc_masks_exit;
      };
      if (prog_all || rt->mc_masks[mask_idx].changed) {
         rt->mc_masks[mask_idx].changed = false;
         reg_val = (uint32_t)(rt->mc_masks[mask_idx].mc_destID);
         reg_val |= (tt_dev16 == rt->mc_masks[mask_idx].tt    )?Tsi578_RIO_MC_IDX_LARGE_SYS:0;
         reg_val |= (            rt->mc_masks[mask_idx].in_use)?Tsi578_RIO_MC_IDX_MC_EN:0;
         rc = DARRegWrite( dev_info, Tsi578_RIO_MC_IDX(mask_idx), reg_val );
         if (RIO_SUCCESS != rc) {
            *imp_rc = PROGRAM_MC_MASKS(3);
            goto program_mc_masks_exit;
         }

         reg_val = (rt->mc_masks[mask_idx].mc_mask << 16) & Tsi578_RIO_MC_MSKX_MC_MSK;
         rc = DARRegWrite( dev_info, Tsi578_RIO_MC_MSKX(mask_idx), reg_val );
         if (RIO_SUCCESS != rc) {
            *imp_rc = PROGRAM_MC_MASKS(4);
            goto program_mc_masks_exit;
         }
      };
   };

   rc      = RIO_SUCCESS;
   *imp_rc = RIO_SUCCESS;

program_mc_masks_exit:
   return rc;
}

uint32_t idt_tsi57x_rt_set_all   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_all_in_t       *in_parms, 
                                 idt_rt_set_all_out_t      *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;

    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->set_on_port ) && 
           ( in_parms->set_on_port >= TSI57X_NUM_PORTS(dev_info)    ) ) ||
         ( NULL == in_parms->rt) ) 
    {
        out_parms->imp_rc = RT_SET_ALL(1);
        goto idt_tsi57x_rt_set_all_exit;
    }

    if (( TSI57X_NUM_PORTS(dev_info) <= in_parms->rt->default_route) &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->rt->default_route))   {
        out_parms->imp_rc = RT_SET_ALL(2);
        goto idt_tsi57x_rt_set_all_exit;
    }

    rc = program_mc_masks( dev_info, in_parms->rt, ALL_MASKS, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_rt_set_all_exit;
    }

    rc = program_rte_entries( dev_info, in_parms->rt, in_parms->set_on_port, ALL_ENTRIES, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) 
       goto idt_tsi57x_rt_set_all_exit;

idt_tsi57x_rt_set_all_exit:

    return rc;
}

uint32_t idt_tsi57x_rt_set_changed   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_changed_in_t       *in_parms, 
                                 idt_rt_set_changed_out_t      *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;

    if ( ( ( (uint8_t)(RIO_ALL_PORTS) != in_parms->set_on_port ) && 
           ( in_parms->set_on_port >= TSI57X_NUM_PORTS(dev_info)    ) ) ||
         ( NULL == in_parms->rt) ) 
    {
        out_parms->imp_rc = RT_SET_CHANGED(1);
        goto idt_tsi57x_rt_set_changed_exit;
    }

    if (( TSI57X_NUM_PORTS(dev_info) <= in_parms->rt->default_route) &&
        !(IDT_DSF_RT_NO_ROUTE == in_parms->rt->default_route))   {
        out_parms->imp_rc = RT_SET_CHANGED(2);
        goto idt_tsi57x_rt_set_changed_exit;
    }

    out_parms->imp_rc = RIO_SUCCESS;
    rc = program_mc_masks( dev_info, in_parms->rt, CHG_MASKS, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_rt_set_changed_exit;
    }

    rc = program_rte_entries( dev_info, in_parms->rt, in_parms->set_on_port, CHG_ENTRIES, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) 
       goto idt_tsi57x_rt_set_changed_exit;

idt_tsi57x_rt_set_changed_exit:

    return rc;
};

uint32_t idt_tsi57x_rt_change_rte (
    DAR_DEV_INFO_t           *dev_info, 
    idt_rt_change_rte_in_t   *in_parms, 
    idt_rt_change_rte_out_t  *out_parms
)
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint16_t idx;

   out_parms->imp_rc = RIO_SUCCESS;

   if (NULL == in_parms->rt) {
      out_parms->imp_rc = RT_CHANGE_RTE(1);
      goto idt_tsi57x_rt_change_rte_exit;
   };

   // Validate rte_value 
   if ( (IDT_DSF_RT_USE_DEVICE_TABLE  != in_parms->rte_value) &&
        (IDT_DSF_RT_USE_DEFAULT_ROUTE != in_parms->rte_value) &&
        (IDT_DSF_RT_NO_ROUTE          != in_parms->rte_value) &&
        (in_parms->rte_value >= TSI57X_NUM_PORTS(dev_info))) {
      out_parms->imp_rc = RT_CHANGE_RTE(2);
      goto idt_tsi57x_rt_change_rte_exit;
   }

   if ( (IDT_DSF_RT_USE_DEVICE_TABLE  == in_parms->rte_value) && (!in_parms->dom_entry)) {
      out_parms->imp_rc = RT_CHANGE_RTE(3);
      goto idt_tsi57x_rt_change_rte_exit;
   };

   rc = RIO_SUCCESS;

   // If entry has not already been changed, see if it is being changed
   if (in_parms->dom_entry) {
      if (  !in_parms->rt->dom_table[in_parms->idx].changed ) {
         if (in_parms->rt->dom_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dom_table[in_parms->idx].changed = true;
      };
      in_parms->rt->dom_table[in_parms->idx].rte_val = in_parms->rte_value;

      // Since only one entry in the domain table can have a value of 
      // IDT_DSF_RT_USE_DEVICE_TABLE, if that entry is marked changed
      // then it is possible that another entry has this value.  Search
      // to clear all other entries with this value, and mark them changed.
      if (IDT_DSF_RT_USE_DEVICE_TABLE == in_parms->rte_value) {
         for (idx = 0; idx < IDT_DAR_RT_DOM_TABLE_SIZE; idx++) {
            if ((IDT_DSF_RT_USE_DEVICE_TABLE == in_parms->rt->dom_table[idx].rte_val) &&
                (idx                         != in_parms->idx                       )) {
               in_parms->rt->dom_table[idx].rte_val = IDT_DSF_RT_NO_ROUTE;
               in_parms->rt->dom_table[idx].changed = true;
            };
         };
      };
   } else {
      if (  !in_parms->rt->dev_table[in_parms->idx].changed ) {
         if (in_parms->rt->dev_table[in_parms->idx].rte_val  != in_parms->rte_value)
             in_parms->rt->dev_table[in_parms->idx].changed = true;
      };
      in_parms->rt->dev_table[in_parms->idx].rte_val = in_parms->rte_value;
   };

idt_tsi57x_rt_change_rte_exit:
   return rc;
}

uint32_t idt_tsi57x_rt_change_mc_mask (
    DAR_DEV_INFO_t               *dev_info, 
    idt_rt_change_mc_mask_in_t   *in_parms, 
    idt_rt_change_mc_mask_out_t  *out_parms
)
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t  mc_idx, chg_idx;
   uint32_t illegal_ports = ~((1 << IDT_MAX_PORTS) - 1);

   out_parms->imp_rc = RIO_SUCCESS;

   if ((NULL == in_parms->rt) || (NULL == dev_info)) {
      out_parms->imp_rc = CHANGE_MC_MASK(1);
      goto idt_tsi57x_rt_change_mc_mask_exit;
   }

   // Check destination ID value against tt, and that the multicast mask
   // does not select ports which do not exist on the Tsi57x device.
   if ( (in_parms->mc_info.mc_destID > IDT_LAST_DEV16_DESTID   ) ||
        ((in_parms->mc_info.mc_destID > IDT_LAST_DEV8_DESTID) &&
         (tt_dev8 == in_parms->mc_info.tt                     )) ||
        (in_parms->mc_info.mc_mask & illegal_ports             )  ) {
      out_parms->imp_rc = CHANGE_MC_MASK(2);
      goto idt_tsi57x_rt_change_mc_mask_exit;
   }

   // Check that the destination ID is not duplicated elsewhere in the
   // multicast table.

   chg_idx = in_parms->mc_mask_rte - IDT_DSF_FIRST_MC_MASK;

   for (mc_idx = 0; mc_idx < Tsi578_MAX_MC_MASKS; mc_idx++)  {
      if ((mc_idx != chg_idx)                                                      &&
          (in_parms->rt->mc_masks[mc_idx].mc_destID == in_parms->mc_info.mc_destID) &&
          (in_parms->rt->mc_masks[mc_idx].tt        == in_parms->mc_info.tt       )) {
         rc = RIO_ERR_ROUTE_ERROR;
         out_parms->imp_rc = CHANGE_MC_MASK(3);
         goto idt_tsi57x_rt_change_mc_mask_exit;
      }
   }

   rc = RIO_SUCCESS;

   // Allow requests to change masks not supported by TSI57x family
   // but there's nothing to do...
   if ((IDT_DSF_FIRST_MC_MASK + Tsi578_MAX_MC_MASKS) <= in_parms->mc_mask_rte)
      goto idt_tsi57x_rt_change_mc_mask_exit;

   // If entry has not already been changed, see if it is being changed
   if ( !in_parms->rt->mc_masks[chg_idx].changed ) {
      if ((in_parms->rt->mc_masks[chg_idx].mc_destID != in_parms->mc_info.mc_destID) ||
          (in_parms->rt->mc_masks[chg_idx].tt        != in_parms->mc_info.tt       ) ||
          (in_parms->rt->mc_masks[chg_idx].mc_mask   != in_parms->mc_info.mc_mask  ) ||
          (in_parms->rt->mc_masks[chg_idx].in_use    != in_parms->mc_info.in_use   ))  {
         in_parms->rt->mc_masks[chg_idx].changed = true;
      };
   };

   in_parms->rt->mc_masks[chg_idx].in_use    = in_parms->mc_info.in_use   ;
   in_parms->rt->mc_masks[chg_idx].mc_destID = in_parms->mc_info.mc_destID;
   in_parms->rt->mc_masks[chg_idx].tt        = in_parms->mc_info.tt       ;
   in_parms->rt->mc_masks[chg_idx].mc_mask   = in_parms->mc_info.mc_mask  ;

idt_tsi57x_rt_change_mc_mask_exit:
   return rc;
}

#define TSI57X_ALL_LOG_ERRS  ((uint32_t)(Tsi578_RIO_LOG_ERR_DET_EN_UNSUP_TRANS_EN | \
                                       Tsi578_RIO_LOG_ERR_DET_EN_ILL_RESP_EN    | \
                                       Tsi578_RIO_LOG_ERR_DET_EN_ILL_TRANS_EN   ))

#define EM_CFG_PW(x) (EM_CFG_PW_0+x)

uint32_t idt_tsi57x_em_cfg_pw  ( DAR_DEV_INFO_t       *dev_info, 
                               idt_em_cfg_pw_in_t   *in_parms, 
                               idt_em_cfg_pw_out_t  *out_parms ) 
{
  uint32_t rc = RIO_ERR_INVALID_PARAMETER;
  uint32_t regData;
  uint8_t  port_num;

  out_parms->imp_rc = RIO_SUCCESS;

  if (in_parms->priority > 3) {
     out_parms->imp_rc = EM_CFG_PW(1);
     goto idt_tsi57x_em_cfg_pw_exit;
  };
      
  // Configure destination ID for port writes.
  regData = ((uint32_t)(in_parms->port_write_destID)) << 16;
  if (tt_dev16 == in_parms->deviceID_tt) {
     regData |= Tsi578_RIO_PW_DESTID_LARGE_DESTID;
  } else {
     regData &= ~(Tsi578_RIO_PW_DESTID_LARGE_DESTID | Tsi578_RIO_PW_DESTID_DESTID_MSB);
  };

  rc = DARRegWrite( dev_info, Tsi578_RIO_PW_DESTID, regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(2);
     goto idt_tsi57x_em_cfg_pw_exit;
  };

  rc = DARRegRead( dev_info, Tsi578_RIO_PW_DESTID, &regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(3);
     goto idt_tsi57x_em_cfg_pw_exit;
  };

  out_parms->deviceID_tt = (regData & Tsi578_RIO_PW_DESTID_LARGE_DESTID)?tt_dev16:tt_dev8;
  out_parms->port_write_destID = (uint16_t)((regData & ( Tsi578_RIO_PW_DESTID_DESTID_LSB 
                                                     | Tsi578_RIO_PW_DESTID_DESTID_MSB )) >> 16);
  // Cannot configure source ID for port-writes on Tsi57x family.
  out_parms->srcID_valid      = true;
  out_parms->port_write_srcID = 0;

  // Configure port-write priority.  Cannot configure CRF.
  
  for (port_num = 0; port_num < TSI57X_NUM_PORTS(dev_info); port_num++) {
     rc = DARRegRead( dev_info, Tsi578_SPX_DISCOVERY_TIMER(port_num), &regData );
     if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = EM_CFG_PW(4);
        goto idt_tsi57x_em_cfg_pw_exit;
     };
   
     regData &= ~Tsi578_SPX_DISCOVERY_TIMER_PW_PRIORITY;
     regData |= (((uint32_t)(in_parms->priority)) << 22) & Tsi578_SPX_DISCOVERY_TIMER_PW_PRIORITY;
     rc = DARRegWrite( dev_info, Tsi578_SPX_DISCOVERY_TIMER(port_num), regData );
     if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = EM_CFG_PW(5);
        goto idt_tsi57x_em_cfg_pw_exit;
     }
  }

  out_parms->priority = in_parms->priority & (Tsi578_SPX_DISCOVERY_TIMER_PW_PRIORITY >> 22);
  out_parms->CRF      = false;

  // Configure port-write re-transmission rate.
  // Assumption: it is better to choose a longer retransmission time than the value requested.
  regData = in_parms->port_write_re_tx / IDT_EM_TSI578_PW_RE_TX_167p7ms;

  switch (regData) {
     case 0: // If the requested retransmission time is shorter than the 
             //    minimum, set the minimum rather than disabling 
             //    retransmission
             if (in_parms->port_write_re_tx) {
               regData = Tsi578_RIO_PW_TIMEOUT_167p7ms;
             } else {
               regData = Tsi578_RIO_PW_TIMEOUT_DISABLE;
             }
             break;
     case 1: regData = Tsi578_RIO_PW_TIMEOUT_167p7ms;
             break;
     case 2: regData = Tsi578_RIO_PW_TIMEOUT_335p5ms;
             break;
     case 3: 
     case 4: regData = Tsi578_RIO_PW_TIMEOUT_671p1ms;
             break;
     default: regData = Tsi578_RIO_PW_TIMEOUT_1340ms;
             break;
  }

  rc = DARRegWrite( dev_info, Tsi578_RIO_PW_TIMEOUT, regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(7);
     goto idt_tsi57x_em_cfg_pw_exit;
  }

  rc = DARRegRead( dev_info, Tsi578_RIO_PW_TIMEOUT, &regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(8);
     goto idt_tsi57x_em_cfg_pw_exit;
  }

  regData = (regData & Tsi578_RIO_PW_TIMEOUT_PW_TIMER) >> 28;

  switch (regData) {
     case 0: out_parms->port_write_re_tx = IDT_EM_PW_RE_TX_DISABLED;
             break;
     case 1: out_parms->port_write_re_tx = IDT_EM_TSI578_PW_RE_TX_167p7ms;
             break;
     case 2: out_parms->port_write_re_tx = IDT_EM_TSI578_PW_RE_TX_335p5ms;
             break;
     case 4: out_parms->port_write_re_tx = IDT_EM_TSI578_PW_RE_TX_671p1ms;
             break;
     case 8: out_parms->port_write_re_tx = IDT_EM_TSI578_PW_RE_TX_1340ms;
             break;
             break;
     default: out_parms->port_write_re_tx = regData;
             rc = RIO_ERR_READ_REG_RETURN_INVALID_VAL;
             out_parms->imp_rc = EM_CFG_PW(9);
  }

  idt_tsi57x_em_cfg_pw_exit:
  return rc; 
};

#define SET_EVENT_PW(x) (EM_SET_EVENT_PW_0+x)

uint32_t idt_tsi57x_set_pw_cfg( DAR_DEV_INFO_t       *dev_info , 
                              struct DAR_ptl       *good_ptl,  /* PTL has already been error-checked, just use it */
                              idt_em_notfn_ctl_t    notfn    ,
                              uint32_t               *imp_rc   )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t regData;
    uint8_t  port_idx, port_num;

    // Validate notfn and pnum
    if (idt_em_notfn_last <= notfn) {
       *imp_rc = SET_EVENT_PW(1);
       goto idt_tsi57x_set_event_pw_cfg_exit;
    };

    // Check if something must be done.
    if (idt_em_notfn_0delta == notfn) {
       rc = RIO_SUCCESS;
       goto idt_tsi57x_set_event_pw_cfg_exit;
    };

	for (port_idx = 0; port_idx <= good_ptl->num_ports; port_idx++) {
	   port_num = good_ptl->pnums[port_idx];
       rc = DARRegRead( dev_info, Tsi578_SPX_MODE(port_num), &regData );
       if (RIO_SUCCESS != rc) {
          *imp_rc = SET_EVENT_PW(2);
          goto idt_tsi57x_set_event_pw_cfg_exit;
       };
              
       switch (notfn) {
          default:  // Default case will not be activated...
          case idt_em_notfn_none:
          case idt_em_notfn_int : 
                              // Disable port-write event notification
                              regData |= Tsi578_SPX_MODE_PW_DIS;
                              break;
          case idt_em_notfn_pw  :
          case idt_em_notfn_both:
                              // Enable port-write event notification
                              regData &= ~(Tsi578_SPX_MODE_PW_DIS);
                              break;
      };
      rc = DARRegWrite( dev_info, Tsi578_SPX_MODE(port_num), regData );
      if (RIO_SUCCESS != rc) {
         *imp_rc = SET_EVENT_PW(3);
         goto idt_tsi57x_set_event_pw_cfg_exit;
      };
   };

idt_tsi57x_set_event_pw_cfg_exit:
   return rc;
}

#define EM_ERR_RATE_EVENT_EXCLUSIONS (Tsi578_SPX_ERR_DET_DELIN_ERR|Tsi578_SPX_ERR_DET_CS_NOT_ACC|Tsi578_SPX_ERR_DET_IMP_SPEC_ERR)
#define SET_EVENT_INT(x) (EM_SET_EVENT_INT_0+x)

uint32_t idt_tsi57x_set_int_cfg( DAR_DEV_INFO_t       *dev_info, 
                               struct DAR_ptl       *good_ptl,  /* PTL has already been error-checked, just use it */
                               idt_em_notfn_ctl_t    notfn   ,
                               uint32_t               *imp_rc  )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t glob_int_en, spx_ctl_indep, spx_mode, i2c_int_en;
    uint8_t  port_idx, port_num = 0;
    bool  en_mecs_int = false, en_rcs_int = false;

    // Validate notfn
    if (idt_em_notfn_last <= notfn) {
       *imp_rc = SET_EVENT_INT(1);
       goto idt_tsi57x_set_event_int_cfg_exit;
    };

    // Check if something must be done.
    if (idt_em_notfn_0delta == notfn) {
       rc = RIO_SUCCESS;
       goto idt_tsi57x_set_event_int_cfg_exit;
    };

    rc = DARRegRead( dev_info, Tsi578_GLOB_INT_ENABLE, &glob_int_en );
    if (RIO_SUCCESS != rc) {
       *imp_rc = SET_EVENT_INT(3);
       goto idt_tsi57x_set_event_int_cfg_exit;
    };

    rc = DARRegRead( dev_info, Tsi578_I2C_INT_ENABLE, &i2c_int_en );
    if (RIO_SUCCESS != rc) {
       *imp_rc = SET_EVENT_INT(4);
       goto idt_tsi57x_set_event_int_cfg_exit;
    };

    // Interrupt enable/disable is complicated by 
    // - Reset interrupts: When interrupts are enabled/disabled, the status of
    //                     idt_em_i_rst_req must be known.  This cannot be determined
    //                     from the state of SPx_MODE.SELF_RST.  Instead, the reserved
    //                     SPx_CTL_INDEP[30] is used to indicate whether Reset interrupts were
    //                     previously enabled, and so should be re-enabled.
    // - MECS interrupts:  Should only be enabled on the Tsi577, as all other devices have an 
    //                     errata against MECS.  
    //                     MECS interrupts are controlled only at the top level, not at the port level.
    // - I2C interrupts:   I2C Interrupts should only be enabled when Tsi578_I2C_INT_ENABLE_BL_FAIL
    //                     is set.
	for (port_idx = 0; port_idx <= good_ptl->num_ports; port_idx++) {
		port_num = good_ptl->pnums[port_idx];
       rc = DARRegRead( dev_info, Tsi578_SPX_MODE(port_num), &spx_mode );
       if (RIO_SUCCESS != rc) {
          *imp_rc = SET_EVENT_INT(5);
          goto idt_tsi57x_set_event_int_cfg_exit;
       };
       rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(port_num), &spx_ctl_indep );
       if (RIO_SUCCESS != rc) {
          *imp_rc = SET_EVENT_INT(6);
          goto idt_tsi57x_set_event_int_cfg_exit;
       };
       switch (notfn) {
          default:  // Default case will not be activated...
          case idt_em_notfn_none:
          case idt_em_notfn_pw  :
                              // Disable interrupt event notification
                              glob_int_en &= ~(1 << port_num);
                              spx_mode &= ~(Tsi578_SPX_MODE_RCS_INT_EN);
                              spx_ctl_indep &= ~Tsi578_SPX_CTL_INDEP_IRQ_EN;
                              break;
          case idt_em_notfn_int : 
          case idt_em_notfn_both:
                              // Enable interrupt event notification
                              glob_int_en |= 1 << port_num;
                              if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_RSVD1) {
                                 spx_mode |= Tsi578_SPX_MODE_RCS_INT_EN;
                                 en_rcs_int = true;
                              };
                              spx_ctl_indep |= Tsi578_SPX_CTL_INDEP_IRQ_EN;
                              if (spx_mode & Tsi578_SPX_MODE_MCS_INT_EN)
                                 en_mecs_int = true;
                              break;
      };
      rc = DARRegWrite( dev_info, Tsi578_SPX_MODE(port_num), spx_mode );
      if (RIO_SUCCESS != rc) {
         *imp_rc = SET_EVENT_INT(6);
         goto idt_tsi57x_set_event_int_cfg_exit;
      };
      rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(port_num), spx_ctl_indep );
      if (RIO_SUCCESS != rc) {
         *imp_rc = SET_EVENT_INT(7);
         goto idt_tsi57x_set_event_int_cfg_exit;
      };
    };

    switch (notfn) {
       default:  // Default case will not be activated...
       case idt_em_notfn_none:
       case idt_em_notfn_pw  :
          // Disable interrupt event notification
                 if (RIO_ALL_PORTS == port_num) {
                    glob_int_en &= ~(Tsi578_GLOB_INT_ENABLE_RCS_EN | Tsi578_GLOB_INT_ENABLE_MCS_EN | Tsi578_GLOB_INT_ENABLE_I2C_EN);
                 }
                 break;
       case idt_em_notfn_int : 
       case idt_em_notfn_both:
                 if (RIO_ALL_PORTS == port_num) {
                    if (en_rcs_int) {
                       glob_int_en |= Tsi578_GLOB_INT_ENABLE_RCS_EN;
                    } else {
                       glob_int_en &= ~Tsi578_GLOB_INT_ENABLE_RCS_EN;
                    };
                    if (en_mecs_int) {
                       glob_int_en |= Tsi578_GLOB_INT_ENABLE_MCS_EN;
                    } else {
                       glob_int_en &= ~Tsi578_GLOB_INT_ENABLE_MCS_EN;
                    };
                 };
                 if (i2c_int_en & Tsi578_I2C_INT_ENABLE_BL_FAIL) {
                    glob_int_en |= Tsi578_GLOB_INT_ENABLE_I2C_EN;
                 } else {
                    glob_int_en |= Tsi578_GLOB_INT_ENABLE_I2C_EN;
                 };

                 rc = DARRegWrite( dev_info, Tsi578_GLOB_INT_ENABLE, glob_int_en );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_INT(8);
                 }; 
    };
              
idt_tsi57x_set_event_int_cfg_exit:
    return rc;
}
#define DET_NOTFN(x) (EM_DET_NOTFN_0+x)

uint32_t tsi57x_em_determine_notfn( DAR_DEV_INFO_t       *dev_info  , 
                                  idt_em_notfn_ctl_t   *notfn      ,
                                  uint8_t                 pnum      ,
                                  uint32_t               *imp_rc    ) 
{
    uint32_t rc;
    uint32_t spx_ctl_indep, spx_mode;

    rc = DARRegRead( dev_info, Tsi578_SPX_MODE(pnum), &spx_mode );
    if (RIO_SUCCESS != rc) {
       *imp_rc = DET_NOTFN(0x01);
       goto em_determine_notfn_exit;
    }
              
    rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &spx_ctl_indep );
    if (RIO_SUCCESS != rc) {
       *imp_rc = DET_NOTFN(0x02);
       goto em_determine_notfn_exit;
    }
              
    // Now figure out the current notification setting for this port.
    if ( spx_ctl_indep & Tsi578_SPX_CTL_INDEP_IRQ_EN ) {
       if (spx_mode & Tsi578_SPX_MODE_PW_DIS) {
          *notfn = idt_em_notfn_int;
       } else {
          *notfn = idt_em_notfn_both;
       }
    } else {
       if (spx_mode & Tsi578_SPX_MODE_PW_DIS) {
          *notfn = idt_em_notfn_none;
       } else {
          *notfn = idt_em_notfn_pw;
       }
    };
em_determine_notfn_exit:
    return rc;
} 

#define EN_ERR_CTR(x) (EM_EN_ERR_CTR_0+x)

uint32_t idt_tsi57x_enable_err_ctr( DAR_DEV_INFO_t  *dev_info        ,   
                                  uint8_t            pnum            ,
                                  uint32_t           spx_rate_en     ,
                                  uint32_t           spx_rate_en_mask,
                                  uint32_t           spx_err_rate    ,
                                  uint32_t           spx_err_thresh  ,
                                  uint32_t          *imp_rc           )
{
    uint32_t rc;
    uint32_t regData;

    // Enable event counting.
    rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &regData );
    if (RIO_SUCCESS != rc) {
       *imp_rc = EN_ERR_CTR(1);
       goto idt_tsi57x_enable_err_ctr_exit;
    }

    regData &= ~spx_rate_en_mask;
    regData |=  spx_rate_en;
               
    rc = DARRegWrite( dev_info, Tsi578_SPX_RATE_EN(pnum), regData );
    if (RIO_SUCCESS != rc) {
       *imp_rc = EN_ERR_CTR(2);
       goto idt_tsi57x_enable_err_ctr_exit;
    }

    // Clear counter, and set thresholds...
    rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_RATE(pnum), spx_err_rate );
    if (RIO_SUCCESS != rc) {
       *imp_rc = EN_ERR_CTR(3);
       goto idt_tsi57x_enable_err_ctr_exit;
    }

    // Hook to clear the "Degraded" threshold value.
    // This enforces a programming model where the 
    // Degraded threshold must be less than or equal to
    // the Failed threshold.  
    //
    // Also has the effect of clearing the reset default 
    // value for the Degraded threshold to 0.
    
    if (((spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RDT) >> 16) > (( spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RFT) >> 24))
        spx_err_thresh &= Tsi578_SPX_ERR_THRESH_ERR_RFT;

    rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_THRESH(pnum), spx_err_thresh );
    if (RIO_SUCCESS != rc) {
       *imp_rc = EN_ERR_CTR(4);
       goto idt_tsi57x_enable_err_ctr_exit;
    }

    // Tsi577/Tsi578/Tsi576/Tsi572 devices all detect PORT_FAIL,
    // and only discard packets within the switch, contrary to the
    // spec which requires continuous discard until PORT_FAIL is cleared.
    // See the errata sheet for these devices.
    //
    // The Tsi578A device, which is otherwise identical to Tsi578,
    // does perform continuous packet discard for PORT_FAIL.

    rc = DARRegRead( dev_info, Tsi578_SPX_CTL(pnum), &regData );
    if (RIO_SUCCESS != rc) {
       *imp_rc = EN_ERR_CTR(5);
       goto idt_tsi57x_enable_err_ctr_exit;
    }

    regData |= Tsi578_SPX_CTL_STOP_FAIL_EN | Tsi578_SPX_CTL_DROP_EN;
               
    rc = DARRegWrite( dev_info, Tsi578_SPX_CTL(pnum), regData );

idt_tsi57x_enable_err_ctr_exit:
    return rc;
};

#define SET_EVENT_EN(x) (EM_SET_EVENT_EN_0+x)

uint32_t idt_tsi57x_set_event_en_cfg( DAR_DEV_INFO_t       *dev_info, 
                                    uint8_t                 pnum    , 
                                    idt_em_cfg_t         *event   , 
                                    uint32_t               *imp_rc   )
{
    uint32_t rc = RIO_SUCCESS;
    uint32_t regData, temp;
    port_mac_relations_t *sw_pmr;

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) { 
       *imp_rc = SET_EVENT_EN(1);
       goto idt_tsi57x_set_event_en_cfg_exit;
    };

    if (( event->em_detect >= idt_em_detect_last ) ||
        ( event->em_event  >= idt_em_last        )) {
       rc      = RIO_ERR_INVALID_PARAMETER;
       *imp_rc = SET_EVENT_EN(2);
       goto idt_tsi57x_set_event_en_cfg_exit;
    };

    // Nothing to do...
    if ( event->em_detect == idt_em_detect_0delta)
       goto idt_tsi57x_set_event_en_cfg_exit;

    switch ( event->em_event ) {
       case idt_em_f_los:  
           // LOS is the dead link timer 
           // If a short period is required, then single Delineation errors also
           //    result in LOS.
           
          {
            bool set_delin_thresh = false;
            if ( event->em_detect == idt_em_detect_on ) {
               // First, set up DLT
               rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[pnum].pwr_mac_num * 2), &regData );
               if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(3);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };
              temp = event->em_info / 81920;
              if (!temp) {
                 temp = 1;
                 set_delin_thresh = true;
              };
              
              regData |= Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN;
              regData &= ~Tsi578_SMACX_DLOOP_CLK_SEL_DLT_THRESH;
              regData |= ((temp << 16) & Tsi578_SMACX_DLOOP_CLK_SEL_DLT_THRESH);
                             
              rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[pnum].pwr_mac_num * 2), regData );
              if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(4);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };

              if (set_delin_thresh) {
                 // Set the delineation error threshold to 1.  
                 // Configure the device to discard packets when the threshold has been met.
                 // NOTE: This is only effective on the Tsi578A device. See errata.
                 //
                 uint32_t spx_err_rate_en, spx_err_rate, spx_err_thresh;
                 rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &spx_err_rate_en );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x27);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &spx_err_rate );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x28);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &spx_err_thresh );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x29);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 // Set the threshold to 1 only if no other threshold based events are enabled
                 if (!(spx_err_rate_en & ~Tsi578_SPX_ERR_DET_DELIN_ERR))
                    spx_err_thresh = 0x01000000;

                 rc = idt_tsi57x_enable_err_ctr( dev_info, pnum, Tsi578_SPX_ERR_DET_DELIN_ERR       , 
                                                                 Tsi578_SPX_ERR_DET_DELIN_ERR       , 
                                                                 spx_err_rate, 
                                                                 spx_err_thresh,
                                                                 imp_rc);
                 if (RIO_SUCCESS != rc) {
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
              };
            } else { // idt_em_detect_off: 
              rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[pnum].pwr_mac_num * 2), &regData );
              if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(5);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };
                                           
              regData &= ~(Tsi578_SMACX_DLOOP_CLK_SEL_DLT_THRESH | Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN);
                             
              rc = DARRegWrite( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[pnum].pwr_mac_num * 2), regData );
              if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(6);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };
            };

            if (!set_delin_thresh) {
              // Disable event counting. 
              rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &regData );
              if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(7);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };

              regData &= ~Tsi578_SPX_ERR_DET_DELIN_ERR;
                                           
              rc = DARRegWrite( dev_info, Tsi578_SPX_RATE_EN(pnum), regData );
              if (RIO_SUCCESS != rc) {
                 *imp_rc = SET_EVENT_EN(8);
                 goto idt_tsi57x_set_event_en_cfg_exit;
              };
            };
         };
            break;

       case idt_em_f_port_err:
            rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &regData );
            if (RIO_SUCCESS != rc) {
               *imp_rc = SET_EVENT_EN(0x10);
               goto idt_tsi57x_set_event_en_cfg_exit;
            };
            if (event->em_detect == idt_em_detect_on) {
                 uint32_t spx_err_rate, spx_err_thresh;
                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &spx_err_rate );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x28);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &spx_err_thresh );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x28);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 if (!(spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RFT))
                    spx_err_thresh |= 0x01000000;
               regData |= Tsi578_SPX_CTL_INDEP_PORT_ERR_EN;

               rc = idt_tsi57x_enable_err_ctr( dev_info, pnum, Tsi578_SPX_ERR_DET_IMP_SPEC_ERR, 
                                                               Tsi578_SPX_ERR_DET_IMP_SPEC_ERR, 
                                                               spx_err_rate, 
                                                               spx_err_thresh,
                                                               imp_rc );
               if (RIO_SUCCESS != rc) {
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
            } else { // idt_em_detect_off: 
               regData &= ~(Tsi578_SPX_CTL_INDEP_PORT_ERR_EN);
            };
            rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), regData );
            if (RIO_SUCCESS != rc) {
               *imp_rc = SET_EVENT_EN(0x13);
               goto idt_tsi57x_set_event_en_cfg_exit;
            };
            break;

       case idt_em_f_2many_retx:
               rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x20);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
               if (event->em_detect == idt_em_detect_on) {
                  uint32_t retry_thresh = event->em_info;
                  uint32_t spx_err_rate, spx_err_thresh;

                  if (!retry_thresh) {
                     rc      = RIO_ERR_INVALID_PARAMETER;
                     *imp_rc = SET_EVENT_EN(0x21);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
                  if (retry_thresh > (Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD >> 8)) {
                     retry_thresh = Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD;
                  } else {
                     retry_thresh = retry_thresh << 8;
                  };
                  regData |= Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN;
                  regData &= ~Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD;
                  regData |= retry_thresh;

                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &spx_err_rate );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x28);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &spx_err_thresh );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x28);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
                 if (!(spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RFT))
                    spx_err_thresh |= 0x01000000;

                 rc = idt_tsi57x_enable_err_ctr( dev_info, pnum, Tsi578_SPX_ERR_DET_IMP_SPEC_ERR, 
                                                                 Tsi578_SPX_ERR_DET_IMP_SPEC_ERR, 
                                                                 spx_err_rate, 
                                                                 spx_err_thresh,
                                                                 imp_rc );
                 if (RIO_SUCCESS != rc) {
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
               } else { // idt_em_detect_off: 
                  // Disable events.
                  regData &= ~(Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN | Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD);
               };
                          
               rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x23);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
               break;

       case idt_em_f_2many_pna:
               // Enable/disable event detection
               if (event->em_detect == idt_em_detect_on) {
                  uint32_t spx_err_rate, spx_err_thresh;
                  rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &spx_err_rate );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x28);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
                  rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &spx_err_thresh );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x28);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };

                  spx_err_thresh &= ~Tsi578_SPX_ERR_THRESH_ERR_RFT;
                  spx_err_thresh |= ((event->em_info << 24) & Tsi578_SPX_ERR_THRESH_ERR_RFT);

                  rc = idt_tsi57x_enable_err_ctr( dev_info, pnum, Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN, 
                                                                  Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN, 
                                                                  spx_err_rate, 
                                                                  spx_err_thresh, 
                                                                  imp_rc );
                  if (RIO_SUCCESS != rc) {
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
                  break;
               };

               // Disable event by not counting PNA's in error counter.
               rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x28);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };

               regData &= ~(Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN);

               rc = DARRegWrite( dev_info, Tsi578_SPX_RATE_EN(pnum), regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x29);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
               break;

       case idt_em_f_err_rate :
               { uint32_t spx_rate_en, spx_err_rate, spx_err_thresh;
                 rc = idt_em_get_f_err_rate_info( event->em_info, &spx_rate_en, 
                                                                  &spx_err_rate, 
                                                                  &spx_err_thresh);
                 if (RIO_SUCCESS != rc) {
                   *imp_rc = SET_EVENT_EN(0x2C);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };

                 spx_rate_en &= ~EM_ERR_RATE_EVENT_EXCLUSIONS;

                 // Enable/disable event detection
                 if (event->em_detect == idt_em_detect_on) {
                    uint32_t curr_thresh;
                    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &curr_thresh );
                    if (RIO_SUCCESS != rc) {
                       *imp_rc = SET_EVENT_EN(0x28);
                       goto idt_tsi57x_set_event_en_cfg_exit;
                    };

                    curr_thresh &= ~Tsi578_SPX_ERR_THRESH_ERR_RFT;
                    curr_thresh |= (spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RFT);

                    rc = idt_tsi57x_enable_err_ctr( dev_info, pnum, spx_rate_en, ~EM_ERR_RATE_EVENT_EXCLUSIONS,
                                                    spx_err_rate, curr_thresh,
                                                    imp_rc             );
                    if (RIO_SUCCESS != rc) {
                       goto idt_tsi57x_set_event_en_cfg_exit;
                    };
                    break;
                 };
  
                 // Disable event by disabling error counting for ALL f_err_rate errors
                 rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &regData );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x2C);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
  
                 regData = regData & EM_ERR_RATE_EVENT_EXCLUSIONS;
  
                 rc = DARRegWrite( dev_info, Tsi578_SPX_RATE_EN(pnum), regData );
                 if (RIO_SUCCESS != rc) {
                    *imp_rc = SET_EVENT_EN(0x2D);
                    goto idt_tsi57x_set_event_en_cfg_exit;
                 };
               }
               break;

       case idt_em_i_init_fail:
              // Enable reporting to top level
              // There is a top-level bit which gates whether or not this
              // event is reported as an interrupt.  It cannot be reported as
              // a port-write.
               rc = DARRegRead( dev_info, Tsi578_I2C_INT_ENABLE, &regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x30);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
              
               if (event->em_detect == idt_em_detect_on) {
                  regData |= Tsi578_I2C_INT_ENABLE_BL_FAIL;
               } else { 
                  regData &= ~(Tsi578_I2C_INT_ENABLE_BL_FAIL);
               };

               rc = DARRegWrite( dev_info, Tsi578_I2C_INT_ENABLE, regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x32);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
               break;

       case idt_em_d_log:
              // Specific logical layer error enables are controlled by info parameter
                  rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET_EN, &regData );
                  if (RIO_SUCCESS != rc) {
                        *imp_rc = SET_EVENT_EN(0x41);
                        goto idt_tsi57x_set_event_en_cfg_exit;
                  };
                  if (event->em_detect == idt_em_detect_on) {
                     regData = (regData & ~(TSI57X_ALL_LOG_ERRS)) | (event->em_info & TSI57X_ALL_LOG_ERRS); 
                  } else {
                     regData &= ~(TSI57X_ALL_LOG_ERRS); 
                  };
                  rc = DARRegWrite( dev_info, Tsi578_RIO_LOG_ERR_DET_EN, regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x42);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              break;
       case idt_em_d_ttl:
              // Nothing to do, event is not supported...
              break;

       case idt_em_d_rte:
                  rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x51);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
                  if (event->em_detect == idt_em_detect_on) {
                     regData |= Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR;
                  } else {
                     regData &= ~(Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR);
                  };
                  rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x52);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              break;

       case idt_em_i_sig_det:
               if ( !event->em_info ) { 
                  idt_pc_get_status_in_t  stat_in;
                  idt_pc_get_status_out_t stat_out;

                  stat_in.ptl.num_ports = 1;
                  stat_in.ptl.pnums[0] = pnum;
                  rc = idt_pc_get_status( dev_info, &stat_in, &stat_out );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x60);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };

                  if (((idt_em_detect_off == event->em_detect) && !stat_out.ps[0].port_ok) ||
                      ((idt_em_detect_on  == event->em_detect) &&  stat_out.ps[0].port_ok))
                     break;
               };

               rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x61);
                     goto idt_tsi57x_set_event_en_cfg_exit;
               };
              
               if (event->em_detect == idt_em_detect_on) {

                  regData |= Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN;

                  rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x62);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };

                  // Must set PORT_LOCKOUT for the event to be detected...
                  rc = DARRegRead( dev_info, Tsi578_SPX_CTL(pnum), &regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x63);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              
                  regData |= Tsi578_SPX_CTL_PORT_LOCKOUT;

                  rc = DARRegWrite( dev_info, Tsi578_SPX_CTL(pnum), regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x64);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              } else {
                  // Disable detection, reporting/counting remain unaffected.
                  // Do not touch PORT_LOCKOUT. Let the PC routine control PORT_LOCKOUT.
                  regData &= ~(Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN);

                  rc = DARRegWrite( dev_info, Tsi578_SPX_CTL_INDEP(pnum), regData );
                  if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x65);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              };
              break;

       case idt_em_i_rst_req:
               rc = DARRegRead( dev_info, Tsi578_SPX_MODE(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                     *imp_rc = SET_EVENT_EN(0x70);
                     goto idt_tsi57x_set_event_en_cfg_exit;
                  };
              
               if (event->em_detect == idt_em_detect_on) {
                 // Can't detect the event unless self_rst is disabled.
                 regData &= ~(Tsi578_SPX_MODE_SELF_RST);
                 regData |= Tsi578_SPX_MODE_RCS_INT_EN;
               } else {
                 // DO NOT CHANGE SELF_RST                                      
                 // This should be handled by the PC routines.
                 regData &= ~(Tsi578_SPX_MODE_RCS_INT_EN);
               };

               rc = DARRegWrite( dev_info, Tsi578_SPX_MODE(pnum), regData );
               if (RIO_SUCCESS != rc) {
                  *imp_rc = SET_EVENT_EN(0x72);
                  goto idt_tsi57x_set_event_en_cfg_exit;
               };
              break;

      case idt_em_a_clr_pwpnd :
      case idt_em_a_no_event  :
              break;

       default: 
          rc      = RIO_ERR_INVALID_PARAMETER;
          *imp_rc = SET_EVENT_EN(0x80);
          goto idt_tsi57x_set_event_en_cfg_exit;
    };

idt_tsi57x_set_event_en_cfg_exit:
    return rc;
}

#define EM_CFG_SET(x) (EM_CFG_SET_0+x)

uint32_t idt_tsi57x_em_cfg_set  ( DAR_DEV_INFO_t        *dev_info, 
                                idt_em_cfg_set_in_t   *in_parms, 
                                idt_em_cfg_set_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t pnum; 
    uint8_t idx, e_idx;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
	struct DAR_ptl good_ptl;
    
    out_parms->imp_rc        = RIO_SUCCESS; 
    out_parms->fail_port_num = RIO_ALL_PORTS;
    out_parms->fail_idx      = idt_em_last;
    out_parms->notfn         = idt_em_notfn_0delta;

    if ( (in_parms->num_events > (uint8_t)(idt_em_last)) ||
         ( NULL == in_parms->events                  )) {
       out_parms->imp_rc = EM_CFG_SET(0x10);
       goto idt_tsi57x_em_cfg_set_exit;
    };

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = EM_CFG_SET(0x15);
          goto idt_tsi57x_em_cfg_set_exit;
    };

	cfg_in.ptl = good_ptl;
    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_cfg_set_exit;
    }

    if (!cfg_out.num_ports) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_cfg_set_exit;
    };

    /* For all available ports, configure the events requested. */
    /* First, disable all events requested.                     */
    for (idx = 0; idx < cfg_out.num_ports; idx++) {
       if ( cfg_out.pc[idx].port_available && cfg_out.pc[idx].powered_up ) {
           pnum = cfg_out.pc[idx].pnum;
           out_parms->fail_port_num = pnum;
           out_parms->fail_idx   = 0;

           for (e_idx = 0; (e_idx < in_parms->num_events) && 
                        (e_idx < (uint8_t)(idt_em_last))   ; e_idx++) {
             if (in_parms->events[e_idx].em_detect == idt_em_detect_off ) {
                out_parms->fail_idx   = e_idx;
                rc = idt_tsi57x_set_event_en_cfg( dev_info, pnum, &(in_parms->events[e_idx]), &out_parms->imp_rc );
                if (RIO_SUCCESS != rc) {
                   goto idt_tsi57x_em_cfg_set_exit;
                }
             }
           };

           for (e_idx = 0; (e_idx < in_parms->num_events) && 
                        (e_idx < (uint8_t)(idt_em_last))   ; e_idx++) {
             if (in_parms->events[e_idx].em_detect == idt_em_detect_on ) {
                out_parms->fail_idx   = e_idx;
                rc = idt_tsi57x_set_event_en_cfg( dev_info, pnum, &(in_parms->events[e_idx]), &out_parms->imp_rc );
                if (RIO_SUCCESS != rc) {
                   goto idt_tsi57x_em_cfg_set_exit;
                }
             }
           };
        };
    };

    out_parms->fail_port_num = RIO_ALL_PORTS;
    out_parms->fail_idx      = idt_em_last;

    rc = idt_tsi57x_set_int_cfg( dev_info, &good_ptl, in_parms->notfn, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_em_cfg_set_exit;
    }

    rc = idt_tsi57x_set_pw_cfg( dev_info, &good_ptl, in_parms->notfn, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_em_cfg_set_exit;
    }

    out_parms->notfn = in_parms->notfn;
    if (( out_parms->notfn >  idt_em_notfn_both ) && (RIO_ALL_PORTS != in_parms->ptl.num_ports))
       rc = tsi57x_em_determine_notfn( dev_info, &out_parms->notfn, in_parms->ptl.pnums[0], &out_parms->imp_rc);

idt_tsi57x_em_cfg_set_exit:

    return rc;
};

#define EM_CFG_GET(x) (EM_CFG_GET_0+x)

uint32_t idt_tsi57x_em_cfg_get  ( DAR_DEV_INFO_t        *dev_info, 
                                idt_em_cfg_get_in_t   *in_parms, 
                                idt_em_cfg_get_out_t  *out_parms ) 
{

    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t spx_ctl, spx_mode, spx_ctl_indep, spx_rate_en, spx_dloop, i2c_int_enable, log_err_en, spx_err_thresh, spx_err_rate;
    uint8_t pnum; 
    uint8_t e_idx;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    
    out_parms->fail_idx = (uint8_t)(idt_em_last); 
    out_parms->imp_rc   = RIO_SUCCESS;
    out_parms->notfn    = idt_em_notfn_0delta;

    if (in_parms->port_num >= TSI57X_NUM_PORTS(dev_info)) {
       out_parms->imp_rc = EM_CFG_GET(0x01);
       goto idt_tsi57x_em_cfg_get_exit;
    } else {
       cfg_in.ptl.num_ports   = 1;
	   cfg_in.ptl.pnums[0] = in_parms->port_num;
    };

    if ((in_parms->num_events > (uint8_t)(idt_em_last)) ||
        (NULL == in_parms->event_list               ) ||
        (NULL == in_parms->events                   )) {
       out_parms->imp_rc = EM_CFG_GET(0x02);
       goto idt_tsi57x_em_cfg_get_exit;
    };

    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_cfg_get_exit;
    };

    if (!cfg_out.num_ports) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = EM_CFG_GET(0x03);
       goto idt_tsi57x_em_cfg_get_exit;
    };

    if (!cfg_out.pc[0].port_available || !cfg_out.pc[0].powered_up ) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = EM_CFG_GET(0x04);
       goto idt_tsi57x_em_cfg_get_exit;
    };

    pnum = in_parms->port_num;

    rc = DARRegRead( dev_info, Tsi578_SPX_MODE(pnum), &spx_mode );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = EM_CFG_GET(0x05);
       goto idt_tsi57x_em_cfg_get_exit;
    }
              
    rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &spx_ctl_indep );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = EM_CFG_GET(0x06);
       goto idt_tsi57x_em_cfg_get_exit;
    }
              
    rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &spx_rate_en );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = EM_CFG_GET(0x07);
       goto idt_tsi57x_em_cfg_get_exit;
    }

    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &spx_err_thresh );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = EM_CFG_GET(0x08);
       goto idt_tsi57x_em_cfg_get_exit;
    }

    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &spx_err_rate );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = EM_CFG_GET(0x09);
       goto idt_tsi57x_em_cfg_get_exit;
    }

    for (e_idx = 0; ((e_idx < in_parms->num_events) && (e_idx < (uint8_t)(idt_em_last))); e_idx++) {
       // Initialize event such that event is disabled.
       out_parms->fail_idx   = e_idx;
       in_parms->events[e_idx].em_event  = in_parms->event_list[e_idx];
       in_parms->events[e_idx].em_detect = idt_em_detect_off;
       in_parms->events[e_idx].em_info   = 0;

       switch ( in_parms->events[e_idx].em_event ) {
          case idt_em_f_los:  
              // Check setting of DLT, return timeout parameter if possible.
               rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(pnum & ~1), &spx_dloop );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CFG_GET(0x10);
                  goto idt_tsi57x_em_cfg_get_exit;
               }

              if ( spx_dloop & Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN ) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
                if (Tsi578_SPX_ERR_DET_DELIN_ERR & spx_rate_en )
                   in_parms->events[e_idx].em_info = 1;
                else {
                   in_parms->events[e_idx].em_info = (spx_dloop & Tsi578_SMACX_DLOOP_CLK_SEL_DLT_THRESH) >> 16;
                   in_parms->events[e_idx].em_info = in_parms->events[e_idx].em_info * 81920;
                };
              };
               break;

       case idt_em_f_port_err:
              if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_PORT_ERR_EN ) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
              };
              break;

       case idt_em_f_2many_retx:
              if ((spx_ctl_indep & Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN) && (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD)) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
                 in_parms->events[e_idx].em_info = (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD) >> 8;
              };
              break;

       case idt_em_f_2many_pna:
              if (spx_rate_en & Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN) {
                  in_parms->events[e_idx].em_detect = idt_em_detect_on;
                  in_parms->events[e_idx].em_info   = (spx_err_thresh & Tsi578_SPX_ERR_THRESH_ERR_RFT) >> 24;
              };
              break;

       case idt_em_f_err_rate:
              rc =  idt_em_compute_f_err_rate_info( spx_rate_en & ~EM_ERR_RATE_EVENT_EXCLUSIONS, spx_err_rate, spx_err_thresh, 
                                                   &in_parms->events[e_idx].em_info           );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = EM_CFG_GET(0x30);
                 goto idt_tsi57x_em_cfg_get_exit;
              };
              if (spx_rate_en & ~EM_ERR_RATE_EVENT_EXCLUSIONS) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
              };
              break;

       case idt_em_i_init_fail:
               rc = DARRegRead( dev_info, Tsi578_I2C_INT_ENABLE, &i2c_int_enable );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CFG_GET(0x11);
                  goto idt_tsi57x_em_cfg_get_exit;
               }
              
               if (i2c_int_enable & Tsi578_I2C_INT_ENABLE_BL_FAIL) {
                  in_parms->events[e_idx].em_detect = idt_em_detect_on;
               };
               break;

       case idt_em_d_ttl:
              // Nothing to do, event is not supported...
              break;

       case idt_em_d_rte:
              if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR) {
                  in_parms->events[e_idx].em_detect = idt_em_detect_on;
              };
              break;
       case idt_em_d_log:
              rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET_EN, &log_err_en );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = EM_CFG_GET(0x12);
                 goto idt_tsi57x_em_cfg_get_exit;
              }
              if (log_err_en) {
                  in_parms->events[e_idx].em_detect = idt_em_detect_on;
                  in_parms->events[e_idx].em_info   = log_err_en & TSI57X_ALL_LOG_ERRS;
              };
              break;
       case idt_em_i_sig_det:
              rc = DARRegRead( dev_info, Tsi578_SPX_CTL(pnum), &spx_ctl );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = EM_CFG_GET(0x13);
                 goto idt_tsi57x_em_cfg_get_exit;
              }
              if ((spx_ctl_indep & Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN) &&
                  (spx_ctl       & Tsi578_SPX_CTL_PORT_LOCKOUT                   ) ) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
              };
              break;

       case idt_em_i_rst_req:
              if (!(spx_mode & Tsi578_SPX_MODE_SELF_RST) && (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_RSVD1)) {
                 in_parms->events[e_idx].em_detect = idt_em_detect_on;
              };
              break;

      case idt_em_a_clr_pwpnd :
      case idt_em_a_no_event  :
              break;

       default: 
          rc = RIO_ERR_INVALID_PARAMETER;
          out_parms->imp_rc = EM_CFG_GET(0x14);
          goto idt_tsi57x_em_cfg_get_exit;
       };
    };

    out_parms->fail_idx = (uint8_t)(idt_em_last); 

    // Now figure out the current notification setting for this port.
    rc = tsi57x_em_determine_notfn( dev_info, &out_parms->notfn, pnum, &out_parms->imp_rc);

idt_tsi57x_em_cfg_get_exit:
    return rc;
};

#define DEV_RPT_CTL(x) (EM_DEV_RPT_CTL_0+x)

uint32_t idt_tsi57x_em_dev_rpt_ctl  ( DAR_DEV_INFO_t            *dev_info, 
                                    idt_em_dev_rpt_ctl_in_t   *in_parms, 
                                    idt_em_dev_rpt_ctl_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	struct DAR_ptl good_ptl;
    
    out_parms->notfn  = idt_em_notfn_0delta;
    out_parms->imp_rc = RIO_SUCCESS;
       
	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = DEV_RPT_CTL(1);
       goto idt_tsi57x_em_dev_rpt_ctl_exit;
    };

    rc = idt_tsi57x_set_int_cfg( dev_info, &good_ptl, in_parms->notfn, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi57x_em_dev_rpt_ctl_exit;
    }

    rc = idt_tsi57x_set_pw_cfg( dev_info, &good_ptl, in_parms->notfn, &out_parms->imp_rc );
    if (RIO_SUCCESS == rc) {
       out_parms->notfn  = in_parms->notfn;
    };

    out_parms->notfn  = in_parms->notfn;
	if ((1 == good_ptl.num_ports) && (idt_em_notfn_0delta == in_parms->notfn)) 
       rc = tsi57x_em_determine_notfn( dev_info, &out_parms->notfn, good_ptl.pnums[0], &out_parms->imp_rc);
              
idt_tsi57x_em_dev_rpt_ctl_exit:
   return rc;
}

typedef struct pw_parsing_info_t_TAG
{
   uint8_t           pw_index;  // Index of 32 bit word to test in the port write, 0-3
   uint32_t          check;     // If the bitwise and of this value and the port-write
                              //    word at "pw_index" is non-zero, then the event has occurred
   idt_em_events_t event;     // Event checked for.
   bool            per_port;  // true if this is a per-port event, false for GLOBAL events
} pw_parsing_info_t;

#define NOT_IN_PW (0x0)
#define PW_IMP_SPEC_OTH (~(PW_MAX_RETRY | PW_ILL_TRANS | PW_PORT_ERR | \
                         PW_LINK_INIT) & 0xFFFFFF00)
#define PW_MAX_RETRY    0x80000000
#define PW_ILL_TRANS    0x20000000
#define PW_PORT_ERR     0x10000000
#define PW_LINK_INIT    0x00200000

pw_parsing_info_t pw_parsing_info[ (uint8_t)(idt_em_last) ] = {
   { 1, Tsi578_SPX_ERR_DET_DELIN_ERR, idt_em_f_los       , true },
   { 2, PW_PORT_ERR                 , idt_em_f_port_err  , true },
   { 2, PW_MAX_RETRY                , idt_em_f_2many_retx, true },
   { 2, NOT_IN_PW                   , idt_em_i_init_fail , false},
   { 2, NOT_IN_PW                   , idt_em_d_ttl       , false},
   { 2, PW_ILL_TRANS                , idt_em_d_rte       , true },
   { 3, 0xFFFFFFFF                  , idt_em_d_log       , false},
   { 2, PW_LINK_INIT                , idt_em_i_sig_det   , true }, 
   { 2, NOT_IN_PW                   , idt_em_i_rst_req   , true }  
};

#define PARSE_PW(x) (EM_PARSE_PW_0+x)

uint32_t idt_tsi57x_em_parse_pw  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_em_parse_pw_in_t   *in_parms, 
                                 idt_em_parse_pw_out_t  *out_parms ) 
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t idx;
   // Do not check COMP_TAG, IMP_SPEC bit or port number field for other events.
   uint32_t oth_masks[4] = {0xFFFFFFFF, 0x80000000, 0x000000FF, 0}; 

   out_parms->imp_rc       = RIO_SUCCESS;
   out_parms->num_events   = 0;
   out_parms->too_many     = false;
   out_parms->other_events = false;

   if (( !in_parms->num_events                        ) 
      || (in_parms->num_events > (uint8_t)(idt_em_last) )
      || (NULL == dev_info                            )
      || (NULL == in_parms->events                    )) {
      out_parms->imp_rc = PARSE_PW(1);
      goto idt_tsi57x_em_parse_pw_exit;
   };

   // Check each bit within the port-write data for information on Error Management events
   for (idx = 0; (idx < (uint8_t)(idt_em_last)) && !(out_parms->too_many); idx++ ) {
      if (in_parms->pw[pw_parsing_info[idx].pw_index] & pw_parsing_info[idx].check) {
         if (out_parms->num_events < in_parms->num_events) {
            in_parms->events[out_parms->num_events].event = pw_parsing_info[idx].event;
            if (pw_parsing_info[idx].per_port)
               in_parms->events[out_parms->num_events].port_num = 
                      (uint8_t)(in_parms->pw[2] & 0x000000FF);
           else
               in_parms->events[out_parms->num_events].port_num = RIO_ALL_PORTS;
           out_parms->num_events++;
        } else {
           out_parms->too_many = true;
        };
      };
   };

   // Check for "other" events that have happenned...
   for (idx = 0; idx < (uint8_t)(idt_em_last); idx++ ) {
      oth_masks [pw_parsing_info[idx].pw_index] |= pw_parsing_info[idx].check; 
   };
   for (idx = 0; (idx < 3) && !out_parms->other_events; idx++) {
      out_parms->other_events = (in_parms->pw[idx] & ~oth_masks[idx])?true:false;
   }

   rc = RIO_SUCCESS;
       
idt_tsi57x_em_parse_pw_exit:
   return rc;
}

#define GET_INT_STAT(x) (EM_GET_INT_STAT_0+x)

uint32_t idt_tsi57x_em_get_int_stat  ( DAR_DEV_INFO_t             *dev_info, 
                                     idt_em_get_int_stat_in_t   *in_parms, 
                                     idt_em_get_int_stat_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t pnum; 
    uint8_t idx;
    uint32_t spx_int_stat, spx_err_stat, spx_err_det, spx_rate_en, spx_ctl_indep, spx_cs_int, spx_dloop;
    uint32_t regData;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    port_mac_relations_t *sw_pmr;
	struct DAR_ptl good_ptl;
    
   out_parms->imp_rc       = RIO_SUCCESS;
   out_parms->num_events   = 0;
   out_parms->too_many     = false;
   out_parms->other_events = false;

    if (( !in_parms->num_events                       ) 
       || (in_parms->num_events > EM_MAX_EVENT_LIST_SIZE )
       || (NULL == in_parms->events                   )) {
       out_parms->imp_rc = GET_INT_STAT(1);
       goto idt_tsi57x_em_get_int_stat_exit;
    };
    
	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = GET_INT_STAT(2);
          goto idt_tsi57x_em_get_int_stat_exit;
    };

	cfg_in.ptl = good_ptl;
    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_get_int_stat_exit;
    }

    if (!cfg_out.num_ports) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = GET_INT_STAT(3);
       goto idt_tsi57x_em_get_int_stat_exit;
    };

    rc = init_sw_pmr( dev_info, &sw_pmr );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = GET_INT_STAT(4);
       goto idt_tsi57x_em_get_int_stat_exit;
    }

   for (idx = 0; idx < cfg_out.num_ports; idx++) {
      if (!( cfg_out.pc[idx].port_available && cfg_out.pc[idx].powered_up ))
         continue;

      pnum = cfg_out.pc[idx].pnum;

      rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(sw_pmr[pnum].mac_num * 2),
                                                             &spx_dloop );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(5);
         goto idt_tsi57x_em_get_int_stat_exit;
      }

      rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &spx_ctl_indep );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(6);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      rc = DARRegRead( dev_info, Tsi578_SPX_INT_uint32_t(pnum), &spx_int_stat );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(7);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), &spx_err_stat );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(8);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &spx_err_det );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(9);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &spx_rate_en );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(0xA);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      rc = DARRegRead( dev_info, Tsi578_SPX_CS_INT_uint32_t(pnum), &spx_cs_int );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(0xB);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
              
      // Loss of signal events are a bit flakey, especially for rapid error detection.
      // Loss of Signal events only occur when the dead link timer has been enabled.
      // If a delineation error occurs and and "Output Fail" condition occurs as well,
      //   this indicates that rapid error detection has probably been invoked.
      // Otherwise, a loss of signal (DLT) event has probably occurred when a PORT_ERR 
      //   is detected without link response timeouts or illegal ackIDs in the link response.
      // Neither of these conditions is deterministic.

      if ( ((Tsi578_SPX_ERR_DET_DELIN_ERR      & spx_err_det ) && 
            (Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL & spx_err_stat)) 
       || ((spx_err_stat & Tsi578_SPX_ERR_uint32_t_PORT_ERR   ) &&
          !(spx_err_det  & (Tsi578_SPX_ERR_DET_LINK_TO | Tsi578_SPX_ERR_DET_LR_ACKID_ILL)))) {
         if ( spx_dloop & Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN ) {
            add_int_event( in_parms, out_parms, pnum, idt_em_f_los );
            if ((RIO_ALL_PORTS        != in_parms->ptl.num_ports) &&   // 
                (Tsi577_RIO_DEVID_VAL != DEV_CODE(dev_info)) && 
                (!sw_pmr[pnum].lane_count_4x           ) ) {
               add_int_event( in_parms, out_parms, sw_pmr[pnum].other_mac_ports[0], idt_em_f_port_err );
            }
         } else {
            out_parms->other_events = true;
         }
      };

      // Note: When the MAC is configured in two 1x mode, when PORT_ERR is set on the
      // odd MAC it is also set on the even MAC.  
      //
      // PORT_ERR occurs when:
      // - there are 4 consecutive link-response timeouts, or 
      // - 4 consecutive link-responses with illegal ackIDs.  
      // - dead link timer expiry on the odd port of a non-Tsi577 device, and this is
      //   the even port.
      // If none of these errors is present, do not report the PORT_ERR.
      // There is a hook to ensure that PORT_ERR is cleared on the EVEN MAC.
      
      if (( spx_err_stat & Tsi578_SPX_ERR_uint32_t_PORT_ERR   ) && 
          ( spx_err_det & (Tsi578_SPX_ERR_DET_LINK_TO | Tsi578_SPX_ERR_DET_LR_ACKID_ILL))) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_PORT_ERR_EN) {
            add_int_event( in_parms, out_parms, pnum, idt_em_f_port_err );
            if ((RIO_ALL_PORTS        != in_parms->ptl.num_ports) &&   // 
                (Tsi577_RIO_DEVID_VAL != DEV_CODE(dev_info)) && 
                (!sw_pmr[pnum].lane_count_4x           ) ) {
               add_int_event( in_parms, out_parms, sw_pmr[pnum].other_mac_ports[0], idt_em_f_port_err );
            }
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_MAX_RETRY   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN) {
            add_int_event( in_parms, out_parms, pnum, idt_em_f_2many_retx );
         } else {
            out_parms->other_events = true;
         }
      };

      // Check for too many PNA and ERR_RATE.  
      if (spx_err_stat & Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL) {
         if (Tsi578_SPX_ERR_DET_CS_NOT_ACC & spx_err_det & spx_rate_en) { 
            add_int_event( in_parms, out_parms, pnum, idt_em_f_2many_pna );
         };
         
         if (~EM_ERR_RATE_EVENT_EXCLUSIONS & spx_err_det & spx_rate_en) {
            add_int_event( in_parms, out_parms, pnum, idt_em_f_err_rate );
         };

         if (spx_err_det & ~spx_rate_en) {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_ILL_TRANS_ERR   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR) {
            add_int_event( in_parms, out_parms, pnum, idt_em_d_rte );
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_LINK_INIT_NOTIFICATION   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN) {
            add_int_event( in_parms, out_parms, pnum, idt_em_i_sig_det );
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_cs_int & Tsi578_SPX_CS_INT_uint32_t_RCS   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_RSVD1) {
            add_int_event( in_parms, out_parms, pnum, idt_em_i_rst_req );
         } else {
            out_parms->other_events = true;
         }
      };
   };

   // Logical Error interrupt occurs when logical layer error detected (rare)
   // and logical error events are enabled 
   rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET, &regData );
   if (RIO_SUCCESS != rc) {
      out_parms->imp_rc = GET_INT_STAT(0x10);
      goto idt_tsi57x_em_get_int_stat_exit;
   }
              
   if (regData) {
      uint32_t enData;

      rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET_EN, &enData );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(0x11);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
      if (regData & enData) {
         add_int_event( in_parms, out_parms, RIO_ALL_PORTS, idt_em_d_log );
      } else {
         out_parms->other_events = true;
      };
   };

   // Check for Initializatio (I2C Register loading) failure
     
   rc = DARRegRead( dev_info, Tsi578_I2C_INT_STAT, &regData );
   if (RIO_SUCCESS != rc) {
      out_parms->imp_rc = GET_INT_STAT(0x21);
      goto idt_tsi57x_em_get_int_stat_exit;
   }
              
   if (regData & Tsi578_I2C_INT_STAT_BL_FAIL) {
      rc = DARRegRead( dev_info, Tsi578_I2C_INT_ENABLE, &regData );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_INT_STAT(0x22);
         goto idt_tsi57x_em_get_int_stat_exit;
      }
      if (regData & Tsi578_I2C_INT_ENABLE_BL_FAIL) {
         add_int_event( in_parms, out_parms, RIO_ALL_PORTS, idt_em_i_init_fail );
      } else {
         out_parms->other_events = true;
      }
   }

idt_tsi57x_em_get_int_stat_exit:
   return rc;
};

#define GET_PW_STAT(x) (EM_GET_PW_STAT_0+x)

uint32_t idt_tsi57x_em_get_pw_stat  ( DAR_DEV_INFO_t            *dev_info, 
                                    idt_em_get_pw_stat_in_t   *in_parms, 
                                    idt_em_get_pw_stat_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t pnum; 
    uint8_t idx;
    uint32_t spx_int_stat, spx_err_stat, spx_err_det, spx_rate_en, spx_ctl_indep, spx_dloop, start_idx;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    uint32_t log_err_det, log_err_en;
	struct DAR_ptl good_ptl;
    
   out_parms->imp_rc       = RIO_SUCCESS;
   out_parms->num_events   = 0;
   out_parms->too_many     = false;
   out_parms->other_events = false;

    if (( !in_parms->num_events                          ) 
       || (in_parms->num_events > EM_MAX_EVENT_LIST_SIZE )
       || ((in_parms->pw_port_num >= TSI57X_NUM_PORTS(dev_info)) && 
           (RIO_ALL_PORTS != in_parms->pw_port_num      ))
       || (NULL == in_parms->events                      )) {
       out_parms->imp_rc = GET_PW_STAT(1);
       goto idt_tsi57x_em_get_pw_stat_exit;
    };
        
	rc = DARrioGetPortList( dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = GET_PW_STAT(2);
          goto idt_tsi57x_em_get_pw_stat_exit;
    };

	cfg_in.ptl = good_ptl;

    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_get_pw_stat_exit;
    };

    if (!cfg_out.num_ports) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = GET_PW_STAT(4);
       goto idt_tsi57x_em_get_pw_stat_exit;
    };

   // As the very first thing, check for a logical layer error.
   rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET, &log_err_det );
   if (RIO_SUCCESS != rc) {
      out_parms->imp_rc = GET_PW_STAT(0x10);
      goto idt_tsi57x_em_get_pw_stat_exit;
   }
   rc = DARRegRead( dev_info, Tsi578_RIO_LOG_ERR_DET_EN, &log_err_en );
   if (RIO_SUCCESS != rc) {
      out_parms->imp_rc = GET_PW_STAT(0x11);
      goto idt_tsi57x_em_get_pw_stat_exit;
   }

   if (log_err_det) {
     if ( log_err_en ) {
        add_pw_event( in_parms, out_parms, RIO_ALL_PORTS, idt_em_d_log );
      } else {
         out_parms->other_events = true;
      }
   }
   for (idx = 0; idx < cfg_out.num_ports; idx++) {
      if (!( cfg_out.pc[idx].port_available && cfg_out.pc[idx].powered_up ))
         continue;

      start_idx = out_parms->num_events;
      pnum = cfg_out.pc[idx].pnum;

      if (pnum == in_parms->pw_port_num)
        continue;

      rc = DARRegRead( dev_info, Tsi578_SMACX_DLOOP_CLK_SEL(pnum & 0xFE), &spx_dloop );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(4);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };

      rc = DARRegRead( dev_info, Tsi578_SPX_CTL_INDEP(pnum), &spx_ctl_indep );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(5);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };
              
      rc = DARRegRead( dev_info, Tsi578_SPX_INT_uint32_t(pnum), &spx_int_stat );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(6);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };
              
      rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), &spx_err_stat );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(7);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };
              
      rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &spx_err_det );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(8);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };
              
      rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &spx_rate_en );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = GET_PW_STAT(8);
         goto idt_tsi57x_em_get_pw_stat_exit;
      };
              
      // Loss of signal events are a bit flakey, especially for rapid error detection.
      // Loss of Signal events only occur when the dead link timer has been enabled.
      // If a delineation error occurs and and "Output Fail" condition occurs as well,
      //   this indicates that rapid error detection has probably been invoked.
      // Otherwise, a loss of signal (DLT) event has probably occurred when a PORT_ERR 
      //   is detected without link response timeouts or illegal ackIDs in the link response.
      // Neither of these conditions is deterministic.
      if (((spx_err_det & Tsi578_SPX_ERR_DET_DELIN_ERR      ) &&
           (spx_err_stat & Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL))
       || ((spx_err_stat & Tsi578_SPX_ERR_uint32_t_PORT_ERR   ) &&
         !(spx_err_det & (Tsi578_SPX_ERR_DET_LINK_TO | Tsi578_SPX_ERR_DET_LR_ACKID_ILL)))) {
         if ( spx_dloop & Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN) {
            add_pw_event( in_parms, out_parms, pnum, idt_em_f_los );
         } else {
            out_parms->other_events = true;
         }
      };

      if (spx_err_stat & Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL) {
         uint32_t errs = spx_err_det & spx_rate_en;

         if (Tsi578_SPX_ERR_DET_CS_NOT_ACC & errs) { 
            add_pw_event( in_parms, out_parms, pnum, idt_em_f_2many_pna );
         };
         if (~EM_ERR_RATE_EVENT_EXCLUSIONS & errs) {
            add_pw_event( in_parms, out_parms, pnum, idt_em_f_err_rate );
         };

         if (spx_err_det & ~spx_rate_en) {
            out_parms->other_events = true;
         }
      };

      // Note: When the MAC is configured in two 1x mode, when PORT_ERR is set on the
      // odd MAC it is also set on the even MAC.  
      //
      // PORT_ERR only occurs when there are too many timeouts or link-responses with
      // illegal ackIDs.  If neither of these errors is present, do not report the PORT_ERR.
      // There is a special little hook to ensure that PORT_ERR is cleared on the EVEN MAC.
      
      if (( spx_err_stat & Tsi578_SPX_ERR_uint32_t_PORT_ERR   ) && 
          ( spx_err_det & (Tsi578_SPX_ERR_DET_LINK_TO | Tsi578_SPX_ERR_DET_LR_ACKID_ILL))) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_PORT_ERR_EN) {
            add_pw_event( in_parms, out_parms, pnum, idt_em_f_port_err );
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_MAX_RETRY   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN) {
           add_pw_event( in_parms, out_parms, pnum, idt_em_f_2many_retx );
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_ILL_TRANS_ERR   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR) {
            add_pw_event( in_parms, out_parms, pnum, idt_em_d_rte );
         } else {
            out_parms->other_events = true;
         }
      };

      if ( spx_int_stat & Tsi578_SPX_INT_uint32_t_LINK_INIT_NOTIFICATION   ) {
         if (spx_ctl_indep & Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN) {
           add_pw_event( in_parms, out_parms, pnum, idt_em_i_sig_det );
         } else {
            out_parms->other_events = true;
         }
      };

      // NOTE: this must be the last per-port event added to the list.
      // If not, it is possible that port-write clearing will not operate as
      // expected.
      //
      // Note: Clear Port-write-pending whether or not there are events
      // reported by the port.  
      if ((start_idx != out_parms->num_events) ||
          (spx_err_stat & Tsi578_SPX_ERR_uint32_t_PORT_W_PEND)) {
         add_pw_event( in_parms, out_parms, pnum, idt_em_a_clr_pwpnd );
      };

      // Reset request cannot trigger a port write.
      // I2C bootload failure cannot trigger a port write.
   };

   if ((RIO_ALL_PORTS != in_parms->pw_port_num) && (!out_parms->too_many)) {
      // Recursively request events for the last port.                  
      // Note that if successful, this will detect a logical layer error.
      idt_em_get_pw_stat_in_t  last_in_parms;
      idt_em_get_pw_stat_out_t last_out_parms;


      last_in_parms.ptl.num_ports = 1;
	  last_in_parms.ptl.pnums[0] = in_parms->pw_port_num;
      last_in_parms.pw_port_num = RIO_ALL_PORTS;
      last_in_parms.num_events  = in_parms->num_events - out_parms->num_events;
      last_in_parms.events      = &(in_parms->events[out_parms->num_events]);

      rc = idt_tsi57x_em_get_pw_stat( dev_info, &last_in_parms, &last_out_parms );
      if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = last_out_parms.imp_rc;
         goto idt_tsi57x_em_get_pw_stat_exit;
      };

      if (last_out_parms.num_events) {
         out_parms->num_events   += last_out_parms.num_events;
         out_parms->too_many      = last_out_parms.too_many;
         out_parms->other_events |= last_out_parms.other_events;
      };
   } else {

   }


idt_tsi57x_em_get_pw_stat_exit:
   return rc;
};

uint32_t idt_tsi57x_em_clr_events   ( DAR_DEV_INFO_t           *dev_info, 
                                    idt_em_clr_events_in_t   *in_parms, 
                                    idt_em_clr_events_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    int pnum; 
    int idx;
    bool clear_port_fail = false;
    uint32_t regData;
    
    out_parms->imp_rc            = RIO_SUCCESS;
    out_parms->failure_idx       = 0;
    out_parms->pw_events_remain  = false;
    out_parms->int_events_remain = false;

    if (( !in_parms->num_events                          ) 
       || (in_parms->num_events > EM_MAX_EVENT_LIST_SIZE )
       || (NULL == in_parms->events                      )) {
       out_parms->imp_rc = EM_CLR_EVENTS(1);
       goto idt_tsi57x_em_clr_events_exit;
    };
        
    for (idx = 0; idx < in_parms->num_events; idx++) {
       out_parms->failure_idx = idx;
       pnum = in_parms->events[idx].port_num;
       // Complex statement below enforces 
       // - valid port number values
       // - RIO_ALL_PORTS cannot be used with any other events.
       // - RIO_ALL_PORTS must be used with idt_em_d_log and idt_em_i_init_fail.
       // - valid event values
       if ( ((pnum >= TSI57X_NUM_PORTS(dev_info)) && (RIO_ALL_PORTS != pnum)) || 
          ((RIO_ALL_PORTS == pnum) && !((idt_em_d_log == in_parms->events[idx].event)
                                     ||(idt_em_i_init_fail == in_parms->events[idx].event)))  ||
          (((idt_em_d_log       == in_parms->events[idx].event) ||
            (idt_em_i_init_fail == in_parms->events[idx].event)) && !(RIO_ALL_PORTS == pnum)) ||
           (idt_em_last <= in_parms->events[idx].event) ) {
          rc = RIO_ERR_INVALID_PARAMETER;
          out_parms->imp_rc = EM_CLR_EVENTS(2);
          goto idt_tsi57x_em_clr_events_exit;
       };

       switch (in_parms->events[idx].event) {       
          case idt_em_f_los       : // Clear DELIN_ERR 
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(3);
                  goto idt_tsi57x_em_clr_events_exit;
               }

               regData &= ~( Tsi578_SPX_ERR_DET_DELIN_ERR );

               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regData );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(4);
                  goto idt_tsi57x_em_clr_events_exit;
               }
               clear_port_fail = true;
               break;

          case idt_em_f_port_err  : // PORT_ERR occurs due to multiple causes,
               clear_port_fail = true;
               break;

          case idt_em_f_2many_retx: 
               //  Clear MAX_RETRY and IMP_SPEC_ERR events
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_uint32_t(pnum), Tsi578_SPX_INT_uint32_t_MAX_RETRY);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x10);
                  goto idt_tsi57x_em_clr_events_exit;
               }
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regData );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x11);
                  goto idt_tsi57x_em_clr_events_exit;
               }

               regData &= ~(Tsi578_SPX_RATE_EN_IMP_SPEC_ERR);

               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regData );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x12);
                  goto idt_tsi57x_em_clr_events_exit;
               }
               clear_port_fail = true;
               break;

         case idt_em_i_init_fail :
               rc = DARRegWrite( dev_info, Tsi578_I2C_INT_STAT, Tsi578_I2C_INT_STAT_BL_FAIL);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x13);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              break;

         case idt_em_d_ttl       : // Do nothing
              break;

         case idt_em_d_rte       :
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_uint32_t(pnum), Tsi578_SPX_INT_uint32_t_ILL_TRANS_ERR);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x20);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              break;

         case idt_em_f_2many_pna :
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regData);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x22);
                  goto idt_tsi57x_em_clr_events_exit;
               };
               regData &= ~Tsi578_SPX_ERR_DET_CS_NOT_ACC;
               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regData);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x23);
                  goto idt_tsi57x_em_clr_events_exit;
               };
              clear_port_fail = true;
              break;
         case idt_em_f_err_rate  :
               // Clear all events that contribute to the fatal error rate event
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regData);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x24);
                  goto idt_tsi57x_em_clr_events_exit;
               };
               regData &= EM_ERR_RATE_EVENT_EXCLUSIONS;
               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regData);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x25);
                  goto idt_tsi57x_em_clr_events_exit;
               };
              clear_port_fail = true;
              break;
                                     
         case idt_em_d_log       : // Clear all logical layer errors, must write 0
                                   // Register will not clear if the value written is != 0.
               rc = DARRegWrite( dev_info, Tsi578_RIO_LOG_ERR_DET, 0 );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x26);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              break;

         case idt_em_i_sig_det   :
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_uint32_t(pnum), Tsi578_SPX_INT_uint32_t_LINK_INIT_NOTIFICATION);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x28);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              break;

         case idt_em_i_rst_req   :
               rc = DARRegWrite( dev_info, Tsi578_SPX_ACKID_STAT(pnum)   , Tsi578_SPX_ACKID_STAT_CLR_PKTS );  
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x2A);
                  goto idt_tsi57x_em_clr_events_exit;
               }
               rc = DARRegWrite( dev_info, Tsi578_SPX_CS_INT_uint32_t(pnum), Tsi578_SPX_CS_INT_uint32_t_RCS);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x2B);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              break;

         case idt_em_a_clr_pwpnd  :
              { uint32_t err_stat;
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), &err_stat);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x2C);
                  goto idt_tsi57x_em_clr_events_exit;
               }
               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), err_stat | Tsi578_SPX_ERR_uint32_t_PORT_W_PEND);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = EM_CLR_EVENTS(0x2E);
                  goto idt_tsi57x_em_clr_events_exit;
               }
              };
              break;

         case idt_em_a_no_event  :
              break;

         default:
               out_parms->imp_rc = EM_CLR_EVENTS(0x2F);
               rc = RIO_ERR_INVALID_PARAMETER;
               goto idt_tsi57x_em_clr_events_exit;
              break;
       };

       if (clear_port_fail) {
          // Clear AckID tracking register   
          rc = DARRegWrite( dev_info, Tsi578_SPX_ACKID_STAT(pnum), Tsi578_SPX_ACKID_STAT_CLR_PKTS );  
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = EM_CLR_EVENTS(0x30);
             goto idt_tsi57x_em_clr_events_exit;
          }

          // Clear PORT_FAIL counter
          rc = DARRegRead( dev_info, Tsi578_SPX_ERR_RATE(pnum), &regData );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = EM_CLR_EVENTS(0x31);
             goto idt_tsi57x_em_clr_events_exit;
          }

          // Clear counter and peak value, retain RR & RB settings
          regData &= ~(Tsi578_SPX_ERR_RATE_ERR_RATE_CNT | Tsi578_SPX_ERR_RATE_PEAK);

          rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_RATE(pnum), regData );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = EM_CLR_EVENTS(0x32);
             goto idt_tsi57x_em_clr_events_exit;
          }

          clear_port_fail = false;
          rc = DARRegRead( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), &regData );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = EM_CLR_EVENTS(0x33);
             goto idt_tsi57x_em_clr_events_exit;
          }

          // Clear all error conditions, except the port write pending indication
          rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_uint32_t(pnum), regData & ~Tsi578_SPX_ERR_uint32_t_PORT_W_PEND);
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = EM_CLR_EVENTS(0x34);
             goto idt_tsi57x_em_clr_events_exit;
          }
       };
    };

    out_parms->failure_idx = EM_MAX_EVENT_LIST_SIZE;

idt_tsi57x_em_clr_events_exit:
   return rc;
};

#define CREATE_RATE(x) (EM_CREATE_RATE_0+x)

uint32_t create_rate_event( DAR_DEV_INFO_t             *dev_info, 
                          uint8_t                       pnum, 
                          uint32_t                      spx_rate_en,
                          uint32_t                     *imp_err )
{
    uint32_t rc;
    uint32_t regVal, limit, idx;

    rc = DARRegRead( dev_info, Tsi578_SPX_RATE_EN(pnum), &regVal );
    if (RIO_SUCCESS != rc) {
       *imp_err = CREATE_RATE(0x80);
       goto create_rate_event_exit;
    };

    // If requested event is not enabled for counting, can't create the event.
    if (!(spx_rate_en & regVal)) {
       rc = RIO_ERR_NOT_SUP_BY_CONFIG;
       *imp_err = CREATE_RATE(0x81);
       goto create_rate_event_exit;
    };
    
    rc = DARRegRead( dev_info, Tsi578_SPX_ERR_THRESH(pnum), &regVal );
    if (RIO_SUCCESS != rc) {
       *imp_err = CREATE_RATE(0x82);
       goto create_rate_event_exit;
    };

    limit = (regVal & Tsi578_SPX_ERR_THRESH_ERR_RFT) >> 24;
    // If there is no error threshold set, cannot create event.
    if (!limit) {
       rc = RIO_ERR_NOT_SUP_BY_CONFIG;
       *imp_err = CREATE_RATE(0x83);
       goto create_rate_event_exit;
    };

    // Create the event.  Preserve other events in the error detect register.
    // Note that this may fail if a "fast" leak rate is set.
    // Note that this may be overkill if multiple events occur for each write.
    
    for (idx = 0; idx < limit; idx++) {
       rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regVal );
       if (RIO_SUCCESS != rc) {
          *imp_err = CREATE_RATE(0x84);
          goto create_rate_event_exit;
       };

       rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regVal | spx_rate_en );
       if (RIO_SUCCESS != rc) {
          *imp_err = CREATE_RATE(0x85);
          goto create_rate_event_exit;
       };
    };

create_rate_event_exit:
    return rc;

};
   
#define CREATE_EVENTS(x) (EM_CREATE_EVENTS_0+x)

uint32_t idt_tsi57x_em_create_events( DAR_DEV_INFO_t              *dev_info, 
                                    idt_em_create_events_in_t   *in_parms, 
                                    idt_em_create_events_out_t  *out_parms ) 
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t regVal;
    uint8_t  pnum; 
    uint8_t  idx;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    
    out_parms->failure_idx = 0;
    out_parms->imp_rc = RIO_SUCCESS;

    if (( !in_parms->num_events                       ) 
       || (in_parms->num_events > EM_MAX_EVENT_LIST_SIZE )
       || (NULL == in_parms->events                   )) {
       out_parms->imp_rc = CREATE_EVENTS(1);
       goto idt_tsi57x_em_create_events_exit;
    };
        
	cfg_in.ptl.num_ports = RIO_ALL_PORTS;

    rc = idt_tsi57x_pc_get_config( dev_info, &cfg_in, &cfg_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto idt_tsi57x_em_create_events_exit;
    };

    if (!cfg_out.num_ports) {
       rc = RIO_ERR_NO_PORT_AVAIL;
       out_parms->imp_rc = CREATE_EVENTS(3);
       goto idt_tsi57x_em_create_events_exit;
    };

    for (idx = 0; idx < in_parms->num_events; idx++) {
       out_parms->failure_idx = idx;
       pnum = in_parms->events[idx].port_num;

       if ( ((pnum >= TSI57X_NUM_PORTS(dev_info)) && (RIO_ALL_PORTS != pnum)) || 
          ((RIO_ALL_PORTS == pnum) && !((idt_em_d_log       == in_parms->events[idx].event) ||
                                        (idt_em_i_init_fail == in_parms->events[idx].event)))  ||
          (((idt_em_d_log       == in_parms->events[idx].event) ||
            (idt_em_i_init_fail == in_parms->events[idx].event)) && !(RIO_ALL_PORTS == pnum)) ||
           (idt_em_last <= in_parms->events[idx].event                                    ) ) {
          rc = RIO_ERR_INVALID_PARAMETER;
          out_parms->imp_rc = CREATE_EVENTS(2);
          goto idt_tsi57x_em_create_events_exit;
       };

       if (RIO_ALL_PORTS != pnum) {
          if (!( cfg_out.pc[pnum].port_available && cfg_out.pc[pnum].powered_up ))
             continue;
       }

       switch (in_parms->events[idx].event) {       
          case idt_em_f_los       : 
              // LOS is equal to a DELIN_ERR  
               rc = DARRegRead( dev_info, Tsi578_SPX_ERR_DET(pnum), &regVal);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x10);
                  goto idt_tsi57x_em_create_events_exit;
               };
               rc = DARRegWrite( dev_info, Tsi578_SPX_ERR_DET(pnum), regVal | Tsi578_SPX_ERR_DET_DELIN_ERR );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x11);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

          case idt_em_f_port_err  :  // Can't create a PORT_ERR
               rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
               out_parms->imp_rc = CREATE_EVENTS(0x12);
               goto idt_tsi57x_em_create_events_exit;
              break;

         case idt_em_f_2many_retx: 
               rc = DARRegRead( dev_info, Tsi578_SPX_INT_uint32_t(pnum), &regVal);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x13);
                  goto idt_tsi57x_em_create_events_exit;
               };
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_GEN(pnum), regVal | Tsi578_SPX_INT_GEN_MAX_RETRY_GEN);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x14);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

         case idt_em_f_2many_pna:
              rc = create_rate_event( dev_info, pnum, Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN, &out_parms->imp_rc );
              if (RIO_SUCCESS != rc) {
                 goto idt_tsi57x_em_create_events_exit;
              };
              break;

         case idt_em_f_err_rate :
              rc = create_rate_event( dev_info, pnum, ~EM_ERR_RATE_EVENT_EXCLUSIONS, &out_parms->imp_rc );
              if (RIO_SUCCESS != rc) {
                 goto idt_tsi57x_em_create_events_exit;
              };
              break;

         case idt_em_d_ttl       : // Do nothing
               rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
               out_parms->imp_rc = CREATE_EVENTS(0x21);
               goto idt_tsi57x_em_create_events_exit;
              break;

         case idt_em_d_rte       :
               rc = DARRegRead( dev_info, Tsi578_SPX_INT_uint32_t(pnum), &regVal);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x22);
                  goto idt_tsi57x_em_create_events_exit;
               };
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_GEN(pnum), regVal | Tsi578_SPX_INT_GEN_ILL_TRANS_GEN);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x23);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

         case idt_em_d_log       : // Set all logical layer errors
               rc = DARRegWrite( dev_info, Tsi578_RIO_LOG_ERR_DET, TSI57X_ALL_LOG_ERRS );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x30);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

         case idt_em_i_sig_det   :
               rc = DARRegRead( dev_info, Tsi578_SPX_INT_uint32_t(pnum), &regVal);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x31);
                  goto idt_tsi57x_em_create_events_exit;
               };
               rc = DARRegWrite( dev_info, Tsi578_SPX_INT_GEN(pnum), regVal | Tsi578_SPX_INT_GEN_LINK_INIT_NOTIFICATION_GEN);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x32);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

         case idt_em_i_rst_req   :
               rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
               out_parms->imp_rc = CREATE_EVENTS(0x33);
               goto idt_tsi57x_em_create_events_exit;
              break;

         case idt_em_i_init_fail :
               rc = DARRegWrite( dev_info, Tsi578_I2C_INT_SET, Tsi578_I2C_INT_SET_BL_FAIL);
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = CREATE_EVENTS(0x20);
                  goto idt_tsi57x_em_create_events_exit;
               };
              break;

         default:
               rc = RIO_ERR_INVALID_PARAMETER;
               out_parms->imp_rc = CREATE_EVENTS(0x34);
               goto idt_tsi57x_em_create_events_exit;
              break;
       };
    };

    out_parms->failure_idx = EM_MAX_EVENT_LIST_SIZE;

idt_tsi57x_em_create_events_exit:
   return rc;

};

uint32_t idt_tsi57x_sc_init_dev_ctrs ( DAR_DEV_INFO_t             *dev_info,
                                     idt_sc_init_dev_ctrs_in_t  *in_parms,
                                     idt_sc_init_dev_ctrs_out_t *out_parms)
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   idt_sc_ctr_val_t init_val = INIT_IDT_SC_CTR_VAL;
   uint8_t idx, cntr_i;
   struct DAR_ptl good_ptl;

   out_parms->imp_rc = RIO_SUCCESS;

   if (NULL == in_parms->dev_ctrs) {
      out_parms->imp_rc = SC_INIT_DEV_CTRS(1);
      goto idt_tsi57x_sc_init_dev_ctrs_exit;
   };

   if (NULL == in_parms->dev_ctrs->p_ctrs) {
      out_parms->imp_rc = SC_INIT_DEV_CTRS(2);
      goto idt_tsi57x_sc_init_dev_ctrs_exit;
   };

   if (  !in_parms->dev_ctrs->num_p_ctrs || 
         (in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS)) {
      out_parms->imp_rc = SC_INIT_DEV_CTRS(3);
      goto idt_tsi57x_sc_init_dev_ctrs_exit;
   };

   if (((RIO_ALL_PORTS == in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < TSI57X_NUM_PORTS(dev_info))) || 
       ((RIO_ALL_PORTS != in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < in_parms->ptl.num_ports))) {
      out_parms->imp_rc =SC_INIT_DEV_CTRS(4); 
      goto idt_tsi57x_sc_init_dev_ctrs_exit;
   };

   rc = DARrioGetPortList( dev_info, &in_parms->ptl, &good_ptl );
   if (RIO_SUCCESS != rc) {
	   out_parms->imp_rc = SC_INIT_DEV_CTRS(0x10);
      goto idt_tsi57x_sc_init_dev_ctrs_exit;
   };

   in_parms->dev_ctrs->valid_p_ctrs = good_ptl.num_ports;
   for (idx = 0; idx < good_ptl.num_ports; idx++) {
      in_parms->dev_ctrs->p_ctrs[idx].pnum = good_ptl.pnums[idx];
      in_parms->dev_ctrs->p_ctrs[idx].ctrs_cnt = Tsi578_NUM_PERF_CTRS;
      for (cntr_i = 0; cntr_i < Tsi578_NUM_PERF_CTRS; cntr_i++) {
        in_parms->dev_ctrs->p_ctrs[idx].ctrs[cntr_i] = init_val;
      };
   };

   rc = RIO_SUCCESS;

idt_tsi57x_sc_init_dev_ctrs_exit:
   return rc;
};

/* Reads enabled counters on selected ports   
*/

uint32_t idt_tsi57x_sc_read_ctrs( DAR_DEV_INFO_t           *dev_info,
                                idt_sc_read_ctrs_in_t    *in_parms,
                                idt_sc_read_ctrs_out_t   *out_parms)
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint8_t p_to_i[Tsi578_MAX_PORTS] = {IDT_MAX_PORTS};
   uint8_t srch_i, srch_p, port_num, cntr;
   bool found;
   struct DAR_ptl good_ptl;

   out_parms->imp_rc = RIO_SUCCESS;

   if (NULL == in_parms->dev_ctrs) {
      out_parms->imp_rc = SC_READ_CTRS(0x01);
      goto idt_tsi57x_sc_read_ctrs_exit;
   };

   if (NULL == in_parms->dev_ctrs->p_ctrs) {
      out_parms->imp_rc = SC_READ_CTRS(0x02);
      goto idt_tsi57x_sc_read_ctrs_exit;
   };

   if (  !in_parms->dev_ctrs->num_p_ctrs || 
         (in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		 (in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
      out_parms->imp_rc = SC_READ_CTRS(0x03);
      goto idt_tsi57x_sc_read_ctrs_exit;
   };

   rc = DARrioGetPortList( dev_info, &in_parms->ptl, &good_ptl );
   if (RIO_SUCCESS != rc) {
	  out_parms->imp_rc = SC_READ_CTRS(0x30);
      goto idt_tsi57x_sc_read_ctrs_exit;
   };


   if ((in_parms->dev_ctrs->num_p_ctrs   < good_ptl.num_ports) ||
       (in_parms->dev_ctrs->valid_p_ctrs < good_ptl.num_ports)) {
      rc = RIO_ERR_INVALID_PARAMETER;
      out_parms->imp_rc = SC_READ_CTRS(0x04);
      goto idt_tsi57x_sc_read_ctrs_exit;
   };

   // For generality, must establish a list of ports.
   // Do not assume that the port number equals the index in the structure...
      
   for (srch_p = 0; srch_p < good_ptl.num_ports; srch_p ++) {
      port_num = good_ptl.pnums[srch_p];
	  found = false;
      for (srch_i = 0; srch_i < in_parms->dev_ctrs->valid_p_ctrs; srch_i++) {
         if ( in_parms->dev_ctrs->p_ctrs[srch_i].pnum == port_num ) {
			found = true;
            // If the port hasn't previously been read and the counter structure is
            // correctly initialized, keep going...
            if ((IDT_MAX_PORTS        == p_to_i[port_num]                           ) && 
                (Tsi578_NUM_PERF_CTRS == in_parms->dev_ctrs->p_ctrs[srch_i].ctrs_cnt)) {
               p_to_i[port_num] = srch_i;
            } else {
               // Port number appears multiple times in the list,
               // or number of performance counters is incorrect/uninitialized...
               rc = RIO_ERR_INVALID_PARAMETER;
               out_parms->imp_rc = SC_READ_CTRS(0x50 + port_num);
               goto idt_tsi57x_sc_read_ctrs_exit;
            };

            // Read the port performance counters...
            for (cntr = 0; cntr < Tsi578_NUM_PERF_CTRS; cntr++) {
               if (idt_sc_disabled != in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[cntr].sc) {
                  rc = DARRegRead( dev_info, Tsi578_SPX_PSCY(port_num, cntr), &in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[cntr].last_inc);
                  if (RIO_SUCCESS != rc) {
                     rc = RIO_ERR_INVALID_PARAMETER;
                     out_parms->imp_rc = SC_READ_CTRS(0x70 + cntr);
                     goto idt_tsi57x_sc_read_ctrs_exit;
                  };
                  in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[cntr].total += in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[cntr].last_inc;
               };
            };
         };
      };
	  if (!found) {
		  rc = RIO_ERR_INVALID_PARAMETER;
		  out_parms->imp_rc = SC_READ_CTRS(0x90 + srch_p);
          goto idt_tsi57x_sc_read_ctrs_exit;
      };
   };

idt_tsi57x_sc_read_ctrs_exit:
   return rc;
};

/* Configure counters on selected ports of a 
 * Tsi device.
 */


uint8_t tsi57x_ctl_to_ctr_t[8] = {
   idt_sc_uc_req_pkts,
   idt_sc_uc_pkts,
   idt_sc_retries,
   idt_sc_all_cs,
   idt_sc_uc_4b_data,
   idt_sc_mc_pkts,
   idt_sc_mecs,
   idt_sc_mc_4b_data
};

#define PRIO_MASK (Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO0 | \
                   Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO1 | \
                   Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO2 | \
                   Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO3)

uint32_t idt_sc_cfg_tsi57x_ctr ( DAR_DEV_INFO_t              *dev_info,
                               idt_sc_cfg_tsi57x_ctr_in_t  *in_parms,
                               idt_sc_cfg_tsi57x_ctr_out_t *out_parms )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t new_ctl = 0, ctl_reg, new_ctl_reg, reg_mask;
   uint8_t p_to_i[Tsi578_MAX_PORTS] = {IDT_MAX_PORTS};
   uint8_t srch_i, srch_p, port_num;
   bool  found;
   struct DAR_ptl good_ptl;

   out_parms->imp_rc = RIO_SUCCESS;
  
   if (NULL == in_parms->dev_ctrs) {
      out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x01);
      goto idt_sc_cfg_tsi57x_ctr_exit;
   };

   if (NULL == in_parms->dev_ctrs->p_ctrs) {
      out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x02);
      goto idt_sc_cfg_tsi57x_ctr_exit;
   };

  rc = DARrioGetPortList( dev_info, &in_parms->ptl, &good_ptl );
  if (RIO_SUCCESS != rc) {
	  out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x4);
	 goto idt_sc_cfg_tsi57x_ctr_exit;
  };

   rc = RIO_ERR_INVALID_PARAMETER;

   if (  !in_parms->dev_ctrs->num_p_ctrs || 
         (in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		 (in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
      out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x06);
      goto idt_sc_cfg_tsi57x_ctr_exit;
   };

   if ((in_parms->dev_ctrs->num_p_ctrs   < good_ptl.num_ports) ||
	   (in_parms->dev_ctrs->valid_p_ctrs < good_ptl.num_ports)) {
      out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x08);
      goto idt_sc_cfg_tsi57x_ctr_exit;
   };

   // Create SC_CTL
   if (idt_sc_disabled != in_parms->ctr_type) {
      new_ctl  = ((uint32_t)(in_parms->prio_mask) << 12) & PRIO_MASK;
      // It is a programming error to have an empty priority mask when counting
      // packets or packet data...
      if (!new_ctl && ((idt_sc_uc_req_pkts == in_parms->ctr_type) ||
                       (idt_sc_uc_pkts     == in_parms->ctr_type) ||
                       (idt_sc_uc_4b_data  == in_parms->ctr_type) ||
                       (idt_sc_mc_pkts     == in_parms->ctr_type) ||
                       (idt_sc_mc_4b_data  == in_parms->ctr_type)))  {
         out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x20);
         goto idt_sc_cfg_tsi57x_ctr_exit;
      };
      new_ctl |=   (in_parms->tx)? Tsi578_SPX_PSC0n1_CTRL_PS1_DIR : 0;
      switch (in_parms->ctr_type) {
         case idt_sc_uc_req_pkts :                               break; 
         case idt_sc_uc_pkts     :  new_ctl |= 0x1            ;  break;
         case idt_sc_retries     :  new_ctl |= 0x2 | PRIO_MASK;  break;
         case idt_sc_all_cs      :  new_ctl |= 0x3 | PRIO_MASK;  break;
         case idt_sc_uc_4b_data  :  new_ctl |= 0x4            ;  break;
         case idt_sc_mc_pkts     :  new_ctl |= 0x5            ;  break;
         case idt_sc_mecs        :  new_ctl |= 0x6 | PRIO_MASK;  break;
         case idt_sc_mc_4b_data  :  new_ctl |= 0x7            ;  break;
         default: out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x30);
                  goto idt_sc_cfg_tsi57x_ctr_exit;
      };
   };

   if (in_parms->ctr_idx & 1) {
      reg_mask = 0x0000FFFF;
   } else {
      reg_mask = 0xFFFF0000;
      new_ctl  = new_ctl << 16;
   };

   // Update hardware and data structures to reflect change in programming.
   // - program the counter control values requested
   // - update the associated counters structure (what is counted)
   // - clear the counter value in the counters structure if what is counted changed
   // - clear the physical counter value if what is counted changed
   //
   for (srch_p = 0; srch_p < good_ptl.num_ports; srch_p ++) {
	   port_num = good_ptl.pnums[srch_p];
	  found = false;
      for (srch_i = 0; srch_i < in_parms->dev_ctrs->valid_p_ctrs; srch_i++) {
         if ( in_parms->dev_ctrs->p_ctrs[srch_i].pnum == port_num )  {
			found = true;
            // If the port hasn't previously been programmed and the counter structure is
            // correctly initialized, keep going...
            if ((IDT_MAX_PORTS        == p_to_i[port_num]                           ) && 
                (Tsi578_NUM_PERF_CTRS == in_parms->dev_ctrs->p_ctrs[srch_i].ctrs_cnt)) {
               p_to_i[port_num] = srch_i;
            } else {
               // Port number appears multiple times in the list,
               // or number of performance counters is incorrect/uninitialized...
		rc = RIO_ERR_INVALID_PARAMETER;
               out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x40 + port_num);
               goto idt_sc_cfg_tsi57x_ctr_exit;
            };
         
            // Always program the control value...
            rc = DARRegRead( dev_info, Tsi578_SPX_PSC_CTRL(port_num, in_parms->ctr_idx), &ctl_reg );
            if (RIO_SUCCESS != rc) {
               out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x50);
               goto idt_sc_cfg_tsi57x_ctr_exit;
            };
            new_ctl_reg  = ctl_reg & ~reg_mask;
            new_ctl_reg |= new_ctl;
            rc = DARRegWrite( dev_info, Tsi578_SPX_PSC_CTRL(port_num, in_parms->ctr_idx), new_ctl_reg );
            if (RIO_SUCCESS != rc) {
               out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x51);
               goto idt_sc_cfg_tsi57x_ctr_exit;
            };
            
            if ((in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].sc != in_parms->ctr_type) ||
                (in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].tx != in_parms->tx) ||
                ( ctl_reg != new_ctl_reg )) {
               // If the counted value has changed in the structure or in hardware, 
               // zero the counters and zero hardware counters
               in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].sc = in_parms->ctr_type;
               in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].tx = in_parms->tx;
               in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].total = 0;
               in_parms->dev_ctrs->p_ctrs[srch_i].ctrs[in_parms->ctr_idx].last_inc = 0;
               rc = DARRegRead( dev_info, Tsi578_SPX_PSCY(port_num, in_parms->ctr_idx), &ctl_reg );
               if (RIO_SUCCESS != rc) {
                  out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x52);
                  goto idt_sc_cfg_tsi57x_ctr_exit;
               };
            };
         };
      };
	  if (!found) {
		  rc = RIO_ERR_INVALID_PARAMETER;
		  out_parms->imp_rc = SC_CFG_TSI57X_CTR(0x53);
          goto idt_sc_cfg_tsi57x_ctr_exit;
      };
   };

idt_sc_cfg_tsi57x_ctr_exit:
   return rc;        
};

/* Routine to bind in all Tsi57x specific Device Specific Function routines.
   Supports Tsi574, Tsi576, Tsi577, Tsi578
*/

uint32_t bind_tsi57x_DSF_support( void )
{
    IDT_DSF_DB_t idt_driver;
    
	IDT_DSF_init_driver( &idt_driver );

    idt_driver.dev_type = 0x0570;

    idt_driver.idt_pc_clr_errs           = idt_tsi57x_pc_clr_errs;
    idt_driver.idt_pc_dev_reset_config   = idt_tsi57x_pc_dev_reset_config;
    idt_driver.idt_pc_get_config         = idt_tsi57x_pc_get_config;
    idt_driver.idt_pc_get_status         = idt_tsi57x_pc_get_status;
    idt_driver.idt_pc_reset_link_partner = idt_tsi57x_pc_reset_link_partner;
    idt_driver.idt_pc_reset_port         = idt_tsi57x_pc_reset_port;
    idt_driver.idt_pc_secure_port        = idt_tsi57x_pc_secure_port;
    idt_driver.idt_pc_set_config         = idt_tsi57x_pc_set_config;
    idt_driver.idt_pc_probe              = default_idt_pc_probe;

    idt_driver.idt_rt_initialize      = idt_tsi57x_rt_initialize;
    idt_driver.idt_rt_probe           = idt_tsi57x_rt_probe;
    idt_driver.idt_rt_probe_all       = idt_tsi57x_rt_probe_all;
    idt_driver.idt_rt_set_all         = idt_tsi57x_rt_set_all;
    idt_driver.idt_rt_set_changed     = idt_tsi57x_rt_set_changed;
    idt_driver.idt_rt_alloc_mc_mask   = IDT_DSF_rt_alloc_mc_mask;
    idt_driver.idt_rt_dealloc_mc_mask = IDT_DSF_rt_dealloc_mc_mask;
    idt_driver.idt_rt_change_rte      = idt_tsi57x_rt_change_rte;
    idt_driver.idt_rt_change_mc_mask  = idt_tsi57x_rt_change_mc_mask;

    idt_driver.idt_em_cfg_pw       = idt_tsi57x_em_cfg_pw       ;
    idt_driver.idt_em_cfg_set      = idt_tsi57x_em_cfg_set      ;
    idt_driver.idt_em_cfg_get      = idt_tsi57x_em_cfg_get      ;
    idt_driver.idt_em_dev_rpt_ctl  = idt_tsi57x_em_dev_rpt_ctl  ;
    idt_driver.idt_em_parse_pw     = idt_tsi57x_em_parse_pw     ;
    idt_driver.idt_em_get_int_stat = idt_tsi57x_em_get_int_stat ;
    idt_driver.idt_em_get_pw_stat  = idt_tsi57x_em_get_pw_stat  ;
    idt_driver.idt_em_clr_events   = idt_tsi57x_em_clr_events   ;
    idt_driver.idt_em_create_events= idt_tsi57x_em_create_events;

    idt_driver.idt_sc_init_dev_ctrs= idt_tsi57x_sc_init_dev_ctrs;
    idt_driver.idt_sc_read_ctrs    = idt_tsi57x_sc_read_ctrs    ;

    IDT_DSF_bind_driver( &idt_driver, &Tsi57x_driver_handle );

    return RIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

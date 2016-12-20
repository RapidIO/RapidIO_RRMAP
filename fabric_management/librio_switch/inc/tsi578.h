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

#ifndef _TS_Tsi578_H_ 
#define _TS_Tsi578_H_ 

                                                                               
#define Tsi578_NUM_REGS_TO_TEST                                     0x000004e7

                                                                               
#define Tsi578_MAX_MAC      8
#define Tsi578_MAX_PORTS   16
#define Tsi578_MAX_LANES    4  
#define Tsi578_MAX_MC_MASKS 8


/* ************************************************ */
/* Tsi578 : Register address offset definitions     */
/* ************************************************ */
#define Tsi578_RIO_DEV_ID                                          (0x00000000)
#define Tsi578_RIO_DEV_INFO                                        (0x00000004)
#define Tsi578_RIO_ASBLY_ID                                        (0x00000008)
#define Tsi578_RIO_ASBLY_INFO                                      (0x0000000c)
#define Tsi578_RIO_PE_FEAT                                         (0x00000010)
#define Tsi578_RIO_SW_PORT                                         (0x00000014)
#define Tsi578_RIO_SRC_OP                                          (0x00000018)
#define Tsi578_RIO_PE_MC_FEAT                                      (0x00000030)
#define Tsi578_RIO_LUT_SIZE                                        (0x00000034)
#define Tsi578_RIO_SW_MC_INFO                                      (0x00000038)
#define Tsi578_RIO_HOST_BASE_ID_LOCK                               (0x00000068)
#define Tsi578_RIO_COMP_TAG                                        (0x0000006c)
#define Tsi578_RIO_ROUTE_CFG_DESTID                                (0x00000070)
#define Tsi578_RIO_ROUTE_CFG_PORT                                  (0x00000074)
#define Tsi578_RIO_LUT_ATTR                                        (0x00000078)
#define Tsi578_RIO_MC_MASK_CFG                                     (0x00000080)
#define Tsi578_RIO_MC_DESTID_CFG                                   (0x00000084)
#define Tsi578_RIO_MC_DESTID_ASSOC                                 (0x00000088)
#define Tsi578_RIO_SW_MB_HEAD                                      (0x00000100)
#define Tsi578_RIO_SW_LT_CTL                                       (0x00000120)
#define Tsi578_RIO_SW_GEN_CTL                                      (0x0000013c)
#define Tsi578_SPX_LM_REQ(X)                               (0x0140 + 0x020*(X))
#define Tsi578_SPX_LM_RESP(X)                              (0x0144 + 0x020*(X))
#define Tsi578_SPX_ACKID_STAT(X)                           (0x0148 + 0x020*(X))
#define Tsi578_SPX_ERR_uint32_t(X)                           (0x0158 + 0x020*(X))
#define Tsi578_SPX_CTL(X)                                  (0x015c + 0x020*(X))
#define Tsi578_RIO_ERR_RPT_BH                                      (0x00001000)
#define Tsi578_RIO_LOG_ERR_DET                                     (0x00001008)
#define Tsi578_RIO_LOG_ERR_DET_EN                                  (0x0000100c)
#define Tsi578_RIO_LOG_ERR_ADDR                                    (0x00001014)
#define Tsi578_RIO_LOG_ERR_DEVID                                   (0x00001018)
#define Tsi578_RIO_LOG_ERR_CTRL_INFO                               (0x0000101c)
#define Tsi578_RIO_PW_DESTID                                       (0x00001028)
#define Tsi578_RIO_PKT_TTL                                         (0x0000102c)
#define Tsi578_SPX_ERR_DET(X)                              (0x1040 + 0x040*(X))
#define Tsi578_SPX_RATE_EN(X)                              (0x1044 + 0x040*(X))
#define Tsi578_SPX_ERR_ATTR_CAPT_DBG0(X)                   (0x1048 + 0x040*(X))
#define Tsi578_SPX_ERR_CAPT_0_DBG1(X)                      (0x104c + 0x040*(X))
#define Tsi578_SPX_ERR_CAPT_1_DBG2(X)                      (0x1050 + 0x040*(X))
#define Tsi578_SPX_ERR_CAPT_2_DBG3(X)                      (0x1054 + 0x040*(X))
#define Tsi578_SPX_ERR_CAPT_3_DBG4(X)                      (0x1058 + 0x040*(X))
#define Tsi578_SPX_ERR_RATE(X)                             (0x1068 + 0x040*(X))
#define Tsi578_SPX_ERR_THRESH(X)                           (0x106c + 0x040*(X))
#define Tsi578_SPBC_DISCOVERY_TIMER                                (0x00010000) 
#define Tsi578_SPX_DISCOVERY_TIMER(X)                     (0x11000 + 0x100*(X))
#define Tsi578_SPBC_MODE                                           (0x00010004) 
#define Tsi578_SPX_MODE(X)                                (0x11004 + 0x100*(X))
#define Tsi578_SPBC_CS_INT_uint32_t                                  (0x00010008) 
#define Tsi578_SPX_CS_INT_uint32_t(X)                       (0x11008 + 0x100*(X))
#define Tsi578_SPBC_RIO_WM                                         (0x0001000c) 
#define Tsi578_SPX_RIO_WM(X)                              (0x1100c + 0x100*(X))
#define Tsi578_SPBC_ROUTE_CFG_DESTID                               (0x00010070) 
#define Tsi578_SPX_ROUTE_CFG_DESTID(X)                    (0x11070 + 0x100*(X))
#define Tsi578_SPBC_ROUTE_CFG_PORT                                 (0x00010074) 
#define Tsi578_SPX_ROUTE_CFG_PORT(X)                      (0x11074 + 0x100*(X))
#define Tsi578_SPBC_ROUTE_BASE                                     (0x00010078) 
#define Tsi578_SPX_ROUTE_BASE(X)                          (0x11078 + 0x100*(X))
#define Tsi578_RIO_MC_IDX(X)                              (0x10300 + 0x004*(X))
#define Tsi578_RIO_MC_MSKX(X)                             (0x10320 + 0x004*(X))
#define Tsi578_SPX_ID(X)                                  (0x13000 + 0x100*(X))
#define Tsi578_SPX_CTL_INDEP(X)                           (0x13004 + 0x100*(X))
#define Tsi578_SPX_SILENCE_TIMER(X)                       (0x13008 + 0x100*(X))
#define Tsi578_SPX_SEND_MCS(X)                            (0x1300c + 0x100*(X))
#define Tsi578_SPX_LUT_PAR_ERR_INFO(X)                    (0x13010 + 0x100*(X))
#define Tsi578_SPX_CS_TX(X)                               (0x13014 + 0x100*(X))
#define Tsi578_SPX_INT_uint32_t(X)                          (0x13018 + 0x100*(X))
#define Tsi578_SPX_INT_GEN(X)                             (0x1301c + 0x100*(X))
#define Tsi578_SPX_PSC0n1_CTRL(X)                         (0x13020 + 0x100*(X))
#define Tsi578_SPX_PSC2n3_CTRL(X)                         (0x13024 + 0x100*(X))
#define Tsi578_SPX_PSC4n5_CTRL(X)                         (0x13028 + 0x100*(X))
#define Tsi578_SPX_PSC_CTRL(X,Y)                          (0x13020 + 0x100*(X)+((Y/2)*4))
#define Tsi578_NUM_PERF_CTRS        6
#define Tsi578_SPX_PSC0(X)                                (0x13040 + 0x100*(X))
#define Tsi578_SPX_PSC1(X)                                (0x13044 + 0x100*(X))
#define Tsi578_SPX_PSC2(X)                                (0x13048 + 0x100*(X))
#define Tsi578_SPX_PSC3(X)                                (0x1304c + 0x100*(X))
#define Tsi578_SPX_PSC4(X)                                (0x13050 + 0x100*(X))
#define Tsi578_SPX_PSC5(X)                                (0x13054 + 0x100*(X))
#define Tsi578_SPX_PSCY(X,Y)                              (0x13040 +(0x100*(X))+(4*Y))
#define Tsi578_SPX_TX_Q_D_THRESH(X)                       (0x13080 + 0x100*(X))
#define Tsi578_SPX_TX_Q_uint32_t(X)                         (0x13084 + 0x100*(X))
#define Tsi578_SPX_TX_Q_PERIOD(X)                         (0x13088 + 0x100*(X))
#define Tsi578_SPX_RX_Q_D_THRESH(X)                       (0x13090 + 0x100*(X))
#define Tsi578_SPX_RX_Q_uint32_t(X)                         (0x13094 + 0x100*(X))
#define Tsi578_SPX_RX_Q_PERIOD(X)                         (0x13098 + 0x100*(X))
#define Tsi578_SPX_REORDER_CTR(X)                         (0x130a0 + 0x100*(X))
#define Tsi578_SMACX_CFG_CH0(X)                           (0x130b0 + 0x100*(X))
#define Tsi578_SMACX_CFG_CH1(X)                           (0x130b4 + 0x100*(X))
#define Tsi578_SMACX_CFG_CH2(X)                           (0x130b8 + 0x100*(X))
#define Tsi578_SMACX_CFG_CH3(X)                           (0x130bc + 0x100*(X))
#define Tsi578_SMACX_CFG_GBL(X)                           (0x130c0 + 0x100*(X))
#define Tsi578_SMACX_CFG_GBLB(X)                          (0x130c4 + 0x100*(X))
#define Tsi578_SMACX_DLOOP_CLK_SEL(X)                     (0x130c8 + 0x100*(X))
#define Tsi578_SMACX_SERDES_OUTPUT_PINS(X)                (0x130cc + 0x100*(X))
#define Tsi578_MCES_PIN_CTRL                                       (0x000130d0)
#define Tsi578_SMACX_SPY_BUS_CFG(X)                       (0x130fc + 0x100*(X))
#define Tsi578_ERRX_INJ_CMP_MASK(X)                       (0x14004 + 0x100*(X))
#define Tsi578_ERRX_INJ_CMP_BIT_VLD(X)                    (0x14008 + 0x100*(X))
#define Tsi578_ERRX_INJ(X)                                (0x1400c + 0x100*(X))
#define Tsi578_ERRX_INJ_BIT_VLD(X)                        (0x14010 + 0x100*(X))
#define Tsi578_FAB_CTL                                             (0x0001aa00)
#define Tsi578_FAB_INT_STAT                                        (0x0001aa04)
#define Tsi578_RIO_MC_LAT_ERR                                      (0x0001aa08)
#define Tsi578_RIO_MC_LAT_ERR_SET                                  (0x0001aa0c)
#define Tsi578_PKTGEN_CNTRL                                        (0x0001aaf0)
#define Tsi578_WRKQ_DATA                                           (0x0001aaf4)
#define Tsi578_LST_DTM_uint32_t                                      (0x0001aaf8)
#define Tsi578_TX_ENBL                                             (0x0001aafc)
#define Tsi578_GLOB_INT_uint32_t                                     (0x0001ac00)
#define Tsi578_GLOB_INT_ENABLE                                     (0x0001ac04)
#define Tsi578_GLOB_DEV_ID_SEL                                     (0x0001ac08)
#define Tsi578_GLOB_SPY_BUS_CFG                                    (0x0001ac0c)
#define Tsi578_RIO_PW_TIMEOUT                                      (0x0001ac14)
#define Tsi578_RIO_PW_OREQ_uint32_t                                  (0x0001ac18)
#define Tsi578_RIOX_MC_REG_VER(X)                         (0x1b000 + 0x100*(X))
#define Tsi578_RIOX_MC_LAT_LIMIT(X)                       (0x1b004 + 0x100*(X))
#define Tsi578_SPX_ISF_WM(X)                              (0x1b008 + 0x100*(X))
#define Tsi578_SPX_WRR_0(X)                               (0x1b010 + 0x100*(X))
#define Tsi578_SPX_WRR_1(X)                               (0x1b014 + 0x100*(X))
#define Tsi578_SPX_WRR_2(X)                               (0x1b018 + 0x100*(X))
#define Tsi578_SPX_WRR_3(X)                               (0x1b01c + 0x100*(X))
#define Tsi578_I2C_DEVID                                           (0x0001d100)
#define Tsi578_I2C_RESET                                           (0x0001d104)
#define Tsi578_I2C_MST_CFG                                         (0x0001d108)
#define Tsi578_I2C_MST_CNTRL                                       (0x0001d10c)
#define Tsi578_I2C_MST_RDATA                                       (0x0001d110)
#define Tsi578_I2C_MST_TDATA                                       (0x0001d114)
#define Tsi578_I2C_ACC_STAT                                        (0x0001d118)
#define Tsi578_I2C_INT_STAT                                        (0x0001d11c)
#define Tsi578_I2C_INT_ENABLE                                      (0x0001d120)
#define Tsi578_I2C_INT_SET                                         (0x0001d124)
#define Tsi578_I2C_SLV_CFG                                         (0x0001d12c)
#define Tsi578_I2C_BOOT_CNTRL                                      (0x0001d140)
#define Tsi578_EXI2C_REG_WADDR                                     (0x0001d200)
#define Tsi578_EXI2C_REG_WDATA                                     (0x0001d204)
#define Tsi578_EXI2C_REG_RADDR                                     (0x0001d210)
#define Tsi578_EXI2C_REG_RDATA                                     (0x0001d214)
#define Tsi578_EXI2C_ACC_STAT                                      (0x0001d220)
#define Tsi578_EXI2C_ACC_CNTRL                                     (0x0001d224)
#define Tsi578_EXI2C_STAT                                          (0x0001d280)
#define Tsi578_EXI2C_STAT_ENABLE                                   (0x0001d284)
#define Tsi578_EXI2C_MBOX_OUT                                      (0x0001d290)
#define Tsi578_EXI2C_MBOX_IN                                       (0x0001d294)
#define Tsi578_I2C_EVENT                                           (0x0001d300) 
#define Tsi578_I2C_SNAP_EVENT                                      (0x0001d304) 
#define Tsi578_I2C_NEW_EVENT                                       (0x0001d308)
#define Tsi578_I2C_EVENT_ENB                                       (0x0001d30c)
#define Tsi578_I2C_DIVIDER                                         (0x0001d320)
#define Tsi578_I2C_FILTER_SCL_CFG                                  (0x0001d328)
#define Tsi578_I2C_FILTER_SDA_CFG                                  (0x0001d32c)
#define Tsi578_I2C_START_SETUP_HOLD                                (0x0001d340)
#define Tsi578_I2C_STOP_IDLE                                       (0x0001d344)
#define Tsi578_I2C_SDA_SETUP_HOLD                                  (0x0001d348)
#define Tsi578_I2C_SCL_PERIOD                                      (0x0001d34c)
#define Tsi578_I2C_SCL_MIN_PERIOD                                  (0x0001d350)
#define Tsi578_I2C_SCL_ARB_TIMEOUT                                 (0x0001d354)
#define Tsi578_I2C_BYTE_TRAN_TIMEOUT                               (0x0001d358)
#define Tsi578_I2C_BOOT_DIAG_TIMER                                 (0x0001d35c)
#define Tsi578_I2C_DIAG_FILTER_SCL                                 (0x0001d3a0)
#define Tsi578_I2C_DIAG_FILTER_SDA                                 (0x0001d3a4)
#define Tsi578_I2C_BOOT_DIAG_PROGRESS                              (0x0001d3b8)
#define Tsi578_I2C_BOOT_DIAG_CFG                                   (0x0001d3bc)
#define Tsi578_I2C_DIAG_CNTRL                                      (0x0001d3c0)
#define Tsi578_I2C_DIAG_STAT                                       (0x0001d3d0)
#define Tsi578_I2C_DIAG_HIST                                       (0x0001d3d4)
#define Tsi578_I2C_DIAG_MST_FSM                                    (0x0001d3d8)
#define Tsi578_I2C_DIAG_SLV_FSM                                    (0x0001d3dc)
#define Tsi578_I2C_DIAG_MST_SDA_SCL                                (0x0001d3e0)
#define Tsi578_I2C_DIAG_MST_SCL_PER                                (0x0001d3e4)
#define Tsi578_I2C_DIAG_MST_ARB_BOOT                               (0x0001d3e8)
#define Tsi578_I2C_DIAG_MST_BYTE_TRAN                              (0x0001d3ec)
#define Tsi578_I2C_DIAG_SLV_SDA_SCL                                (0x0001d3f0)
#define Tsi578_I2C_DIAG_SLV_BYTE_TRAN                              (0x0001d3f4)
#define Tsi578_SMACX_Tx_Ctl_STAT_0(X)                     (0x1e000 + 0x100*(X))
#define Tsi578_SMACX_Rx_STAT_0(X)                         (0x1e004 + 0x100*(X))
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0(X)                  (0x1e008 + 0x100*(X))
#define Tsi578_SMACX_Rx_Ctl_STAT_0(X)                     (0x1e00c + 0x100*(X))
#define Tsi578_SMACX_PG_CTL_0(X)                          (0x1e020 + 0x100*(X))
#define Tsi578_SMACX_PM_CTL_0(X)                          (0x1e030 + 0x100*(X))
#define Tsi578_SMACX_FP_VAL_0(X)                          (0x1e034 + 0x100*(X))
#define Tsi578_SMACX_SCP_RX_CTL_0(X)                      (0x1e038 + 0x100*(X))
#define Tsi578_SMACX_Rx_Dbg_0(X)                          (0x1e03c + 0x100*(X))
#define Tsi578_SMACX_RESET(X)                             (0x1e100 + 0x100*(X))
#define Tsi578_SMACX_CMP_A(X)                             (0x1e104 + 0x100*(X))
#define Tsi578_SMACX_CMP_B(X)                             (0x1e108 + 0x100*(X))
#define Tsi578_SMACX_SCP(X)                               (0x1e10c + 0x100*(X))
#define Tsi578_SMACX_DAC_RTUNE(X)                         (0x1e110 + 0x100*(X))
#define Tsi578_SMACX_ADC(X)                               (0x1e114 + 0x100*(X))
#define Tsi578_SMACX_ID(X)                                (0x1e118 + 0x100*(X))
#define Tsi578_SMACX_CTL_VAL(X)                           (0x1e11c + 0x100*(X))
#define Tsi578_SMACX_LVL_CTL_VAL(X)                       (0x1e120 + 0x100*(X))
#define Tsi578_SMACX_CTL_OVR(X)                           (0x1e124 + 0x100*(X))
#define Tsi578_SMACX_LVL_OVR(X)                           (0x1e128 + 0x100*(X))
#define Tsi578_SMACX_MPLL_CTL(X)                          (0x1e12c + 0x100*(X))



/* ************************************************ */
/* Tsi578 : Register Bit Masks and Reset Values     */
/*           definitions for every register         */
/* ************************************************ */



/* Tsi578_RIO_DEV_ID : Register Bits Masks Definitions */
#define Tsi578_RIO_DEV_IDENT_VEND                               (0x0000ffff)
#define Tsi578_RIO_DEV_IDENT_DEVI                                   (0xffff0000)

#define Tsi578_RIO_DEVID_VAL 0x0578
#define Tsi576_RIO_DEVID_VAL 0x0576
#define Tsi574_RIO_DEVID_VAL 0x0574
#define Tsi572_RIO_DEVID_VAL 0x0572
#define Tsi577_RIO_DEVID_VAL 0x0577

/* Tsi578_RIO_DEV_INFO : Register Bits Masks Definitions */
#define Tsi578_RIO_DEV_INFO_METAL_REV                              (0x0000000f)
#define Tsi578_RIO_DEV_INFO_SILICON_REV                            (0x000000f0)

/* Tsi578_RIO_ASBLY_ID : Register Bits Masks Definitions */
#define Tsi578_RIO_ASBLY_ID_ASBLY_VEN_ID                           (0x0000ffff)
#define Tsi578_RIO_ASBLY_ID_ASBLY_ID                               (0xffff0000)

/* Tsi578_RIO_ASBLY_INFO : Register Bits Masks Definitions */
#define Tsi578_RIO_ASSY_INF_EFB_PTR                         (0x0000ffff)
#define Tsi578_RIO_ASSY_INF_ASSY_REV                            (0xffff0000)

/* Tsi578_RIO_PE_FEAT : Register Bits Masks Definitions */
#define Tsi578_RIO_PE_FEAT_EXT_AS                                  (0x00000007)
#define Tsi578_RIO_PE_FEAT_EXT_FEA                                 (0x00000008)
#define Tsi578_RIO_PE_FEAT_CTLS                                    (0x00000010)
#define Tsi578_RIO_PE_FEAT_SBR                                     (0x00000100)
#define Tsi578_RIO_PE_FEAT_MC                                      (0x00000400)
#define Tsi578_RIO_PE_FEAT_SW                                      (0x10000000)
#define Tsi578_RIO_PE_FEAT_PROC                                    (0x20000000)
#define Tsi578_RIO_PE_FEAT_MEM                                     (0x40000000)
#define Tsi578_RIO_PE_FEAT_BRDG                                    (0x80000000)

/* Tsi578_RIO_SW_PORT : Register Bits Masks Definitions */
#define Tsi578_RIO_SW_PORT_PORT_NUM                                (0x000000ff)
#define Tsi578_RIO_SW_PORT_PORT_TOTAL                              (0x0000ff00)

/* Tsi578_RIO_SRC_OP : Register Bits Masks Definitions */
#define Tsi578_RIO_SRC_OP_PORT_WR                                  (0x00000004)
#define Tsi578_RIO_SRC_OP_A_CLEAR                                  (0x00000010)
#define Tsi578_RIO_SRC_OP_A_SET                                    (0x00000020)
#define Tsi578_RIO_SRC_OP_A_DEC                                    (0x00000040)
#define Tsi578_RIO_SRC_OP_A_INC                                    (0x00000080)
#define Tsi578_RIO_SRC_OP_A_TSWAP                                  (0x00000100)
#define Tsi578_RIO_SRC_OP_DBELL                                    (0x00000400)
#define Tsi578_RIO_SRC_OP_D_MSG                                    (0x00000800)
#define Tsi578_RIO_SRC_OP_WR_RES                                   (0x00001000)
#define Tsi578_RIO_SRC_OP_STRM_WR                                  (0x00002000)
#define Tsi578_RIO_SRC_OP_WRITE                                    (0x00004000)
#define Tsi578_RIO_SRC_OP_READ                                     (0x00008000)
#define Tsi578_RIO_SRC_OP_IMPLEMENT_DEF                            (0x00030000)

/* Tsi578_RIO_PE_MC_FEAT : Register Bits Masks Definitions */
#define Tsi578_RIO_PE_MC_FEAT_SIMP                                 (0x80000000)

/* Tsi578_RIO_LUT_SIZE : Register Bits Masks Definitions */
#define Tsi578_RIO_LUT_SIZE_LUT_SIZE                               (0x0000ffff)

/* Tsi578_RIO_SW_MC_INFO : Register Bits Masks Definitions */
#define Tsi578_RIO_SW_MC_INFO_MAX_MASKS                            (0x0000ffff)
#define Tsi578_RIO_SW_MC_INFO_MAX_DESTID_ASSOC                     (0x3fff0000)
#define Tsi578_RIO_SW_MC_INFO_ASSOC_SCOPE                          (0x40000000)
#define Tsi578_RIO_SW_MC_INFO_ASSOC_MODE                           (0x80000000)

/* Tsi578_RIO_HOST_BASE_ID_LOCK : Register Bits Masks Definitions */
#define Tsi578_RIO_HOST_BASE_ID_LOCK_HOST_BASE_ID                  (0x0000ffff)

/* Tsi578_RIO_COMP_TAG : Register Bits Masks Definitions */
#define Tsi578_RIO_COMP_TAG_CTAG                                   (0xffffffff)

/* Tsi578_RIO_ROUTE_CFG_DESTID : Register Bits Masks Definitions */
#define Tsi578_RIO_ROUTE_CFG_DESTID_CFG_DEST_ID                    (0x000000ff)
#define Tsi578_RIO_ROUTE_CFG_DESTID_LRG_CFG_DEST_ID                (0x0000ff00)

/* Tsi578_RIO_ROUTE_CFG_PORT : Register Bits Masks Definitions */
#define Tsi578_RIO_ROUTE_CFG_PORT_PORT                             (0x000000ff)

/* Tsi578_RIO_LUT_ATTR : Register Bits Masks Definitions */
#define Tsi578_RIO_LUT_ATTR_DEFAULT_PORT                           (0x000000ff)

/* Tsi578_RIO_MC_MASK_CFG : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_MASK_CFG_PORT_PRESENT                        (0x00000001)
#define Tsi578_RIO_MC_MASK_CFG_MASK_CMD                            (0x00000070)
#define Tsi578_RIO_MC_MASK_CFG_EG_PORT_NUM                         (0x00000f00)
#define Tsi578_RIO_MC_MASK_CFG_MC_MASK_NUM                         (0x00070000)

/* Tsi578_RIO_MC_DESTID_CFG : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_DESTID_CFG_MASK_NUM_BASE                     (0x00000007)
#define Tsi578_RIO_MC_DESTID_CFG_DESTID_BASE                       (0x00ff0000)
#define Tsi578_RIO_MC_DESTID_CFG_DESTID_BASE_LT                    (0xff000000)

/* Tsi578_RIO_MC_DESTID_ASSOC : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_DESTID_ASSOC_ASSOC_PRESENT                   (0x00000001)
#define Tsi578_RIO_MC_DESTID_ASSOC_CMD                             (0x00000060)
#define Tsi578_RIO_MC_DESTID_ASSOC_LARGE                           (0x00000080)
#define Tsi578_RIO_MC_DESTID_ASSOC_INGRESS_PORT                    (0x0000ff00)
#define Tsi578_RIO_MC_DESTID_ASSOC_ASSOC_BLK_SIZE                  (0xffff0000)

/* Tsi578_RIO_SW_MB_HEAD : Register Bits Masks Definitions */
#define Tsi578_RIO_SW_MB_HEAD_EF_ID                                (0x0000ffff)
#define Tsi578_RIO_SW_MB_HEAD_EF_PTR                               (0xffff0000)

/* Tsi578_RIO_SW_LT_CTL : Register Bits Masks Definitions */
#define Tsi578_RIO_SW_LT_CTL_TVAL                                  (0xffffff00)

/* Tsi578_RIO_SW_GEN_CTL : Register Bits Masks Definitions */
#define Tsi578_RIO_SW_GEN_CTL_DISC                                 (0x20000000)

/* Tsi578_SPX_LM_REQ : Register Bits Masks Definitions */
#define Tsi578_SPX_LM_REQ_CMD                                      (0x00000007)

/* Tsi578_SPX_LM_RESP : Register Bits Masks Definitions */
#define Tsi578_SPX_LM_RESP_LINK_STAT                               (0x0000001f)
#define Tsi578_SPX_LM_RESP_ACK_ID_STAT                             (0x000003e0)
#define Tsi578_SPX_LM_RESP_RESP_VLD                                (0x80000000)

/* Tsi578_SPX_ACKID_STAT : Register Bits Masks Definitions */
#define Tsi578_SPX_ACKID_STAT_OUTBOUND                             (0x0000001f)
#define Tsi578_SPX_ACKID_STAT_OUTSTANDING                          (0x00001f00)
#define Tsi578_SPX_ACKID_STAT_INBOUND                              (0x1f000000)
#define Tsi578_SPX_ACKID_STAT_CLR_PKTS                             (0x80000000)

/* Tsi578_SPX_ERR_uint32_t : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_uint32_t_PORT_UNINIT                          (0x00000001)
#define Tsi578_SPX_ERR_uint32_t_PORT_OK                              (0x00000002)
#define Tsi578_SPX_ERR_uint32_t_PORT_ERR                             (0x00000004)
#define Tsi578_SPX_ERR_uint32_t_PORT_W_PEND                          (0x00000010)
#define Tsi578_SPX_ERR_uint32_t_INPUT_ERR_STOP                       (0x00000100)
#define Tsi578_SPX_ERR_uint32_t_INPUT_ERR                            (0x00000200)
#define Tsi578_SPX_ERR_uint32_t_INPUT_RS                             (0x00000400)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_ERR_STOP                      (0x00010000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_ERR                           (0x00020000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_RS                            (0x00040000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_R                             (0x00080000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_RE                            (0x00100000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_DEG                           (0x01000000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_FAIL                          (0x02000000)
#define Tsi578_SPX_ERR_uint32_t_OUTPUT_DROP                          (0x04000000)

/* Tsi578_SPX_CTL : Register Bits Masks Definitions */
#define Tsi578_SPX_CTL_PORT_TYPE                                   (0x00000001)
#define Tsi578_SPX_CTL_PORT_LOCKOUT                                (0x00000002)
#define Tsi578_SPX_CTL_DROP_EN                                     (0x00000004)
#define Tsi578_SPX_CTL_STOP_FAIL_EN                                (0x00000008)
#define Tsi578_SPX_CTL_ENUM_B                                      (0x00020000)
#define Tsi578_SPX_CTL_MCS_EN                                      (0x00080000)
#define Tsi578_SPX_CTL_ERR_DIS                                     (0x00100000)
#define Tsi578_SPX_CTL_INPUT_EN                                    (0x00200000)
#define Tsi578_SPX_CTL_OUTPUT_EN                                   (0x00400000)
#define Tsi578_SPX_CTL_PORT_DIS                                    (0x00800000)
#define Tsi578_SPX_CTL_OVER_PWIDTH                                 (0x07000000)
#define Tsi578_SPX_CTL_INIT_PWIDTH                                 (0x38000000)
#define Tsi578_SPX_CTL_PORT_WIDTH                                  (0xc0000000)

/* Tsi578_RIO_ERR_RPT_BH : Register Bits Masks Definitions */
#define Tsi578_RIO_ERR_RPT_BH_EF_ID                                (0x0000ffff)
#define Tsi578_RIO_ERR_RPT_BH_EF_PTR                               (0xffff0000)

/* Tsi578_RIO_LOG_ERR_DET : Register Bits Masks Definitions */
#define Tsi578_RIO_LOG_ERR_DET_L_UNSUP_TRANS                       (0x00400000)
#define Tsi578_RIO_LOG_ERR_DET_L_ILL_RESP                          (0x00800000)
#define Tsi578_RIO_LOG_ERR_DET_L_ILL_TRANS                         (0x08000000)

/* Tsi578_RIO_LOG_ERR_DET_EN : Register Bits Masks Definitions */
#define Tsi578_RIO_LOG_ERR_DET_EN_UNSUP_TRANS_EN                   (0x00400000)
#define Tsi578_RIO_LOG_ERR_DET_EN_ILL_RESP_EN                      (0x00800000)
#define Tsi578_RIO_LOG_ERR_DET_EN_ILL_TRANS_EN                     (0x08000000)

/* Tsi578_RIO_LOG_ERR_ADDR : Register Bits Masks Definitions */
#define Tsi578_RIO_LOG_ERR_ADDR_WDPTR                              (0x00000002)
#define Tsi578_RIO_LOG_ERR_ADDR_ADDRESS                            (0x00fffff8)

/* Tsi578_RIO_LOG_ERR_DEVID : Register Bits Masks Definitions */
#define Tsi578_RIO_LOG_ERR_DEVID_SRCID                             (0x000000ff)
#define Tsi578_RIO_LOG_ERR_DEVID_SRCID_MSB                         (0x0000ff00)

/* Tsi578_RIO_LOG_ERR_CTRL_INFO : Register Bits Masks Definitions */
#define Tsi578_RIO_LOG_ERR_CTRL_INFO_TTYPE                         (0x0f000000)
#define Tsi578_RIO_LOG_ERR_CTRL_INFO_FTYPE                         (0xf0000000)

/* Tsi578_RIO_PW_DESTID : Register Bits Masks Definitions */
#define Tsi578_RIO_PW_DESTID_LARGE_DESTID                          (0x00008000)
#define Tsi578_RIO_PW_DESTID_DESTID_LSB                            (0x00ff0000)
#define Tsi578_RIO_PW_DESTID_DESTID_MSB                            (0xff000000)

/* Tsi578_RIO_PKT_TTL : Register Bits Masks Definitions */
#define Tsi578_RIO_PKT_TTL_TTL                                     (0xffff0000)

/* Tsi578_SPX_ERR_DET : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_DET_LINK_TO                                 (0x00000001)
#define Tsi578_SPX_ERR_DET_CS_ACK_ILL                              (0x00000002)
#define Tsi578_SPX_ERR_DET_DELIN_ERR                               (0x00000004)
#define Tsi578_SPX_ERR_DET_PROT_ERR                                (0x00000010)
#define Tsi578_SPX_ERR_DET_LR_ACKID_ILL                            (0x00000020)
#define Tsi578_SPX_ERR_DET_PKT_ILL_SIZE                            (0x00020000)
#define Tsi578_SPX_ERR_DET_PKT_CRC_ERR                             (0x00040000)
#define Tsi578_SPX_ERR_DET_PKT_ILL_ACKID                           (0x00080000)
#define Tsi578_SPX_ERR_DET_CS_NOT_ACC                              (0x00100000)
#define Tsi578_SPX_ERR_DET_CS_ILL_ID                               (0x00200000)
#define Tsi578_SPX_ERR_DET_CS_CRC_ERR                              (0x00400000)
#define Tsi578_SPX_ERR_DET_IMP_SPEC_ERR                            (0x80000000)

/* Tsi578_SPX_RATE_EN : Register Bits Masks Definitions */
#define Tsi578_SPX_RATE_EN_LINK_TO_EN                              (0x00000001)
#define Tsi578_SPX_RATE_EN_CS_ACK_ILL_EN                           (0x00000002)
#define Tsi578_SPX_RATE_EN_DELIN_ERR_EN                            (0x00000004)
#define Tsi578_SPX_RATE_EN_PROT_ERR_EN                             (0x00000010)
#define Tsi578_SPX_RATE_EN_LR_ACKID_ILL_EN                         (0x00000020)
#define Tsi578_SPX_RATE_EN_PKT_ILL_SIZE_EN                         (0x00020000)
#define Tsi578_SPX_RATE_EN_PKT_CRC_ERR_EN                          (0x00040000)
#define Tsi578_SPX_RATE_EN_PKT_ILL_ACKID_EN                        (0x00080000)
#define Tsi578_SPX_RATE_EN_CS_NOT_ACC_EN                           (0x00100000)
#define Tsi578_SPX_RATE_EN_CS_ILL_ID_EN                            (0x00200000)
#define Tsi578_SPX_RATE_EN_CS_CRC_ERR_EN                           (0x00400000)
#define Tsi578_SPX_RATE_EN_IMP_SPEC_ERR                            (0x80000000)

/* Tsi578_SPX_ERR_ATTR_CAPT_DBG0 : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_ATTR_CAPT_DBG0_VAL_CAPT                     (0x00000001)
#define Tsi578_SPX_ERR_ATTR_CAPT_DBG0_ERR_TYPE                     (0x1f000000)
#define Tsi578_SPX_ERR_ATTR_CAPT_DBG0_INFO_TYPE                    (0xc0000000)

/* Tsi578_SPX_ERR_CAPT_0_DBG1 : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_CAPT_0_DBG1_CAPT_0                          (0xffffffff)

/* Tsi578_SPX_ERR_CAPT_1_DBG2 : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_CAPT_1_DBG2_CAPT_1                          (0xffffffff)

/* Tsi578_SPX_ERR_CAPT_2_DBG3 : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_CAPT_2_DBG3_CAPT_2                          (0xffffffff)

/* Tsi578_SPX_ERR_CAPT_3_DBG4 : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_CAPT_3_DBG4_CAPT_3                          (0xffffffff)

/* Tsi578_SPX_ERR_RATE : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_RATE_ERR_RATE_CNT                           (0x000000ff)
#define Tsi578_SPX_ERR_RATE_PEAK                                   (0x0000ff00)
#define Tsi578_SPX_ERR_RATE_ERR_RR                                 (0x00030000)
#define Tsi578_SPX_ERR_RATE_ERR_RB                                 (0xff000000)

/* Tsi578_SPX_ERR_THRESH : Register Bits Masks Definitions */
#define Tsi578_SPX_ERR_THRESH_ERR_RDT                              (0x00ff0000)
#define Tsi578_SPX_ERR_THRESH_ERR_RFT                              (0xff000000)

/* Tsi578_SPX_DISCOVERY_TIMER : Register Bits Masks Definitions */
#define Tsi578_SPX_DISCOVERY_TIMER_PW_PRIORITY                     (0x00c00000)
#define Tsi578_SPX_DISCOVERY_TIMER_DISCOVERY_TIMER                 (0xf0000000)

/* Tsi578_SPX_MODE : Register Bits Masks Definitions */
#define Tsi578_SPX_MODE_RCS_INT_EN                                 (0x00000001)
#define Tsi578_SPX_MODE_MCS_INT_EN                                 (0x00000002)
#define Tsi578_SPX_MODE_LUT_512                                    (0x01000000)
#define Tsi578_SPX_MODE_SELF_RST                                   (0x02000000)
#define Tsi578_SPX_MODE_PW_DIS                                     (0x08000000)
#define Tsi578_SPX_MODE_IDLE_ERR_DIS                               (0x20000000)

/* Tsi578_SPX_CS_INT_uint32_t : Register Bits Masks Definitions */
#define Tsi578_SPX_CS_INT_uint32_t_RCS                               (0x00000001)
#define Tsi578_SPX_CS_INT_uint32_t_MCS                               (0x00000002)

/* Tsi578_SPX_RIO_WM : Register Bits Masks Definitions */
#define Tsi578_SPX_RIO_WM_PRIO0WM                                  (0x00000007)
#define Tsi578_SPX_RIO_WM_PRIO1WM                                  (0x00000700)
#define Tsi578_SPX_RIO_WM_PRIO2WM                                  (0x00070000)

/* Tsi578_SPX_ROUTE_CFG_DESTID : Register Bits Masks Definitions */
#define Tsi578_SPX_ROUTE_CFG_DESTID_CFG_DEST_ID                    (0x000000ff)
#define Tsi578_SPX_ROUTE_CFG_DESTID_LRG_CFG_DEST_ID                (0x0000ff00)
#define Tsi578_SPX_ROUTE_CFG_DESTID_PAR_INVERT                     (0x40000000)
#define Tsi578_SPX_ROUTE_CFG_DESTID_AUTO_INC                       (0x80000000)

/* Tsi578_SPX_ROUTE_CFG_PORT : Register Bits Masks Definitions */
#define Tsi578_SPX_ROUTE_CFG_PORT_PORT                             (0x000000ff)

/* Tsi578_SPX_ROUTE_BASE : Register Bits Masks Definitions */
#define Tsi578_SPX_ROUTE_BASE_BASE                                 (0xff000000)

/* Tsi578_RIO_MC_IDX : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_IDX_MC_ID_DEV8                               (0x000000ff)
#define Tsi578_RIO_MC_IDX_MC_ID                                    (0x0000ffff)
#define Tsi578_RIO_MC_IDX_LARGE_SYS                                (0x40000000)
#define Tsi578_RIO_MC_IDX_MC_EN                                    (0x80000000)

/* Tsi578_RIO_MC_MSKX : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_MSKX_MC_MSK                                  (0xffff0000)

/* Tsi578_SPX_ID : Register Bits Masks Definitions */
#define Tsi578_SPX_ID_PORT_ID                                      (0x000000ff)

/* Tsi578_SPX_CTL_INDEP : Register Bits Masks Definitions */
#define Tsi578_SPX_CTL_INDEP_TEA_EN                                (0x00000001)
#define Tsi578_SPX_CTL_INDEP_RSVD1                                 (0x00000002)
#define Tsi578_SPX_CTL_INDEP_INB_RDR_EN                            (0x00000004)
#define Tsi578_SPX_CTL_INDEP_INB_DEPTH_EN                          (0x00000008)
#define Tsi578_SPX_CTL_INDEP_OUTB_DEPTH_EN                         (0x00000010)
#define Tsi578_SPX_CTL_INDEP_MAX_RETRY_EN                          (0x00000020)
#define Tsi578_SPX_CTL_INDEP_IRQ_EN                                (0x00000040)
#define Tsi578_SPX_CTL_INDEP_ILL_TRANS_ERR                         (0x00000080)
#define Tsi578_SPX_CTL_INDEP_MAX_RETRY_THRESHOLD                   (0x0000ff00)
#define Tsi578_SPX_CTL_INDEP_LUT_PAR_ERR_EN                        (0x00010000)
#define Tsi578_SPX_CTL_INDEP_LINK_INIT_NOTIFICATION_EN             (0x00020000)
#define Tsi578_SPX_CTL_INDEP_MC_TEA_EN                             (0x00040000)
#define Tsi578_SPX_CTL_INDEP_PORT_ERR_EN                           (0x00080000)
#define Tsi578_SPX_CTL_INDEP_SEND_DBG_PKT                          (0x00400000)
#define Tsi578_SPX_CTL_INDEP_DEBUG_MODE                            (0x00800000)
#define Tsi578_SPX_CTL_INDEP_TRANS_MODE                            (0x01000000)
#define Tsi578_SPX_CTL_INDEP_FORCE_REINIT                          (0x04000000)
#define Tsi578_SPX_CTL_INDEP_SCRATCH                               (0x20000000)

/* Tsi578_SPX_SILENCE_TIMER : Register Bits Masks Definitions */
#define Tsi578_SPX_SILENCE_TIMER_SILENCE_TIMER                     (0xf0000000)

/* Tsi578_SPX_SEND_MCS : Register Bits Masks Definitions */
#define Tsi578_SPX_SEND_MCS_SEND                                   (0x00000001)
#define Tsi578_SPX_SEND_MCS_DONE                                   (0x00000002)

/* Tsi578_SPX_LUT_PAR_ERR_INFO : Register Bits Masks Definitions */
#define Tsi578_SPX_LUT_PAR_ERR_INFO_PORT_NUM                       (0x0000000f)
#define Tsi578_SPX_LUT_PAR_ERR_INFO_LUT_VLD                        (0x00000040)
#define Tsi578_SPX_LUT_PAR_ERR_INFO_PTY_BIT                        (0x00000080)
#define Tsi578_SPX_LUT_PAR_ERR_INFO_LG_DESTID                      (0x00008000)
#define Tsi578_SPX_LUT_PAR_ERR_INFO_DESTID_LSB                     (0x00ff0000)
#define Tsi578_SPX_LUT_PAR_ERR_INFO_DESTID_MSB                     (0xff000000)

/* Tsi578_SPX_CS_TX : Register Bits Masks Definitions */
#define Tsi578_SPX_CS_TX_CS_EMB                                    (0x00001000)
#define Tsi578_SPX_CS_TX_CMD                                       (0x0000e000)
#define Tsi578_SPX_CS_TX_STYPE_1                                   (0x00070000)
#define Tsi578_SPX_CS_TX_PAR_1                                     (0x00f80000)
#define Tsi578_SPX_CS_TX_PAR_0                                     (0x1f000000)
#define Tsi578_SPX_CS_TX_STYPE_0                                   (0xe0000000)

/* Tsi578_SPX_INT_uint32_t : Register Bits Masks Definitions */
#define Tsi578_SPX_INT_uint32_t_TEA                                  (0x00000001)
#define Tsi578_SPX_INT_uint32_t_TTL_EXPIRED                          (0x00000002)
#define Tsi578_SPX_INT_uint32_t_INB_RDR                              (0x00000004)
#define Tsi578_SPX_INT_uint32_t_INB_DEPTH                            (0x00000008)
#define Tsi578_SPX_INT_uint32_t_OUTB_DEPTH                           (0x00000010)
#define Tsi578_SPX_INT_uint32_t_MAX_RETRY                            (0x00000020)
#define Tsi578_SPX_INT_uint32_t_IRQ_ERR                              (0x00000040)
#define Tsi578_SPX_INT_uint32_t_ILL_TRANS_ERR                        (0x00000080)
#define Tsi578_SPX_INT_uint32_t_LUT_PAR_ERR                          (0x00010000)
#define Tsi578_SPX_INT_uint32_t_LINK_INIT_NOTIFICATION               (0x00020000)
#define Tsi578_SPX_INT_uint32_t_MC_TEA                               (0x00040000)

/* Tsi578_SPX_INT_GEN : Register Bits Masks Definitions */
#define Tsi578_SPX_INT_GEN_TEA_GEN                                 (0x00000001)
#define Tsi578_SPX_INT_GEN_TTL_EXPIRED_GEN                         (0x00000002)
#define Tsi578_SPX_INT_GEN_INB_RDR_GEN                             (0x00000004)
#define Tsi578_SPX_INT_GEN_INB_DEPTH_GEN                           (0x00000008)
#define Tsi578_SPX_INT_GEN_OUTB_DEPTH_GEN                          (0x00000010)
#define Tsi578_SPX_INT_GEN_MAX_RETRY_GEN                           (0x00000020)
#define Tsi578_SPX_INT_GEN_ILL_TRANS_GEN                           (0x00000080)
#define Tsi578_SPX_INT_GEN_LUT_PAR_ERR_GEN                         (0x00010000)
#define Tsi578_SPX_INT_GEN_LINK_INIT_NOTIFICATION_GEN              (0x00020000)
#define Tsi578_SPX_INT_GEN_MC_TEA_GEN                              (0x00040000)

/* Tsi578_SPX_PSC0n1_CTRL : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC0n1_CTRL_PS1_TYPE                            (0x00000007)
#define Tsi578_SPX_PSC0n1_CTRL_PS1_DIR                             (0x00000100)
#define Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO0                           (0x00001000)
#define Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO1                           (0x00002000)
#define Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO2                           (0x00004000)
#define Tsi578_SPX_PSC0n1_CTRL_PS1_PRIO3                           (0x00008000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_TYPE                            (0x00070000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_DIR                             (0x01000000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_PRIO0                           (0x10000000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_PRIO1                           (0x20000000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_PRIO2                           (0x40000000)
#define Tsi578_SPX_PSC0n1_CTRL_PS0_PRIO3                           (0x80000000)

/* Tsi578_SPX_PSC2n3_CTRL : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC2n3_CTRL_PS3_TYPE                            (0x00000007)
#define Tsi578_SPX_PSC2n3_CTRL_PS3_DIR                             (0x00000100)
#define Tsi578_SPX_PSC2n3_CTRL_PS3_PRIO0                           (0x00001000)
#define Tsi578_SPX_PSC2n3_CTRL_PS3_PRIO1                           (0x00002000)
#define Tsi578_SPX_PSC2n3_CTRL_PS3_PRIO2                           (0x00004000)
#define Tsi578_SPX_PSC2n3_CTRL_PS3_PRIO3                           (0x00008000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_TYPE                            (0x00070000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_DIR                             (0x01000000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_PRIO0                           (0x10000000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_PRIO1                           (0x20000000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_PRIO2                           (0x40000000)
#define Tsi578_SPX_PSC2n3_CTRL_PS2_PRIO3                           (0x80000000)

/* Tsi578_SPX_PSC4n5_CTRL : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC4n5_CTRL_PS5_TYPE                            (0x00000007)
#define Tsi578_SPX_PSC4n5_CTRL_PS5_DIR                             (0x00000100)
#define Tsi578_SPX_PSC4n5_CTRL_PS5_PRIO0                           (0x00001000)
#define Tsi578_SPX_PSC4n5_CTRL_PS5_PRIO1                           (0x00002000)
#define Tsi578_SPX_PSC4n5_CTRL_PS5_PRIO2                           (0x00004000)
#define Tsi578_SPX_PSC4n5_CTRL_PS5_PRIO3                           (0x00008000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_TYPE                            (0x00070000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_DIR                             (0x01000000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_PRIO0                           (0x10000000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_PRIO1                           (0x20000000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_PRIO2                           (0x40000000)
#define Tsi578_SPX_PSC4n5_CTRL_PS4_PRIO3                           (0x80000000)

/* Tsi578_SPX_PSC0 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC0_PS0_CTR                                    (0xffffffff)

/* Tsi578_SPX_PSC1 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC1_PS1_CTR                                    (0xffffffff)

/* Tsi578_SPX_PSC2 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC2_PS2_CTR                                    (0xffffffff)

/* Tsi578_SPX_PSC3 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC3_PS3_CTR                                    (0xffffffff)

/* Tsi578_SPX_PSC4 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC4_PS4_CTR                                    (0xffffffff)

/* Tsi578_SPX_PSC5 : Register Bits Masks Definitions */
#define Tsi578_SPX_PSC5_PS5_CTR                                    (0xffffffff)

/* Tsi578_SPX_TX_Q_D_THRESH : Register Bits Masks Definitions */
#define Tsi578_SPX_TX_Q_D_THRESH_LEAK_RT                           (0x000007ff)
#define Tsi578_SPX_TX_Q_D_THRESH_DEPTH                             (0x0000f000)
#define Tsi578_SPX_TX_Q_D_THRESH_CONG_PERIOD                       (0xffff0000)

/* Tsi578_SPX_TX_Q_uint32_t : Register Bits Masks Definitions */
#define Tsi578_SPX_TX_Q_uint32_t_CONG_THRESH                         (0x0000ffff)
#define Tsi578_SPX_TX_Q_uint32_t_CONG_CTR                            (0xffff0000)

/* Tsi578_SPX_TX_Q_PERIOD : Register Bits Masks Definitions */
#define Tsi578_SPX_TX_Q_PERIOD_CONG_PERIOD_CTR                     (0xffffffff)

/* Tsi578_SPX_RX_Q_D_THRESH : Register Bits Masks Definitions */
#define Tsi578_SPX_RX_Q_D_THRESH_DEPTH                             (0x0000f000)
#define Tsi578_SPX_RX_Q_D_THRESH_CONG_PERIOD                       (0xffff0000)

/* Tsi578_SPX_RX_Q_uint32_t : Register Bits Masks Definitions */
#define Tsi578_SPX_RX_Q_uint32_t_CONG_THRESH                         (0x0000ffff)
#define Tsi578_SPX_RX_Q_uint32_t_CONG_CTR                            (0xffff0000)

/* Tsi578_SPX_RX_Q_PERIOD : Register Bits Masks Definitions */
#define Tsi578_SPX_RX_Q_PERIOD_CONG_PERIOD_CTR                     (0xffffffff)

/* Tsi578_SPX_REORDER_CTR : Register Bits Masks Definitions */
#define Tsi578_SPX_REORDER_CTR_THRESH                              (0x0000ffff)
#define Tsi578_SPX_REORDER_CTR_CTR                                 (0xffff0000)

/* Tsi578_SMACX_CFG_CH0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_CH0_RX_ALIGN_EN                           (0x00000002)
#define Tsi578_SMACX_CFG_CH0_LOS_CTL                               (0x0000000c)
#define Tsi578_SMACX_CFG_CH0_TX_CKO_EN                             (0x00000010)
#define Tsi578_SMACX_CFG_CH0_RX_DPLL_MODE                          (0x000000e0)
#define Tsi578_SMACX_CFG_CH0_RX_EQ_VAL                             (0x00000700)
#define Tsi578_SMACX_CFG_CH0_DPLL_RESET                            (0x00002000)
#define Tsi578_SMACX_CFG_CH0_RX_EN                                 (0x00004000)
#define Tsi578_SMACX_CFG_CH0_RX_PLL_PWRON                          (0x00008000)
#define Tsi578_SMACX_CFG_CH0_TX_BOOST                              (0x000f0000)
#define Tsi578_SMACX_CFG_CH0_TX_EN                                 (0x00700000)
#define Tsi578_SMACX_CFG_CH0_TX_ATTEN                              (0x07000000)
#define Tsi578_SMACX_CFG_CH0_TX_CALC                               (0x40000000)
#define Tsi578_SMACX_CFG_CH0_HALF_RATE                             (0x80000000)

/* Tsi578_SMACX_CFG_CH1 : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_CH1_RX_ALIGN_EN                           (0x00000002)
#define Tsi578_SMACX_CFG_CH1_LOS_CTL                               (0x0000000c)
#define Tsi578_SMACX_CFG_CH1_TX_CKO_EN                             (0x00000010)
#define Tsi578_SMACX_CFG_CH1_RX_DPLL_MODE                          (0x000000e0)
#define Tsi578_SMACX_CFG_CH1_RX_EQ_VAL                             (0x00000700)
#define Tsi578_SMACX_CFG_CH1_DPLL_RESET                            (0x00002000)
#define Tsi578_SMACX_CFG_CH1_RX_EN                                 (0x00004000)
#define Tsi578_SMACX_CFG_CH1_RX_PLL_PWRON                          (0x00008000)
#define Tsi578_SMACX_CFG_CH1_TX_BOOST                              (0x000f0000)
#define Tsi578_SMACX_CFG_CH1_TX_EN                                 (0x00700000)
#define Tsi578_SMACX_CFG_CH1_TX_ATTEN                              (0x07000000)
#define Tsi578_SMACX_CFG_CH1_TX_CALC                               (0x40000000)
#define Tsi578_SMACX_CFG_CH1_HALF_RATE                             (0x80000000)

/* Tsi578_SMACX_CFG_CH2 : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_CH2_RX_ALIGN_EN                           (0x00000002)
#define Tsi578_SMACX_CFG_CH2_LOS_CTL                               (0x0000000c)
#define Tsi578_SMACX_CFG_CH2_TX_CKO_EN                             (0x00000010)
#define Tsi578_SMACX_CFG_CH2_RX_DPLL_MODE                          (0x000000e0)
#define Tsi578_SMACX_CFG_CH2_RX_EQ_VAL                             (0x00000700)
#define Tsi578_SMACX_CFG_CH2_DPLL_RESET                            (0x00002000)
#define Tsi578_SMACX_CFG_CH2_RX_EN                                 (0x00004000)
#define Tsi578_SMACX_CFG_CH2_RX_PLL_PWRON                          (0x00008000)
#define Tsi578_SMACX_CFG_CH2_TX_BOOST                              (0x000f0000)
#define Tsi578_SMACX_CFG_CH2_TX_EN                                 (0x00700000)
#define Tsi578_SMACX_CFG_CH2_TX_ATTEN                              (0x07000000)
#define Tsi578_SMACX_CFG_CH2_TX_CALC                               (0x40000000)
#define Tsi578_SMACX_CFG_CH2_HALF_RATE                             (0x80000000)

/* Tsi578_SMACX_CFG_CH3 : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_CH3_RX_ALIGN_EN                           (0x00000002)
#define Tsi578_SMACX_CFG_CH3_LOS_CTL                               (0x0000000c)
#define Tsi578_SMACX_CFG_CH3_TX_CKO_EN                             (0x00000010)
#define Tsi578_SMACX_CFG_CH3_RX_DPLL_MODE                          (0x000000e0)
#define Tsi578_SMACX_CFG_CH3_RX_EQ_VAL                             (0x00000700)
#define Tsi578_SMACX_CFG_CH3_DPLL_RESET                            (0x00002000)
#define Tsi578_SMACX_CFG_CH3_RX_EN                                 (0x00004000)
#define Tsi578_SMACX_CFG_CH3_RX_PLL_PWRON                          (0x00008000)
#define Tsi578_SMACX_CFG_CH3_TX_BOOST                              (0x000f0000)
#define Tsi578_SMACX_CFG_CH3_TX_EN                                 (0x00700000)
#define Tsi578_SMACX_CFG_CH3_TX_ATTEN                              (0x07000000)
#define Tsi578_SMACX_CFG_CH3_TX_CALC                               (0x40000000)
#define Tsi578_SMACX_CFG_CH3_HALF_RATE                             (0x80000000)

/* Tsi578_SMACX_CFG_GBL : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_GBL_MPLL_CK_OFF                           (0x00000040)
#define Tsi578_SMACX_CFG_GBL_MPLL_PWR_ON                           (0x00000080)
#define Tsi578_SMACX_CFG_GBL_LOS_LVL                               (0x00001f00)
#define Tsi578_SMACX_CFG_GBL_ACJT_LVL                              (0x001f0000)
#define Tsi578_SMACX_CFG_GBL_TX_LVL                                (0x1f000000)
#define Tsi578_SMACX_CFG_GBL_BYPASS_INIT                           (0x40000000)
#define Tsi578_SMACX_CFG_GBL_SERDES_RSTN                           (0x80000000)

/* Tsi578_SMACX_CFG_GBLB : Register Bits Masks Definitions */
#define Tsi578_SMACX_CFG_GBLB_MPLL_NCY5                            (0x000000c0)
#define Tsi578_SMACX_CFG_GBLB_MPLL_PRESCALE                        (0x00300000)

/* Tsi578_SMACX_DLOOP_CLK_SEL : Register Bits Masks Definitions */
#define Tsi578_SMACX_DLOOP_CLK_SEL_CLK_SEL                         (0x00000003)
#define Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4                         (0x00000004)
#define Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1                         (0x00000008)
#define Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4                     (0x00000010)
#define Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1                     (0x00000020)
#define Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_RX                         (0x00000040)
#define Tsi578_SMACX_DLOOP_CLK_SEL_SWAP_TX                         (0x00000080)
#define Tsi578_SMACX_DLOOP_CLK_SEL_DLB_EVEN_EN                     (0x00000100)
#define Tsi578_SMACX_DLOOP_CLK_SEL_DLB_ODD_EN                      (0x00000200)
#define Tsi578_SMACX_DLOOP_CLK_SEL_MAC_MODE                        (0x00000400)
#define Tsi578_SMACX_DLOOP_CLK_SEL_LINE_LB                         (0x0000f000)
#define Tsi578_SMACX_DLOOP_CLK_SEL_DLT_THRESH                      (0x7fff0000)
#define Tsi578_SMACX_DLOOP_CLK_SEL_DLT_EN                          (0x80000000)

/* Tsi578_SMACX_SERDES_OUTPUT_PINS : Register Bits Masks Definitions */
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_POWER_GOOD                 (0x00000004)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_OP_DONE                    (0x00000008)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_LOS                        (0x000000f0)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_RX_VALID                   (0x00000f00)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_RX_PLL_STATE               (0x0000f000)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_TX_RXPRES                  (0x000f0000)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_TX_DONE                    (0x00f00000)
#define Tsi578_SMACX_SERDES_OUTPUT_PINS_RX_COMMA_DET               (0xff000000)

/* Tsi578_MCES_PIN_CTRL : Register Bits Masks Definitions */
#define Tsi578_MCES_PIN_CTRL_MCES_CTRL                             (0x30000000)

/* Tsi578_SMACX_SPY_BUS_CFG : Register Bits Masks Definitions */
#define Tsi578_SMACX_SPY_BUS_CFG_SEL                               (0x0fffffff)
#define Tsi578_SMACX_SPY_BUS_CFG_EN                                (0xf0000000)

/* Tsi578_ERRX_INJ_CMP_MASK : Register Bits Masks Definitions */
#define Tsi578_ERRX_INJ_CMP_MASK_CMP_MASK                          (0xffffffff)

/* Tsi578_ERRX_INJ_CMP_BIT_VLD : Register Bits Masks Definitions */
#define Tsi578_ERRX_INJ_CMP_BIT_VLD_CMP_BIT_VLD                    (0xffffffff)

/* Tsi578_ERRX_INJ : Register Bits Masks Definitions */
#define Tsi578_ERRX_INJ_ERR_INJ                                    (0xffffffff)

/* Tsi578_ERRX_INJ_BIT_VLD : Register Bits Masks Definitions */
#define Tsi578_ERRX_INJ_BIT_VLD_ERR_INJ_BIT_VLD                    (0xffffffff)

/* Tsi578_FAB_CTL : Register Bits Masks Definitions */
#define Tsi578_FAB_CTL_TEA_OUT                                     (0x0000ffff)
#define Tsi578_FAB_CTL_TEA_EN                                      (0x00010000)
#define Tsi578_FAB_CTL_TEA_INT_EN                                  (0x00020000)
#define Tsi578_FAB_CTL_IN_ARB_MODE                                 (0x00300000)
#define Tsi578_FAB_CTL_RDR_LIMIT_EN                                (0x00800000)
#define Tsi578_FAB_CTL_RDR_LIMIT                                   (0x0f000000)

/* Tsi578_FAB_INT_STAT : Register Bits Masks Definitions */
#define Tsi578_FAB_INT_STAT_PORT0_IRQ                              (0x00000001)
#define Tsi578_FAB_INT_STAT_PORT1_IRQ                              (0x00000002)
#define Tsi578_FAB_INT_STAT_PORT2_IRQ                              (0x00000004)
#define Tsi578_FAB_INT_STAT_PORT3_IRQ                              (0x00000008)
#define Tsi578_FAB_INT_STAT_PORT4_IRQ                              (0x00000010)
#define Tsi578_FAB_INT_STAT_PORT5_IRQ                              (0x00000020)
#define Tsi578_FAB_INT_STAT_PORT6_IRQ                              (0x00000040)
#define Tsi578_FAB_INT_STAT_PORT7_IRQ                              (0x00000080)
#define Tsi578_FAB_INT_STAT_PORT8_IRQ                              (0x00000100)
#define Tsi578_FAB_INT_STAT_PORT9_IRQ                              (0x00000200)
#define Tsi578_FAB_INT_STAT_PORT10_IRQ                             (0x00000400)
#define Tsi578_FAB_INT_STAT_PORT11_IRQ                             (0x00000800)
#define Tsi578_FAB_INT_STAT_PORT12_IRQ                             (0x00001000)
#define Tsi578_FAB_INT_STAT_PORT13_IRQ                             (0x00002000)
#define Tsi578_FAB_INT_STAT_PORT14_IRQ                             (0x00004000)
#define Tsi578_FAB_INT_STAT_PORT15_IRQ                             (0x00008000)

/* Tsi578_RIO_MC_LAT_ERR : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_LAT_ERR_P0_ERR                               (0x00000001)
#define Tsi578_RIO_MC_LAT_ERR_P1_ERR                               (0x00000002)
#define Tsi578_RIO_MC_LAT_ERR_P2_ERR                               (0x00000004)
#define Tsi578_RIO_MC_LAT_ERR_P3_ERR                               (0x00000008)
#define Tsi578_RIO_MC_LAT_ERR_P4_ERR                               (0x00000010)
#define Tsi578_RIO_MC_LAT_ERR_P5_ERR                               (0x00000020)
#define Tsi578_RIO_MC_LAT_ERR_P6_ERR                               (0x00000040)
#define Tsi578_RIO_MC_LAT_ERR_P7_ERR                               (0x00000080)
#define Tsi578_RIO_MC_LAT_ERR_P8_ERR                               (0x00000100)
#define Tsi578_RIO_MC_LAT_ERR_P9_ERR                               (0x00000200)
#define Tsi578_RIO_MC_LAT_ERR_P10_ERR                              (0x00000400)
#define Tsi578_RIO_MC_LAT_ERR_P11_ERR                              (0x00000800)
#define Tsi578_RIO_MC_LAT_ERR_P12_ERR                              (0x00001000)
#define Tsi578_RIO_MC_LAT_ERR_P13_ERR                              (0x00002000)
#define Tsi578_RIO_MC_LAT_ERR_P14_ERR                              (0x00004000)
#define Tsi578_RIO_MC_LAT_ERR_P15_ERR                              (0x00008000)

/* Tsi578_RIO_MC_LAT_ERR_SET : Register Bits Masks Definitions */
#define Tsi578_RIO_MC_LAT_ERR_SET_P0_SET                           (0x00000001)
#define Tsi578_RIO_MC_LAT_ERR_SET_P1_SET                           (0x00000002)
#define Tsi578_RIO_MC_LAT_ERR_SET_P2_SET                           (0x00000004)
#define Tsi578_RIO_MC_LAT_ERR_SET_P3_SET                           (0x00000008)
#define Tsi578_RIO_MC_LAT_ERR_SET_P4_SET                           (0x00000010)
#define Tsi578_RIO_MC_LAT_ERR_SET_P5_SET                           (0x00000020)
#define Tsi578_RIO_MC_LAT_ERR_SET_P6_SET                           (0x00000040)
#define Tsi578_RIO_MC_LAT_ERR_SET_P7_SET                           (0x00000080)
#define Tsi578_RIO_MC_LAT_ERR_SET_P8_SET                           (0x00000100)
#define Tsi578_RIO_MC_LAT_ERR_SET_P9_SET                           (0x00000200)
#define Tsi578_RIO_MC_LAT_ERR_SET_P10_SET                          (0x00000400)
#define Tsi578_RIO_MC_LAT_ERR_SET_P11_SET                          (0x00000800)
#define Tsi578_RIO_MC_LAT_ERR_SET_P12_SET                          (0x00001000)
#define Tsi578_RIO_MC_LAT_ERR_SET_P13_SET                          (0x00002000)
#define Tsi578_RIO_MC_LAT_ERR_SET_P14_SET                          (0x00004000)
#define Tsi578_RIO_MC_LAT_ERR_SET_P15_SET                          (0x00008000)

/* Tsi578_PKTGEN_CNTRL : Register Bits Masks Definitions */
#define Tsi578_PKTGEN_CNTRL_REPEAT                                 (0x00ff0000)
#define Tsi578_PKTGEN_CNTRL_PACE                                   (0x3f000000)
#define Tsi578_PKTGEN_CNTRL_INIT                                   (0x40000000)
#define Tsi578_PKTGEN_CNTRL_PKTGEN_EN                              (0x80000000)

/* Meaningful field values                               */
#define Tsi578_PKTGEN_CNTRL_PACE_RANDOM      (0x3F000000)
#define Tsi578_PKTGEN_CNTRL_PACE_SLOWEST     (0x3E000000)
#define Tsi578_PKTGEN_CNTRL_PACE_FASTEST     (0x00000000)
#define Tsi578_PKTGEN_CNTRL_REPEAT_ONCE      (0x00010000)
#define Tsi578_PKTGEN_CNTRL_REPEAT_FOREVER   (0x00000000)

// Multicast buffer holds 8 maximum sized packets.
#define Tsi578_PKTGEN_BUFF_SIZE (35 * 8)

/* Tsi578_WRKQ_DATA : Register Bits Masks Definitions */
#define Tsi578_WRKQ_DATA_RD_WR_DATA                                (0xffffffff)

/* Tsi578_PKTGEN_CNTRL : Register Bits Masks Definitions */
#define Tsi578_LST_DTM_uint32_t_EOP            (0x80000000)
#define Tsi578_LST_DTM_uint32_t_uint32_t         (0x60000000)
#define Tsi578_LST_DTM_uint32_t_MC_PKT_DISCARD (0x01000000)
#define Tsi578_LST_DTM_uint32_t_RD_PTR         (0x000003FF)

/* Odd or even multiple of 4 bytes                       */
#define Tsi578_LST_DTM_uint32_t_uint32_t_ODD     (0x20000000)
#define Tsi578_LST_DTM_uint32_t_uint32_t_EVEN    (0x60000000)

/* Tsi578_TX_ENBL        Register Bits Masks Definitions */
#define Tsi578_TX_ENBL_START                 (0x80000000)
#define Tsi578_TX_ENBL_DONE                  (0x40000000)

/* Tsi578_GLOB_INT_uint32_t : Register Bits Masks Definitions */
#define Tsi578_GLOB_INT_uint32_t_PORT0                               (0x00000001)
#define Tsi578_GLOB_INT_uint32_t_PORT1                               (0x00000002)
#define Tsi578_GLOB_INT_uint32_t_PORT2                               (0x00000004)
#define Tsi578_GLOB_INT_uint32_t_PORT3                               (0x00000008)
#define Tsi578_GLOB_INT_uint32_t_PORT4                               (0x00000010)
#define Tsi578_GLOB_INT_uint32_t_PORT5                               (0x00000020)
#define Tsi578_GLOB_INT_uint32_t_PORT6                               (0x00000040)
#define Tsi578_GLOB_INT_uint32_t_PORT7                               (0x00000080)
#define Tsi578_GLOB_INT_uint32_t_PORT8                               (0x00000100)
#define Tsi578_GLOB_INT_uint32_t_PORT9                               (0x00000200)
#define Tsi578_GLOB_INT_uint32_t_PORT10                              (0x00000400)
#define Tsi578_GLOB_INT_uint32_t_PORT11                              (0x00000800)
#define Tsi578_GLOB_INT_uint32_t_PORT12                              (0x00001000)
#define Tsi578_GLOB_INT_uint32_t_PORT13                              (0x00002000)
#define Tsi578_GLOB_INT_uint32_t_PORT14                              (0x00004000)
#define Tsi578_GLOB_INT_uint32_t_PORT15                              (0x00008000)
#define Tsi578_GLOB_INT_uint32_t_MC_LAT                              (0x00020000)
#define Tsi578_GLOB_INT_uint32_t_TEA                                 (0x01000000)
#define Tsi578_GLOB_INT_uint32_t_I2C                                 (0x02000000)
#define Tsi578_GLOB_INT_uint32_t_MCS                                 (0x04000000)
#define Tsi578_GLOB_INT_uint32_t_RCS                                 (0x08000000)

/* Tsi578_GLOB_INT_ENABLE : Register Bits Masks Definitions */
#define Tsi578_GLOB_INT_ENABLE_PORT0_EN                            (0x00000001)
#define Tsi578_GLOB_INT_ENABLE_PORT1_EN                            (0x00000002)
#define Tsi578_GLOB_INT_ENABLE_PORT2_EN                            (0x00000004)
#define Tsi578_GLOB_INT_ENABLE_PORT3_EN                            (0x00000008)
#define Tsi578_GLOB_INT_ENABLE_PORT4_EN                            (0x00000010)
#define Tsi578_GLOB_INT_ENABLE_PORT5_EN                            (0x00000020)
#define Tsi578_GLOB_INT_ENABLE_PORT6_EN                            (0x00000040)
#define Tsi578_GLOB_INT_ENABLE_PORT7_EN                            (0x00000080)
#define Tsi578_GLOB_INT_ENABLE_PORT8_EN                            (0x00000100)
#define Tsi578_GLOB_INT_ENABLE_PORT9_EN                            (0x00000200)
#define Tsi578_GLOB_INT_ENABLE_PORT10_EN                           (0x00000400)
#define Tsi578_GLOB_INT_ENABLE_PORT11_EN                           (0x00000800)
#define Tsi578_GLOB_INT_ENABLE_PORT12_EN                           (0x00001000)
#define Tsi578_GLOB_INT_ENABLE_PORT13_EN                           (0x00002000)
#define Tsi578_GLOB_INT_ENABLE_PORT14_EN                           (0x00004000)
#define Tsi578_GLOB_INT_ENABLE_PORT15_EN                           (0x00008000)
#define Tsi578_GLOB_INT_ENABLE_MC_LAT_EN                           (0x00020000)
#define Tsi578_GLOB_INT_ENABLE_TEA_EN                              (0x01000000)
#define Tsi578_GLOB_INT_ENABLE_I2C_EN                              (0x02000000)
#define Tsi578_GLOB_INT_ENABLE_MCS_EN                              (0x04000000)
#define Tsi578_GLOB_INT_ENABLE_RCS_EN                              (0x08000000)

/* Tsi578_GLOB_DEV_ID_SEL : Register Bits Masks Definitions */
#define Tsi578_GLOB_DEV_ID_SEL_SEL                                 (0x00000003)

/* Tsi578_GLOB_SPY_BUS_CFG : Register Bits Masks Definitions */
#define Tsi578_GLOB_SPY_BUS_CFG_EN                                 (0x00000001)
#define Tsi578_GLOB_SPY_BUS_CFG_CTRL0                              (0x000f0000)
#define Tsi578_GLOB_SPY_BUS_CFG_CTRL1                              (0x0f000000)

/* Tsi578_RIO_PW_TIMEOUT : Register Bits Masks Definitions */
#define Tsi578_RIO_PW_TIMEOUT_PW_TIMER                             (0xf0000000)
#define Tsi578_RIO_PW_TIMEOUT_DISABLE                              (0x00000000)
#define Tsi578_RIO_PW_TIMEOUT_167p7ms                              (0x10000000)
#define Tsi578_RIO_PW_TIMEOUT_335p5ms                              (0x20000000)
#define Tsi578_RIO_PW_TIMEOUT_671p1ms                              (0x40000000)
#define Tsi578_RIO_PW_TIMEOUT_1340ms                               (0x80000000)

/* Tsi578_RIO_PW_OREQ_uint32_t : Register Bits Masks Definitions */
#define Tsi578_RIO_PW_OREQ_uint32_t_PORTX_OREG                       (0x0000ffff)

/* Tsi578_RIOX_MC_REG_VER : Register Bits Masks Definitions */
#define Tsi578_RIOX_MC_REG_VER_REG_VER                             (0x000000ff)

/* Tsi578_RIOX_MC_LAT_LIMIT : Register Bits Masks Definitions */
#define Tsi578_RIOX_MC_LAT_LIMIT_MAX_MC_LAT                        (0x00ffffff)
#define Tsi578_RIOX_MC_LAT_LIMIT_AUTODEAD                          (0x80000000)

/* Tsi578_SPX_ISF_WM : Register Bits Masks Definitions */
#define Tsi578_SPX_ISF_WM_PRIO0WM                                  (0x00000007)
#define Tsi578_SPX_ISF_WM_PRIO1WM                                  (0x00000700)
#define Tsi578_SPX_ISF_WM_PRIO2WM                                  (0x00070000)

/* Tsi578_SPX_WRR_0 : Register Bits Masks Definitions */
#define Tsi578_SPX_WRR_0_WEIGHT                                    (0x0000000f)
#define Tsi578_SPX_WRR_0_CHOOSE_UC                                 (0x01000000)
#define Tsi578_SPX_WRR_0_WRR_EN                                    (0x02000000)

/* Tsi578_SPX_WRR_1 : Register Bits Masks Definitions */
#define Tsi578_SPX_WRR_1_WEIGHT                                    (0x0000000f)
#define Tsi578_SPX_WRR_1_CHOOSE_UC                                 (0x01000000)
#define Tsi578_SPX_WRR_1_WRR_EN                                    (0x02000000)

/* Tsi578_SPX_WRR_2 : Register Bits Masks Definitions */
#define Tsi578_SPX_WRR_2_WEIGHT                                    (0x0000000f)
#define Tsi578_SPX_WRR_2_CHOOSE_UC                                 (0x01000000)
#define Tsi578_SPX_WRR_2_WRR_EN                                    (0x02000000)

/* Tsi578_SPX_WRR_3 : Register Bits Masks Definitions */
#define Tsi578_SPX_WRR_3_WEIGHT                                    (0x0000000f)
#define Tsi578_SPX_WRR_3_CHOOSE_UC                                 (0x01000000)
#define Tsi578_SPX_WRR_3_WRR_EN                                    (0x02000000)

/* Tsi578_I2C_DEVID : Register Bits Masks Definitions */
#define Tsi578_I2C_DEVID_REV                                       (0x0000000f)

/* Tsi578_I2C_RESET : Register Bits Masks Definitions */
#define Tsi578_I2C_RESET_SRESET                                    (0x80000000)

/* Tsi578_I2C_MST_CFG : Register Bits Masks Definitions */
#define Tsi578_I2C_MST_CFG_DEV_ADDR                                (0x0000007f)
#define Tsi578_I2C_MST_CFG_PA_SIZE                                 (0x00030000)
#define Tsi578_I2C_MST_CFG_DORDER                                  (0x00800000)

/* Tsi578_I2C_MST_CNTRL : Register Bits Masks Definitions */
#define Tsi578_I2C_MST_CNTRL_PADDR                                 (0x0000ffff)
#define Tsi578_I2C_MST_CNTRL_SIZE                                  (0x07000000)
#define Tsi578_I2C_MST_CNTRL_WRITE                                 (0x40000000)
#define Tsi578_I2C_MST_CNTRL_START                                 (0x80000000)

/* Tsi578_I2C_MST_RDATA : Register Bits Masks Definitions */
#define Tsi578_I2C_MST_RDATA_RBYTE0                                (0x000000ff)
#define Tsi578_I2C_MST_RDATA_RBYTE1                                (0x0000ff00)
#define Tsi578_I2C_MST_RDATA_RBYTE2                                (0x00ff0000)
#define Tsi578_I2C_MST_RDATA_RBYTE3                                (0xff000000)

/* Tsi578_I2C_MST_TDATA : Register Bits Masks Definitions */
#define Tsi578_I2C_MST_TDATA_TBYTE0                                (0x000000ff)
#define Tsi578_I2C_MST_TDATA_TBYTE1                                (0x0000ff00)
#define Tsi578_I2C_MST_TDATA_TBYTE2                                (0x00ff0000)
#define Tsi578_I2C_MST_TDATA_TBYTE3                                (0xff000000)

/* Tsi578_I2C_ACC_STAT : Register Bits Masks Definitions */
#define Tsi578_I2C_ACC_STAT_MST_NBYTES                             (0x0000000f)
#define Tsi578_I2C_ACC_STAT_MST_AN                                 (0x00000100)
#define Tsi578_I2C_ACC_STAT_MST_PHASE                              (0x00000e00)
#define Tsi578_I2C_ACC_STAT_MST_ACTIVE                             (0x00008000)
#define Tsi578_I2C_ACC_STAT_SLV_PA                                 (0x00ff0000)
#define Tsi578_I2C_ACC_STAT_SLV_AN                                 (0x01000000)
#define Tsi578_I2C_ACC_STAT_SLV_PHASE                              (0x06000000)
#define Tsi578_I2C_ACC_STAT_SLV_WAIT                               (0x08000000)
#define Tsi578_I2C_ACC_STAT_BUS_ACTIVE                             (0x40000000)
#define Tsi578_I2C_ACC_STAT_SLV_ACTIVE                             (0x80000000)

/* Tsi578_I2C_INT_STAT : Register Bits Masks Definitions */
#define Tsi578_I2C_INT_STAT_MA_OK                                  (0x00000001)
#define Tsi578_I2C_INT_STAT_MA_ATMO                                (0x00000002)
#define Tsi578_I2C_INT_STAT_MA_NACK                                (0x00000004)
#define Tsi578_I2C_INT_STAT_MA_TMO                                 (0x00000008)
#define Tsi578_I2C_INT_STAT_MA_COL                                 (0x00000010)
#define Tsi578_I2C_INT_STAT_MA_DIAG                                (0x00000080)
#define Tsi578_I2C_INT_STAT_SA_OK                                  (0x00000100)
#define Tsi578_I2C_INT_STAT_SA_READ                                (0x00000200)
#define Tsi578_I2C_INT_STAT_SA_WRITE                               (0x00000400)
#define Tsi578_I2C_INT_STAT_SA_FAIL                                (0x00000800)
#define Tsi578_I2C_INT_STAT_BL_OK                                  (0x00010000)
#define Tsi578_I2C_INT_STAT_BL_FAIL                                (0x00020000)
#define Tsi578_I2C_INT_STAT_IMB_FULL                               (0x01000000)
#define Tsi578_I2C_INT_STAT_OMB_EMPTY                              (0x02000000)

/* Tsi578_I2C_INT_ENABLE : Register Bits Masks Definitions */
#define Tsi578_I2C_INT_ENABLE_MA_OK                                (0x00000001)
#define Tsi578_I2C_INT_ENABLE_MA_ATMO                              (0x00000002)
#define Tsi578_I2C_INT_ENABLE_MA_NACK                              (0x00000004)
#define Tsi578_I2C_INT_ENABLE_MA_TMO                               (0x00000008)
#define Tsi578_I2C_INT_ENABLE_MA_COL                               (0x00000010)
#define Tsi578_I2C_INT_ENABLE_MA_DIAG                              (0x00000080)
#define Tsi578_I2C_INT_ENABLE_SA_OK                                (0x00000100)
#define Tsi578_I2C_INT_ENABLE_SA_READ                              (0x00000200)
#define Tsi578_I2C_INT_ENABLE_SA_WRITE                             (0x00000400)
#define Tsi578_I2C_INT_ENABLE_SA_FAIL                              (0x00000800)
#define Tsi578_I2C_INT_ENABLE_BL_OK                                (0x00010000)
#define Tsi578_I2C_INT_ENABLE_BL_FAIL                              (0x00020000)
#define Tsi578_I2C_INT_ENABLE_IMB_FULL                             (0x01000000)
#define Tsi578_I2C_INT_ENABLE_OMB_EMPTY                            (0x02000000)

/* Tsi578_I2C_INT_SET : Register Bits Masks Definitions */
#define Tsi578_I2C_INT_SET_MA_OK                                   (0x00000001)
#define Tsi578_I2C_INT_SET_MA_ATMO                                 (0x00000002)
#define Tsi578_I2C_INT_SET_MA_NACK                                 (0x00000004)
#define Tsi578_I2C_INT_SET_MA_TMO                                  (0x00000008)
#define Tsi578_I2C_INT_SET_MA_COL                                  (0x00000010)
#define Tsi578_I2C_INT_SET_MA_DIAG                                 (0x00000080)
#define Tsi578_I2C_INT_SET_SA_OK                                   (0x00000100)
#define Tsi578_I2C_INT_SET_SA_READ                                 (0x00000200)
#define Tsi578_I2C_INT_SET_SA_WRITE                                (0x00000400)
#define Tsi578_I2C_INT_SET_SA_FAIL                                 (0x00000800)
#define Tsi578_I2C_INT_SET_BL_OK                                   (0x00010000)
#define Tsi578_I2C_INT_SET_BL_FAIL                                 (0x00020000)
#define Tsi578_I2C_INT_SET_IMB_FULL                                (0x01000000)
#define Tsi578_I2C_INT_SET_OMB_EMPTY                               (0x02000000)

/* Tsi578_I2C_SLV_CFG : Register Bits Masks Definitions */
#define Tsi578_I2C_SLV_CFG_SLV_ADDR                                (0x0000007f)
#define Tsi578_I2C_SLV_CFG_SLV_UNLK                                (0x01000000)
#define Tsi578_I2C_SLV_CFG_SLV_EN                                  (0x10000000)
#define Tsi578_I2C_SLV_CFG_ALRT_EN                                 (0x20000000)
#define Tsi578_I2C_SLV_CFG_WR_EN                                   (0x40000000)
#define Tsi578_I2C_SLV_CFG_RD_EN                                   (0x80000000)

/* Tsi578_I2C_BOOT_CNTRL : Register Bits Masks Definitions */
#define Tsi578_I2C_BOOT_CNTRL_PADDR                                (0x00001fff)
#define Tsi578_I2C_BOOT_CNTRL_PAGE_MODE                            (0x0000e000)
#define Tsi578_I2C_BOOT_CNTRL_BOOT_ADDR                            (0x007f0000)
#define Tsi578_I2C_BOOT_CNTRL_BUNLK                                (0x10000000)
#define Tsi578_I2C_BOOT_CNTRL_BINC                                 (0x20000000)
#define Tsi578_I2C_BOOT_CNTRL_PSIZE                                (0x40000000)
#define Tsi578_I2C_BOOT_CNTRL_CHAIN                                (0x80000000)

/* Tsi578_EXI2C_REG_WADDR : Register Bits Masks Definitions */
#define Tsi578_EXI2C_REG_WADDR_ADDR                                (0xfffffffc)

/* Tsi578_EXI2C_REG_WDATA : Register Bits Masks Definitions */
#define Tsi578_EXI2C_REG_WDATA_WDATA                               (0xffffffff)

/* Tsi578_EXI2C_REG_RADDR : Register Bits Masks Definitions */
#define Tsi578_EXI2C_REG_RADDR_ADDR                                (0xfffffffc)

/* Tsi578_EXI2C_REG_RDATA : Register Bits Masks Definitions */
#define Tsi578_EXI2C_REG_RDATA_RDATA                               (0xffffffff)

/* Tsi578_EXI2C_ACC_STAT : Register Bits Masks Definitions */
#define Tsi578_EXI2C_ACC_STAT_ALERT_FLAG                           (0x00000001)
#define Tsi578_EXI2C_ACC_STAT_IMB_FLAG                             (0x00000004)
#define Tsi578_EXI2C_ACC_STAT_OMB_FLAG                             (0x00000008)
#define Tsi578_EXI2C_ACC_STAT_ACC_OK                               (0x00000080)

/* Tsi578_EXI2C_ACC_CNTRL : Register Bits Masks Definitions */
#define Tsi578_EXI2C_ACC_CNTRL_WINC                                (0x00000004)
#define Tsi578_EXI2C_ACC_CNTRL_RINC                                (0x00000008)
#define Tsi578_EXI2C_ACC_CNTRL_WSIZE                               (0x00000030)
#define Tsi578_EXI2C_ACC_CNTRL_RSIZE                               (0x000000c0)

/* Tsi578_EXI2C_STAT : Register Bits Masks Definitions */
#define Tsi578_EXI2C_STAT_PORT0                                    (0x00000001)
#define Tsi578_EXI2C_STAT_PORT1                                    (0x00000002)
#define Tsi578_EXI2C_STAT_PORT2                                    (0x00000004)
#define Tsi578_EXI2C_STAT_PORT3                                    (0x00000008)
#define Tsi578_EXI2C_STAT_PORT4                                    (0x00000010)
#define Tsi578_EXI2C_STAT_PORT5                                    (0x00000020)
#define Tsi578_EXI2C_STAT_PORT6                                    (0x00000040)
#define Tsi578_EXI2C_STAT_PORT7                                    (0x00000080)
#define Tsi578_EXI2C_STAT_PORT8                                    (0x00000100)
#define Tsi578_EXI2C_STAT_PORT9                                    (0x00000200)
#define Tsi578_EXI2C_STAT_PORT10                                   (0x00000400)
#define Tsi578_EXI2C_STAT_PORT11                                   (0x00000800)
#define Tsi578_EXI2C_STAT_PORT12                                   (0x00001000)
#define Tsi578_EXI2C_STAT_PORT13                                   (0x00002000)
#define Tsi578_EXI2C_STAT_PORT14                                   (0x00004000)
#define Tsi578_EXI2C_STAT_PORT15                                   (0x00008000)
#define Tsi578_EXI2C_STAT_MCE                                      (0x00010000)
#define Tsi578_EXI2C_STAT_MC_LAT                                   (0x00020000)
#define Tsi578_EXI2C_STAT_LOGICAL                                  (0x00040000)
#define Tsi578_EXI2C_STAT_MCS                                      (0x00400000)
#define Tsi578_EXI2C_STAT_RCS                                      (0x00800000)
#define Tsi578_EXI2C_STAT_TEA                                      (0x01000000)
#define Tsi578_EXI2C_STAT_I2C                                      (0x02000000)
#define Tsi578_EXI2C_STAT_IMBR                                     (0x04000000)
#define Tsi578_EXI2C_STAT_OMBW                                     (0x08000000)
#define Tsi578_EXI2C_STAT_SW_STAT0                                 (0x10000000)
#define Tsi578_EXI2C_STAT_SW_STAT1                                 (0x20000000)
#define Tsi578_EXI2C_STAT_SW_STAT2                                 (0x40000000)
#define Tsi578_EXI2C_STAT_RESET                                    (0x80000000)

/* Tsi578_EXI2C_STAT_ENABLE : Register Bits Masks Definitions */
#define Tsi578_EXI2C_STAT_ENABLE_PORT0                             (0x00000001)
#define Tsi578_EXI2C_STAT_ENABLE_PORT1                             (0x00000002)
#define Tsi578_EXI2C_STAT_ENABLE_PORT2                             (0x00000004)
#define Tsi578_EXI2C_STAT_ENABLE_PORT3                             (0x00000008)
#define Tsi578_EXI2C_STAT_ENABLE_PORT4                             (0x00000010)
#define Tsi578_EXI2C_STAT_ENABLE_PORT5                             (0x00000020)
#define Tsi578_EXI2C_STAT_ENABLE_PORT6                             (0x00000040)
#define Tsi578_EXI2C_STAT_ENABLE_PORT7                             (0x00000080)
#define Tsi578_EXI2C_STAT_ENABLE_PORT8                             (0x00000100)
#define Tsi578_EXI2C_STAT_ENABLE_PORT9                             (0x00000200)
#define Tsi578_EXI2C_STAT_ENABLE_PORT10                            (0x00000400)
#define Tsi578_EXI2C_STAT_ENABLE_PORT11                            (0x00000800)
#define Tsi578_EXI2C_STAT_ENABLE_PORT12                            (0x00001000)
#define Tsi578_EXI2C_STAT_ENABLE_PORT13                            (0x00002000)
#define Tsi578_EXI2C_STAT_ENABLE_PORT14                            (0x00004000)
#define Tsi578_EXI2C_STAT_ENABLE_PORT15                            (0x00008000)
#define Tsi578_EXI2C_STAT_ENABLE_MCE                               (0x00010000)
#define Tsi578_EXI2C_STAT_ENABLE_MC_LAT                            (0x00020000)
#define Tsi578_EXI2C_STAT_ENABLE_LOGICAL                           (0x00040000)
#define Tsi578_EXI2C_STAT_ENABLE_MCS                               (0x00400000)
#define Tsi578_EXI2C_STAT_ENABLE_RCS                               (0x00800000)
#define Tsi578_EXI2C_STAT_ENABLE_TEA                               (0x01000000)
#define Tsi578_EXI2C_STAT_ENABLE_I2C                               (0x02000000)
#define Tsi578_EXI2C_STAT_ENABLE_IMBR                              (0x04000000)
#define Tsi578_EXI2C_STAT_ENABLE_OMBW                              (0x08000000)
#define Tsi578_EXI2C_STAT_ENABLE_SW_STAT0                          (0x10000000)
#define Tsi578_EXI2C_STAT_ENABLE_SW_STAT1                          (0x20000000)
#define Tsi578_EXI2C_STAT_ENABLE_SW_STAT2                          (0x40000000)
#define Tsi578_EXI2C_STAT_ENABLE_RESET                             (0x80000000)

/* Tsi578_EXI2C_MBOX_OUT : Register Bits Masks Definitions */
#define Tsi578_EXI2C_MBOX_OUT_DATA                                 (0xffffffff)

/* Tsi578_EXI2C_MBOX_IN : Register Bits Masks Definitions */
#define Tsi578_EXI2C_MBOX_IN_DATA                                  (0xffffffff)

/* Tsi578_I2C_X : Register Bits Masks Definitions */
#define Tsi578_I2C_X_MARBTO                                        (0x00000001)
#define Tsi578_I2C_X_MSCLTO                                        (0x00000002)
#define Tsi578_I2C_X_MBTTO                                         (0x00000004)
#define Tsi578_I2C_X_MTRTO                                         (0x00000008)
#define Tsi578_I2C_X_MCOL                                          (0x00000010)
#define Tsi578_I2C_X_MNACK                                         (0x00000020)
#define Tsi578_I2C_X_BLOK                                          (0x00000100)
#define Tsi578_I2C_X_BLNOD                                         (0x00000200)
#define Tsi578_I2C_X_BLSZ                                          (0x00000400)
#define Tsi578_I2C_X_BLERR                                         (0x00000800)
#define Tsi578_I2C_X_BLTO                                          (0x00001000)
#define Tsi578_I2C_X_MTD                                           (0x00004000)
#define Tsi578_I2C_X_SSCLTO                                        (0x00020000)
#define Tsi578_I2C_X_SBTTO                                         (0x00040000)
#define Tsi578_I2C_X_STRTO                                         (0x00080000)
#define Tsi578_I2C_X_SCOL                                          (0x00100000)
#define Tsi578_I2C_X_OMBR                                          (0x00400000)
#define Tsi578_I2C_X_IMBW                                          (0x00800000)
#define Tsi578_I2C_X_DCMDD                                         (0x01000000)
#define Tsi578_I2C_X_DHIST                                         (0x02000000)
#define Tsi578_I2C_X_DTIMER                                        (0x04000000)
#define Tsi578_I2C_X_SD                                            (0x10000000)
#define Tsi578_I2C_X_SDR                                           (0x20000000)
#define Tsi578_I2C_X_SDW                                           (0x40000000)

/* Tsi578_I2C_NEW_EVENT : Register Bits Masks Definitions */
#define Tsi578_I2C_NEW_EVENT_MARBTO                                (0x00000001)
#define Tsi578_I2C_NEW_EVENT_MSCLTO                                (0x00000002)
#define Tsi578_I2C_NEW_EVENT_MBTTO                                 (0x00000004)
#define Tsi578_I2C_NEW_EVENT_MTRTO                                 (0x00000008)
#define Tsi578_I2C_NEW_EVENT_MCOL                                  (0x00000010)
#define Tsi578_I2C_NEW_EVENT_MNACK                                 (0x00000020)
#define Tsi578_I2C_NEW_EVENT_BLOK                                  (0x00000100)
#define Tsi578_I2C_NEW_EVENT_BLNOD                                 (0x00000200)
#define Tsi578_I2C_NEW_EVENT_BLSZ                                  (0x00000400)
#define Tsi578_I2C_NEW_EVENT_BLERR                                 (0x00000800)
#define Tsi578_I2C_NEW_EVENT_BLTO                                  (0x00001000)
#define Tsi578_I2C_NEW_EVENT_MTD                                   (0x00004000)
#define Tsi578_I2C_NEW_EVENT_SSCLTO                                (0x00020000)
#define Tsi578_I2C_NEW_EVENT_SBTTO                                 (0x00040000)
#define Tsi578_I2C_NEW_EVENT_STRTO                                 (0x00080000)
#define Tsi578_I2C_NEW_EVENT_SCOL                                  (0x00100000)
#define Tsi578_I2C_NEW_EVENT_OMBR                                  (0x00400000)
#define Tsi578_I2C_NEW_EVENT_IMBW                                  (0x00800000)
#define Tsi578_I2C_NEW_EVENT_DCMDD                                 (0x01000000)
#define Tsi578_I2C_NEW_EVENT_DHIST                                 (0x02000000)
#define Tsi578_I2C_NEW_EVENT_DTIMER                                (0x04000000)
#define Tsi578_I2C_NEW_EVENT_SD                                    (0x10000000)
#define Tsi578_I2C_NEW_EVENT_SDR                                   (0x20000000)
#define Tsi578_I2C_NEW_EVENT_SDW                                   (0x40000000)

/* Tsi578_I2C_EVENT_ENB : Register Bits Masks Definitions */
#define Tsi578_I2C_EVENT_ENB_MARBTO                                (0x00000001)
#define Tsi578_I2C_EVENT_ENB_MSCLTO                                (0x00000002)
#define Tsi578_I2C_EVENT_ENB_MBTTO                                 (0x00000004)
#define Tsi578_I2C_EVENT_ENB_MTRTO                                 (0x00000008)
#define Tsi578_I2C_EVENT_ENB_MCOL                                  (0x00000010)
#define Tsi578_I2C_EVENT_ENB_MNACK                                 (0x00000020)
#define Tsi578_I2C_EVENT_ENB_BLOK                                  (0x00000100)
#define Tsi578_I2C_EVENT_ENB_BLNOD                                 (0x00000200)
#define Tsi578_I2C_EVENT_ENB_BLSZ                                  (0x00000400)
#define Tsi578_I2C_EVENT_ENB_BLERR                                 (0x00000800)
#define Tsi578_I2C_EVENT_ENB_BLTO                                  (0x00001000)
#define Tsi578_I2C_EVENT_ENB_MTD                                   (0x00004000)
#define Tsi578_I2C_EVENT_ENB_SSCLTO                                (0x00020000)
#define Tsi578_I2C_EVENT_ENB_SBTTO                                 (0x00040000)
#define Tsi578_I2C_EVENT_ENB_STRTO                                 (0x00080000)
#define Tsi578_I2C_EVENT_ENB_SCOL                                  (0x00100000)
#define Tsi578_I2C_EVENT_ENB_OMBR                                  (0x00400000)
#define Tsi578_I2C_EVENT_ENB_IMBW                                  (0x00800000)
#define Tsi578_I2C_EVENT_ENB_DCMDD                                 (0x01000000)
#define Tsi578_I2C_EVENT_ENB_DHIST                                 (0x02000000)
#define Tsi578_I2C_EVENT_ENB_DTIMER                                (0x04000000)
#define Tsi578_I2C_EVENT_ENB_SD                                    (0x10000000)
#define Tsi578_I2C_EVENT_ENB_SDR                                   (0x20000000)
#define Tsi578_I2C_EVENT_ENB_SDW                                   (0x40000000)

/* Tsi578_I2C_DIVIDER : Register Bits Masks Definitions */
#define Tsi578_I2C_DIVIDER_MSDIV                                   (0x00000fff)
#define Tsi578_I2C_DIVIDER_USDIV                                   (0x0fff0000)

/* Tsi578_I2C_FILTER_SCL_CFG : Register Bits Masks Definitions */
#define Tsi578_I2C_FILTER_SCL_CFG_END_TAP                          (0x00001f00)
#define Tsi578_I2C_FILTER_SCL_CFG_THRES0                           (0x001f0000)
#define Tsi578_I2C_FILTER_SCL_CFG_THRES1                           (0x1f000000)

/* Tsi578_I2C_FILTER_SDA_CFG : Register Bits Masks Definitions */
#define Tsi578_I2C_FILTER_SDA_CFG_END_TAP                          (0x00001f00)
#define Tsi578_I2C_FILTER_SDA_CFG_THRES0                           (0x001f0000)
#define Tsi578_I2C_FILTER_SDA_CFG_THRES1                           (0x1f000000)

/* Tsi578_I2C_START_SETUP_HOLD : Register Bits Masks Definitions */
#define Tsi578_I2C_START_SETUP_HOLD_START_HOLD                     (0x0000ffff)
#define Tsi578_I2C_START_SETUP_HOLD_START_SETUP                    (0xffff0000)

/* Tsi578_I2C_STOP_IDLE : Register Bits Masks Definitions */
#define Tsi578_I2C_STOP_IDLE_IDLE_DET                              (0x0000ffff)
#define Tsi578_I2C_STOP_IDLE_STOP_SETUP                            (0xffff0000)

/* Tsi578_I2C_SDA_SETUP_HOLD : Register Bits Masks Definitions */
#define Tsi578_I2C_SDA_SETUP_HOLD_SDA_HOLD                         (0x0000ffff)
#define Tsi578_I2C_SDA_SETUP_HOLD_SDA_SETUP                        (0xffff0000)

/* Tsi578_I2C_SCL_PERIOD : Register Bits Masks Definitions */
#define Tsi578_I2C_SCL_PERIOD_SCL_LOW                              (0x0000ffff)
#define Tsi578_I2C_SCL_PERIOD_SCL_HIGH                             (0xffff0000)

/* Tsi578_I2C_SCL_MIN_PERIOD : Register Bits Masks Definitions */
#define Tsi578_I2C_SCL_MIN_PERIOD_SCL_MINL                         (0x0000ffff)
#define Tsi578_I2C_SCL_MIN_PERIOD_SCL_MINH                         (0xffff0000)

/* Tsi578_I2C_SCL_ARB_TIMEOUT : Register Bits Masks Definitions */
#define Tsi578_I2C_SCL_ARB_TIMEOUT_ARB_TO                          (0x0000ffff)
#define Tsi578_I2C_SCL_ARB_TIMEOUT_SCL_TO                          (0xffff0000)

/* Tsi578_I2C_BYTE_TRAN_TIMEOUT : Register Bits Masks Definitions */
#define Tsi578_I2C_BYTE_TRAN_TIMEOUT_TRAN_TO                       (0x0000ffff)
#define Tsi578_I2C_BYTE_TRAN_TIMEOUT_BYTE_TO                       (0xffff0000)

/* Tsi578_I2C_BOOT_DIAG_TIMER : Register Bits Masks Definitions */
#define Tsi578_I2C_BOOT_DIAG_TIMER_COUNT                           (0x0000ffff)
#define Tsi578_I2C_BOOT_DIAG_TIMER_FREERUN                         (0x80000000)

/* Tsi578_I2C_DIAG_FILTER_SCL : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_FILTER_SCL_SAMPLES                         (0xffffffff)

/* Tsi578_I2C_DIAG_FILTER_SDA : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_FILTER_SDA_SAMPLES                         (0xffffffff)

/* Tsi578_I2C_BOOT_DIAG_PROGRESS : Register Bits Masks Definitions */
#define Tsi578_I2C_BOOT_DIAG_PROGRESS_PADDR                        (0x0000ffff)
#define Tsi578_I2C_BOOT_DIAG_PROGRESS_REGCNT                       (0xffff0000)

/* Tsi578_I2C_BOOT_DIAG_CFG : Register Bits Masks Definitions */
#define Tsi578_I2C_BOOT_DIAG_CFG_BOOT_ADDR                         (0x0000007f)
#define Tsi578_I2C_BOOT_DIAG_CFG_PINC                              (0x10000000)
#define Tsi578_I2C_BOOT_DIAG_CFG_PASIZE                            (0x20000000)
#define Tsi578_I2C_BOOT_DIAG_CFG_BDIS                              (0x40000000)
#define Tsi578_I2C_BOOT_DIAG_CFG_BOOTING                           (0x80000000)

/* Tsi578_I2C_DIAG_CNTRL : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_CNTRL_SDAB                                 (0x00000001)
#define Tsi578_I2C_DIAG_CNTRL_SDAOV                                (0x00000002)
#define Tsi578_I2C_DIAG_CNTRL_SCLB                                 (0x00000004)
#define Tsi578_I2C_DIAG_CNTRL_SCLOV                                (0x00000008)
#define Tsi578_I2C_DIAG_CNTRL_START_CMD                            (0x00000100)
#define Tsi578_I2C_DIAG_CNTRL_CMD                                  (0x00000e00)
#define Tsi578_I2C_DIAG_CNTRL_NO_STRETCH                           (0x00002000)
#define Tsi578_I2C_DIAG_CNTRL_NOIDLE                               (0x00004000)
#define Tsi578_I2C_DIAG_CNTRL_NOFAIL                               (0x00008000)
#define Tsi578_I2C_DIAG_CNTRL_WRITE_DATA                           (0x00ff0000)

/* Tsi578_I2C_DIAG_STAT : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_STAT_SCLIN                                 (0x00000001)
#define Tsi578_I2C_DIAG_STAT_SCLOUT                                (0x00000002)
#define Tsi578_I2C_DIAG_STAT_SDAIN                                 (0x00000004)
#define Tsi578_I2C_DIAG_STAT_SDAOUT                                (0x00000008)
#define Tsi578_I2C_DIAG_STAT_IDSTATE                               (0x000000e0)
#define Tsi578_I2C_DIAG_STAT_READ_DATA                             (0x0000ff00)
#define Tsi578_I2C_DIAG_STAT_ACK                                   (0x00010000)
#define Tsi578_I2C_DIAG_STAT_S_SCL                                 (0x00100000)
#define Tsi578_I2C_DIAG_STAT_S_SDA                                 (0x00200000)
#define Tsi578_I2C_DIAG_STAT_M_SCL                                 (0x00400000)
#define Tsi578_I2C_DIAG_STAT_M_SDA                                 (0x00800000)
#define Tsi578_I2C_DIAG_STAT_SDA_TIMER                             (0x07000000)
#define Tsi578_I2C_DIAG_STAT_SCLIT                                 (0x08000000)
#define Tsi578_I2C_DIAG_STAT_SCLHT                                 (0x10000000)
#define Tsi578_I2C_DIAG_STAT_SCLMHT                                (0x20000000)
#define Tsi578_I2C_DIAG_STAT_SLVSDA                                (0x40000000)

/* Tsi578_I2C_DIAG_HIST : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_HIST_COUNT                                 (0x0000000f)
#define Tsi578_I2C_DIAG_HIST_H1                                    (0x00000030)
#define Tsi578_I2C_DIAG_HIST_H2                                    (0x000000c0)
#define Tsi578_I2C_DIAG_HIST_H3                                    (0x00000300)
#define Tsi578_I2C_DIAG_HIST_H4                                    (0x00000c00)
#define Tsi578_I2C_DIAG_HIST_H5                                    (0x00003000)
#define Tsi578_I2C_DIAG_HIST_H6                                    (0x0000c000)
#define Tsi578_I2C_DIAG_HIST_H7                                    (0x00030000)
#define Tsi578_I2C_DIAG_HIST_H8                                    (0x000c0000)
#define Tsi578_I2C_DIAG_HIST_H9                                    (0x00300000)
#define Tsi578_I2C_DIAG_HIST_H10                                   (0x00c00000)
#define Tsi578_I2C_DIAG_HIST_H11                                   (0x03000000)
#define Tsi578_I2C_DIAG_HIST_H12                                   (0x0c000000)
#define Tsi578_I2C_DIAG_HIST_H13                                   (0x30000000)
#define Tsi578_I2C_DIAG_HIST_H14                                   (0xc0000000)

/* Tsi578_I2C_DIAG_MST_FSM : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_MST_FSM_BIT_FSM                            (0x0000001f)
#define Tsi578_I2C_DIAG_MST_FSM_BYTE_FSM                           (0x000001e0)
#define Tsi578_I2C_DIAG_MST_FSM_PTOB_FSM                           (0x00000e00)
#define Tsi578_I2C_DIAG_MST_FSM_PROTO_FSM                          (0x0001f000)
#define Tsi578_I2C_DIAG_MST_FSM_MST_FSM                            (0x000e0000)
#define Tsi578_I2C_DIAG_MST_FSM_BOOT_FSM                           (0x00f00000)

/* Tsi578_I2C_DIAG_SLV_FSM : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_SLV_FSM_BIT_FSM                            (0x0000000f)
#define Tsi578_I2C_DIAG_SLV_FSM_BYTE_FSM                           (0x000000f0)
#define Tsi578_I2C_DIAG_SLV_FSM_PTOB_FSM                           (0x00000700)
#define Tsi578_I2C_DIAG_SLV_FSM_PROTO_FSM                          (0x00007800)

/* Tsi578_I2C_DIAG_MST_SDA_SCL : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_MST_SDA_SCL_SCLTO_IDLE                     (0x0000ffff)
#define Tsi578_I2C_DIAG_MST_SDA_SCL_SDA_CNT                        (0xffff0000)

/* Tsi578_I2C_DIAG_MST_SCL_PER : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_MST_SCL_PER_SCL_MIN_PER                    (0x0000ffff)
#define Tsi578_I2C_DIAG_MST_SCL_PER_SCL_PER                        (0xffff0000)

/* Tsi578_I2C_DIAG_MST_ARB_BOOT : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_MST_ARB_BOOT_BOOT_CNT                      (0x0000ffff)
#define Tsi578_I2C_DIAG_MST_ARB_BOOT_ARB_TO_CNT                    (0xffff0000)

/* Tsi578_I2C_DIAG_MST_BYTE_TRAN : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_MST_BYTE_TRAN_TRAN_TO_CNT                  (0x0000ffff)
#define Tsi578_I2C_DIAG_MST_BYTE_TRAN_BYTE_TO_CNT                  (0xffff0000)

/* Tsi578_I2C_DIAG_SLV_SDA_SCL : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_SLV_SDA_SCL_SCLTO                          (0x0000ffff)
#define Tsi578_I2C_DIAG_SLV_SDA_SCL_SDA_CNT                        (0xffff0000)

/* Tsi578_I2C_DIAG_SLV_BYTE_TRAN : Register Bits Masks Definitions */
#define Tsi578_I2C_DIAG_SLV_BYTE_TRAN_TRAN_TO_CNT                  (0x0000ffff)
#define Tsi578_I2C_DIAG_SLV_BYTE_TRAN_BYTE_TO_CNT                  (0xffff0000)

/* Tsi578_SMACX_Tx_Ctl_STAT_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_CK0_EN                       (0x00010000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_EN                           (0x000e0000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_CLK_ALIGN                    (0x00100000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_CALC                         (0x00200000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_BOOST                        (0x03c00000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_ATTEN                        (0x1c000000)
#define Tsi578_SMACX_Tx_Ctl_STAT_0_TX_EDGERATE                     (0x60000000)

/* Tsi578_SMACX_Rx_STAT_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_Rx_STAT_0_HALF_RATE                           (0x00000001)
#define Tsi578_SMACX_Rx_STAT_0_RX_PLL_PWRON                        (0x00000002)
#define Tsi578_SMACX_Rx_STAT_0_RX_EN                               (0x00000004)
#define Tsi578_SMACX_Rx_STAT_0_RX_ALIGN_EN                         (0x00000008)
#define Tsi578_SMACX_Rx_STAT_0_RX_TERM_EN                          (0x00000010)
#define Tsi578_SMACX_Rx_STAT_0_RX_EQ_VAL                           (0x000000e0)
#define Tsi578_SMACX_Rx_STAT_0_RX_DPLL_MODE                        (0x00000700)
#define Tsi578_SMACX_Rx_STAT_0_DPLL_RESET                          (0x00000800)
#define Tsi578_SMACX_Rx_STAT_0_LOS_CTL                             (0x00003000)
#define Tsi578_SMACX_Rx_STAT_0_RX_VALID                            (0x00010000)
#define Tsi578_SMACX_Rx_STAT_0_RX_PLL_STATE                        (0x00020000)
#define Tsi578_SMACX_Rx_STAT_0_LOS                                 (0x00040000)
#define Tsi578_SMACX_Rx_STAT_0_TX_DONE                             (0x00080000)
#define Tsi578_SMACX_Rx_STAT_0_TX_RXPRES                           (0x00100000)

/* Tsi578_SMACX_Tx_Rx_Ctl_STAT_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_CK0_EN                    (0x00000001)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_EN                        (0x0000000e)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_CLK_ALIGN                 (0x00000010)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_CALC                      (0x00000020)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_BOOST                     (0x000003c0)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_ATTEN                     (0x00001c00)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_TX_EDGERATE                  (0x00006000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_OVRD_1                       (0x00008000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_HALF_RATE                    (0x00010000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_PLL_PWRON                 (0x00020000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_EN                        (0x00040000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_ALIGN_EN                  (0x00080000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_TERM_EN                   (0x00100000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_EQ_VAL                    (0x00e00000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_RX_DPLL_MODE                 (0x07000000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_DPLL_RESET                   (0x08000000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_LOS_CTL                      (0x30000000)
#define Tsi578_SMACX_Tx_Rx_Ctl_STAT_0_OVRD_0                       (0x40000000)

/* Tsi578_SMACX_Rx_Ctl_STAT_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_Rx_Ctl_STAT_0_RX_VALID                        (0x00000001)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_RX_PLL_STATE                    (0x00000002)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_LOS                             (0x00000004)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_TX_DONE                         (0x00000008)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_TX_RXPRES                       (0x00000010)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_OVRD                            (0x00000020)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_ZERO_TX_DATA                    (0x00010000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_ZERO_RX_DATA                    (0x00020000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_INVERT_TX                       (0x00040000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_INVERT_RX                       (0x00080000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_DISABLE_RX_CK                   (0x00100000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_DTB_SEL0                        (0x03e00000)
#define Tsi578_SMACX_Rx_Ctl_STAT_0_DTB_SEL1                        (0x7c000000)

/* Tsi578_SMACX_PG_CTL_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_PG_CTL_0_MODE                                 (0x00000007)
#define Tsi578_SMACX_PG_CTL_0_TRIGGER_ERR                          (0x00000008)
#define Tsi578_SMACX_PG_CTL_0_PAT0                                 (0x00003ff0)

/* Tsi578_SMACX_PM_CTL_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_PM_CTL_0_MODE                                 (0x00000007)
#define Tsi578_SMACX_PM_CTL_0_SYNC                                 (0x00000008)
#define Tsi578_SMACX_PM_CTL_0_COUNT                                (0x7fff0000)
#define Tsi578_SMACX_PM_CTL_0_OV14                                 (0x80000000)

/* Tsi578_SMACX_FP_VAL_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_FP_VAL_0_DTHR_1                               (0x00000001)
#define Tsi578_SMACX_FP_VAL_0_PVAL                                 (0x000007fe)
#define Tsi578_SMACX_FP_VAL_0_DTHR_0                               (0x00010000)
#define Tsi578_SMACX_FP_VAL_0_FVAL                                 (0x3ffe0000)

/* Tsi578_SMACX_SCP_RX_CTL_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_SCP_RX_CTL_0_MODE                             (0x00000003)
#define Tsi578_SMACX_SCP_RX_CTL_0_DELAY                            (0x000007fc)
#define Tsi578_SMACX_SCP_RX_CTL_0_BASE                             (0x00007800)

/* Tsi578_SMACX_Rx_Dbg_0 : Register Bits Masks Definitions */
#define Tsi578_SMACX_Rx_Dbg_0_DTB_SEL0                             (0x0000000f)
#define Tsi578_SMACX_Rx_Dbg_0_DTB_SEL1                             (0x000000f0)

/* Tsi578_SMACX_RESET : Register Bits Masks Definitions */
#define Tsi578_SMACX_RESET_SERDES_RESET_1                          (0x00000001)
#define Tsi578_SMACX_RESET_SERDES_RESET_0                          (0x00010000)

/* Tsi578_SMACX_CMP_A : Register Bits Masks Definitions */
#define Tsi578_SMACX_CMP_A_CRCMP_GT_LIMIT                          (0x0000ffff)

/* Tsi578_SMACX_CMP_B : Register Bits Masks Definitions */
#define Tsi578_SMACX_CMP_B_S0_LOW                                  (0x00000001)
#define Tsi578_SMACX_CMP_B_S0_HIGH                                 (0x00000002)
#define Tsi578_SMACX_CMP_B_S1_S0_LOW                               (0x00000004)
#define Tsi578_SMACX_CMP_B_S1_S0_HIGH                              (0x00000008)
#define Tsi578_SMACX_CMP_B_S0_OUTSIDE                              (0x00000010)
#define Tsi578_SMACX_CMP_B_S1_S0_OUTSIDE                           (0x00000020)
#define Tsi578_SMACX_CMP_B_HOLD_SCRATCH_0                          (0x00010000)
#define Tsi578_SMACX_CMP_B_HOLD_SCRATCH_1                          (0x00020000)

/* Tsi578_SMACX_SCP : Register Bits Masks Definitions */
#define Tsi578_SMACX_SCP_SCOPE_SAMPLES                             (0x0000ffff)
#define Tsi578_SMACX_SCP_SCOPE_COUNT                               (0xffff0000)

/* Tsi578_SMACX_DAC_RTUNE : Register Bits Masks Definitions */
#define Tsi578_SMACX_DAC_RTUNE_DAC_VAL                             (0x000003ff)
#define Tsi578_SMACX_DAC_RTUNE_OVRD_RTUNE_TX                       (0x00000400)
#define Tsi578_SMACX_DAC_RTUNE_OVRD_RTUNE_RX                       (0x00000800)
#define Tsi578_SMACX_DAC_RTUNE_DAC_MODE                            (0x00007000)
#define Tsi578_SMACX_DAC_RTUNE_MODE                                (0x00030000)
#define Tsi578_SMACX_DAC_RTUNE_FRC_PWRON                           (0x00040000)
#define Tsi578_SMACX_DAC_RTUNE_PWRON_LCL                           (0x00080000)
#define Tsi578_SMACX_DAC_RTUNE_SEL_ATBP                            (0x00100000)
#define Tsi578_SMACX_DAC_RTUNE_RSC_x4                              (0x00200000)
#define Tsi578_SMACX_DAC_RTUNE_DAC_CHOP                            (0x00400000)
#define Tsi578_SMACX_DAC_RTUNE_CMP_INVERT                          (0x00800000)
#define Tsi578_SMACX_DAC_RTUNE_RTUNE_DIS                           (0x01000000)
#define Tsi578_SMACX_DAC_RTUNE_RTUNE_TRIG                          (0x02000000)
#define Tsi578_SMACX_DAC_RTUNE_ADC_TRIG                            (0x04000000)

/* Tsi578_SMACX_ADC : Register Bits Masks Definitions */
#define Tsi578_SMACX_ADC_ADC_OUT                                   (0x000003ff)
#define Tsi578_SMACX_ADC_FRESH                                     (0x00000400)

/* Tsi578_SMACX_ID : Register Bits Masks Definitions */
#define Tsi578_SMACX_ID_SERDES_ID_HI                               (0x0000ffff)
#define Tsi578_SMACX_ID_SERDES_ID_LO                               (0xffff0000)

/* Tsi578_SMACX_CTL_VAL : Register Bits Masks Definitions */
#define Tsi578_SMACX_CTL_VAL_PROP_CTL                              (0x00000007)
#define Tsi578_SMACX_CTL_VAL_INT_CTL                               (0x00000038)
#define Tsi578_SMACX_CTL_VAL_NCY5                                  (0x000000c0)
#define Tsi578_SMACX_CTL_VAL_NCY                                   (0x00001f00)
#define Tsi578_SMACX_CTL_VAL_PRESCALE                              (0x00006000)
#define Tsi578_SMACX_CTL_VAL_USE_REFCLK_ALT                        (0x00010000)
#define Tsi578_SMACX_CTL_VAL_MPLL_CLK_OFF                          (0x00020000)
#define Tsi578_SMACX_CTL_VAL_MPLL_PWRON                            (0x00040000)
#define Tsi578_SMACX_CTL_VAL_MPLL_SS_EN                            (0x00080000)
#define Tsi578_SMACX_CTL_VAL_CKO_ALIVE_CON                         (0x00300000)
#define Tsi578_SMACX_CTL_VAL_CKO_WORD_CON                          (0x01c00000)
#define Tsi578_SMACX_CTL_VAL_RTUNE_DO_TUNE                         (0x04000000)
#define Tsi578_SMACX_CTL_VAL_WIDE_XFACE                            (0x08000000)
#define Tsi578_SMACX_CTL_VAL_VPH_IS_3P3                            (0x10000000)
#define Tsi578_SMACX_CTL_VAL_VPH_IS_1P2                            (0x20000000)
#define Tsi578_SMACX_CTL_VAL_FAST_TECH                             (0x40000000)

/* Tsi578_SMACX_LVL_CTL_VAL : Register Bits Masks Definitions */
#define Tsi578_SMACX_LVL_CTL_VAL_ACJT_LVL                          (0x0000001f)
#define Tsi578_SMACX_LVL_CTL_VAL_LOS_LVL                           (0x000003e0)
#define Tsi578_SMACX_LVL_CTL_VAL_TX_LVL                            (0x00007c00)
#define Tsi578_SMACX_LVL_CTL_VAL_CR_READ                           (0x00010000)
#define Tsi578_SMACX_LVL_CTL_VAL_CR_WRITE                          (0x00020000)
#define Tsi578_SMACX_LVL_CTL_VAL_CR_CAP_DATA                       (0x00040000)
#define Tsi578_SMACX_LVL_CTL_VAL_CR_CAP_ADDR                       (0x00080000)
#define Tsi578_SMACX_LVL_CTL_VAL_CR_ACK                            (0x00200000)
#define Tsi578_SMACX_LVL_CTL_VAL_POWER_GOOD                        (0x00400000)
#define Tsi578_SMACX_LVL_CTL_VAL_OP_DONE                           (0x00800000)

/* Tsi578_SMACX_CTL_OVR : Register Bits Masks Definitions */
#define Tsi578_SMACX_CTL_OVR_PROP_CTL                              (0x00000007)
#define Tsi578_SMACX_CTL_OVR_INT_CTL                               (0x00000038)
#define Tsi578_SMACX_CTL_OVR_NCY5                                  (0x000000c0)
#define Tsi578_SMACX_CTL_OVR_NCY                                   (0x00001f00)
#define Tsi578_SMACX_CTL_OVR_PRESCALE                              (0x00006000)
#define Tsi578_SMACX_CTL_OVR_OVRD                                  (0x00008000)
#define Tsi578_SMACX_CTL_OVR_USE_REFCLK_ALT                        (0x00010000)
#define Tsi578_SMACX_CTL_OVR_MPLL_CLK_OFF                          (0x00020000)
#define Tsi578_SMACX_CTL_OVR_MPLL_PWRON                            (0x00040000)
#define Tsi578_SMACX_CTL_OVR_MPLL_SS_EN                            (0x00080000)
#define Tsi578_SMACX_CTL_OVR_CK0_ALIVE_CON                         (0x00300000)
#define Tsi578_SMACX_CTL_OVR_CK0_WORD_CON                          (0x01c00000)
#define Tsi578_SMACX_CTL_OVR_OVRD_CLK                              (0x02000000)
#define Tsi578_SMACX_CTL_OVR_RTUNE_DO_TUNE                         (0x04000000)
#define Tsi578_SMACX_CTL_OVR_WIDE_XFACE                            (0x08000000)
#define Tsi578_SMACX_CTL_OVR_VPH_IS_3P3                            (0x10000000)
#define Tsi578_SMACX_CTL_OVR_VPH_IS_1P2                            (0x20000000)
#define Tsi578_SMACX_CTL_OVR_FAST_TECH                             (0x40000000)
#define Tsi578_SMACX_CTL_OVR_OVRD_STATIC                           (0x80000000)

/* Tsi578_SMACX_LVL_OVR : Register Bits Masks Definitions */
#define Tsi578_SMACX_LVL_OVR_ACJT_LVL                              (0x0000001f)
#define Tsi578_SMACX_LVL_OVR_LOS_LVL                               (0x000003e0)
#define Tsi578_SMACX_LVL_OVR_TX_LVL                                (0x00007c00)
#define Tsi578_SMACX_LVL_OVR_OVERIDE                               (0x00008000)
#define Tsi578_SMACX_LVL_OVR_CR_READ                               (0x00010000)
#define Tsi578_SMACX_LVL_OVR_CR_WRITE                              (0x00020000)
#define Tsi578_SMACX_LVL_OVR_CR_CAP_DATA                           (0x00040000)
#define Tsi578_SMACX_LVL_OVR_CR_CAP_ADDR                           (0x00080000)
#define Tsi578_SMACX_LVL_OVR_OVRD_IN                               (0x00100000)
#define Tsi578_SMACX_LVL_OVR_CR_ACK                                (0x00200000)
#define Tsi578_SMACX_LVL_OVR_POWER_GOOD                            (0x00400000)
#define Tsi578_SMACX_LVL_OVR_OP_DONE                               (0x00800000)
#define Tsi578_SMACX_LVL_OVR_OVRD_OUT                              (0x01000000)

/* Tsi578_SMACX_MPLL_CTL : Register Bits Masks Definitions */
#define Tsi578_SMACX_MPLL_CTL_CLKDRV_ANA                           (0x00000001)
#define Tsi578_SMACX_MPLL_CTL_CLKDRV_DIG                           (0x00000002)
#define Tsi578_SMACX_MPLL_CTL_OVRD_CLKDRV                          (0x00000004)
#define Tsi578_SMACX_MPLL_CTL_DIS_PARA_CREG                        (0x00000008)
#define Tsi578_SMACX_MPLL_CTL_REFCLK_DELAY                         (0x00000010)
#define Tsi578_SMACX_MPLL_CTL_DTB_SEL0                             (0x000003e0)
#define Tsi578_SMACX_MPLL_CTL_DTB_SEL1                             (0x00007c00)
#define Tsi578_SMACX_MPLL_CTL_ATB_SENSE                            (0x00010000)
#define Tsi578_SMACX_MPLL_CTL_MEAS_GD                              (0x00020000)
#define Tsi578_SMACX_MPLL_CTL_MEAS_IV                              (0x1ffc0000)
#define Tsi578_SMACX_MPLL_CTL_RESET_VAL                            (0x20000000)
#define Tsi578_SMACX_MPLL_CTL_GEARSGIFT_VAL                        (0x40000000)
#define Tsi578_SMACX_MPLL_CTL_OVRD_CTL                             (0x80000000)



#endif /* _TS_Tsi578_H_ */

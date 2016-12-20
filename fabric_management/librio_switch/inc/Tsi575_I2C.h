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
#ifndef _TS_Tsi575_I2C_H_ 
#define _TS_Tsi575_I2C_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#define Tsi575_I2C_NUM_REGS_TO_TEST                                 0x00000034

                                                                               

#ifndef I2C_BASE
#define I2C_BASE                                                   (0x0001D000)
#endif


/* ************************************************ */
/* Tsi575 : Register address offset definitions     */
/* ************************************************ */
#define Tsi575_I2C_DEVID                                (I2C_BASE + 0x00000100)
#define Tsi575_I2C_RESET                                (I2C_BASE + 0x00000104)
#define Tsi575_I2C_MST_CFG                              (I2C_BASE + 0x00000108)
#define Tsi575_I2C_MST_CNTRL                            (I2C_BASE + 0x0000010c)
#define Tsi575_I2C_MST_RDATA                            (I2C_BASE + 0x00000110)
#define Tsi575_I2C_MST_TDATA                            (I2C_BASE + 0x00000114)
#define Tsi575_I2C_ACC_STAT                             (I2C_BASE + 0x00000118)
#define Tsi575_I2C_INT_STAT                             (I2C_BASE + 0x0000011c)
#define Tsi575_I2C_INT_ENABLE                           (I2C_BASE + 0x00000120)
#define Tsi575_I2C_INT_SET                              (I2C_BASE + 0x00000124)
#define Tsi575_I2C_SLV_CFG                              (I2C_BASE + 0x0000012c)
#define Tsi575_I2C_BOOT_CNTRL                           (I2C_BASE + 0x00000140)
#define Tsi575_EXI2C_REG_WADDR                          (I2C_BASE + 0x00000200)
#define Tsi575_EXI2C_REG_WDATA                          (I2C_BASE + 0x00000204)
#define Tsi575_EXI2C_REG_RADDR                          (I2C_BASE + 0x00000210)
#define Tsi575_EXI2C_REG_RDATA                          (I2C_BASE + 0x00000214)
#define Tsi575_EXI2C_ACC_STAT                           (I2C_BASE + 0x00000220)
#define Tsi575_EXI2C_ACC_CNTRL                          (I2C_BASE + 0x00000224)
#define Tsi575_EXI2C_STAT                               (I2C_BASE + 0x00000280)
#define Tsi575_EXI2C_STAT_ENABLE                        (I2C_BASE + 0x00000284)
#define Tsi575_EXI2C_MBOX_OUT                           (I2C_BASE + 0x00000290)
#define Tsi575_EXI2C_MBOX_IN                            (I2C_BASE + 0x00000294)
#define Tsi575_I2C_EVENT                                (I2C_BASE + 0x00000300) 
#define Tsi575_I2C_SNAP_EVENT                           (I2C_BASE + 0x00000304) 
#define Tsi575_I2C_NEW_EVENT                            (I2C_BASE + 0x00000308)
#define Tsi575_I2C_EVENT_ENB                            (I2C_BASE + 0x0000030c)
#define Tsi575_I2C_DIVIDER                              (I2C_BASE + 0x00000320)
#define Tsi575_I2C_FILTER_SCL_CFG                       (I2C_BASE + 0x00000328)
#define Tsi575_I2C_FILTER_SDA_CFG                       (I2C_BASE + 0x0000032c)
#define Tsi575_I2C_START_SETUP_HOLD                     (I2C_BASE + 0x00000340)
#define Tsi575_I2C_STOP_IDLE                            (I2C_BASE + 0x00000344)
#define Tsi575_I2C_SDA_SETUP_HOLD                       (I2C_BASE + 0x00000348)
#define Tsi575_I2C_SCL_PERIOD                           (I2C_BASE + 0x0000034c)
#define Tsi575_I2C_SCL_MIN_PERIOD                       (I2C_BASE + 0x00000350)
#define Tsi575_I2C_SCL_ARB_TIMEOUT                      (I2C_BASE + 0x00000354)
#define Tsi575_I2C_BYTE_TRAN_TIMEOUT                    (I2C_BASE + 0x00000358)
#define Tsi575_I2C_BOOT_DIAG_TIMER                      (I2C_BASE + 0x0000035c)
#define Tsi575_I2C_DIAG_FILTER_SCL                      (I2C_BASE + 0x000003a0)
#define Tsi575_I2C_DIAG_FILTER_SDA                      (I2C_BASE + 0x000003a4)
#define Tsi575_I2C_BOOT_DIAG_PROGRESS                   (I2C_BASE + 0x000003b8)
#define Tsi575_I2C_BOOT_DIAG_CFG                        (I2C_BASE + 0x000003bc)
#define Tsi575_I2C_DIAG_CNTRL                           (I2C_BASE + 0x000003c0)
#define Tsi575_I2C_DIAG_STAT                            (I2C_BASE + 0x000003d0)
#define Tsi575_I2C_DIAG_HIST                            (I2C_BASE + 0x000003d4)
#define Tsi575_I2C_DIAG_MST_FSM                         (I2C_BASE + 0x000003d8)
#define Tsi575_I2C_DIAG_SLV_FSM                         (I2C_BASE + 0x000003dc)
#define Tsi575_I2C_DIAG_MST_SDA_SCL                     (I2C_BASE + 0x000003e0)
#define Tsi575_I2C_DIAG_MST_SCL_PER                     (I2C_BASE + 0x000003e4)
#define Tsi575_I2C_DIAG_MST_ARB_BOOT                    (I2C_BASE + 0x000003e8)
#define Tsi575_I2C_DIAG_MST_BYTE_TRAN                   (I2C_BASE + 0x000003ec)
#define Tsi575_I2C_DIAG_SLV_SDA_SCL                     (I2C_BASE + 0x000003f0)
#define Tsi575_I2C_DIAG_SLV_BYTE_TRAN                   (I2C_BASE + 0x000003f4)


/* ************************************************ */
/* Tsi575 : Register Bit Masks and Reset Values    */
/*           definitions for every register         */
/* ************************************************ */



/* Tsi575_I2C_DEVID : Register Bits Masks Definitions */
#define Tsi575_I2C_DEVID_REV                                       (0x0000000f)

/* Tsi575_I2C_RESET : Register Bits Masks Definitions */
#define Tsi575_I2C_RESET_SRESET                                    (0x80000000)

/* Tsi575_I2C_MST_CFG : Register Bits Masks Definitions */
#define Tsi575_I2C_MST_CFG_DEV_ADDR                                (0x0000007f)
#define Tsi575_I2C_MST_CFG_PA_SIZE                                 (0x00030000)
#define Tsi575_I2C_MST_CFG_DORDER                                  (0x00800000)

/* Tsi575_I2C_MST_CNTRL : Register Bits Masks Definitions */
#define Tsi575_I2C_MST_CNTRL_PADDR                                 (0x0000ffff)
#define Tsi575_I2C_MST_CNTRL_SIZE                                  (0x07000000)
#define Tsi575_I2C_MST_CNTRL_WRITE                                 (0x40000000)
#define Tsi575_I2C_MST_CNTRL_START                                 (0x80000000)

/* Tsi575_I2C_MST_RDATA : Register Bits Masks Definitions */
#define Tsi575_I2C_MST_RDATA_RBYTE0                                (0x000000ff)
#define Tsi575_I2C_MST_RDATA_RBYTE1                                (0x0000ff00)
#define Tsi575_I2C_MST_RDATA_RBYTE2                                (0x00ff0000)
#define Tsi575_I2C_MST_RDATA_RBYTE3                                (0xff000000)

/* Tsi575_I2C_MST_TDATA : Register Bits Masks Definitions */
#define Tsi575_I2C_MST_TDATA_TBYTE0                                (0x000000ff)
#define Tsi575_I2C_MST_TDATA_TBYTE1                                (0x0000ff00)
#define Tsi575_I2C_MST_TDATA_TBYTE2                                (0x00ff0000)
#define Tsi575_I2C_MST_TDATA_TBYTE3                                (0xff000000)

/* Tsi575_I2C_ACC_STAT : Register Bits Masks Definitions */
#define Tsi575_I2C_ACC_STAT_MST_NBYTES                             (0x0000000f)
#define Tsi575_I2C_ACC_STAT_MST_AN                                 (0x00000100)
#define Tsi575_I2C_ACC_STAT_MST_PHASE                              (0x00000e00)
#define Tsi575_I2C_ACC_STAT_MST_ACTIVE                             (0x00008000)
#define Tsi575_I2C_ACC_STAT_SLV_PA                                 (0x00ff0000)
#define Tsi575_I2C_ACC_STAT_SLV_AN                                 (0x01000000)
#define Tsi575_I2C_ACC_STAT_SLV_PHASE                              (0x06000000)
#define Tsi575_I2C_ACC_STAT_SLV_WAIT                               (0x08000000)
#define Tsi575_I2C_ACC_STAT_BUS_ACTIVE                             (0x40000000)
#define Tsi575_I2C_ACC_STAT_SLV_ACTIVE                             (0x80000000)

/* Tsi575_I2C_INT_STAT : Register Bits Masks Definitions */
#define Tsi575_I2C_INT_STAT_MA_OK                                  (0x00000001)
#define Tsi575_I2C_INT_STAT_MA_ATMO                                (0x00000002)
#define Tsi575_I2C_INT_STAT_MA_NACK                                (0x00000004)
#define Tsi575_I2C_INT_STAT_MA_TMO                                 (0x00000008)
#define Tsi575_I2C_INT_STAT_MA_COL                                 (0x00000010)
#define Tsi575_I2C_INT_STAT_MA_DIAG                                (0x00000080)
#define Tsi575_I2C_INT_STAT_SA_OK                                  (0x00000100)
#define Tsi575_I2C_INT_STAT_SA_READ                                (0x00000200)
#define Tsi575_I2C_INT_STAT_SA_WRITE                               (0x00000400)
#define Tsi575_I2C_INT_STAT_SA_FAIL                                (0x00000800)
#define Tsi575_I2C_INT_STAT_BL_OK                                  (0x00010000)
#define Tsi575_I2C_INT_STAT_BL_FAIL                                (0x00020000)
#define Tsi575_I2C_INT_STAT_IMB_FULL                               (0x01000000)
#define Tsi575_I2C_INT_STAT_OMB_EMPTY                              (0x02000000)

/* Tsi575_I2C_INT_ENABLE : Register Bits Masks Definitions */
#define Tsi575_I2C_INT_ENABLE_MA_OK                                (0x00000001)
#define Tsi575_I2C_INT_ENABLE_MA_ATMO                              (0x00000002)
#define Tsi575_I2C_INT_ENABLE_MA_NACK                              (0x00000004)
#define Tsi575_I2C_INT_ENABLE_MA_TMO                               (0x00000008)
#define Tsi575_I2C_INT_ENABLE_MA_COL                               (0x00000010)
#define Tsi575_I2C_INT_ENABLE_MA_DIAG                              (0x00000080)
#define Tsi575_I2C_INT_ENABLE_SA_OK                                (0x00000100)
#define Tsi575_I2C_INT_ENABLE_SA_READ                              (0x00000200)
#define Tsi575_I2C_INT_ENABLE_SA_WRITE                             (0x00000400)
#define Tsi575_I2C_INT_ENABLE_SA_FAIL                              (0x00000800)
#define Tsi575_I2C_INT_ENABLE_BL_OK                                (0x00010000)
#define Tsi575_I2C_INT_ENABLE_BL_FAIL                              (0x00020000)
#define Tsi575_I2C_INT_ENABLE_IMB_FULL                             (0x01000000)
#define Tsi575_I2C_INT_ENABLE_OMB_EMPTY                            (0x02000000)

/* Tsi575_I2C_INT_SET : Register Bits Masks Definitions */
#define Tsi575_I2C_INT_SET_MA_OK                                   (0x00000001)
#define Tsi575_I2C_INT_SET_MA_ATMO                                 (0x00000002)
#define Tsi575_I2C_INT_SET_MA_NACK                                 (0x00000004)
#define Tsi575_I2C_INT_SET_MA_TMO                                  (0x00000008)
#define Tsi575_I2C_INT_SET_MA_COL                                  (0x00000010)
#define Tsi575_I2C_INT_SET_MA_DIAG                                 (0x00000080)
#define Tsi575_I2C_INT_SET_SA_OK                                   (0x00000100)
#define Tsi575_I2C_INT_SET_SA_READ                                 (0x00000200)
#define Tsi575_I2C_INT_SET_SA_WRITE                                (0x00000400)
#define Tsi575_I2C_INT_SET_SA_FAIL                                 (0x00000800)
#define Tsi575_I2C_INT_SET_BL_OK                                   (0x00010000)
#define Tsi575_I2C_INT_SET_BL_FAIL                                 (0x00020000)
#define Tsi575_I2C_INT_SET_IMB_FULL                                (0x01000000)
#define Tsi575_I2C_INT_SET_OMB_EMPTY                               (0x02000000)

/* Tsi575_I2C_SLV_CFG : Register Bits Masks Definitions */
#define Tsi575_I2C_SLV_CFG_SLV_ADDR                                (0x0000007f)
#define Tsi575_I2C_SLV_CFG_SLV_UNLK                                (0x01000000)
#define Tsi575_I2C_SLV_CFG_SLV_EN                                  (0x10000000)
#define Tsi575_I2C_SLV_CFG_ALRT_EN                                 (0x20000000)
#define Tsi575_I2C_SLV_CFG_WR_EN                                   (0x40000000)
#define Tsi575_I2C_SLV_CFG_RD_EN                                   (0x80000000)

/* Tsi575_I2C_BOOT_CNTRL : Register Bits Masks Definitions */
#define Tsi575_I2C_BOOT_CNTRL_PADDR                                (0x00001fff)
#define Tsi575_I2C_BOOT_CNTRL_PAGE_MODE                            (0x0000e000)
#define Tsi575_I2C_BOOT_CNTRL_BOOT_ADDR                            (0x007f0000)
#define Tsi575_I2C_BOOT_CNTRL_BUNLK                                (0x10000000)
#define Tsi575_I2C_BOOT_CNTRL_BINC                                 (0x20000000)
#define Tsi575_I2C_BOOT_CNTRL_PSIZE                                (0x40000000)
#define Tsi575_I2C_BOOT_CNTRL_CHAIN                                (0x80000000)

/* Tsi575_EXI2C_REG_WADDR : Register Bits Masks Definitions */
#define Tsi575_EXI2C_REG_WADDR_ADDR                                (0xfffffffc)

/* Tsi575_EXI2C_REG_WDATA : Register Bits Masks Definitions */
#define Tsi575_EXI2C_REG_WDATA_WDATA                               (0xffffffff)

/* Tsi575_EXI2C_REG_RADDR : Register Bits Masks Definitions */
#define Tsi575_EXI2C_REG_RADDR_ADDR                                (0xfffffffc)

/* Tsi575_EXI2C_REG_RDATA : Register Bits Masks Definitions */
#define Tsi575_EXI2C_REG_RDATA_RDATA                               (0xffffffff)

/* Tsi575_EXI2C_ACC_STAT : Register Bits Masks Definitions */
#define Tsi575_EXI2C_ACC_STAT_ALERT_FLAG                           (0x00000001)
#define Tsi575_EXI2C_ACC_STAT_IMB_FLAG                             (0x00000004)
#define Tsi575_EXI2C_ACC_STAT_OMB_FLAG                             (0x00000008)
#define Tsi575_EXI2C_ACC_STAT_ACC_OK                               (0x00000080)

/* Tsi575_EXI2C_ACC_CNTRL : Register Bits Masks Definitions */
#define Tsi575_EXI2C_ACC_CNTRL_WINC                                (0x00000004)
#define Tsi575_EXI2C_ACC_CNTRL_RINC                                (0x00000008)
#define Tsi575_EXI2C_ACC_CNTRL_WSIZE                               (0x00000030)
#define Tsi575_EXI2C_ACC_CNTRL_RSIZE                               (0x000000c0)

/* Tsi575_EXI2C_STAT : Register Bits Masks Definitions */
#define Tsi575_EXI2C_STAT_PORT0                                    (0x00000001)
#define Tsi575_EXI2C_STAT_PORT1                                    (0x00000002)
#define Tsi575_EXI2C_STAT_PORT2                                    (0x00000004)
#define Tsi575_EXI2C_STAT_PORT3                                    (0x00000008)
#define Tsi575_EXI2C_STAT_PORT4                                    (0x00000010)
#define Tsi575_EXI2C_STAT_PORT5                                    (0x00000020)
#define Tsi575_EXI2C_STAT_PORT6                                    (0x00000040)
#define Tsi575_EXI2C_STAT_PORT7                                    (0x00000080)
#define Tsi575_EXI2C_STAT_PORT8                                    (0x00000100)
#define Tsi575_EXI2C_STAT_CLK_GEN                                  (0x00002000)
#define Tsi575_EXI2C_STAT_GPIO0                                    (0x00004000)
#define Tsi575_EXI2C_STAT_GPIO1                                    (0x00008000)
#define Tsi575_EXI2C_STAT_MCE                                      (0x00010000)
#define Tsi575_EXI2C_STAT_MC_LAT                                   (0x00020000)
#define Tsi575_EXI2C_STAT_LOGICAL                                  (0x00040000)
#define Tsi575_EXI2C_STAT_SREP_INT                                 (0x00080000)
#define Tsi575_EXI2C_STAT_BISF_INT                                 (0x00100000)
#define Tsi575_EXI2C_STAT_PCI_INT                                  (0x00200000)
#define Tsi575_EXI2C_STAT_MCS                                      (0x00400000)
#define Tsi575_EXI2C_STAT_RCS                                      (0x00800000)
#define Tsi575_EXI2C_STAT_TEA                                      (0x01000000)
#define Tsi575_EXI2C_STAT_I2C                                      (0x02000000)
#define Tsi575_EXI2C_STAT_IMBR                                     (0x04000000)
#define Tsi575_EXI2C_STAT_OMBW                                     (0x08000000)
#define Tsi575_EXI2C_STAT_SW_STAT0                                 (0x10000000)
#define Tsi575_EXI2C_STAT_SW_STAT1                                 (0x20000000)
#define Tsi575_EXI2C_STAT_SW_STAT2                                 (0x40000000)
#define Tsi575_EXI2C_STAT_RESET                                    (0x80000000)

/* Tsi575_EXI2C_STAT_ENABLE : Register Bits Masks Definitions */
#define Tsi575_EXI2C_STAT_ENABLE_PORT0                             (0x00000001)
#define Tsi575_EXI2C_STAT_ENABLE_PORT1                             (0x00000002)
#define Tsi575_EXI2C_STAT_ENABLE_PORT2                             (0x00000004)
#define Tsi575_EXI2C_STAT_ENABLE_PORT3                             (0x00000008)
#define Tsi575_EXI2C_STAT_ENABLE_PORT4                             (0x00000010)
#define Tsi575_EXI2C_STAT_ENABLE_PORT5                             (0x00000020)
#define Tsi575_EXI2C_STAT_ENABLE_PORT6                             (0x00000040)
#define Tsi575_EXI2C_STAT_ENABLE_PORT7                             (0x00000080)
#define Tsi575_EXI2C_STAT_ENABLE_PORT8                             (0x00000100)
#define Tsi575_EXI2C_STAT_ENABLE_CLK_GEN                           (0x00002000)
#define Tsi575_EXI2C_STAT_ENABLE_GPIO0                             (0x00004000)
#define Tsi575_EXI2C_STAT_ENABLE_GPIO1                             (0x00008000)
#define Tsi575_EXI2C_STAT_ENABLE_MCE                               (0x00010000)
#define Tsi575_EXI2C_STAT_ENABLE_MC_LAT                            (0x00020000)
#define Tsi575_EXI2C_STAT_ENABLE_LOGICAL                           (0x00040000)
#define Tsi575_EXI2C_STAT_ENABLE_SREP_INT                          (0x00080000)
#define Tsi575_EXI2C_STAT_ENABLE_BISF_INT                          (0x00100000)
#define Tsi575_EXI2C_STAT_ENABLE_PCI_INT                           (0x00200000)
#define Tsi575_EXI2C_STAT_ENABLE_MCS                               (0x00400000)
#define Tsi575_EXI2C_STAT_ENABLE_RCS                               (0x00800000)
#define Tsi575_EXI2C_STAT_ENABLE_TEA                               (0x01000000)
#define Tsi575_EXI2C_STAT_ENABLE_I2C                               (0x02000000)
#define Tsi575_EXI2C_STAT_ENABLE_IMBR                              (0x04000000)
#define Tsi575_EXI2C_STAT_ENABLE_OMBW                              (0x08000000)
#define Tsi575_EXI2C_STAT_ENABLE_SW_STAT0                          (0x10000000)
#define Tsi575_EXI2C_STAT_ENABLE_SW_STAT1                          (0x20000000)
#define Tsi575_EXI2C_STAT_ENABLE_SW_STAT2                          (0x40000000)
#define Tsi575_EXI2C_STAT_ENABLE_RESET                             (0x80000000)

/* Tsi575_EXI2C_MBOX_OUT : Register Bits Masks Definitions */
#define Tsi575_EXI2C_MBOX_OUT_DATA                                 (0xffffffff)

/* Tsi575_EXI2C_MBOX_IN : Register Bits Masks Definitions */
#define Tsi575_EXI2C_MBOX_IN_DATA                                  (0xffffffff)

/* Tsi575_I2C_X : Register Bits Masks Definitions */
#define Tsi575_I2C_X_MARBTO                                        (0x00000001)
#define Tsi575_I2C_X_MSCLTO                                        (0x00000002)
#define Tsi575_I2C_X_MBTTO                                         (0x00000004)
#define Tsi575_I2C_X_MTRTO                                         (0x00000008)
#define Tsi575_I2C_X_MCOL                                          (0x00000010)
#define Tsi575_I2C_X_MNACK                                         (0x00000020)
#define Tsi575_I2C_X_BLOK                                          (0x00000100)
#define Tsi575_I2C_X_BLNOD                                         (0x00000200)
#define Tsi575_I2C_X_BLSZ                                          (0x00000400)
#define Tsi575_I2C_X_BLERR                                         (0x00000800)
#define Tsi575_I2C_X_BLTO                                          (0x00001000)
#define Tsi575_I2C_X_MTD                                           (0x00004000)
#define Tsi575_I2C_X_SSCLTO                                        (0x00020000)
#define Tsi575_I2C_X_SBTTO                                         (0x00040000)
#define Tsi575_I2C_X_STRTO                                         (0x00080000)
#define Tsi575_I2C_X_SCOL                                          (0x00100000)
#define Tsi575_I2C_X_OMBR                                          (0x00400000)
#define Tsi575_I2C_X_IMBW                                          (0x00800000)
#define Tsi575_I2C_X_DCMDD                                         (0x01000000)
#define Tsi575_I2C_X_DHIST                                         (0x02000000)
#define Tsi575_I2C_X_DTIMER                                        (0x04000000)
#define Tsi575_I2C_X_SD                                            (0x10000000)
#define Tsi575_I2C_X_SDR                                           (0x20000000)
#define Tsi575_I2C_X_SDW                                           (0x40000000)

/* Tsi575_I2C_NEW_EVENT : Register Bits Masks Definitions */
#define Tsi575_I2C_NEW_EVENT_MARBTO                                (0x00000001)
#define Tsi575_I2C_NEW_EVENT_MSCLTO                                (0x00000002)
#define Tsi575_I2C_NEW_EVENT_MBTTO                                 (0x00000004)
#define Tsi575_I2C_NEW_EVENT_MTRTO                                 (0x00000008)
#define Tsi575_I2C_NEW_EVENT_MCOL                                  (0x00000010)
#define Tsi575_I2C_NEW_EVENT_MNACK                                 (0x00000020)
#define Tsi575_I2C_NEW_EVENT_BLOK                                  (0x00000100)
#define Tsi575_I2C_NEW_EVENT_BLNOD                                 (0x00000200)
#define Tsi575_I2C_NEW_EVENT_BLSZ                                  (0x00000400)
#define Tsi575_I2C_NEW_EVENT_BLERR                                 (0x00000800)
#define Tsi575_I2C_NEW_EVENT_BLTO                                  (0x00001000)
#define Tsi575_I2C_NEW_EVENT_MTD                                   (0x00004000)
#define Tsi575_I2C_NEW_EVENT_SSCLTO                                (0x00020000)
#define Tsi575_I2C_NEW_EVENT_SBTTO                                 (0x00040000)
#define Tsi575_I2C_NEW_EVENT_STRTO                                 (0x00080000)
#define Tsi575_I2C_NEW_EVENT_SCOL                                  (0x00100000)
#define Tsi575_I2C_NEW_EVENT_OMBR                                  (0x00400000)
#define Tsi575_I2C_NEW_EVENT_IMBW                                  (0x00800000)
#define Tsi575_I2C_NEW_EVENT_DCMDD                                 (0x01000000)
#define Tsi575_I2C_NEW_EVENT_DHIST                                 (0x02000000)
#define Tsi575_I2C_NEW_EVENT_DTIMER                                (0x04000000)
#define Tsi575_I2C_NEW_EVENT_SD                                    (0x10000000)
#define Tsi575_I2C_NEW_EVENT_SDR                                   (0x20000000)
#define Tsi575_I2C_NEW_EVENT_SDW                                   (0x40000000)

/* Tsi575_I2C_EVENT_ENB : Register Bits Masks Definitions */
#define Tsi575_I2C_EVENT_ENB_MARBTO                                (0x00000001)
#define Tsi575_I2C_EVENT_ENB_MSCLTO                                (0x00000002)
#define Tsi575_I2C_EVENT_ENB_MBTTO                                 (0x00000004)
#define Tsi575_I2C_EVENT_ENB_MTRTO                                 (0x00000008)
#define Tsi575_I2C_EVENT_ENB_MCOL                                  (0x00000010)
#define Tsi575_I2C_EVENT_ENB_MNACK                                 (0x00000020)
#define Tsi575_I2C_EVENT_ENB_BLOK                                  (0x00000100)
#define Tsi575_I2C_EVENT_ENB_BLNOD                                 (0x00000200)
#define Tsi575_I2C_EVENT_ENB_BLSZ                                  (0x00000400)
#define Tsi575_I2C_EVENT_ENB_BLERR                                 (0x00000800)
#define Tsi575_I2C_EVENT_ENB_BLTO                                  (0x00001000)
#define Tsi575_I2C_EVENT_ENB_MTD                                   (0x00004000)
#define Tsi575_I2C_EVENT_ENB_SSCLTO                                (0x00020000)
#define Tsi575_I2C_EVENT_ENB_SBTTO                                 (0x00040000)
#define Tsi575_I2C_EVENT_ENB_STRTO                                 (0x00080000)
#define Tsi575_I2C_EVENT_ENB_SCOL                                  (0x00100000)
#define Tsi575_I2C_EVENT_ENB_OMBR                                  (0x00400000)
#define Tsi575_I2C_EVENT_ENB_IMBW                                  (0x00800000)
#define Tsi575_I2C_EVENT_ENB_DCMDD                                 (0x01000000)
#define Tsi575_I2C_EVENT_ENB_DHIST                                 (0x02000000)
#define Tsi575_I2C_EVENT_ENB_DTIMER                                (0x04000000)
#define Tsi575_I2C_EVENT_ENB_SD                                    (0x10000000)
#define Tsi575_I2C_EVENT_ENB_SDR                                   (0x20000000)
#define Tsi575_I2C_EVENT_ENB_SDW                                   (0x40000000)

/* Tsi575_I2C_DIVIDER : Register Bits Masks Definitions */
#define Tsi575_I2C_DIVIDER_MSDIV                                   (0x00000fff)
#define Tsi575_I2C_DIVIDER_USDIV                                   (0x0fff0000)

/* Tsi575_I2C_FILTER_SCL_CFG : Register Bits Masks Definitions */
#define Tsi575_I2C_FILTER_SCL_CFG_END_TAP                          (0x00001f00)
#define Tsi575_I2C_FILTER_SCL_CFG_THRES0                           (0x001f0000)
#define Tsi575_I2C_FILTER_SCL_CFG_THRES1                           (0x1f000000)

/* Tsi575_I2C_FILTER_SDA_CFG : Register Bits Masks Definitions */
#define Tsi575_I2C_FILTER_SDA_CFG_END_TAP                          (0x00001f00)
#define Tsi575_I2C_FILTER_SDA_CFG_THRES0                           (0x001f0000)
#define Tsi575_I2C_FILTER_SDA_CFG_THRES1                           (0x1f000000)

/* Tsi575_I2C_START_SETUP_HOLD : Register Bits Masks Definitions */
#define Tsi575_I2C_START_SETUP_HOLD_START_HOLD                     (0x0000ffff)
#define Tsi575_I2C_START_SETUP_HOLD_START_SETUP                    (0xffff0000)

/* Tsi575_I2C_STOP_IDLE : Register Bits Masks Definitions */
#define Tsi575_I2C_STOP_IDLE_IDLE_DET                              (0x0000ffff)
#define Tsi575_I2C_STOP_IDLE_STOP_SETUP                            (0xffff0000)

/* Tsi575_I2C_SDA_SETUP_HOLD : Register Bits Masks Definitions */
#define Tsi575_I2C_SDA_SETUP_HOLD_SDA_HOLD                         (0x0000ffff)
#define Tsi575_I2C_SDA_SETUP_HOLD_SDA_SETUP                        (0xffff0000)

/* Tsi575_I2C_SCL_PERIOD : Register Bits Masks Definitions */
#define Tsi575_I2C_SCL_PERIOD_SCL_LOW                              (0x0000ffff)
#define Tsi575_I2C_SCL_PERIOD_SCL_HIGH                             (0xffff0000)

/* Tsi575_I2C_SCL_MIN_PERIOD : Register Bits Masks Definitions */
#define Tsi575_I2C_SCL_MIN_PERIOD_SCL_MINL                         (0x0000ffff)
#define Tsi575_I2C_SCL_MIN_PERIOD_SCL_MINH                         (0xffff0000)

/* Tsi575_I2C_SCL_ARB_TIMEOUT : Register Bits Masks Definitions */
#define Tsi575_I2C_SCL_ARB_TIMEOUT_ARB_TO                          (0x0000ffff)
#define Tsi575_I2C_SCL_ARB_TIMEOUT_SCL_TO                          (0xffff0000)

/* Tsi575_I2C_BYTE_TRAN_TIMEOUT : Register Bits Masks Definitions */
#define Tsi575_I2C_BYTE_TRAN_TIMEOUT_TRAN_TO                       (0x0000ffff)
#define Tsi575_I2C_BYTE_TRAN_TIMEOUT_BYTE_TO                       (0xffff0000)

/* Tsi575_I2C_BOOT_DIAG_TIMER : Register Bits Masks Definitions */
#define Tsi575_I2C_BOOT_DIAG_TIMER_COUNT                           (0x0000ffff)
#define Tsi575_I2C_BOOT_DIAG_TIMER_FREERUN                         (0x80000000)

/* Tsi575_I2C_DIAG_FILTER_SCL : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_FILTER_SCL_SAMPLES                         (0xffffffff)

/* Tsi575_I2C_DIAG_FILTER_SDA : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_FILTER_SDA_SAMPLES                         (0xffffffff)

/* Tsi575_I2C_BOOT_DIAG_PROGRESS : Register Bits Masks Definitions */
#define Tsi575_I2C_BOOT_DIAG_PROGRESS_PADDR                        (0x0000ffff)
#define Tsi575_I2C_BOOT_DIAG_PROGRESS_REGCNT                       (0xffff0000)

/* Tsi575_I2C_BOOT_DIAG_CFG : Register Bits Masks Definitions */
#define Tsi575_I2C_BOOT_DIAG_CFG_BOOT_ADDR                         (0x0000007f)
#define Tsi575_I2C_BOOT_DIAG_CFG_PINC                              (0x10000000)
#define Tsi575_I2C_BOOT_DIAG_CFG_PASIZE                            (0x20000000)
#define Tsi575_I2C_BOOT_DIAG_CFG_BDIS                              (0x40000000)
#define Tsi575_I2C_BOOT_DIAG_CFG_BOOTING                           (0x80000000)

/* Tsi575_I2C_DIAG_CNTRL : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_CNTRL_SDAB                                 (0x00000001)
#define Tsi575_I2C_DIAG_CNTRL_SDAOV                                (0x00000002)
#define Tsi575_I2C_DIAG_CNTRL_SCLB                                 (0x00000004)
#define Tsi575_I2C_DIAG_CNTRL_SCLOV                                (0x00000008)
#define Tsi575_I2C_DIAG_CNTRL_START_CMD                            (0x00000100)
#define Tsi575_I2C_DIAG_CNTRL_CMD                                  (0x00000e00)
#define Tsi575_I2C_DIAG_CNTRL_NO_STRETCH                           (0x00002000)
#define Tsi575_I2C_DIAG_CNTRL_NOIDLE                               (0x00004000)
#define Tsi575_I2C_DIAG_CNTRL_NOFAIL                               (0x00008000)
#define Tsi575_I2C_DIAG_CNTRL_WRITE_DATA                           (0x00ff0000)

/* Tsi575_I2C_DIAG_STAT : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_STAT_SCLIN                                 (0x00000001)
#define Tsi575_I2C_DIAG_STAT_SCLOUT                                (0x00000002)
#define Tsi575_I2C_DIAG_STAT_SDAIN                                 (0x00000004)
#define Tsi575_I2C_DIAG_STAT_SDAOUT                                (0x00000008)
#define Tsi575_I2C_DIAG_STAT_IDSTATE                               (0x000000e0)
#define Tsi575_I2C_DIAG_STAT_READ_DATA                             (0x0000ff00)
#define Tsi575_I2C_DIAG_STAT_ACK                                   (0x00010000)
#define Tsi575_I2C_DIAG_STAT_S_SCL                                 (0x00100000)
#define Tsi575_I2C_DIAG_STAT_S_SDA                                 (0x00200000)
#define Tsi575_I2C_DIAG_STAT_M_SCL                                 (0x00400000)
#define Tsi575_I2C_DIAG_STAT_M_SDA                                 (0x00800000)
#define Tsi575_I2C_DIAG_STAT_SDA_TIMER                             (0x07000000)
#define Tsi575_I2C_DIAG_STAT_SCLIT                                 (0x08000000)
#define Tsi575_I2C_DIAG_STAT_SCLHT                                 (0x10000000)
#define Tsi575_I2C_DIAG_STAT_SCLMHT                                (0x20000000)
#define Tsi575_I2C_DIAG_STAT_SLVSDA                                (0x40000000)

/* Tsi575_I2C_DIAG_HIST : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_HIST_COUNT                                 (0x0000000f)
#define Tsi575_I2C_DIAG_HIST_H1                                    (0x00000030)
#define Tsi575_I2C_DIAG_HIST_H2                                    (0x000000c0)
#define Tsi575_I2C_DIAG_HIST_H3                                    (0x00000300)
#define Tsi575_I2C_DIAG_HIST_H4                                    (0x00000c00)
#define Tsi575_I2C_DIAG_HIST_H5                                    (0x00003000)
#define Tsi575_I2C_DIAG_HIST_H6                                    (0x0000c000)
#define Tsi575_I2C_DIAG_HIST_H7                                    (0x00030000)
#define Tsi575_I2C_DIAG_HIST_H8                                    (0x000c0000)
#define Tsi575_I2C_DIAG_HIST_H9                                    (0x00300000)
#define Tsi575_I2C_DIAG_HIST_H10                                   (0x00c00000)
#define Tsi575_I2C_DIAG_HIST_H11                                   (0x03000000)
#define Tsi575_I2C_DIAG_HIST_H12                                   (0x0c000000)
#define Tsi575_I2C_DIAG_HIST_H13                                   (0x30000000)
#define Tsi575_I2C_DIAG_HIST_H14                                   (0xc0000000)

/* Tsi575_I2C_DIAG_MST_FSM : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_MST_FSM_BIT_FSM                            (0x0000001f)
#define Tsi575_I2C_DIAG_MST_FSM_BYTE_FSM                           (0x000001e0)
#define Tsi575_I2C_DIAG_MST_FSM_PTOB_FSM                           (0x00000e00)
#define Tsi575_I2C_DIAG_MST_FSM_PROTO_FSM                          (0x0001f000)
#define Tsi575_I2C_DIAG_MST_FSM_MST_FSM                            (0x000e0000)
#define Tsi575_I2C_DIAG_MST_FSM_BOOT_FSM                           (0x00f00000)

/* Tsi575_I2C_DIAG_SLV_FSM : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_SLV_FSM_BIT_FSM                            (0x0000000f)
#define Tsi575_I2C_DIAG_SLV_FSM_BYTE_FSM                           (0x000000f0)
#define Tsi575_I2C_DIAG_SLV_FSM_PTOB_FSM                           (0x00000700)
#define Tsi575_I2C_DIAG_SLV_FSM_PROTO_FSM                          (0x00007800)

/* Tsi575_I2C_DIAG_MST_SDA_SCL : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_MST_SDA_SCL_SCLTO_IDLE                     (0x0000ffff)
#define Tsi575_I2C_DIAG_MST_SDA_SCL_SDA_CNT                        (0xffff0000)

/* Tsi575_I2C_DIAG_MST_SCL_PER : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_MST_SCL_PER_SCL_MIN_PER                    (0x0000ffff)
#define Tsi575_I2C_DIAG_MST_SCL_PER_SCL_PER                        (0xffff0000)

/* Tsi575_I2C_DIAG_MST_ARB_BOOT : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_MST_ARB_BOOT_BOOT_CNT                      (0x0000ffff)
#define Tsi575_I2C_DIAG_MST_ARB_BOOT_ARB_TO_CNT                    (0xffff0000)

/* Tsi575_I2C_DIAG_MST_BYTE_TRAN : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_MST_BYTE_TRAN_TRAN_TO_CNT                  (0x0000ffff)
#define Tsi575_I2C_DIAG_MST_BYTE_TRAN_BYTE_TO_CNT                  (0xffff0000)

/* Tsi575_I2C_DIAG_SLV_SDA_SCL : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_SLV_SDA_SCL_SCLTO                          (0x0000ffff)
#define Tsi575_I2C_DIAG_SLV_SDA_SCL_SDA_CNT                        (0xffff0000)

/* Tsi575_I2C_DIAG_SLV_BYTE_TRAN : Register Bits Masks Definitions */
#define Tsi575_I2C_DIAG_SLV_BYTE_TRAN_TRAN_TO_CNT                  (0x0000ffff)
#define Tsi575_I2C_DIAG_SLV_BYTE_TRAN_BYTE_TO_CNT                  (0xffff0000)

                                                                               

#ifdef __cplusplus
}
#endif

#endif /* _TS_Tsi575_H_ */

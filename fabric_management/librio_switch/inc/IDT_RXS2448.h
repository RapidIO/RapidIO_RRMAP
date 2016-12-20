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

#ifndef _IDT_RXS2448_H_
#define _IDT_RXS2448_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RXS2448_MAX_PORTS	24
#define RXS2448_MAX_LANES	48
#define RXS2448_MAX_SC    	8

#define RXS_MAX_L1_GROUP	3
#define RXS_MAX_L2_GROUP	4

#define FIRST_BYTE_MASK		(0x000000FF)
#define SECOND_BYTE_MASK	(0x0000FF00)
#define THIRD_BYTE_MASK		(0x00FF0000)
#define FOURTH_BYTE_MASK	(0xFF000000)

/* ************************************************ */
/* RXS2448 : Register address offset definitions    */
/* ************************************************ */
#define RXS_RIO_DEV_ID                                        (0x00000000)
#define RXS_RIO_ROUTE_DFLT_PORT                               (0x00000078)
#define RXS_RIO_SPX_LM_REQ(X)                                 (0x00000140 +(0x40*X))
#define RXS_RIO_SPX_LM_RESP(X)                                (0x00000144 +(0x40*X))
#define RXS_RIO_SPX_CTL2(X)                                   (0x00000154 +(0x40*X))
#define RXS_RIO_SPX_ERR_STAT(X)                               (0x00000158 +(0x40*X))
#define RXS_RIO_SPX_CTL(X)                                    (0x0000015c +(0x40*X))
#define RXS_RIO_SP_LT_CTL                                     (0x00000120)
#define RXS_RIO_SR_RSP_TO                                     (0x00000124)

#define RXS_RIO_ERR_DET                                       (0x00001008)
#define RXS_RIO_ERR_EN                                        (0x0000100C)
#define RXS_RIO_ID_CAPT                                       (0x00001018)
#define RXS_RIO_PW_TGT_ID                                     (0x00001028)
#define RXS_RIO_PKT_TIME_LIVE                                 (0x0000102c)
#define RXS_RIO_SPX_ERR_DET(X)                                (0x00001040 +(0x40*X))
#define RXS_RIO_SPX_RATE_EN(X)                                (0x00001044 +(0x40*X))
#define RXS_RIO_SPX_DLT_CSR(X)                                (0x00001070 +(0x40*X))

#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL(X)                       (0x00010100 +(0x100*(X)))
#define RXS_RIO_PLM_SPX_STAT(X)                               (0x00010110 +(0x100*(X)))
#define RXS_RIO_PLM_SPX_INT_EN(X)                             (0x00010114 +(0x100*(X)))
#define RXS_RIO_PLM_SPX_PW_EN(X)                              (0x00010118 +(0x100*(X)))
#define RXS_PLM_SPX_POL_CTL(X)                                (0x00010140 +(0x100*(X)))
#define RXS_RIO_PLM_SPX_DENIAL_CTL(X)                         (0x00010148 +(0x100*(X)))
#define RXS_RIO_PBM_SPX_INT_EN(X)                             (0x00018114 +(0x100*(X)))
#define RXS_RIO_PBM_SPX_PW_EN(X)                              (0x00018118 +(0x100*(X)))
#define RXS_RIO_PCNTR_CTL                                     (0x0001c004)
#define RXS_RIO_SPX_PCNTR_EN(X)                               (0x0001c100 +(0x100*(X)))
#define RXS_RIO_SPX_PCNTR_CTL(X,Y)                            (0x0001c110 +(0x100*(X))+(4*Y))
#define RXS_RIO_SPX_PCNTR_CNT(X,Y)                            (0x0001c130 +(0x100*(X))+(4*Y))

#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN(X)                      (0x00024110 +(0x100*(X)))
#define RXS_RIO_SPX_DBG_EL_INT_EN(X)                          (0x0002410C +(0x100*(X)))
#define RXS_RIO_LANEX_PRBS_STATUS(X)                          (0x00028108 +(0x100*(X)))

#define RXS_RIO_BC_L0_G0_ENTRYX_CSR(X)                        (0x00030000 +(0x4*X))
#define RXS_RIO_BC_L1_GX_ENTRYY_CSR(X, Y)                     (0x00030400 +(0x400*(X))+(0x4*Y))
#define RXS_RIO_BC_L2_GX_ENTRYY_CSR(X, Y)                     (0x00031000 +(0x400*(X))+(0x4*Y))
#define RXS_RIO_BC_MC_X_S_CSR(X)                              (0x00032000 +(0x8*(X)))

#define RXS_RIO_EM_INT_EN                                     (0x00040014)
#define RXS_RIO_EM_PW_EN                                      (0x00040024)
#define RXS_RIO_EM_RST_INT_EN                                 (0x00040068)
#define RXS_RIO_EM_RST_PW_EN                                  (0x00040070)
#define RXS_RIO_PW_CTL                                        (0x00040204)
#define RXS_RIO_MPM_MTC_RESP_PRIO                             (0x00040478)

#define RXS_RIO_SPX_L0_G0_ENTRYY_CSR(X, Y)                    (0x00050000 +(0x2000*(X))+(0x4*Y))
#define RXS_RIO_SPX_L1_GY_ENTRYZ_CSR(X, Y, Z)                 (0x00050400 +(0x2000*(X))+(0x400*Y)+(0x4*Z))
#define RXS_RIO_SPX_L2_GY_ENTRYZ_CSR(X, Y, Z)                 (0x00051000 +(0x2000*(X))+(0x400*Y)+(0x4*Z))

#define RXS_RIO_SPX_MC_Y_S_CSR(X, Y)                          (0x00080000 +(0x1000*(X))+(0x8*Y))

/* RXS_RIO_DEV_ID : Register Bits Masks Definitions */
#define RXS_RIO_DEV_IDENT_VEND                                (0x0000ffff)
#define RXS_RIO_DEV_IDENT_DEVI                                (0xffff0000)
#define RXS_RIO_DEVICE_VENDOR                                 (0x00000038)

/* RXS_RIO_ROUTE_DFLT_PORT : Register Bits Masks Definitions */
#define RXS_RIO_ROUTE_DFLT_PORT_CAPTURE                       (0x80000000)
#define RXS_RIO_ROUTE_DFLT_PORT_ROUTE_TYPE                    (0x00000300)
#define RXS_RIO_ROUTE_DFLT_PORT_DEFAULT_OUT_PORT              (0x000000FF)

/* RXS_RIO_SPX_LM_RESP : Register Bits Masks Definitions */
#define RXS_RIO_SPX_LM_RESP_LINK_STAT                         (0x0000001f)
#define RXS_RIO_SPX_LM_RESP_ACK_ID_STAT                       (0x0001ffe0)
#define RXS_RIO_SPX_LM_RESP_STAT_CS64                         (0x01ffe000)
#define RXS_RIO_SPX_LM_RESP_RESP_VLD                          (0x80000000)

/* RXS_RIO_SPX_CTL2 : Register Bits Masks Definitions */
#define RXS_RIO_SPX_CTL2_RTEC_EN                              (0x00000001)
#define RXS_RIO_SPX_CTL2_RTEC                                 (0x00000002)
#define RXS_RIO_SPX_CTL2_D_SCRM_DIS                           (0x00000004)
#define RXS_RIO_SPX_CTL2_INACT_EN                             (0x00000008)
#define RXS_RIO_SPX_CTL2_D_RETRAIN_EN                         (0x00000010)
#define RXS_RIO_SPX_CTL2_GB_12p5_EN                           (0x00001000)
#define RXS_RIO_SPX_CTL2_GB_12p5                              (0x00002000)
#define RXS_RIO_SPX_CTL2_GB_10p3_EN                           (0x00004000)
#define RXS_RIO_SPX_CTL2_GB_10p3                              (0x00008000)
#define RXS_RIO_SPX_CTL2_GB_6p25_EN                           (0x00010000)
#define RXS_RIO_SPX_CTL2_GB_6p25                              (0x00020000)
#define RXS_RIO_SPX_CTL2_GB_5p0_EN                            (0x00040000)
#define RXS_RIO_SPX_CTL2_GB_5p0                               (0x00080000)
#define RXS_RIO_SPX_CTL2_GB_3p125_EN                          (0x00100000)
#define RXS_RIO_SPX_CTL2_GB_3p125                             (0x00200000)
#define RXS_RIO_SPX_CTL2_GB_2p5_EN                            (0x00400000)
#define RXS_RIO_SPX_CTL2_GB_2p5                               (0x00800000)
#define RXS_RIO_SPX_CTL2_GB_1p25_EN                           (0x01000000)
#define RXS_RIO_SPX_CTL2_GB_1p25                              (0x02000000)
#define RXS_RIO_SPX_CTL2_BAUD_DISC                            (0x08000000)
#define RXS_RIO_SPX_CTL2_BAUD_SEL                             (0xf0000000)

/* RXS_RIO_SPX_ERR_STAT : Register Bits Masks Definitions */
#define RXS_RIO_SPX_ERR_STAT_PORT_UNIT                        (0x00000001)
#define RXS_RIO_SPX_ERR_STAT_PORT_OK                          (0x00000002)
#define RXS_RIO_SPX_ERR_STAT_PORT_ERR                         (0x00000004)
#define RXS_RIO_SPX_ERR_STAT_PORT_UNAVL                       (0x00000008)
#define RXS_RIO_SPX_ERR_STAT_PORT_W_P                         (0x00000010)
#define RXS_RIO_SPX_ERR_STAT_PORT_W_DIS                       (0x00000020)
#define RXS_RIO_SPX_ERR_STAT_INPUT_ERR_STOP                   (0x00000100)
#define RXS_RIO_SPX_ERR_STAT_INPUT_ERR_ENCTR                  (0x00000200)
#define RXS_RIO_SPX_ERR_STAT_INPUT_RS                         (0x00000400)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_ERR_STOP                  (0x00010000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_ERR_ENCTR                 (0x00020000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_RS                        (0x00040000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_R                         (0x00080000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_RE                        (0x00100000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_DEGR                      (0x01000000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_FAIL                      (0x02000000)
#define RXS_RIO_SPX_ERR_STAT_OUTPUT_DROP                      (0x04000000)
#define RXS_RIO_SPX_ERR_STAT_TXFC                             (0x08000000)
#define RXS_RIO_SPX_ERR_STAT_IDLE_SEQ                         (0x20000000)
#define RXS_RIO_SPX_ERR_STAT_IDLE2_EN                         (0x40000000)
#define RXS_RIO_SPX_ERR_STAT_IDLE2                            (0x80000000)

/* RXS_RIO_SPX_CTL : Register Bits Masks Definitions */
#define RXS_RIO_SPX_CTL_PTYP                                  (0x00000001)
#define RXS_RIO_SPX_CTL_PORT_LOCKOUT                          (0x00000002)
#define RXS_RIO_SPX_CTL_DROP_EN                               (0x00000004)
#define RXS_RIO_SPX_CTL_STOP_FAIL_EN                          (0x00000008)
#define RXS_RIO_SPX_CTL_PORT_WIDTH2                           (0x00003000)
#define RXS_RIO_SPX_CTL_OVER_PWIDTH2                          (0x0000c000)
#define RXS_RIO_SPX_CTL_FLOW_ARB                              (0x00010000)
#define RXS_RIO_SPX_CTL_ENUM_B                                (0x00020000)
#define RXS_RIO_SPX_CTL_FLOW_CTRL                             (0x00040000)
#define RXS_RIO_SPX_CTL_MULT_CS                               (0x00080000)
#define RXS_RIO_SPX_CTL_ERR_DIS                               (0x00100000)
#define RXS_RIO_SPX_CTL_INP_EN                                (0x00200000)
#define RXS_RIO_SPX_CTL_OTP_EN                                (0x00400000)
#define RXS_RIO_SPX_CTL_PORT_DIS                              (0x00800000)
#define RXS_RIO_SPX_CTL_OVER_PWIDTH                           (0x07000000)
#define RXS_RIO_SPX_CTL_INIT_PWIDTH                           (0x38000000)
#define RXS_RIO_SPX_CTL_PORT_WIDTH                            (0xc0000000)

/* RXS_RIO_PKT_TIME_LIVE : Register Bits Masks Definitions */
#define RXS_RIO_PKT_TIME_LIVE_PKT_TIME_LIVE                   (0xffff0000)

/* RXS_RIO_PLM_SP0_IMP_SPEC_CTL : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_PA_BKLOG_THRESH          (0x0000003f)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_CONT_PNA                 (0x00000040)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_CONT_LR                  (0x00000080)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_BLIP_CS                  (0x00000100)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_LOST_CS_DIS              (0x00000200)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_INFER_SELF_RST           (0x00000400)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_DLT_FATAL                (0x00000800)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_PRE_SILENCE              (0x00001000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_OK2U_FATAL               (0x00002000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_MAXD_FATAL               (0x00004000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_DWNGD_FATAL              (0x00008000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_RX                  (0x00030000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_TX                  (0x000c0000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SELF_RST                 (0x00100000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_PORT_SELF_RST            (0x00200000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_RESET_REG                (0x00400000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_LLB_EN                   (0x00800000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_CS_FIELD1                (0x01000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SOFT_RST_PORT            (0x02000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_FORCE_REINIT             (0x04000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_DME_TRAINING             (0x08000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_DLB_EN                   (0x10000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_USE_IDLE1                (0x20000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_USE_IDLE2                (0x40000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_USE_IDLE3                (0x80000000)

#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_RX_NONE             (0x00000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_RX_1032             (0x00010000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_RX_3210             (0x00020000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_RX_2301             (0x00030000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_TX_NONE             (0x00000000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_TX_1032             (0x00040000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_TX_3210             (0x00080000)
#define RXS_RIO_PLM_SP0_IMP_SPEC_CTL_SWAP_TX_2301             (0x000C0000)

/* RXS_RIO_PW_TGT_ID : Register Bits Masks Definitions */
#define RXS_RIO_PW_TGT_ID_DEV32                               (0x00004000)
#define RXS_RIO_PW_TGT_ID_DEV16                               (0x00008000)
#define RXS_RIO_PW_TGT_ID_PW_TGT_ID                           (0x00ff0000)
#define RXS_RIO_PW_TGT_ID_MSB_PW_ID                           (0xff000000)

/* RXS_RIO_SPX_ERR_DET : Register Bits Masks Definitions */
#define RXS_RIO_SPX_ERR_DET_OK_TO_UNINIT                      (0x04000000)
#define RXS_RIO_SPX_ERR_DET_DLT                               (0x02000000)
#define RXS_RIO_SPX_ERR_DET_LINK_INIT                         (0x01000000)

/* RXS_RIO_SPX_RATE_EN : Register Bits Masks Definitions */
#define RXS_RIO_SPX_RATE_EN_OK_TO_UNINIT                      (0x04000000)
#define RXS_RIO_SPX_RATE_EN_DLT                               (0x02000000)
#define RXS_RIO_SPX_RATE_EN_LINK_INIT                         (0x01000000)

/* RXS_RIO_SPX_DLT_CSR : Register Bits Masks Definitions */
#define RXS_RIO_SPX_DLT_CSR_TIMEOUT                           (0xFFFFFF00)

/* RXS_RIO_PLM_SPX_IMP_SPEC_CTL : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_PA_BKLOG_THRESH          (0x0000003f)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_CONT_PNA                 (0x00000040)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_CONT_LR                  (0x00000080)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_BLIP_CS                  (0x00000100)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_LOST_CS_DIS              (0x00000200)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_INFER_SELF_RST           (0x00000400)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_DLT_FATAL                (0x00000800)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_PRE_SILENCE              (0x00001000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_OK2U_FATAL               (0x00002000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_MAXD_FATAL               (0x00004000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_DWNGD_FATAL              (0x00008000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX                  (0x00030000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_TX                  (0x000c0000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SELF_RST                 (0x00100000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_PORT_SELF_RST            (0x00200000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_RESET_REG                (0x00400000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_LLB_EN                   (0x00800000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_CS_FIELD1                (0x01000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SOFT_RST_PORT            (0x02000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_FORCE_REINIT             (0x04000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_DME_TRAINING             (0x08000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_DLB_EN                   (0x10000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_USE_IDLE1                (0x20000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_USE_IDLE2                (0x40000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_USE_IDLE3                (0x80000000)

#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX_NONE             (0x00000000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX_1032             (0x00010000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX_3210             (0x00020000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_RX_2301             (0x00030000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_TX_1032             (0x00040000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_TX_3210             (0x00080000)
#define RXS_RIO_PLM_SPX_IMP_SPEC_CTL_SWAP_TX_2301             (0x000C0000)

/* RXS_RIO_ERR_DET : Register Bits Masks Definitions */
#define RXS_RIO_ERR_DET_ILL_ID                                (0x04000000)
#define RXS_RIO_ERR_DET_UNS_RSP                               (0x00800000)
#define RXS_RIO_ERR_DET_ILL_TYPE                              (0x00400000)

/* RXS_RIO_ERR_EN : Register Bits Masks Definitions */
#define RXS_RIO_ERR_EN_ILL_ID_EN                              (0x04000000)
#define RXS_RIO_ERR_EN_UNS_RSP_EN                             (0x00800000)
#define RXS_RIO_ERR_EN_ILL_TYPE_EN                            (0x00400000)

/* RXS_RIO_ID_CAPT : Register Bits Masks Definitions */
#define RXS_RIO_ID_CAPT_MSB_DEST_ID                           (0xFF000000)
#define RXS_RIO_ID_CAPT_DEST_ID                               (0x00FF0000)
#define RXS_RIO_ID_CAPT_MSB_SRC_ID                            (0x0000FF00)
#define RXS_RIO_ID_CAPT_SRC_ID                                (0x000000FF)

/* RXS_RIO_PW_TGT_ID : Register Bits Masks Definitions */
#define RXS_RIO_PW_TGT_ID_DEV32                               (0x00004000)
#define RXS_RIO_PW_TGT_ID_DEV16                               (0x00008000)
#define RXS_RIO_PW_TGT_ID_PW_TGT_ID                           (0x00ff0000)
#define RXS_RIO_PW_TGT_ID_MSB_PW_ID                           (0xff000000)

/* RXS_RIO_PLM_SPX_STAT : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SPX_STAT_MAX_DENIAL                       (0x80000000)  
#define RXS_RIO_PLM_SPX_STAT_OK_TO_UNINIT                     (0x40000000)
#define RXS_RIO_PLM_SPX_STAT_DLT                              (0x20000000)
#define RXS_RIO_PLM_SPX_STAT_LINK_INIT                        (0x10000000)
#define RXS_RIO_PLM_SPX_STAT_PORT_ERR                         (0x08000000)
#define RXS_RIO_PLM_SPX_STAT_PBM_FATAL                        (0x04000000)
#define RXS_RIO_PLM_SPX_STAT_DWNGD                            (0x02000000)
#define RXS_RIO_PLM_SPX_STAT_PCAP                             (0x01000000)
#define RXS_RIO_PLM_SPX_STAT_II_CHG_3                         (0x00800000)
#define RXS_RIO_PLM_SPX_STAT_II_CHG_2                         (0x00400000)
#define RXS_RIO_PLM_SPX_STAT_II_CHG_1                         (0x00200000)
#define RXS_RIO_PLM_SPX_STAT_II_CHG_0                         (0x00100000)
#define RXS_RIO_PLM_SPX_STAT_EL_INTB                          (0x00080000)
#define RXS_RIO_PLM_SPX_STAT_EL_INTA                          (0x00040000)
#define RXS_RIO_PLM_SPX_STAT_PRST_REQ                         (0x00020000)
#define RXS_RIO_PLM_SPX_STAT_RST_REQ                          (0x00010000)
#define RXS_RIO_PLM_SPX_STAT_PBM_PW                           (0x00008000)
#define RXS_RIO_PLM_SPX_STAT_TLM_PW                           (0x00004000)
#define RXS_RIO_PLM_SPX_STAT_MECS                             (0x00001000)
#define RXS_RIO_PLM_SPX_STAT_PBM_INT                          (0x00000800)
#define RXS_RIO_PLM_SPX_STAT_TLM_INT                          (0x00000400)
#define RXS_RIO_PLM_SPX_STAT_BERM_3                           (0x00000008)
#define RXS_RIO_PLM_SPX_STAT_BERM_2                           (0x00000004)
#define RXS_RIO_PLM_SPX_STAT_BERM_1                           (0x00000002)
#define RXS_RIO_PLM_SPX_STAT_BERM_0                           (0x00000001)

/* RXS_RIO_PLM_SPX_INT_EN : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SPX_INT_EN_MAX_DENIAL                     (0x80000000)
#define RXS_RIO_PLM_SPX_INT_EN_OK_TO_UNINIT                   (0x40000000) 
#define RXS_RIO_PLM_SPX_INT_EN_DLT                            (0x20000000)
#define RXS_RIO_PLM_SPX_INT_EN_LINK_INIT                      (0x10000000)
#define RXS_RIO_PLM_SPX_INT_EN_PORT_ERR                       (0x08000000)
#define RXS_RIO_PLM_SPX_INT_EN_PBM_FATAL                      (0x04000000)
#define RXS_RIO_PLM_SPX_INT_EN_DWNGD                          (0x02000000)
#define RXS_RIO_PLM_SPX_INT_EN_PCAP                           (0x01000000)
#define RXS_RIO_PLM_SPX_INT_EN_II_CHG_3                       (0x00800000)
#define RXS_RIO_PLM_SPX_INT_EN_II_CHG_2                       (0x00400000)
#define RXS_RIO_PLM_SPX_INT_EN_II_CHG_1                       (0x00200000)
#define RXS_RIO_PLM_SPX_INT_EN_II_CHG_0                       (0x00100000)
#define RXS_RIO_PLM_SPX_INT_EN_EL_INTB                        (0x00080000)
#define RXS_RIO_PLM_SPX_INT_EN_EL_INTA                        (0x00040000)
#define RXS_RIO_PLM_SPX_INT_EN_BERM_3                         (0x00000008)
#define RXS_RIO_PLM_SPX_INT_EN_BERM_2                         (0x00000004)
#define RXS_RIO_PLM_SPX_INT_EN_BERM_1                         (0x00000002)
#define RXS_RIO_PLM_SPX_INT_EN_BERM_0                         (0x00000001)

/* RXS_RIO_PLM_SPX_PW_EN : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SPX_PW_EN_MAX_DENIAL                      (0x80000000)
#define RXS_RIO_PLM_SPX_PW_EN_OK_TO_UNINIT                    (0x40000000) 
#define RXS_RIO_PLM_SPX_PW_EN_DLT                             (0x20000000)
#define RXS_RIO_PLM_SPX_PW_EN_LINK_INIT                       (0x10000000)
#define RXS_RIO_PLM_SPX_PW_EN_PORT_ERR                        (0x08000000)
#define RXS_RIO_PLM_SPX_PW_EN_PBM_FATAL                       (0x04000000)
#define RXS_RIO_PLM_SPX_PW_EN_DWNGD                           (0x02000000)
#define RXS_RIO_PLM_SPX_PW_EN_PCAP                            (0x01000000)
#define RXS_RIO_PLM_SPX_PW_EN_II_CHG_3                        (0x00800000)
#define RXS_RIO_PLM_SPX_PW_EN_II_CHG_2                        (0x00400000)
#define RXS_RIO_PLM_SPX_PW_EN_II_CHG_1                        (0x00200000)
#define RXS_RIO_PLM_SPX_PW_EN_II_CHG_0                        (0x00100000)
#define RXS_RIO_PLM_SPX_PW_EN_EL_INTB                         (0x00080000)
#define RXS_RIO_PLM_SPX_PW_EN_EL_INTA                         (0x00040000)
#define RXS_RIO_PLM_SPX_PW_EN_BERM_3                          (0x00000008)
#define RXS_RIO_PLM_SPX_PW_EN_BERM_2                          (0x00000004)
#define RXS_RIO_PLM_SPX_PW_EN_BERM_1                          (0x00000002)
#define RXS_RIO_PLM_SPX_PW_EN_BERM_0                          (0x00000001)

/* RXS_PLM_SPX_POL_CTL : Register Bits Masks Definitions */
#define RXS_PLM_SPX_POL_CTL_TX3_POL                           (0x00080000)
#define RXS_PLM_SPX_POL_CTL_TX2_POL                           (0x00040000)
#define RXS_PLM_SPX_POL_CTL_TX1_POL                           (0x00020000)
#define RXS_PLM_SPX_POL_CTL_TX0_POL                           (0x00010000)
#define RXS_PLM_SPX_POL_CTL_RX3_POL                           (0x00000008)
#define RXS_PLM_SPX_POL_CTL_RX2_POL                           (0x00000004)
#define RXS_PLM_SPX_POL_CTL_RX1_POL                           (0x00000002)
#define RXS_PLM_SPX_POL_CTL_RX0_POL                           (0x00000001)

/* RXS_RIO_PLM_SPX_DENIAL_CTL : Register Bits Masks Definitions */
#define RXS_RIO_PLM_SPX_DENIAL_CTL_CNT_PNA                    (0x20000000)
#define RXS_RIO_PLM_SPX_DENIAL_CTL_CNT_RTY                    (0x10000000)
#define RXS_RIO_PLM_SPX_DENIAL_CTL_DENIAL_THRESH              (0x0000FFFF)

/* RXS_RIO_PBM_SPX_INT_EN : Register Bits Masks Definitions */
#define RXS_RIO_PBM_SPX_INT_EN_EG_DATA_COR                    (0x00001000)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DATA_UNCOR                  (0x00000800)
#define RXS_RIO_PBM_SPX_INT_EN_EG_TTL_EXPIRED                 (0x00000200)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DOH_COR                     (0x00000100)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DOH_FATAL                   (0x00000080)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DNFL_COR                    (0x00000040)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DNFL_FATAL                  (0x00000020)
#define RXS_RIO_PBM_SPX_INT_EN_EG_DATA_OVERFLOW               (0x00000010)
#define RXS_RIO_PBM_SPX_INT_EN_EG_CRQ_OVERFLOW                (0x00000008)
#define RXS_RIO_PBM_SPX_INT_EN_EG_BAD_CHANNEL                 (0x00000002)
#define RXS_RIO_PBM_SPX_INT_EN_EG_BABBLE_PACKET               (0x00000001)

/* RXS_RIO_PBM_SPX_PW_EN : Register Bits Masks Definitions */
#define RXS_RIO_PBM_SPX_PW_EN_EG_DATA_COR                     (0x00001000)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DATA_UNCOR                   (0x00000800)
#define RXS_RIO_PBM_SPX_PW_EN_EG_TTL_EXPIRED                  (0x00000200)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DOH_COR                      (0x00000100)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DOH_FATAL                    (0x00000080)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DNFL_COR                     (0x00000040)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DNFL_FATAL                   (0x00000020)
#define RXS_RIO_PBM_SPX_PW_EN_EG_DATA_OVERFLOW                (0x00000010)
#define RXS_RIO_PBM_SPX_PW_EN_EG_CRQ_OVERFLOW                 (0x00000008)
#define RXS_RIO_PBM_SPX_PW_EN_EG_BAD_CHANNEL                  (0x00000002)
#define RXS_RIO_PBM_SPX_PW_EN_EG_BABBLE_PACKET                (0x00000001)

/* RXS_RIO_PCNTR_CTL : Register Bits Masks Definitions */
#define RXS_RIO_PCNTR_CTL_CNTR_FRZ                            (0x80000000)
#define RXS_RIO_PCNTR_CTL_CNTR_CLR                            (0x40000000)

/* RXS_RIO_SPX_PCNTR_EN : Register Bits Masks Definitions */
#define RXS_RIO_SPX_PCNTR_EN_ENABLE                           (0x80000000)

/* RXS_RIO_SPX_PCNTR_CTL : Register Bits Masks Definitions */
#define RXS_RIO_SPX_PCNTR_CTL_TX                              (0x00000080)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO0                           (0x00000100)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO0C                          (0x00000200)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO1                           (0x00000400)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO1C                          (0x00000800)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO2                           (0x00001000)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO2C                          (0x00002000)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO3                           (0x00004000)
#define RXS_RIO_SPX_PCNTR_CTL_PRIO3C                          (0x00008000)
#define RXS_RIO_SPC_PCNTR_CTL_PRIO RXS_RIO_SPX_PCNTR_CTL_PRIO0 | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO0C | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO1  | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO1C | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO2 | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO2C | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO3 | \
				RXS_RIO_SPX_PCNTR_CTL_PRIO3C
#define RXS_RIO_SPX_PCNTR_CTL_SEL                             (0x0000007F)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_PKT                     (0x00000000)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_FAB_PKT                     (0x00000001)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_PAYLOAD                 (0x00000002)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_FAB_PAYLOAD                 (0x00000003)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_RIO_TTL_PKTCNTR             (0x00000007)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_RETRIES                     (0x00000008)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_PNA                         (0x00000009)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_PKT_DROP                    (0x0000000A)
#define RXS_RIO_SPX_PCNTR_CTL_SEL_DISABLED                    (0x0000007F)

/* RXS_RIO_SPX_DBG_EL_SRC_LOG_EN : Register Bits Masks Definitions */
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_CLR                     (0x80000000)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_15                   (0x00008000)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_14                   (0x00004000)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_13                   (0x00002000)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_12                   (0x00001000)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_11                   (0x00000800)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_10                   (0x00000400)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_9                    (0x00000200)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_8                    (0x00000100)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_7                    (0x00000080)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_6                    (0x00000040)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_5                    (0x00000020)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_4                    (0x00000010)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_3                    (0x00000008)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_2                    (0x00000004)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_1                    (0x00000002)
#define RXS_RIO_SPX_DBG_EL_SRC_LOG_EN_EN_0                    (0x00000001)

/* RXS_RIO_SPX_DBG_EL_INT_EN : Register Bits Masks Definitions */
#define RXS_RIO_SPX_DBG_EL_INT_EN_EN_B                        (0x00000002)
#define RXS_RIO_SPX_DBG_EL_INT_EN_EN_A                        (0x00000001)

/* RXS_RIO_LANEX_PRBS_STATUS : Register Bits Masks Definitions */
#define RXS_RIO_LANEX_PRBS_STATUS_PRBS_LOS                    (0x00000002)
#define RXS_RIO_LANEX_PRBS_STATUS_FIXED_UNLOCK                (0x00000001)

/* RXS_RIO_EM_RST_INT_EN : Register Bits Masks Definitions */
#define RXS_RIO_EM_RST_INT_EN_RST_INT_EN                      (0x00ffffff)

/* RXS_RIO_EM_RST_PW_EN : Register Bits Masks Definitions */
#define RXS_RIO_EM_RST_PW_EN_RST_PW_EN                        (0x00ffffff)

/* RXS_RIO_PW_CTL : Register Bits Masks Definitions */
#define RXS_RIO_PW_CTL_PW_TMR                                 (0xffffff00)

/* RXS_RIO_BC_L0_GX_ENTRYY_CSR : Register Bits Masks Definitions */
#define RXS_RIO_BC_L0_GX_ENTRYY_CSR_CAPTURE                   (0x80000000)
#define RXS_RIO_BC_L0_GX_ENTRYY_CSR_ROUTING_VALUE             (0x000003FF)

/* RXS_RIO_BC_L1_GX_ENTRYY_CSR : Register Bits Masks Definitions */
#define RXS_RIO_BC_L1_GX_ENTRYY_CSR_CAPTURE                   (0x80000000)
#define RXS_RIO_BC_L1_GX_ENTRYY_CSR_ROUTING_VALUE             (0x000003FF)

/* RXS_RIO_BC_L2_GX_ENTRYY_CSR : Register Bits Masks Definitions */
#define RXS_RIO_BC_L2_GX_ENTRYY_CSR_CAPTURE                   (0x80000000)
#define RXS_RIO_BC_L2_GX_ENTRYY_CSR_ROUTING_VALUE             (0x000003FF)

/* RXS_RIO_SPX_L0_G0_ENTRYY_CSR : Register Bits Masks Definitions */
#define RXS_RIO_SPX_L0_G0_ENTRYY_CSR_CAPTURE                  (0x80000000) 
#define RXS_RIO_SPX_L0_G0_ENTRYY_CSR_ROUTING_VALUE            (0x000003FF)

/* RXS_RIO_SPX_L1_GY_ENTRYZ_CSR : Register Bits Masks Definitions */
#define RXS_RIO_SPX_L1_GY_ENTRYZ_CSR_CAPTURE                  (0x80000000)
#define RXS_RIO_SPX_L1_GY_ENTRYZ_CSR_ROUTING_VALUE            (0x000003FF)

/* RXS_RIO_SPX_L2_GY_ENTRYZ_CSR : Register Bits Masks Definitions */
#define RXS_RIO_SPX_L2_GY_ENTRYZ_CSR_CAPTURE                  (0x80000000)
#define RXS_RIO_SPX_L2_GY_ENTRYZ_CSR_ROUTING_VALUE            (0x000003FF)

/* RXS_RIO_SPX_MC_Y_S_CSR : Register Bits Masks Definitions */
#define RXS2448_RIO_SPX_MC_Y_S_CSR_SET                        (0x00FFFFFF)
#define RXS1632_RIO_SPX_MC_Y_S_CSR_SET                        (0x0000FFFF)

/* RXS_RIO_BC_MC_X_S_CSR : Register Bits Masks Definitions */
#define RXS2448_RIO_BC_MC_X_S_CSR_SET                         (0x00FFFFFF)
#define RXS1632_RIO_BC_MC_X_S_CSR_SET                         (0x0000FFFF)

#ifdef __cplusplus
}
#endif

#endif /* _IDT_RXS2448_H_  */


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
#ifndef __IDT_CPS_GEN1_API_H__
#define __IDT_CPS_GEN1_API_H__

#include <IDT_DSF_DB_Private.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CPS1_MAX_SHARED_PORTS_TO_PLL                4
#define CPS1_MAX_QUAD_NO                            10
#define CPS1_10Q_MAX_QUAD_NO                        10
#define CPS1_6Q_MAX_QUAD_NO                         6
#define CPS1_16_MAX_QUAD_NO                         4
#define CPS1_12_MAX_QUAD_NO                         3
#define CPS1_8_MAX_QUAD_NO                          2
#define CPS1_MAX_PORT_NO                            16

#define CPS1_QUAD_CONF_CSR                          0xFF0000
#define CPS1_QUAD_CONF_ADDR(quad) \
            ((uint32_t)CPS1_QUAD_CONF_CSR + ((uint32_t)quad << 12))

#define CPS1_QUAD_ENH_MODE                          0x00000020
#define IS_CPS1_QUAD_ENH_MODE(csr) ((uint32_t)csr & CPS1_QUAD_ENH_MODE)

#define CPS1_QUAD_SPEED_MASK                        3
#define CPS1_SPEED_RATE_1_25G                       0
#define CPS1_SPEED_RATE_2_5G                        1
#define CPS1_SPEED_RATE_3_125G                      2
#define CPS1_QUAD_SPEED_SEL(csr)   ((uint32_t)csr & CPS1_QUAD_SPEED_MASK)

#define IS_CPS1_SPEED_SEL_VALID(lane_rate) \
            ( !((uint32_t)lane_rate & 0xFFFFFFFC) )

#define CPS1_SINGLE_LANE0_PORT                      0
#define CPS1_SINGLE_LANE2_PORT                      1
#define CPS1_4x_LANE_PORT                           2

/* The following 3 IDENTIFIERs is provided for comparability with CPS GEN2
*/
#define CPS1_PKT_TIME_TO_LIVE_BASE                  0
#define CPS1_LANE_N_ACTIVE                          0
#define CPS1_PLL_N_ACTIVE                           0

#define CPS1_NO_PW_OVERRIDE                         0x0
#define CPS1_PW_1x_LANE0_MODE                       0x1
#define CPS1_PW_1x_LANE2_MODE                       0x2
#define CPS1_PW_4x_MODE                             0x0
#define CPS1_PW_1x_4x_MODE                          0x0

#define CPS1_PW_OVERRIDE_REG_MASK                   0x07000000

#define CPS1_GET_PW_OVERRIDE_VAL(port_n_ctrl1) \
            (((uint32_t)port_n_ctrl1 & CPS1_PW_OVERRIDE_REG_MASK) >> 24)

#define CPS1_PUT_PW_OVERRIDE_VAL(pw_override) \
            ((uint32_t)pw_override << 24)

#define CPS1_1x_SUPPORT                             0x0
#define CPS1_4x_SUPPORT                             0x1
#define CPS1_1x_4x_SUPPORT                          0x1

#define CPS1_1x_4x_SUPPORT_REG_MASK                 0xC0000000

#define CPS1_GET_PW_SUPPORT_VAL(port_n_ctrl1) \
            (((uint32_t)port_n_ctrl1 & CPS1_1x_4x_SUPPORT_REG_MASK) >> 30)

#define CPS1_PUT_PW_SUPPORT_VAL(pw_support)         ((uint32_t)pw_support << 30)

#define CPS1_TX_RATE_SEL                            0x00000007
#define CPS1_TX_RATE_1_25G                          0x00000000
#define CPS1_TX_RATE_2_5G                           0x00000001
#define CPS1_TX_RATE_3_125G                         0x00000002

#define CPS1_RX_RATE_SEL                            0x00000007
#define CPS1_RX_RATE_1_25G                          0x00000000
#define CPS1_RX_RATE_2_5G                           0x00000001
#define CPS1_RX_RATE_3_125G                         0x00000002

#define CPS1_SOFT_RESET                             0x00F20040
#define CPS1_PORT_RESET_MASK                        0x80000000

#define CPS1_CONV_PORT_RESET_REG(p)                 (1 << (15-(uint8_t)p))

/* Check whether a trace/filter function is enabled for the device or not
*/
#define IS_CPS1_TRACE_FILTER_ENABLED(dev_ctrl1) ( (uint32_t)dev_ctrl1 & 0x4000 )

/* Make a trace/filter function enable                                         
*/
#define CPS1_TRACE_FILTER_ENABLE(dev_ctrl1) ( (uint32_t)dev_ctrl1 | 0x4000 )

/* Make a trace/filter function disable                                        
*/
#define CPS1_TRACE_FILTER_DISABLE(dev_ctrl1) ( (uint32_t)dev_ctrl1 & ~0x4000 )

/* Check whether a trace output port is enabled or not
*/
#define IS_CPS1_TRACE_OUT_PORT_ENABLED( port, dev_ctrl1 ) \
            ( (uint8_t)port == (((uint8_t)dev_ctrl1 & 0x1E) >> 1) )

#define CPS1_PORT_N_ACK_CNTR_ADDR               0xF40010
#define CPS1_PORT_N_NACK_CNTR_ADDR              0xF40014
#define CPS1_PORT_N_SW_PKT_CNTR_ADDR            0xF4001C
#define CPS1_PORT_N_TRACE_MATCH_CNTR1_ADDR      0xF40020
#define CPS1_PORT_N_TRACE_MATCH_CNTR2_ADDR      0xF40024
#define CPS1_PORT_N_TRACE_MATCH_CNTR3_ADDR      0xF40028
#define CPS1_PORT_N_TRACE_MATCH_CNTR4_ADDR      0xF4002C
#define CPS1_PORT_N_FILTER_MATCH_CNTR1_ADDR     0xF40030
#define CPS1_PORT_N_FILTER_MATCH_CNTR2_ADDR     0xF40034
#define CPS1_PORT_N_FILTER_MATCH_CNTR3_ADDR     0xF40038
#define CPS1_PORT_N_FILTER_MATCH_CNTR4_ADDR     0xF4003C

#define CPS1_PORT_N_ACK_CNTR(p) \
            ( CPS1_PORT_N_ACK_CNTR_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_NACK_CNTR(p) \
            ( CPS1_PORT_N_NACK_CNTR_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_SW_PKT_CNTR(p) \
            ( CPS1_PORT_N_SW_PKT_CNTR_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_TRACE_MATCH_CNTR1(p) \
            ( CPS1_PORT_N_TRACE_MATCH_CNTR1_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_TRACE_MATCH_CNTR2(p) \
            ( CPS1_PORT_N_TRACE_MATCH_CNTR2_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_TRACE_MATCH_CNTR3(p) \
            ( CPS1_PORT_N_TRACE_MATCH_CNTR3_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_TRACE_MATCH_CNTR4(p) \
            ( CPS1_PORT_N_TRACE_MATCH_CNTR4_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_FILTER_MATCH_CNTR1(p) \
            ( CPS1_PORT_N_FILTER_MATCH_CNTR1_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_FILTER_MATCH_CNTR2(p) \
            ( CPS1_PORT_N_FILTER_MATCH_CNTR2_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_FILTER_MATCH_CNTR3(p) \
            ( CPS1_PORT_N_FILTER_MATCH_CNTR3_ADDR + ((uint32_t)p << 8) )

#define CPS1_PORT_N_FILTER_MATCH_CNTR4(p) \
            ( CPS1_PORT_N_FILTER_MATCH_CNTR4_ADDR + ((uint32_t)p << 8) )

/* Conversion a structure field (Lane Speed) to register fields (Lane Rate)
*/
uint32_t idt_cps1_conv_lane_speed(
    idt_pc_ls_t  lane_speed,
    uint32_t       *lane_rate
);

/* Routine to bind in all CPS GEN1 specific DAR support routines.
   Supports CPS16, CPS12, CPS8, CPS6Q, CPS10Q
*/
uint32_t bind_CPSGEN1_DAR_support( void );

/* Routine to bind in all CPS GEN1 specific Device Specific Function routines.
   Supports CPS16, CPS12, CPS8, CPS6Q, CPS10Q
*/
uint32_t bind_CPSGEN1_DSF_support( void );

#ifdef __cplusplus
}
#endif

#endif /* __IDT_CPS_GEN1_API_H__ */

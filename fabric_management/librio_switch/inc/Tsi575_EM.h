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
#ifndef _TS_Tsi575_EM_H_ 
#define _TS_Tsi575_EM_H_ 

                                                                               
#ifdef __cplusplus
extern "C" {
#endif

#define Tsi575_EM_NUM_REGS_TO_TEST                                  0x00000004

                                                                               

#ifndef EM_BASE
#define EM_BASE                                                    (0x0001AC00)
#endif


/* ************************************************ */
/* Tsi575 : Register address offset definitions     */
/* ************************************************ */
#define Tsi575_SW_SREP_EVENT_ROUTE                       (EM_BASE + 0x00000000)
#define Tsi575_BRIDGE_EVENT_ROUTE                        (EM_BASE + 0x00000004)
#define Tsi575_BLOCK_uint32_t                              (EM_BASE + 0x00000010)
#define Tsi575_BLOCK_GEN                                 (EM_BASE + 0x00000014)


/* ************************************************ */
/* Tsi575 : Register Bit Masks and Reset Values     */
/*           definitions for every register         */
/* ************************************************ */



/* Tsi575_SW_SREP_EVENT_ROUTE : Register Bits Masks Definitions */
#define Tsi575_SW_SREP_EVENT_ROUTE_SREP_ERR                        (0x00000007)
#define Tsi575_SW_SREP_EVENT_ROUTE_SREP_DB_RX                      (0x00000070)
#define Tsi575_SW_SREP_EVENT_ROUTE_SREP_PW_RX                      (0x00000700)
#define Tsi575_SW_SREP_EVENT_ROUTE_SREP_MCS_RX                     (0x00007000)
#define Tsi575_SW_SREP_EVENT_ROUTE_SREP_RESET_RX                   (0x00070000)
#define Tsi575_SW_SREP_EVENT_ROUTE_SWITCH_ERR                      (0x00700000)
#define Tsi575_SW_SREP_EVENT_ROUTE_I2C                             (0x07000000)
#define Tsi575_SW_SREP_EVENT_ROUTE_BISF_ERR                        (0x70000000)

/* Tsi575_BRIDGE_EVENT_ROUTE : Register Bits Masks Definitions */
#define Tsi575_BRIDGE_EVENT_ROUTE_CLK_GEN                          (0x00000700)
#define Tsi575_BRIDGE_EVENT_ROUTE_GPIO0                            (0x00070000)
#define Tsi575_BRIDGE_EVENT_ROUTE_GPIO1                            (0x00700000)
#define Tsi575_BRIDGE_EVENT_ROUTE_PCI_ERR                          (0x07000000)

/* Tsi575_BLOCK_uint32_t : Register Bits Masks Definitions */
#define Tsi575_BLOCK_uint32_t_SREP_ERR                               (0x00000001)
#define Tsi575_BLOCK_uint32_t_SREP_DB_RX                             (0x00000002)
#define Tsi575_BLOCK_uint32_t_SREP_PW_RX                             (0x00000004)
#define Tsi575_BLOCK_uint32_t_SREP_MCS_RX                            (0x00000008)
#define Tsi575_BLOCK_uint32_t_SREP_RESET_RX                          (0x00000010)
#define Tsi575_BLOCK_uint32_t_SWITCH_ERR                             (0x00000020)
#define Tsi575_BLOCK_uint32_t_I2C                                    (0x00000040)
#define Tsi575_BLOCK_uint32_t_BISF_ERR                               (0x00000080)
#define Tsi575_BLOCK_uint32_t_PCI_ERR                                (0x00000100)
#define Tsi575_BLOCK_uint32_t_GPIO0                                  (0x00000200)
#define Tsi575_BLOCK_uint32_t_GPIO1                                  (0x00000400)
#define Tsi575_BLOCK_uint32_t_CLK_GEN                                (0x00000800)

/* Tsi575_BLOCK_GEN : Register Bits Masks Definitions */
#define Tsi575_BLOCK_GEN_PCI_ERR                                   (0x00000100)

#ifdef __cplusplus
}
#endif

#endif /* _TS_Tsi575_H_ */

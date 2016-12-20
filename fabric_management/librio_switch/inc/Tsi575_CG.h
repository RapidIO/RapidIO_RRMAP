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
#ifndef _TS_Tsi575_CG_H_ 
#define _TS_Tsi575_CG_H_ 

                                                                               
#ifdef __cplusplus
extern "C" {
#endif

#define Tsi575_CG_NUM_REGS_TO_TEST                                 0x00000009

                                                                               

#ifndef CG_BASE
#define CG_BASE                                                   (0x0001AD00)
#endif


/* ************************************************ */
/* Tsi575 : Register address offset definitions     */
/* ************************************************ */
#define Tsi575_CG_INT_uint32_t                             (CG_BASE + 0x00000000)
#define Tsi575_CG_INT_SET                                (CG_BASE + 0x00000004)
#define Tsi575_CG_INT_CTRL                               (CG_BASE + 0x00000008)
#define Tsi575_CG_PLL0_CTRL0                             (CG_BASE + 0x00000010)
#define Tsi575_CG_PLL0_CTRL1                             (CG_BASE + 0x00000020)
#define Tsi575_CG_IO_CTRL                                (CG_BASE + 0x00000030)
#define Tsi575_CG_RIO_PWRUP_uint32_t                       (CG_BASE + 0x00000034)
#define Tsi575_CG_PWRUP_uint32_t                           (CG_BASE + 0x00000038)
#define Tsi575_CG_PCI_OUT_DLY                            (CG_BASE + 0x00000050)


/* ************************************************ */
/* Tsi575 : Register Bit Masks and Reset Values     */
/*           definitions for every register         */
/* ************************************************ */



/* Tsi575_CG_INT_uint32_t : Register Bits Masks Definitions */
#define Tsi575_CG_INT_uint32_t_R_SLIP                                (0x00000001)
#define Tsi575_CG_INT_uint32_t_FB_SLIP                               (0x00000100)
#define Tsi575_CG_INT_uint32_t_LOL                                   (0x00030000)

/* Tsi575_CG_INT_SET : Register Bits Masks Definitions */
#define Tsi575_CG_INT_SET_R_SLIP_SET                               (0x00000001)
#define Tsi575_CG_INT_SET_FB_SLIP_SET                              (0x00000100)
#define Tsi575_CG_INT_SET_LOL_SET                                  (0x00030000)

/* Tsi575_CG_INT_CTRL : Register Bits Masks Definitions */
#define Tsi575_CG_INT_CTRL_R_SLIP_EN                               (0x00000001)
#define Tsi575_CG_INT_CTRL_FB_SLIP_EN                              (0x00000100)
#define Tsi575_CG_INT_CTRL_LOL_EN                                  (0x00030000)

/* Tsi575_CG_PLL0_CTRL0 : Register Bits Masks Definitions */
#define Tsi575_CG_PLL0_CTRL0_PWRDWN                                (0x40000000)

/* Tsi575_CG_PLL0_CTRL1 : Register Bits Masks Definitions */
#define Tsi575_CG_PLL0_CTRL1_BWADJ                                 (0x000001ff)

/* Tsi575_CG_IO_CTRL : Register Bits Masks Definitions */
#define Tsi575_CG_IO_CTRL_PCI_EN                                   (0x00001f00)
#define Tsi575_CG_IO_CTRL_PCI_VAL                                  (0x001f0000)

/* Tsi575_CG_RIO_PWRUP_uint32_t : Register Bits Masks Definitions */
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP1_PWRDN                       (0x00000002)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP2_PWRDN                       (0x00000004)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP3_PWRDN                       (0x00000008)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP4_PWRDN                       (0x00000010)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP5_PWRDN                       (0x00000020)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP6_PWRDN                       (0x00000040)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP0_MODE                        (0x00010000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP2_MODE                        (0x00020000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP4_MODE                        (0x00040000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP6_MODE                        (0x00080000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_IO_SPEED                     (0x03000000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_REF_CLK                      (0x0c000000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_RX_SWAP                      (0x10000000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_TX_SWAP                      (0x20000000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_HOST                         (0x40000000)
#define Tsi575_CG_RIO_PWRUP_uint32_t_SP_MAST_EN                      (0x80000000)

/* Tsi575_CG_PWRUP_uint32_t : Register Bits Masks Definitions */
#define Tsi575_CG_PWRUP_uint32_t_I2C_SA                              (0x0000007f)
#define Tsi575_CG_PWRUP_uint32_t_I2C_MA                              (0x00000100)
#define Tsi575_CG_PWRUP_uint32_t_I2C_SLAVE                           (0x00000200)
#define Tsi575_CG_PWRUP_uint32_t_I2C_DISABLE                         (0x00000400)
#define Tsi575_CG_PWRUP_uint32_t_I2C_SEL                             (0x00000800)
#define Tsi575_CG_PWRUP_uint32_t_PCI_PLL_BYPASS                      (0x00010000)
#define Tsi575_CG_PWRUP_uint32_t_PCI_M66EN                           (0x00020000)
#define Tsi575_CG_PWRUP_uint32_t_PCI_RSTDIR                          (0x00040000)
#define Tsi575_CG_PWRUP_uint32_t_PCI_ARBEN                           (0x00080000)
#define Tsi575_CG_PWRUP_uint32_t_PCI_HOLD_BOOT                       (0x00100000)
#define Tsi575_CG_PWRUP_uint32_t_VARIANT                             (0x03000000)

/* Tsi575_CG_PCI_OUT_DLY : Register Bits Masks Definitions */
#define Tsi575_CG_PCI_OUT_DLY_PCI_DLY                              (0x00000003)

                                                                               


#ifdef __cplusplus
}
#endif

#endif /* _TS_Tsi575_H_ */

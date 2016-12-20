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
#ifndef __IDT_COMMON_H__
#define __IDT_COMMON_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Controls what device drivers are bound into the DAR 
*/
#define IDT_TSI57X_DAR_WANTED
#define IDT_CPS_GEN2_DAR_WANTED
#define IDT_CPS_GEN1_DAR_WANTED
#define IDT_RXSx_DAR_WANTED


#ifdef IDT_TSI721_SUPPORT
#define IDT_TSI721_DAR_WANTED
#endif

#define IDT_CPS_VENDOR_ID                           0x0038
#define IDT_TSI_VENDOR_ID                           0x000D

#define IDT_CPS8_DEV_ID                             0x035C 
#define IDT_CPS12_DEV_ID                            0x035D
#define IDT_CPS16_DEV_ID                            0x035B
#define IDT_CPS6Q_DEV_ID                            0x035F
#define IDT_CPS10Q_DEV_ID                           0x035E

#define IDT_CPS1848_DEV_ID                          0x0374
#define IDT_CPS1432_DEV_ID                          0x0375
#define IDT_CPS1616_DEV_ID                          0x0379
#define IDT_VPS1616_DEV_ID                          0x0377
#define IDT_SPS1616_DEV_ID                          0x0378

#define IDT_TSI57x_DEV_ID                           0x0570

#define TSI721_RIO_DEVICE_ID                    0x000080AB

#define IDT_RXSx_RIO_DEVICE_ID                  0x000080e0
#define IDT_RXS2448_RIO_DEVICE_ID               0x000080e6
#define IDT_RXS1632_RIO_DEVICE_ID               0x000080e5

#define IDT_TSI_ID_57x                          0x0570000D
#define IDT_CPS_ID_8                            0x035C0038
#define IDT_CPS_ID_12                           0x035D0038
#define IDT_CPS_ID_16                           0x035B0038
#define IDT_CPS_ID_6Q                           0x035F0038
#define IDT_CPS_ID_10Q                          0x035E0038
#define IDT_CPS_ID_1848                         0x03740038
#define IDT_CPS_ID_1432                         0x03750038
#define IDT_CPS_ID_1616                         0x03790038
#define IDT_VPS_ID_1616                         0x03770038
#define IDT_SPS_ID_1616D                        0x03780038

/* Check a device whether it is belong to CPS1 device or not
*/
#define IS_CPS1_DEVICE(dev_info) \
          (   ( VEND_CODE(dev_info) == IDT_CPS_VENDOR_ID) && \
            ( ( DEV_CODE(dev_info) == IDT_CPS8_DEV_ID  ) || \
              ( DEV_CODE(dev_info) == IDT_CPS12_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_CPS16_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_CPS6Q_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_CPS10Q_DEV_ID ) ) )

/* Check a device whether it is belong to CPS2 device or not
*/
#define IS_CPS2_DEVICE(dev_info) \
          (   ( VEND_CODE(dev_info) == IDT_CPS_VENDOR_ID) &&  \
            ( ( DEV_CODE(dev_info) == IDT_CPS1848_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_CPS1432_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_CPS1616_DEV_ID ) || \
              ( DEV_CODE(dev_info) == IDT_SPS1616_DEV_ID ) )  )

/* Maximum number of resources supported by IDT switches.
*/
#define IDT_MAX_PORTS                           DAR_MAX_PORTS
#define IDT_MAX_MCAST                           40
#define IDT_MAX_LANES                           4
#define IDT_MAX_DEV_LANES                       48 
#define IDT_MAX_QUADRANT_PORTS                  5

#define IDT_MAX_PLLS                            12

/* Routine to bind IDT device drivers into the DAR
   Also makes device specific functions available
*/
extern uint32_t IDT_bind_DAR_routines( void );

#ifdef __cplusplus
}
#endif
#endif /* __IDT_COMMON_H__ */

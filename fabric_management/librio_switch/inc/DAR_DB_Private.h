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

#include "rio_standard.h"
#include <DAR_DB.h>
#include <DAR_DevDriver.h>

#ifndef __DAR_DB_PRIVATE_H__
#define __DAR_DB_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Device Access Routines (DAR) Private interface.
*
*  Private interface, contains implementation specific details of the DAR.
*
*  Trust me, you don't want to use this in your implementation of DAR routines.
*/

extern DAR_DB_Driver_t driver_db[DAR_DB_MAX_DRIVERS];

/* Hide these two macros in order to preserve details of
*  the device handle implementation.
*/
#define VALIDATE_DEV_INFO(dev_info) ( (0 != dev_info) \
                 && (   ((dev_info->db_h >> 16) & RIO_DEV_IDENT_VEND)    \
                        == (dev_info->devID       & RIO_DEV_IDENT_VEND)) \
                 && ((DAR_DB_INDEX(dev_info)) < DAR_DB_MAX_DRIVERS) )

#define DAR_DB_INDEX(dev_info) (((dev_info->db_h & 0x0000FFFF) >= DAR_DB_MAX_DRIVERS) \
		                ? (DAR_DB_MAX_DRIVERS - 1):(dev_info->db_h & 0x0000FFFF))

#define VENDOR_ID(dev_info) ((uint16_t)(dev_info->devID & RIO_DEV_IDENT_VEND))

#define DEVICE_ID(dev_info) ((uint16_t)((dev_info->devID & \
                                RIO_DEV_IDENT_DEVI) >> 16))

#define DECODE_VENDOR_ID(device) ((uint16_t)(device & RIO_DEV_IDENT_VEND))

#define DECODE_DEVICE_ID(device) ((uint16_t)((device & RIO_DEV_IDENT_DEVI) >> 16))

#ifdef __cplusplus
}
#endif
#endif /* __DAR_DB_PRIVATE_H__ */


/*
****************************************************************************
opyright (c) 2016, Integrated Device Technology Inc.
Copyright (c) 2016, RapidIO Trade Association
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

#ifndef __IDT_RXS_AP_H__
#define __IDT_RXS_API_H__

#include <DAR_DB.h>
#include <DAR_DB_Private.h>
#include <DAR_DevDriver.h>
#include <IDT_Statistics_Counter_API.h>
#include <IDT_DSF_DB_Private.h>
#include "IDT_Port_Config_API.h"
#include "DAR_Utilities.h"
#include "IDT_Error_Management_API.h"
#include "IDT_RXS2448.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Routine to bind in all RXSs specific DAR support routines.
 * */
uint32_t bind_rxs_DAR_support( void );

/* Routine to bind in all RXSs specific Device Specific Function routines.
 * */
uint32_t bind_rxs_DSF_support( void );


#ifdef __cplusplus
}
#endif

#endif /* __IDT_RXS_API_H__ */

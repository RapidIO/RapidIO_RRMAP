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

// The routines in this file are exposed for testing purposes only.
//
// They are not part of the standard API.
// Generic API code should not use these routines

#ifndef __IDT_TSI57X_API_PRIVATE_H__
#define __IDT_TSI57X_API_PRIVATE_H__

#include <DAR_DB.h>
#include <DAR_DB_Private.h>
#include <DAR_Utilities.h>
#include <IDT_Common.h>
#include <IDT_Port_Config_API.h>
#include <IDT_Routing_Table_Config_API.h>
#include <IDT_Statistics_Counter_API.h>
#include <IDT_DSF_DB_Private.h>
#include <IDT_Tsi57x_API.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OTHER_MAC_PORTS 3

typedef struct port_mac_relations_t_TAG {
    uint8_t    port_num       ;    // Port number
    uint8_t    mac_num        ;    // MAC number associated with configuration for this port.
    uint8_t    first_mac_lane ;    // Port associated with the first lane of the MAC
    uint8_t    lane_count_4x  ;    // If the MAC is in 4x mode, how many lanes?
    uint8_t    lane_count_1x  ;    // If the MAC is in 1x mode, how many lanes?
    uint8_t    pwr_mac_num    ;    // MAC number for power control of this port.
                                 //    usually the same as mac_num, not on Tsi577
    uint32_t   pwr_down_mask  ;    // Either Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X4 or
                                 //    Tsi578_SMACX_DLOOP_CLK_SEL_PWDN_X1
    uint32_t   rst_mask       ;    // Either Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X4 or
                                 //    Tsi578_SMACX_DLOOP_CLK_SEL_SOFT_RST_X1
    uint8_t   other_mac_ports[MAX_OTHER_MAC_PORTS];    
                                 // List of other MAC ports related to this MAC.
                                 //    Terminated with RIO_ALL_PORTS.
} port_mac_relations_t;

// Sets up sw_pmr pointer. 
// Only exposed here for software verification purposes.

uint32_t init_sw_pmr(DAR_DEV_INFO_t        *dev_info, 
                 port_mac_relations_t **sw_pmr   );

struct scrpad_info {
       uint32_t offset;
	uint32_t rw_mask;
};

const struct scrpad_info *get_scrpad_info( void );

#ifdef __cplusplus
}
#endif

#endif /* __IDT_TSI57X_API_PRIVATE_H__ */

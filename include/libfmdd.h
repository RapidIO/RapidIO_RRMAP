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

#ifndef __LIBFMDD_H__
#define __LIBFMDD_H__

/**
 * @file libfmdd.h
 * Fabric Management Daemon Device Directory Library for applications
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include "rio_route.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flag value for absent endpoint
 */
#define FMDD_FLAG_NOK  0x00
/**
 * @brief Flag value for present endpoint
 */
#define FMDD_FLAG_OK   0x01
/**
 * @brief Flag value for endpoint that is the local mport
 */
#define FMDD_FLAG_MP   0x02
/**
 * @brief Flag value for future use (additional apps)
 */
#define FMDD_RSVD_FLAG 0x04
/**
 * @brief Flag value for RSKT Daemon
 */
#define FMDD_RSKT_FLAG 0x08
/**
 * @brief Flag value for RDMA Daemon
 */
#define FMDD_RDMA_FLAG 0x10
/**
 * @brief Flag value for RRMAP CLI connection to FMD
 */
#define FMDD_RRMAP_CLI_FLAG 0x20
/**
 * @brief Flag value for Future application 2
 */
#define FMDD_APP2_FLAG 0x40
/**
 * @brief Flag value for Future application 3
 */
#define FMDD_APP3_FLAG 0x80

/**
 * @brief Flag value to include mports and other devices
 */
#define FMDD_FLAG_OK_MP (FMDD_FLAG_OK | 2)

/**
 * @brief Blank flag value
 */
#define FMDD_NO_FLAG   0x00
/**
 * @brief Any flag value
 */
#define FMDD_ANY_FLAG  0xFF

/**
 * @brief Handle for Fabric Management Device Database access
 */
typedef void *fmdd_h;

/**
 * @brief Get a Fabric Management Device Database handle
 *
 * @param[in] my_name Name of application requesting the handle.
 * @param[in] flag Flag value associated with this application.
 * @return Fabric Management Device Database handle.
 * @retval NULL if the call failed.
 *
 * The flag value passed in will be propagated to all other
 * endpoints, and connected applications will be notified that
 * the flagged application is now available.
 */
fmdd_h fmdd_get_handle(char *my_name, uint8_t flag);

/**
 * @brief Destroy a Fabric Management Device Database handle
 *
 * @param[in] dd_h fmdd_h returned by fmdd_get_handle
 *
 * The application will be removed from the device database, and
 * all connected applications will be notified that the flagged 
 * application is no longer available.
 */
void fmdd_destroy_handle(fmdd_h *dd_h);

/**
 * @brief Checks what flags are associated with a component tag
 *
 * @param[in] h fmdd_h returned by fmdd_get_handle
 * @param[in] ct Component tag identifying the subject device
 * @param[in] flag Flag values to check for
 * @return Flag value bitwise-anded with flag parameter
 * @retval 0 means no requested flags were present
 */

uint8_t fmdd_check_ct(fmdd_h h, ct_t ct, uint8_t flag); /* OK if > 0 */

/**
 * @brief Checks what flags are associated with a device ID
 *
 * @param[in] h fmdd_h returned by fmdd_get_handle
 * @param[in] did_val Device ID identifying the subject device
 * @param[in] flag Flag values to check for
 * @return Flag value bitwise-anded with flag parameter
 * @retval 0 means no requested flags were present
 */
uint8_t fmdd_check_did(fmdd_h h, did_val_t did_val, uint8_t flag); /* OK if > 0 */

/**
 * @brief Blocks until there is a change in the Device Database
 *
 * @param[in] h fmdd_h returned by fmdd_get_handle
 * @return 0 for success, -1 for failure
 */
int fmdd_wait_for_dd_change(fmdd_h h);

/**
 * @brief Gets the list of device IDs now present in the system
 *
 * @param[in] h fmdd_h returned by fmdd_get_handle
 * @param[out] did_list_sz Number of device IDs returned
 * @param[in,out] did_list Pointer to array of device ID values
 * @return 0 for success, -1 for failure
 */
int fmdd_get_did_list(fmdd_h h, uint32_t *did_list_sz, did_val_t **did_list);

/**
 * @brief Frees the list of device IDs allocated by fmdd_get_did_list
 *
 * @param[in] h fmdd_h returned by fmdd_get_handle
 * @param[in,out] did_list Updated pointer to array of device ID values
 * @return 0 for success, -1 for failure
 */
int fmdd_free_did_list(fmdd_h h, did_val_t **did_list);

/**
 * @brief If the application includes libcli, bind available fmdd commands
 */
void fmdd_bind_dbg_cmds(void);

void libfmdd_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LIBFMDD_H__ */

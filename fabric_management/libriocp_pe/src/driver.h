/*
 * Copyright (c) 2014, Prodrive Technologies
 * Copyright (c) 2015, Integrated Device Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __RIOCP_PE_DRIVER_H__
#define __RIOCP_PE_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>

#include "did.h"
#include "riocp_pe.h"
#include "pe.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Callback wrappers */

int RIOCP_WU riocp_drv_init_pe(struct riocp_pe *pe, struct riocp_pe *peer,
		char *name);
int RIOCP_WU riocp_drv_destroy_pe(struct riocp_pe *pe);
int RIOCP_WU riocp_drv_recover_port(struct riocp_pe *pe, uint8_t port,
		uint8_t lp_port);
int RIOCP_WU riocp_drv_get_route_entry(struct riocp_pe *pe, uint8_t port,
		did_t did, pe_rt_val *rt_val);
int RIOCP_WU riocp_drv_set_route_entry(struct riocp_pe *pe, uint8_t port,
		did_t did, pe_rt_val rt_val);
int RIOCP_WU riocp_drv_get_port_state(struct riocp_pe *pe, uint8_t port,
		struct riocp_pe_port_state_t *state);
int RIOCP_WU riocp_drv_reset_port(struct riocp_pe *pe, uint8_t port,
		bool reset_lp);

#ifdef __cplusplus
}
#endif

#endif /* __RIOCP_PE_DRIVER_H__ */

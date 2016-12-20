/*
 * Copyright (c) 2014, Prodrive Technologies
 * Copyright (c) 2015, Integrated Device Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file driver.h
 * RapidIO driver interface for libriocp_pe
 */
#ifndef RIOCP_PE_DRIVER_H__
#define RIOCP_PE_DRIVER_H__

#include <stdbool.h>
#include "riocp_pe.h"

#include "pe.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Callback wrappers */

int RIOCP_WU riocp_drv_init_pe(struct riocp_pe *pe, uint32_t *ct,
				struct riocp_pe *peer, char *name);
int RIOCP_WU riocp_drv_destroy_pe(struct riocp_pe *pe);
int RIOCP_WU riocp_drv_recover_port(struct riocp_pe *pe, uint8_t port, uint8_t lp_port);
int RIOCP_WU riocp_drv_get_route_entry(struct riocp_pe *pe,
                        uint8_t port, uint32_t did, pe_rt_val *rt_val);
int RIOCP_WU riocp_drv_set_route_entry(struct riocp_pe *pe,
                        uint8_t port, uint32_t did, pe_rt_val rt_val);
int RIOCP_WU riocp_drv_get_port_state(struct riocp_pe *pe,
                        uint8_t port, struct riocp_pe_port_state_t *state);
int RIOCP_WU riocp_drv_reset_port(struct riocp_pe *pe, uint8_t port,
								bool reset_lp);

int RIOCP_WU riocp_get_mport_regs(int mp_num, struct mport_regs *regs);

int RIOCP_WU riocp_drv_reg_rd(struct riocp_pe *pe,
                        uint32_t offset, uint32_t *val);
int RIOCP_WU riocp_drv_reg_wr(struct riocp_pe *pe,
                        uint32_t offset, uint32_t val);
int RIOCP_WU riocp_drv_raw_reg_rd(struct riocp_pe *pe, uint32_t did, hc_t hc,
                        uint32_t offset, uint32_t *val);
int RIOCP_WU riocp_drv_raw_reg_wr(struct riocp_pe *pe, uint32_t did, hc_t hc,
                        uint32_t offset, uint32_t val);

/* Driver binding/initialization */

int RIOCP_WU riocp_bind_driver(struct riocp_pe_driver *driver);

#ifdef __cplusplus
}
#endif

#endif /* RIOCP_PE_DRIVER_H__ */

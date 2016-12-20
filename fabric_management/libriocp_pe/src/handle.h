/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file handle.h
 * Processing element handle administration
 */
#ifndef RIOCP_PE_HANDLE_H__
#define RIOCP_PE_HANDLE_H__

#include <stdbool.h>

/*
 * Handles are formed with a 16 bit Unique ID, and 16 bit Destination ID.
 * The handle is written to the standard component tag register of
 * all processing elements.
 */
#include "inc/riocp_pe.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIOCP_PE_HANDLE_FD_UNSET 0

int RIOCP_WU riocp_pe_handle_addr_aton(char *addr, uint8_t **address, size_t *address_len);
const char RIOCP_WU * riocp_pe_handle_addr_ntoa(uint8_t *address, size_t address_len);
int RIOCP_WU riocp_pe_handle_open_mport(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_handle_create_pe(struct riocp_pe *pe, struct riocp_pe **handle,
	hc_t hopcount, uint32_t destid, uint8_t port, ct_t *comptag_in,
	char *name);
int RIOCP_WU riocp_pe_handle_create_mport(uint8_t mport, bool is_host,
	struct riocp_pe **handle, struct riocp_reg_rw_driver *drv,
	ct_t *comptag, char *name);
void riocp_pe_handle_mport_get(struct riocp_pe *mport);
void riocp_pe_handle_mport_put(struct riocp_pe **mport);
int RIOCP_WU riocp_pe_handle_pe_exists(struct riocp_pe *mport, ct_t comptag,
	struct riocp_pe **peer);
int RIOCP_WU riocp_pe_handle_mport_exists(uint8_t mport, bool is_host, struct riocp_pe **pe);

#ifdef __cplusplus
}
#endif

#endif /* RIOCP_PE_HANDLE_H__ */

/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file maint.h
 * Processing element maintainance access using librio_maint
 */
#ifndef RIOCP_PE_MAINT_H__
#define RIOCP_PE_MAINT_H__

#include <sys/types.h>
#include "inc/riocp_pe.h"

#ifdef __cplusplus
extern "C" {
#endif

int RIOCP_SO_ATTR riocp_pe_maint_read(struct riocp_pe *pe, uint32_t offset, uint32_t *val);
int RIOCP_SO_ATTR riocp_pe_maint_write(struct riocp_pe *pe, uint32_t offset, uint32_t val);

/*
int riocp_pe_maint_write_local(struct riocp_pe *mport, uint32_t offset, uint32_t val);
int riocp_pe_maint_read_local(struct riocp_pe *mport, uint32_t offset, uint32_t *val);

int riocp_pe_maint_write_remote(struct riocp_pe *mport, uint32_t destid,
		hc_t hopcount, uint32_t offset, uint32_t val);
int riocp_pe_maint_read_remote(struct riocp_pe *mport, uint32_t destid,
		hc_t hopcount, uint32_t offset, uint32_t *val);
*/


int RIOCP_WU riocp_pe_maint_set_anyid_route(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_maint_unset_anyid_route(struct riocp_pe *pe);

#ifdef __cplusplus
}
#endif

#endif /* RIOCP_PE_MAINT_H__ */

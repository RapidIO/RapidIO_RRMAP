/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __RIOCP_PE_LOCK_H__
#define __RIOCP_PE_LOCK_H__

#include <stdint.h>
#include "rio_route.h"
#include "did.h"

/**
 * @file lock.h
 * Processing element lock functions
 */

#ifdef __cplusplus
extern "C" {
#endif

int riocp_pe_lock_read(struct riocp_pe *pe, did_t did, hc_t hopcount, uint32_t *lock);
int riocp_pe_lock_write(struct riocp_pe *pe, did_t did, hc_t hopcount, uint32_t lock);
int riocp_pe_lock_set(struct riocp_pe *mport, did_t did, hc_t hopcount);
int riocp_pe_lock_clear(struct riocp_pe *mport, did_t did, hc_t hopcount);

#ifdef __cplusplus
}
#endif

#endif /* __RIOCP_PE_LOCK_H__ */

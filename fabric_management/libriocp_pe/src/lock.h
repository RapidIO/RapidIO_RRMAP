/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file lock.h
 * Processing element lock functions
 */
#ifndef RIOCP_PE_LOCK_H__
#define RIOCP_PE_LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

int riocp_pe_lock_read(struct riocp_pe *pe, uint32_t destid, hc_t hopcount, uint32_t *lock);
int riocp_pe_lock_write(struct riocp_pe *pe, uint32_t destid, hc_t hopcount, uint32_t lock);
int riocp_pe_lock_set(struct riocp_pe *mport, uint32_t destid, hc_t hopcount);
int riocp_pe_lock_clear(struct riocp_pe *mport, uint32_t destid, hc_t hopcount);

#ifdef __cplusplus
}
#endif

#endif /* RIOCP_PE_LOCK_H__ */

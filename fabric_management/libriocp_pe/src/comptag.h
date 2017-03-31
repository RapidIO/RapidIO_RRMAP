/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __COMPTAG_H__
#define __COMPTAG_H__

#include "ct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIOCP_PE_COMPTAG_DESTID(ct) ((ct) & 0xffff)

int riocp_pe_comptag_read(struct riocp_pe *pe, ct_t *comptag);
int riocp_pe_comptag_write(struct riocp_pe *pe, ct_t comptag);

#ifdef __cplusplus
}
#endif

#endif /* __COMPTAG_H__ */

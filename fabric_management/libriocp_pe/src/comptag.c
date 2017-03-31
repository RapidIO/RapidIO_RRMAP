/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file comptag.c
 * Processing element component tag functions
 */
#define _XOPEN_SOURCE 500

#include <stdint.h>

#include "ct.h"
#include "rio_standard.h"
#include "inc/riocp_pe_internal.h"
#include "maint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read the component tag
 * @param pe Target PE
 * @param comptag Component tag to write
 * @retval -EIO Could not write to the device
 */
int riocp_pe_comptag_read(struct riocp_pe *pe, ct_t *comptag)
{
	int ret;

	ret = riocp_pe_maint_read(pe, RIO_COMPTAG, comptag);
	if (ret < 0) {
		return ret;
	}
	return 0;
}

/**
 * Write the component tag
 * @param pe Target PE
 * @param comptag Component tag to write
 * @retval -EIO Could not write to the device
 */
int riocp_pe_comptag_write(struct riocp_pe *pe, ct_t comptag)
{
	int ret;

	ret = riocp_pe_maint_write(pe, RIO_COMPTAG, comptag);
	if (ret < 0) {
		return ret;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif


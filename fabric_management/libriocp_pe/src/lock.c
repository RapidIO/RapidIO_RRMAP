/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file lock.c
 * Processing element lock functions
 */
#include <stdint.h>

#include "inc/riocp_pe_internal.h"

#include "rio_standard.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write lock at hopcount, destid. Make sure the route is programmed!
 */
int riocp_pe_lock_read(struct riocp_pe *pe, did_t did, hc_t hopcount,
		uint32_t *lock)
{
	int ret;
	uint32_t _lock;

	ret = riocp_drv_raw_reg_rd(pe, did, hopcount,
	RIO_HOST_LOCK, &_lock);
	if (ret) {
		return -EIO;
	}

	*lock = _lock & RIO_HOST_LOCK_BASE_ID_MASK;
	return 0;
}

/**
 * Read lock at hopcount, destid. Make sure the route is programmed!
 */
int riocp_pe_lock_write(struct riocp_pe *pe, did_t did, hc_t hopcount,
		uint32_t lock)
{
	int ret;

	ret = riocp_drv_raw_reg_wr(pe, did, hopcount,
	RIO_HOST_LOCK, lock);
	if (ret) {
		return -EIO;
	}

	return 0;
}

/**
 * Set the lock: read, verify, write, read and verify
 */
int riocp_pe_lock_set(struct riocp_pe *mport, did_t did, hc_t hopcount)
{
	int ret;
	did_val_t did_val;
	uint32_t lock;

	did_val = did_get_value(did);
	RIOCP_TRACE("Set lock d: %u, h: %u\n", did_val, hopcount);

	ret = riocp_pe_lock_read(mport, did, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	if (lock == mport->did_reg_val) {
		RIOCP_TRACE(
				"Lock already set by mport (d: %u, h: %u, lock: 0x%08x)\n",
				did_val, hopcount, lock);
		return 0;
	}

	ret = riocp_pe_lock_write(mport, did, hopcount, mport->did_reg_val);
	if (ret) {
		RIOCP_ERROR("Unable to write lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	ret = riocp_pe_lock_read(mport, did, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	if (lock == mport->did_reg_val) {
		RIOCP_DEBUG("Lock set d: %u, h: %u, lock: 0x%08x\n", did_val,
				hopcount, lock);
		return 0;
	}

	return -EAGAIN;
}

/**
 * Clear the lock: write, read and verify
 */
int riocp_pe_lock_clear(struct riocp_pe *mport, did_t did, hc_t hopcount)
{
	int ret;
	did_val_t did_val;
	uint32_t lock;

	did_val = did_get_value(did);
	RIOCP_TRACE("Clear lock d: %u, h: %u\n", did_val, hopcount);

	ret = riocp_pe_lock_read(mport, did, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	RIOCP_DEBUG("Lock set to 0x%08x (d: %u, h: %u)\n", lock, did_val,
			hopcount);

	if (lock == RIO_HOST_LOCK_BASE_ID_MASK) {
		return 0;
	}

	ret = riocp_pe_lock_write(mport, did, hopcount, lock);
	if (ret) {
		RIOCP_ERROR("Unable to write lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	ret = riocp_pe_lock_read(mport, did, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n", did_val,
				hopcount);
		return -EIO;
	}

	RIOCP_DEBUG("New lock value 0x%08x (d: %u, h: %u)\n", lock, did_val,
			hopcount);

	return 0;
}

#ifdef __cplusplus
}
#endif

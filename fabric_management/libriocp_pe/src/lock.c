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
int riocp_pe_lock_read(struct riocp_pe *pe, uint32_t destid, hc_t hopcount, uint32_t *lock)
{
	int ret;
	uint32_t _lock;

	ret = riocp_drv_raw_reg_rd(pe, destid, hopcount,
						RIO_HOST_LOCK, &_lock);
	if (ret)
		return -EIO;

	*lock = _lock & RIO_HOST_LOCK_BASE_ID_MASK;

	return 0;
}

/**
 * Read lock at hopcount, destid. Make sure the route is programmed!
 */
int riocp_pe_lock_write(struct riocp_pe *pe, uint32_t destid, hc_t hopcount, uint32_t lock)
{
	int ret;

	ret = riocp_drv_raw_reg_wr(pe, destid, hopcount,
						RIO_HOST_LOCK, lock);
	if (ret)
		return -EIO;

	return 0;
}

/**
 * Set the lock: read, verify, write, read and verify
 */
int riocp_pe_lock_set(struct riocp_pe *mport, uint32_t destid, hc_t hopcount)
{
	int ret;
	uint32_t lock;

	RIOCP_TRACE("Set lock d: %u, h: %u\n", destid, hopcount);

	ret = riocp_pe_lock_read(mport, destid, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	if (lock == mport->destid) {
		RIOCP_TRACE("Lock already set by mport (d: %u, h: %u, lock: 0x%08x)\n",
			destid, hopcount, lock);
		return 0;
	}

	ret = riocp_pe_lock_write(mport, destid, hopcount, mport->destid);
	if (ret) {
		RIOCP_ERROR("Unable to write lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	ret = riocp_pe_lock_read(mport, destid, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	if (lock == mport->destid) {
		RIOCP_DEBUG("Lock set d: %u, h: %u, lock: 0x%08x\n",
			destid, hopcount, lock);
		return 0;
	}

	return -EAGAIN;
}

/**
 * Clear the lock: write, read and verify
 */
int riocp_pe_lock_clear(struct riocp_pe *mport, uint32_t destid, hc_t hopcount)
{
	int ret;
	uint32_t lock;

	RIOCP_TRACE("Clear lock d: %u, h: %u\n", destid, hopcount);

	ret = riocp_pe_lock_read(mport, destid, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	RIOCP_DEBUG("Lock set to 0x%08x (d: %u, h: %u)\n",
		lock, destid, hopcount);

	if (lock == RIO_HOST_LOCK_BASE_ID_MASK)
		return 0;

	ret = riocp_pe_lock_write(mport, destid, hopcount, lock);
	if (ret) {
		RIOCP_ERROR("Unable to write lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	ret = riocp_pe_lock_read(mport, destid, hopcount, &lock);
	if (ret) {
		RIOCP_ERROR("Unable to read lock d: %u, h: %u\n",
			destid, hopcount);
		return -EIO;
	}

	RIOCP_DEBUG("New lock value 0x%08x (d: %u, h: %u)\n",
		lock, destid, hopcount);

	return 0;
}

#ifdef __cplusplus
}
#endif

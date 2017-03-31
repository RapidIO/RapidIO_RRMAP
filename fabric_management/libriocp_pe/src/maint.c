/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file maint.c
 * Processing element maintenance access using librio_maint
 */
#define _XOPEN_SOURCE 500

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

#include <rapidio_mport_mgmt.h>

#include "inc/riocp_pe.h"
#include "inc/riocp_pe_internal.h"

#include "rio_route.h"
#include "rio_standard.h"
#include "lock.h"
#include "maint.h"
#include "handle.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Program the ANY_ID route from hopcount 0 to pe->hopcount in the global switch LUT
 *  it will program according to route in variable pe->address.
 * @note Keep in mind that this function will set the locks of the path!
 * @param pe Target PE
 * @retval 0 When read/write was successfull or skipped
 * @retval -EIO When read/write was unsuccessfull
 */
int riocp_pe_maint_set_anyid_route(struct riocp_pe *pe)
{
	int32_t i;
	int ret = 0;
	struct riocp_pe *ith_pe = pe->mport->peers[0].peer;
	struct riocp_pe *pes[HC_MP];

	if (!RIOCP_PE_IS_HOST(pe))
		return 0;

	/* If the ANY_ID is already programmed for this pe, skip it */
	if (pe == pe->mport->minfo->any_id_target)
		return 0;

	RIOCP_TRACE("Programming ANY_ID route to PE 0x%08x\n", pe->comptag);

	/* Write ANY_ID route until pe */
	for (i = 0; i < pe->hopcount; i++) {
		pes[i] = ith_pe;

		ret = riocp_pe_lock_set(ith_pe->mport, DID_ANY_DEV8_ID, i);
		if (ret) {
			RIOCP_TRACE("Could not set lock at hopcount %u\n", i);
			ret = -EIO;
			goto err;
		}

		ret = riocp_drv_set_route_entry(ith_pe, RIOCP_PE_ALL_PE_PORTS,
				DID_ANY_DEV8_ID, pe->address[i]);

		RIOCP_TRACE("switch[hop: %d] DID_ANY_DEV8_ID -> port %d programmed\n",
				i, pe->address[i]);
		if (i + 1 < pe->hopcount) {
			ith_pe = ith_pe->peers[pe->address[i]].peer;
		}
	}

	pe->mport->minfo->any_id_target = pe;

	RIOCP_TRACE("Programming ANY_ID route to PE 0x%08x successfull\n",
			pe->comptag);

	return ret;

err:
	/* Write DID_ANY_DEV8_ID route until pe */
	for (; i >= 0; i--) {
		if (riocp_pe_lock_clear(pes[i], DID_ANY_DEV8_ID, i)) {
			RIOCP_TRACE("Could not clear lock at hopcount %u\n", i);
			goto fail;
		}
	}

fail:
	pe->mport->minfo->any_id_target = NULL;
	RIOCP_TRACE("Error in programming ANY_ID route\n");
	return ret;
}

int RIOCP_WU riocp_pe_maint_set_route(struct riocp_pe *pe, did_t did, pe_port_t pnum)
{
	int32_t i;
	struct riocp_pe *ith_pe = pe->mport->peers[0].peer;
	did_val_t did_val;
	int ret = 0;

	if (!RIOCP_PE_IS_HOST(pe)) {
		return 0;
	}

	if (RIOCP_PE_IS_MPORT(pe)) {
		return 0;
	}

	/* Write destid route */
	did_val = did_get_value(did);
	for (i = 0; i < pe->hopcount; i++) {
		ret = riocp_drv_set_route_entry(ith_pe, RIOCP_PE_ALL_PE_PORTS,
				did, pe->address[i]);
		if (ret) {
			goto err;
		}

		RIOCP_TRACE("switch[hop: %d] %d -> port %d programmed\n", i,
				did_val, pe->address[i]);
		if (i + 1 < pe->hopcount) {
			ith_pe = ith_pe->peers[pe->address[i]].peer;
		}
	}

	ret = riocp_drv_set_route_entry(pe, RIOCP_PE_ALL_PE_PORTS, did, pnum);
	if (ret) {
		goto err;
	}

	RIOCP_TRACE("Programming did 0x%08x route to PE 0x%08x successfull\n",
			did_val, pe->comptag);

	return ret;

err:
	RIOCP_TRACE("Error in programming did 0x%08x route\n", did_val);
	return ret;
}

/**
 * Clear the ANY_ID route locks from pe->hopcount - 1 to 0
 * @note Keep in mind that this function will clear the locks of the path in reverse order!
 * @param pe Target PE
 * @retval 0 When read/write was successfull or skipped
 * @retval -EIO When read/write was unsuccessfull
 */

int riocp_pe_maint_unset_anyid_route(struct riocp_pe *pe)
{
	int32_t i;
	int ret = 0;
	struct riocp_pe *ith_pe = pe->mport->peers[0].peer;
	struct riocp_pe *pes[HC_MP];

	if (!RIOCP_PE_IS_HOST(pe))
		return 0;

	if (RIOCP_PE_IS_MPORT(pe))
		return 0;

	/* If the ANY_ID is already programmed for this pe, skip it */
	if (pe->mport->minfo->any_id_target == NULL)
		return 0;

	RIOCP_TRACE("Unset DID_ANY_DEV8_ID route locks to PE 0x%08x\n", pe->comptag);

	for (i = 0; i < pe->hopcount; i++) {
		pes[i] = ith_pe;
		if (((hc_t)i) + 1 < pe->hopcount)
			ith_pe = ith_pe->peers[pe->address[i]].peer;
	}

	/* Write ANY_ID route until pe */
	for (i = pe->hopcount - 1; i >= 0; i--) {

		ret = riocp_pe_lock_clear(pes[i], DID_ANY_DEV8_ID, (hc_t)i);
		if (ret) {
			RIOCP_TRACE("Could not clear lock at hopcount %u\n", i);
			ret = -EIO;
			goto err;
		}
	}

	pe->mport->minfo->any_id_target = NULL;

	RIOCP_TRACE("Unset DID_ANY_DEV8_ID route to PE 0x%08x successfull\n",
			pe->comptag);

	return ret;

err:
	pe->mport->minfo->any_id_target = NULL;
	RIOCP_TRACE("Error in unset DID_ANY_DEV8_ID route\n");
	return ret;
}

/**
 * Maintenance read from local (when mport) or remote device
 * @note  When writing to the remote PE the ANY_ID rioid is always used and not the pe->destid
 * @param pe     Target PE
 * @param offset Offset in the RapidIO maintenance address space
 * @param val    Value of register
 */
int RIOCP_SO_ATTR riocp_pe_maint_read(struct riocp_pe *pe, uint32_t offset, uint32_t *val)
{
	int ret;

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_ERROR("Handle invalid\n");
		return ret;
	}

	if (RIOCP_PE_IS_MPORT(pe)) {
		ret = riocp_drv_reg_rd(pe, offset, val);
		if (ret)
			return -EIO;
	} else {
		/* Program and lock ANY_ID route */
		ret = riocp_pe_maint_set_anyid_route(pe);
		if (ret) {
			RIOCP_ERROR("Could not program ANY_ID to pe: %s\n", strerror(-ret));
			return -EIO;
		}

		ret = riocp_drv_reg_rd(pe, offset, val);
		if (ret) {
			RIOCP_ERROR("Read remote error device %s err %d\n",
				pe->sysfs_name, ret);
			return -EIO;
		}

		RIOCP_TRACE("Read remote ok %s o: %x\n",
			pe->sysfs_name, offset);

		/* Unlock ANY_ID route */
		ret = riocp_pe_maint_unset_anyid_route(pe);
		if (ret) {
			RIOCP_ERROR("Could unset ANY_ID route to pe: %s\n",
				strerror(-ret));
			return -EIO;
		}
	}

	return ret;
}

/**
 * Maintenance write to local (when mport) or remote device
 * @note  When writing to the remote PE the ANY_ID rioid is always used and not the pe->destid
 * @param pe     Target PE
 * @param offset Offset in the RapidIO maintenance address space
 * @param val    Value to write register
 */
int RIOCP_SO_ATTR riocp_pe_maint_write(struct riocp_pe *pe, uint32_t offset, uint32_t val)
{
	int ret;

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_ERROR("Handle invalid\n");
		return ret;
	}

	if (RIOCP_PE_IS_MPORT(pe)) {
		ret = riocp_drv_reg_wr(pe, offset, val);
		if (ret)
			return -EIO;
	} else {
		/* Program and lock ANY_ID route */
		ret = riocp_pe_maint_set_anyid_route(pe);
		if (ret) {
			RIOCP_ERROR("Could not program ANY_ID to pe: %s\n", strerror(-ret));
			return -EIO;
		}

		RIOCP_TRACE("Write %s o: 0x%08x, v: 0x%08x\n",
			pe->sysfs_name, offset, val);

		ret = riocp_drv_reg_wr(pe, offset, val);
		if (ret) {
			RIOCP_ERROR("Write returned error: %s %s\n",
				pe->sysfs_name, strerror(-ret));
			return -EIO;
		}

		/* Unlock ANY_ID route */
		ret = riocp_pe_maint_unset_anyid_route(pe);
		if (ret) {
			RIOCP_ERROR("Could unset ANY_ID route to pe: %s\n", strerror(-ret));
			return -EIO;
		}
	}

	return ret;
}

#ifdef __cplusplus
}
#endif

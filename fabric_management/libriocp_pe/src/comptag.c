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

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <stdlib.h>
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "inc/riocp_pe_internal.h"

#include "rio_standard.h"
#include "maint.h"
#include "comptag.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIOCP_PE_COMPTAG_POOL_REALLOC_SIZE 256 /**< Amount of elements to grow the pool */

/**
 * Grow the size of component tag pool with amount of elements
 * @param pe Target PE
 */
static int riocp_pe_comptag_grow_pool(struct riocp_pe *pe, size_t size)
{
	size_t i;
	size_t old_size;
	size_t new_size;

	old_size = pe->mport->minfo->comptag_pool_size;
	new_size = old_size + size;

	RIOCP_TRACE("old_size: %zu, new_size: %zu\n",
		old_size, new_size);

	if (new_size >= UINT16_MAX)
		return -E2BIG;

	pe->mport->minfo->comptag_pool = (struct riocp_pe **)realloc(
			pe->mport->minfo->comptag_pool,
			new_size * sizeof(struct riocp_pe *));

	if (pe->mport->minfo->comptag_pool == NULL) {
		pe->mport->minfo->comptag_pool_size = 0;
		RIOCP_ERROR("Error in realloc, pool is NULL");
		return -ENOMEM;
	}

	for (i = old_size; i < new_size; i++) {
		RIOCP_TRACE("comptag_pool[%u] = NULL\n", i);
		pe->mport->minfo->comptag_pool[i] = NULL;
	}

	pe->mport->minfo->comptag_pool_size = new_size;

	RIOCP_TRACE("realloc to new_size %u succeeded\n",
		new_size);

	return 0;
}

/**
 * Grow to new size in blocks of RIOCP_PE_COMPTAG_POOL_REALLOC_SIZE
 * @param Target PE
 * @param new_size New size of pool
 */
static int riocp_pe_comptag_grow_pool_size(struct riocp_pe *pe, size_t new_size)
{
	size_t old_size;
	size_t grow_size;

	old_size  = pe->mport->minfo->comptag_pool_size;
	grow_size = ((new_size - old_size) / RIOCP_PE_COMPTAG_POOL_REALLOC_SIZE) + 1;
	grow_size *= RIOCP_PE_COMPTAG_POOL_REALLOC_SIZE;

	RIOCP_TRACE("old_size: %zu, new_size: %zu, grow_size: %zu\n",
		old_size, new_size, grow_size);

	return riocp_pe_comptag_grow_pool(pe, grow_size);
}

/**
 * Get next free unique component tag number from pool
 * @param pe Target PE
 * @param comptag_nr Component tag unique number
 * @retval 0 Saved PE handle address to comptag_pool slot at comptag_nr
 * @retval -ENOENT No free slot found in pool
 */
static int riocp_pe_comptag_get_free_slot(struct riocp_pe *pe, ct_nr_t *comptag_nr)
{
	unsigned int i;
	ct_nr_t _comptag_nr;

	if (pe->mport->minfo->comptag_pool_size == 0)
		return -ENOENT;

	/* Start at slot 1, slot 0 is reserved */
	for (i = 1; i < pe->mport->minfo->comptag_pool_size; i++) {
		if (pe->mport->minfo->comptag_pool[i] == NULL) {
			_comptag_nr = i;
			goto found;
		}
	}

	return -ENOENT;

found:
	*comptag_nr = _comptag_nr;
	RIOCP_TRACE("Got new comptag for PE: pe(%p), comptag_nr(%u, 0x%08x)\n",
		pe, _comptag_nr, _comptag_nr);
	return 0;
}

/**
 * Set PE at slot
 * @param pe Target PE
 * @param comptag_nr Component tag unique number
 * @retval 0 Saved PE handle address to comptag_pool slot at comptag_nr
 * @retval -ENOENT Slot already taken
 */
int riocp_pe_comptag_set_slot(struct riocp_pe *pe, ct_nr_t comptag_nr)
{
	int ret;

	RIOCP_TRACE("Set PE at slot %u\n", comptag_nr);

	if (comptag_nr >= pe->mport->minfo->comptag_pool_size) {
		ret = riocp_pe_comptag_grow_pool_size(pe, comptag_nr);
		if (ret)
			return ret;
	}

	if (pe == pe->mport->minfo->comptag_pool[comptag_nr]) {
		RIOCP_WARN("PE %u already set\n", comptag_nr);
		return 0;
	}

	if (pe->mport->minfo->comptag_pool[comptag_nr] != NULL) {
		RIOCP_ERROR("Slot %u already taken\n", comptag_nr);
		return -ENOENT;
	}

	pe->mport->minfo->comptag_pool[comptag_nr] = pe;

	RIOCP_TRACE("Set PE at slot %u successfull\n", comptag_nr);

	return 0;
}

/**
 * Get PE from slot
 * @param pe Target PE
 * @param comptag_nr Component tag unique number
 * @retval 0 Saved PE handle address to comptag_pool slot at comptag_nr
 * @retval -ENOENT Slot already taken
 */
int riocp_pe_comptag_get_slot(struct riocp_pe *mport, ct_nr_t comptag_nr, struct riocp_pe **pe)
{
	struct riocp_pe *_pe;

	RIOCP_TRACE("Get PE at slot %u\n", comptag_nr);

	if (comptag_nr >= mport->minfo->comptag_pool_size)
		return -ENOMEM;

	_pe = mport->minfo->comptag_pool[comptag_nr];
	if (_pe == NULL)
		return -ENOENT;

	*pe = _pe;
	RIOCP_TRACE("PE 0x%08x at slot %u\n", _pe->comptag, comptag_nr);
	return 0;
}

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
	if (ret < 0)
		return ret;

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
	if (ret < 0)
		return ret;

	return riocp_pe_comptag_set_slot(pe, RIOCP_PE_COMPTAG_GET_NR(comptag));
}

/**
 * Set next unique component tag (from mport->comptag_pool) for PE
 *  Write the component tag, and set the handle.
 * @param pe Target PE
 * @retval -ENODEV When component tag could not be written
 */
int riocp_pe_comptag_init(struct riocp_pe *pe)
{
	ct_t comptag;
	ct_nr_t _comptag_nr;
	int ret = 0;

	RIOCP_TRACE("Initialise PE comptag\n");

	ret = riocp_pe_comptag_get_free_slot(pe, &_comptag_nr);
	if (ret) {
		ret = riocp_pe_comptag_grow_pool(pe, RIOCP_PE_COMPTAG_POOL_REALLOC_SIZE);
		if (ret)
			return ret;

		ret = riocp_pe_comptag_get_free_slot(pe, &_comptag_nr);
		if (ret) {
			RIOCP_ERROR("Could not get free slot after pool resize");
			return ret;
		}
	}

	comptag = RIOCP_PE_COMPTAG_DESTID(pe->mport->destid);
	comptag |= RIOCP_PE_COMPTAG_NR(_comptag_nr);

	ret = riocp_pe_comptag_write(pe, comptag);
	if (ret) {
		RIOCP_ERROR("Could not write comptag\n");
		return ret;
	}

	ret = riocp_pe_comptag_set_slot(pe, _comptag_nr);
	if (ret) {
		RIOCP_ERROR("Could not set PE at slot %u\n", _comptag_nr);
		return ret;
	}

	pe->comptag = comptag;

	return 0;
}

#ifdef __cplusplus
}
#endif


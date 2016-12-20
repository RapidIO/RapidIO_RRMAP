/*
 ****************************************************************************
 Copyright (c) 2016, Integrated Device Technology Inc.
 Copyright (c) 2016, RapidIO Trade Association
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "did_test.h"
#include "rio_standard.h"

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

did_sz_t did_ids[RIO_LAST_DEV16];
uint32_t did_idx = 0;

/**
 * Convert an integer representation of the device Id, from a config file for example,
 * to the appropriate device Id size
 * @param[out] size the size of the device Id
 * @param[in[ asInt the integer representation of the size from a config file
 * @retval -EINVAL the size is NULL or out of range
 * @retval -EPERM the operation is not supported
 */
int did_size_from_int(did_sz_t *size, uint32_t asInt)
{
	if (NULL == size) {
		return -EINVAL;
	}

	switch (asInt) {
	case 0:
		*size = dev08_sz;
		return 0;
	case 1:
		*size = dev16_sz;
		return 0;
	case 2:
		*size = dev32_sz;
		return -EINVAL;
	default:
		*size = invld_sz;
		return -EINVAL;
	}
}

/**
 * Create a device Id for the specified size
 * @param[out] did the device Id
 * @param[in] size the device Id size
 * @retval 0 the device Id is valid
 * @retval -EINVAL the provided did reference is NULL
 * @retval -EPERM the operation is not supported
 * @retval -ENOBUFS there were no device Ids available
 */
int did_create(did_t *did, did_sz_t size)
{
	uint32_t i;
	uint32_t upb;

	// lazy initialization
	if (0 == did_idx) {
		did_ids[0] = dev08_sz;
		did_ids[255] = dev08_sz;
		did_idx = 1;
	}

	if (NULL == did) {
		return -EINVAL;
	}

	switch (size) {
	case dev08_sz:
		upb = RIO_LAST_DEV8;
		break;
	case dev16_sz:
		upb = RIO_LAST_DEV16;
		break;
	default:
		// only 8 and 16 bit DIDs are supported
		*did = DID_INVALID_ID;
		return -EPERM;
	}

	// find the next available did from the last used position
	for (i = did_idx; i < upb; i++) {
		if (invld_sz == did_ids[i]) {
			did_idx = i;
			goto found;
		}
	}

	// look for a free did from the beginning of the structure
	for (i = 1; i < did_idx; i++) {
		if (invld_sz == did_ids[i]) {
			did_idx = i;
			goto found;
		}
	}

	// no available did_ids
	*did = DID_INVALID_ID;
	return -ENOBUFS;

found:
	// return current, then incr to next available
	did_ids[did_idx] = size;

	did->value = did_idx++;
	did->size = size;
	return 0;
}

/**
 * Create a device Id for the specified value and size
 * @param[out] did the device Id
 * @param[in] value the device Id value
 * @param[in] size the device Id size
 * @retval 0 the device Id is set
 * @retval -EINVAL the specified device Id is out of range
 * @retval -EPERM the operation is not supported
 * @retval -ENOTUNIQ the specified device Id was already in use
 */
int did_create_from_data(did_t *did, did_val_t value, did_sz_t size)
{
	// lazy initialization
	if (0 == did_idx) {
		did_ids[0] = dev08_sz;
		did_ids[255] = dev08_sz;
		did_idx = 1;
	}

	if (NULL == did) {
		return -EINVAL;
	}

	if ((0 == value) || (255 == value)) {
		*did = DID_INVALID_ID;
		return -EINVAL;
	}

	switch (size) {
	case dev08_sz:
		if (value > RIO_LAST_DEV8 - 1) {
			*did = DID_INVALID_ID;
			return -EINVAL;
		}
		break;
	case dev16_sz:
		if (value > RIO_LAST_DEV16 - 1) {
			*did = DID_INVALID_ID;
			return -EINVAL;
		}
		break;
	default:
		// only 8 and 16 bit DIDs are supported
		*did = DID_INVALID_ID;
		return -EPERM;
	}

	if (invld_sz == did_ids[value]) {
		// do not update the index
		did_ids[value] = size;
		did->value = value;
		did->size = size;
		return 0;
	}

	*did = DID_INVALID_ID;
	return -ENOTUNIQ;
}

/**
 * Release a device Id
 * @param did the device Id
 * @retval 0 the device Id is released
 * @retval -EINVAL the specified device Id is out of range
 * @retval -EPERM the operation is not supported
 * @retval -EKEYEXPIRED the provided device Id was not in use
 */
int did_release(did_t did)
{
	did_val_t idx = did.value;
	if ((0 == idx) || (255 == idx)) {
		return -EINVAL;
	}

	switch (did.size) {
	case dev08_sz:
		if (idx > (RIO_LAST_DEV8 - 1)) {
			return -EINVAL;
		}
		break;
	case dev16_sz:
		if (idx > (RIO_LAST_DEV16 - 1)) {
			return -EINVAL;
		}
		break;
	default:
		// only 8 and 16 bit DIDs are supported
		return -EPERM;
	}

	if (invld_sz == did_ids[idx]) {
		return -EKEYEXPIRED;
	}

	if (did.size != did_ids[idx]) {
		return -EINVAL;
	}

	did_ids[idx] = invld_sz;
	return 0;
}

/**
 * Get an existing device Id for the specified value and size
 * @param[out] did the device Id
 * @param[in] value the device Id value to get
 * @param[in] size the device Id size
 * @retval 0 the device Id exists
 * @retval -EINVAL the specified device Id is out of range
 * @retval -EPERM the operation is not supported
 * @retval -EKEYEXPIRED the specified device Id is not in use
 */
int did_get(did_t *did, did_val_t value, did_sz_t size)
{
	if (NULL == did) {
		return -EINVAL;
	}

	if ((0 == value) || (RIO_LAST_DEV8 == value)) {
		*did = DID_INVALID_ID;
		return -EINVAL;
	}

	switch (size) {
	case dev08_sz:
		if (value > (RIO_LAST_DEV8 - 1)) {
			*did = DID_INVALID_ID;
			return -EINVAL;
		}
		break;
	case dev16_sz:
		if (value > (RIO_LAST_DEV16 - 1)) {
			*did = DID_INVALID_ID;
			return -EINVAL;
		}
		break;
	default:
		// only 8 and 16 bit DIDs are supported
		*did = DID_INVALID_ID;
		return -EPERM;
	}

	if (size == did_ids[value]) {
		did->value = value;
		did->size = size;
		return 0;
	}

	*did = DID_INVALID_ID;
	return -EKEYEXPIRED;
}

/**
 * Check if a device Id is in use
 * @param did the device Id
 * @retval 0 if the device Id is not in use
 * @retval 1 if the device Id is in use
 * @retval -EINVAL the specified device Id is out of range
 * @retval -EPERM the operation is not supported
 */
int did_not_inuse(did_t did)
{
	if ((0 == did.value) || (RIO_LAST_DEV8 == did.value)) {
		return -EINVAL;
	}

	switch (did.size) {
	case dev08_sz:
		if (did.value > (RIO_LAST_DEV8 - 1)) {
			return -EINVAL;
		}
		break;
	case dev16_sz:
		if (did.value > (RIO_LAST_DEV16 - 1)) {
			return -EINVAL;
		}
		break;
	default:
		// only 8 and 16 bit DIDs are supported
		return -EPERM;
	}
	return (did.size == did_ids[did.value] ? 1 : 0);
}

/**
 * Return the value of the device Id
 * @param did the device Id
 * @retval the value of the device Id
 */
did_val_t did_get_value(did_t did)
{
	return did.value;
}

/**
 * Return the size of the device Id
 * @param did the device Id
 * @retval the size of the device Id
 */
did_sz_t did_get_size(did_t did)
{
	return did.size;
}

#ifdef UNIT_TESTING
void did_reset()
{
	memset(did_ids, 0, sizeof(did_ids));
	did_idx = 0;
}

/**
 * Determine if the provided did is as expected
 *
 * @param[in] did the did to be tested
 * @param[in] value the expected value
 * @param[in] size the expected size
 *
 * @retval 0 the did is as expected
 * @retval 1 the value is not as expected
 * @retval 2 the size is not as expected
 */

int did_equal(did_t did, did_val_t value, did_sz_t size)
{
	if (value != did.value) {
		return 1;
	}
	if (size != did.size) {
		return 2;
	}
	return 0;
}

/**
 * Determine if the provided did is invalid
 *
 * @param[in] did the did to be tested
 *
 * @retval 0 the did is invalid
 * @retval 1 the value is not equal
 * @retval 2 the size is not equal
 */
int did_invalid(did_t did)
{
	return did_equal(did, 0, invld_sz);
}
#endif

#ifdef __cplusplus
}
#endif

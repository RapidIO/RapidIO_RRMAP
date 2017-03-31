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

#include "ct_test.h"
#include "rio_standard.h"

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 1 - UINT16_MAX are available for use */
#define NUMBER_OF_CTS (1 << 16)
#define CT_NR_MASK (0x0ffff0000)
#define CT_DID_MASK (0x0000ffff)

/**
 * The nr value may have multiple DIDs associated with it. Once the last DID associated
 * with the nr value is released the nr is not available for re-use.
 *
 * a value of -1 indicates the nr is not available for use, they are not recycled
 * a value 0f 0 indicates the nr is available for use
 * a value of 1 or greater indicates the nr is in use, and may have other dids associated with it
 */
int ct_ids[NUMBER_OF_CTS];
uint32_t ct_idx = 0;

#define CT_FROM_NR_DID(n,d) (((n << 16) & CT_NR_MASK) | (CT_DID_MASK & d))

static void initialize()
{
	ct_ids[0] = -1;
	ct_idx = 1;
}

/**
 * Find the next free number.  Note that this routine does not reserve the
 * number - it is up to the calling routine to reserve the number and update
 * ct_idx, as necessary.
 *
 * @returns The next valid component tag number.
 * @retval 0 Invalid component tag number, failure
 * @retval -ENOBUFS There were no component tags available
 */
int ct_next_nr(ct_nr_t *nr)
{
	uint32_t i;

	// find the next available nr from the last used position
	for (i = ct_idx; i < NUMBER_OF_CTS; i++) {
		if (0 == ct_ids[i]) {
			*nr = (ct_nr_t)i;
			goto found;
		}
	}

	// look for a free nr from the beginning of the structure
	for (i = 1; i < ct_idx; i++) {
		if (0 == ct_ids[i]) {
			*nr = (ct_nr_t)i;
			goto found;
		}
	}

	return -ENOBUFS;

found:
	return 0;
}

/**
 * Create a component tag and device Id for the specified device Id size.
 *
 * @param[out] ct the component tag
 * @param[out] did the device Id associated with the component tag
 * @param[in] size the size of the device Id
 * @retval 0 the component tag and device Id are valid
 * @retval -EINVAL the provided ct or did reference is NULL, or no dids available
 * @retval -EPERM the operation is not supported
 * @retval -ENOBUFS there were no component tags available
 */
int ct_create_all(ct_t *ct, did_t *did, did_sz_t size)
{
	ct_nr_t nr;
	int rc;

	// lazy initialization
	if (0 == ct_idx) {
		// yes this will initialize whenever the nr value loops - no harm done
		initialize();
	}

	if ((NULL == ct) || (NULL == did)) {
		if (NULL != ct) {
			*ct = COMPTAG_UNSET;
		}
		if (NULL != did) {
			*did = DID_INVALID_ID;
		}
		return -EINVAL;
	}

	rc = ct_next_nr(&nr);
	if (rc) {
		*ct = COMPTAG_UNSET;
		return rc;
	}

	// did created last so you don't need to release it if the nr call failed
	rc = did_create(did, size);
	if (rc) {
		// no available dids
		*ct = COMPTAG_UNSET;
		return rc;
	}

	// return current, increment to next available
	*ct = CT_FROM_NR_DID(nr, did_get_value(*did));
	ct_ids[nr]++;
	ct_idx = ++nr;
	return 0;
}

/**
 * Create a component tag and device Id from the specified nr and did values
 *
 * @param[out] ct the component tag
 * @param[out] did the device Id associated with the component tag
 * @param[in] nr the index value of the component tag
 * @param[in] did_value the device Id value of the component tag
 * @param[in] did_size the size of the device Id
 * @retval 0 the component tag is valid
 * @retval -EINVAL invalid parameter values
 * @retval -ENOTUNIQ the specified component tag was already in use
 */
int ct_create_from_data(ct_t *ct, did_t *did, ct_nr_t nr, did_val_t did_value,
		did_sz_t did_size)
{
	int rc;

	// lazy initialization
	if (0 == ct_idx) {
		// yes this will initialize whenever the nr value loops - no harm done
		initialize();
	}

	if ((NULL == ct) || (NULL == did)) {
		if (NULL != ct) {
			*ct = COMPTAG_UNSET;
		}
		if (NULL != did) {
			*did = DID_INVALID_ID;
		}
		return -EINVAL;
	}

	rc = did_create_from_data(did, did_value, did_size);
	if (rc) {
		*ct = COMPTAG_UNSET;
		return rc;
	}

	rc = ct_create_from_nr_and_did(ct, nr, *did);
	if (rc) {
		*ct = COMPTAG_UNSET;
		did_release(*did);
		*did = DID_INVALID_ID;
	}
	return rc;

}

/**
 * Create a component tag from the specified nr and previously created did
 *
 * @param[out] ct the component tag
 * @param[in] nr the index value of the component tag
 * @param[in] did the device Id of the component tag
 * @retval 0 the component tag is valid
 * @retval -EINVAL invalid parameter values
 * @retval -ENOTUNIQ the specified component tag was already in use
 * @retval -EKEYEXPIRED the nr component may not be re-used
 */
int ct_create_from_nr_and_did(ct_t *ct, ct_nr_t nr, did_t did)
{
	int rc;
	did_t cached_did;

	// lazy initialization
	if (0 == ct_idx) {
		// yes this will initialize whenever the nr value loops - no harm done
		initialize();
	}

	if (NULL == ct) {
		return -EINVAL;
	}

	if (-1 == ct_ids[nr]) {
		*ct = COMPTAG_UNSET;
		return -EKEYEXPIRED;
	}

	rc = did_get(&cached_did, did_get_value(did));
	if (rc) {
		*ct = COMPTAG_UNSET;
		return rc;
	}

	// do not incr ct_idx
	*ct = CT_FROM_NR_DID(nr, did_get_value(did));
	ct_ids[nr]++;
	return 0;
}

/**
 * Create a component tag for the previously created did
 *
 * @param[out] ct the component tag
 * @param[in] did the device Id for the component tag
 * @retval 0 the component tag is valid
 * @retval -EINVAL invalid parameter values
 * @retval -ENOBUFS No more CTs available
 */
int ct_create_from_did(ct_t *ct, did_t did)
{
	ct_nr_t nr;
	did_t cached_did;
	int rc;

	// lazy initialization
	if (0 == ct_idx) {
		// yes this will initialize whenever the nr value loops - no harm done
		initialize();
	}

	if (NULL == ct) {
		return -EINVAL;
	}

	rc = ct_next_nr(&nr);
	if (rc) {
		*ct = COMPTAG_UNSET;
		return rc;
	}

	// verify the did is valid
	rc = did_get(&cached_did, did_get_value(did));
	if (rc) {
		*ct = COMPTAG_UNSET;
		return rc;
	}

	*ct = CT_FROM_NR_DID(nr, did_get_value(did));
	ct_ids[nr]++;
	ct_idx = ++nr;
	return 0;
}

/*
 * Create or get a component tag and device Id as transported via messaging
 * between master and slave.
 *
 * @param[out] ct the component tag
 * @param[in] ct_val the component tag value
 * @retval 0 the ct was created or exists
 * @retval -EINVAL invalid parameter values
 * @retval -EPERM the operation is not supported
 *
 * Note value and size have must be corrected for network order prior to calling
 * this function.
 */
int ct_from_value(ct_t *ct, uint32_t ct_val)
{
	ct_nr_t nr;
	did_val_t value;
	did_sz_t size;
	did_t cached_did;
	int rc;
	uint32_t incr;
	uint32_t sz;

	// lazy initialization
	if (0 == ct_idx) {
		// yes this will initialize whenever the nr value loops - no harm done
		initialize();
	}

	if (NULL == ct) {
		return -EINVAL;
	}

	// if already created, don't account for it again.
	value = (did_val_t)(CT_DID_MASK & ct_val);
	size = value > DID_ANY_DEV8_ID.value ? dev16_sz : dev08_sz;

	cached_did = (did_t) {value, size};
	if (did_not_inuse(cached_did)) {
		incr = 1;
	} else {
		sz = (size == dev16_sz ? 1 : 0);
		rc = did_from_value(&cached_did, value, sz);
		if (rc) {
			*ct = COMPTAG_UNSET;
			return rc;
		}
		incr = 0;
	}

	*ct = ct_val;
	if (incr) {
		nr = CT_NR_MASK & ct_val;
		ct_ids[nr]++;
	}
	return 0;
}

/**
 * Release the provided component tag and the associated device Id
 *
 * @param[out] ct the component tag to be released
 * @param[in] did the device Id associated with the ct
 * @retval 0 the component tag is valid
 * @retval -EINVAL invalid parameter values
 */
int ct_release(ct_t ct, did_t did)
{
	int rc;
	ct_nr_t nr;

	rc = ct_get_nr(&nr, ct);
	if (rc) {
		return rc;
	}

	if (ct_ids[nr] < 1) {
		return -EINVAL;
	}

	rc = did_release(did);
	if (rc) {
		return rc;
	}

	// when the last did is released, do not recycle the nr
	if (1 == ct_ids[nr]) {
		ct_ids[nr] = -1;
	} else {
		ct_ids[nr]--;
	}
	return 0;
}

/**
 * Return the nr of the component tag
 *
 * @param[out] nr the nr
 * @param[in] ct the component tag
 * @retval 0 the nr is valid
 * @retval -EINVAL nr is invalid
 */
int ct_get_nr(ct_nr_t *nr, ct_t ct)
{
	if (NULL == nr) {
		return -EINVAL;
	}

	*nr = (ct_nr_t)(0x0000ffff & (ct >> 16));
	return (0 == *nr ? -EINVAL : 0);
}

/**
 * Return the device Id of the component tag
 * @param[out] did the device Id
 * @param[in] ct the component tag
 * @retval 0 the did is valid
 * @retval -EINVAL the did is null or the did of the provided ct is not in use
 */
int ct_get_destid(did_t *did, ct_t ct)
{
	return did_get(did, (CT_DID_MASK & ct));
}

/**
 * Determine if the specified component tag is in use.
 * @param[in] ct the component tag
 * @retval 0 if the ct is not in use
 * @retval 1 if the ct is in use
 * @retval -EINVAL nr or did size is invalid
 * @retval -EPERM the operation is not supported
 *
 * \note A component tag with an nr value of 0 is considered not in use
 */
int ct_not_inuse(ct_t ct)
{
	ct_nr_t nr;
	did_t did;
	int rc;

	rc = ct_get_nr(&nr, ct);
	if (rc) {
		return rc;
	}

	if (ct_ids[nr] > 0) {
		return (did_get(&did, ct & CT_DID_MASK) ? 0 : 1);
	}
	return 0;

}

#ifdef UNIT_TESTING
void ct_reset()
{
	memset(ct_ids, 0, sizeof(ct_ids));
	initialize();
}
#endif

#ifdef __cplusplus
}
#endif

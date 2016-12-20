/*
 ****************************************************************************
 Copyright (c) 2016, Integrated Device Technology Inc.
 Copyright (c) 2016, RapidIO Trade Association
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 l of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this l of conditions and the following disclaimer in the documentation
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"

#include "did_test.h"
#include "src/did.c"
#include "rio_standard.h"

#ifdef __cplusplus
extern "C" {
#endif

void assumptions(void **state)
{
	// ensure enumerated values are correct
	assert_int_equal(4, sizeof(did_sz_t));
	assert_int_equal(0, invld_sz);
	assert_int_equal(8, dev08_sz);
	assert_int_equal(16, dev16_sz);
	assert_int_equal(32, dev32_sz);

	// although it appears a little bogus, duplicate the internal
	// structures for testing. ensure they remain the same and other
	// assumptions in subsequent tests are valid
	struct dup_did_t {
		did_val_t value;
		did_sz_t size;
	};
	assert_int_equal(sizeof(struct dup_did_t), sizeof(did_t));

	// verify constants
	assert_int_equal(0xff, ANY_ID);
	assert_int_equal(0,
			did_equal(DID_ANY_DEV8_ID, RIO_LAST_DEV8, dev08_sz));
	assert_int_equal(0,
			did_equal(DID_ANY_DEV16_ID, RIO_LAST_DEV16, dev16_sz));
	assert_int_equal(0, did_equal(DID_INVALID_ID, 0, invld_sz));

	// assuming this is the first test ever ran, then can verify the internal did structures
	assert_int_equal(0, did_idx);
	assert_int_equal(RIO_LAST_DEV16, sizeof(did_ids) / sizeof(did_ids[0]));
	assert_int_equal(0, did_ids[0]);
	assert_int_equal(0, did_ids[100]);
	assert_int_equal(0, did_ids[RIO_LAST_DEV16 - 1]);

	(void)state; // unused
}

void did_size_from_int_null_parms_test(void **state)
{
	int i;
	for (i = 0; i < (dev32_sz + 5); i++) {
		assert_int_equal(-EINVAL, did_size_from_int(NULL, i));
	}

	(void)state; // unused
}

void did_size_from_int_test(void **state)
{
	int rc;
	did_sz_t size;
	int i;

	// only 8 and 16 bit dids are supported
	for (i = 0; i < (dev32_sz + 5); i++) {
		rc = did_size_from_int(&size, i);
		switch (i) {
		case 0:
			assert_int_equal(0, rc);
			assert_int_equal(dev08_sz, size);
			break;
		case 1:
			assert_int_equal(0, rc);
			assert_int_equal(dev16_sz, size);
			break;
		case 2:
			assert_int_equal(-EINVAL, rc);
			assert_int_equal(dev32_sz, size);
			break;

		default:
			assert_int_equal(-EINVAL, rc);
			assert_int_equal(invld_sz, size);
		}
	}

	(void)state; // unused
}

void did_create_null_parms_test(void **state)
{
	// would be nice to iterate over the enum, but not contiguous
	// since it is test code, just hit the values
	assert_int_equal(-EINVAL, did_create(NULL, invld_sz));
	assert_int_equal(-EINVAL, did_create(NULL, dev08_sz));
	assert_int_equal(-EINVAL, did_create(NULL, dev16_sz));
	assert_int_equal(-EINVAL, did_create(NULL, dev32_sz));

	(void)state; // unused
}

void did_create_test(void **state)
{
	did_t did;

	did_reset();

	// initialize the return structure, and verify it has been
	// correctly updated

	// invalid request (size parm)
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(-EPERM, did_create(&did, invld_sz));
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, did_idx); // first call reset the did_idx

	// 8-bit did request
	did.size = dev16_sz;
	did.value = 0x12;
	assert_int_equal(invld_sz, did_ids[1]);
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	assert_int_equal(2, did_idx);
	assert_int_equal(dev08_sz, did_ids[1]);

	// 16-bit did request
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(invld_sz, did_ids[2]);
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, 2, dev16_sz));
	assert_int_equal(3, did_idx);
	assert_int_equal(dev16_sz, did_ids[2]);

	// invalid request (size parm)
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(invld_sz, did_ids[3]);
	assert_int_equal(-EPERM, did_create(&did, dev32_sz));
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(3, did_idx);
	assert_int_equal(invld_sz, did_ids[3]);

	// reset internal structures
	did_reset();

	// verify lazy initialization
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(0, did_idx);
	assert_int_equal(invld_sz, did_ids[0]);
	assert_int_equal(invld_sz, did_ids[RIO_LAST_DEV8]);
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(dev08_sz, did_ids[0]);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8]);

	// verify the 16-bit did requested
	assert_int_equal(0, did_equal(did, 1, dev16_sz));
	assert_int_equal(2, did_idx);
	assert_int_equal(dev16_sz, did_ids[1]);

	// reset internal structures
	did_reset();

	// verify wrap around behaviour
	// request 8-bit did at beginning of structure
	did.size = dev16_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	assert_int_equal(2, did_idx);
	assert_int_equal(dev08_sz, did_ids[1]);

	// 8-bit boundary condition - move the internal index first
	did.size = dev16_sz;
	did.value = 0x12;
	did_idx = RIO_LAST_DEV8 - 2;
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV8 - 2, dev08_sz));
	assert_int_equal(RIO_LAST_DEV8-1, did_idx);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8-2]);

	did.size = dev16_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV8 - 1, dev08_sz));
	assert_int_equal(RIO_LAST_DEV8, did_idx);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8-1]);

	// next 8-bit did should wrap
	did.size = dev16_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, 2, dev08_sz));
	assert_int_equal(3, did_idx);
	assert_int_equal(dev08_sz, did_ids[2]);

	// next 16-bit did should follow the previously created 8-bit did
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, 3, dev16_sz));
	assert_int_equal(4, did_idx);
	assert_int_equal(dev16_sz, did_ids[3]);

	// valid 16-bit did at 8-bit did boundary - move the internal index
	did.size = dev08_sz;
	did.value = 0x12;
	did_idx = RIO_LAST_DEV8 - 1;
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV8+1, dev16_sz));
	assert_int_equal(RIO_LAST_DEV8+2, did_idx);
	assert_int_equal(dev16_sz, did_ids[RIO_LAST_DEV8+1]);

	// 16-bit boundary condition - move the internal index first
	did.size = dev08_sz;
	did.value = 0x12;
	did_idx = RIO_LAST_DEV16 - 2;
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV16-2, dev16_sz));
	assert_int_equal(RIO_LAST_DEV16-1, did_idx);
	assert_int_equal(dev16_sz, did_ids[RIO_LAST_DEV16-2]);

	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV16-1, dev16_sz));
	assert_int_equal(RIO_LAST_DEV16, did_idx);
	assert_int_equal(dev16_sz, did_ids[RIO_LAST_DEV16-1]);

	// next 16-bit did should wrap
	did.size = dev08_sz;
	did.value = 0x12;
	assert_int_equal(0, did_create(&did, dev16_sz));
	assert_int_equal(0, did_equal(did, 4, dev16_sz));
	assert_int_equal(5, did_idx);
	assert_int_equal(dev16_sz, did_ids[4]);

	// verify final structure
	assert_int_equal(dev08_sz, did_ids[0]);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8]);

	assert_int_equal(dev08_sz, did_ids[1]);
	assert_int_equal(invld_sz, did_ids[RIO_LAST_DEV8-3]);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8-2]);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8-1]);
	assert_int_equal(dev08_sz, did_ids[2]);
	assert_int_equal(dev16_sz, did_ids[3]);
	assert_int_equal(dev16_sz, did_ids[4]);
	assert_int_equal(invld_sz, did_ids[5]);
	assert_int_equal(invld_sz, did_ids[RIO_LAST_DEV16-3]);
	assert_int_equal(dev16_sz, did_ids[RIO_LAST_DEV16-2]);
	assert_int_equal(dev16_sz, did_ids[RIO_LAST_DEV16-2]);

	(void)state; // unused
}

void did_create_from_data_null_parms_test(void **state)
{
	// would be nice to iterate over the enum, but not contiguous
	// since it is test code, just hit the values
	assert_int_equal(-EINVAL, did_create_from_data(NULL, 0x12, invld_sz));
	assert_int_equal(-EINVAL, did_create_from_data(NULL, 0x34, dev08_sz));
	assert_int_equal(-EINVAL, did_create_from_data(NULL, 0x56, dev16_sz));
	assert_int_equal(-EINVAL, did_create_from_data(NULL, 0x787, dev32_sz));

	(void)state; // unused
}

void did_create_from_data_test(void **state)
{
	did_t did;

	// the internal did_idx should not change with these calls
	did_reset();
	assert_int_equal(0, did_idx);

	// 0 is reserved
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, did_create_from_data(&did, 0, dev08_sz));
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, did_idx); // first call reset the did_idx

	// 255 is reserved
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL,
			did_create_from_data(&did, RIO_LAST_DEV8, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// invalid size
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EPERM, did_create_from_data(&did, 0xca, invld_sz));
	assert_int_equal(0, did_invalid(did));

	// invalid size
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EPERM, did_create_from_data(&did, 0xad, dev32_sz));
	assert_int_equal(0, did_invalid(did));

	// valid 8-bit size, invalid value
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL,
			did_create_from_data(&did, RIO_LAST_DEV8 + 5, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// valid 16-bit size, invalid value
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL,
			did_create_from_data(&did, RIO_LAST_DEV16 + 2, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	assert_int_equal(1, did_idx);

	// valid values, first try
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(0, did_create_from_data(&did, 123, dev16_sz));
	assert_int_equal(0, did_equal(did, 123, dev16_sz));
	assert_int_equal(dev16_sz, did_ids[123]);

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(0, did_create_from_data(&did, 321, dev16_sz));
	assert_int_equal(0, did_equal(did, 321, dev16_sz));
	assert_int_equal(dev16_sz, did_ids[321]);

	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(0, did_create_from_data(&did, 21, dev08_sz));
	assert_int_equal(0, did_equal(did, 21, dev08_sz));
	assert_int_equal(dev08_sz, did_ids[21]);

	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(0, did_create_from_data(&did, 1, dev08_sz));
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	assert_int_equal(dev08_sz, did_ids[1]);

	assert_int_equal(1, did_idx);

	// repeat a value
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-ENOTUNIQ, did_create_from_data(&did, 1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-ENOTUNIQ, did_create_from_data(&did, 321, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	// verify final structure
	assert_int_equal(dev08_sz, did_ids[0]);
	assert_int_equal(dev08_sz, did_ids[RIO_LAST_DEV8]);

	assert_int_equal(dev08_sz, did_ids[1]);
	assert_int_equal(dev08_sz, did_ids[21]);
	assert_int_equal(dev16_sz, did_ids[123]);
	assert_int_equal(dev16_sz, did_ids[321]);
	assert_int_equal(invld_sz, did_ids[5]); // random verification
	assert_int_equal(invld_sz, did_ids[RIO_LAST_DEV16-3]); // random verification

	(void)state; // unused
}

void did_get_null_parms_test(void **state)
{
	// would be nice to iterate over the enum, but not contiguous
	// since it is test code, just hit the values
	assert_int_equal(-EINVAL, did_get(NULL, 0x01, invld_sz));
	assert_int_equal(-EINVAL, did_get(NULL, 0x02, dev08_sz));
	assert_int_equal(-EINVAL, did_get(NULL, 0x03, dev16_sz));
	assert_int_equal(-EINVAL, did_get(NULL, 0x04, dev32_sz));

	(void)state; // unused
}

void did_get_test(void **state)
{
	did_t did;

	did_reset();
	assert_int_equal(0, did_idx);

	// 0 is an invalid request
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, did_get(&did, 0, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, did_get(&did, 0, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	// 255 is an invalid request
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, did_get(&did, RIO_LAST_DEV8, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, did_get(&did, RIO_LAST_DEV8, dev32_sz));
	assert_int_equal(0, did_invalid(did));

	// invalid size
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EPERM, did_get(&did, 0xca, invld_sz));
	assert_int_equal(0, did_invalid(did));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EPERM, did_get(&did, 0xca, dev32_sz));
	assert_int_equal(0, did_invalid(did));

	// value out of range
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, did_get(&did, RIO_LAST_DEV8+1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, did_get(&did, RIO_LAST_DEV16, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	// look for it, add it, look for it again
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EKEYEXPIRED, did_get(&did, 1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	assert_int_equal(0, did_create_from_data(&did, 1, dev16_sz));
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(0, did_get(&did, 1, dev16_sz));
	assert_int_equal(0, did_equal(did, 1, dev16_sz));

	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EKEYEXPIRED, did_get(&did, 1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// look for it, add it, look for it again
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EKEYEXPIRED,
			did_get(&did, RIO_LAST_DEV8-1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	assert_int_equal(0,
			did_create_from_data(&did, RIO_LAST_DEV8-1, dev08_sz));
	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(0, did_get(&did, RIO_LAST_DEV8-1, dev08_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV8-1, dev08_sz));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EKEYEXPIRED,
			did_get(&did, RIO_LAST_DEV8-1, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	// look for it, add it, look for it again
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EKEYEXPIRED,
			did_get(&did, RIO_LAST_DEV16-1, dev16_sz));
	assert_int_equal(0, did_invalid(did));

	assert_int_equal(0,
			did_create_from_data(&did, RIO_LAST_DEV16-1, dev16_sz));
	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(0, did_get(&did, RIO_LAST_DEV16-1, dev16_sz));
	assert_int_equal(0, did_equal(did, RIO_LAST_DEV16-1, dev16_sz));

	did.value = 0x12;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, did_get(&did, RIO_LAST_DEV16-1, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	(void)state; // unused
}

void did_release_test(void **state)
{
	did_t did;

	did_reset();

	// invalid size
	did.value = 0x12;
	did.size = invld_sz;
	assert_int_equal(-EPERM, did_release(did));

	did.size = dev32_sz;
	assert_int_equal(-EPERM, did_release(did));

	// 0 is reserved - cannot be released
	did.size = dev08_sz;
	did.value = 0;
	assert_int_equal(-EINVAL, did_release(did));

	// 255 is reserved - cannot be released
	did.value = RIO_LAST_DEV8;
	assert_int_equal(-EINVAL, did_release(did));

	// 8-bit did, out of range
	did.value = RIO_LAST_DEV8 + 1;
	assert_int_equal(-EINVAL, did_release(did));

	// 16-bit did, out of range
	did.size = dev16_sz;
	did.value = RIO_LAST_DEV16;
	assert_int_equal(-EINVAL, did_release(did));

	// not in use
	did.size = dev08_sz;
	did.value = 1;
	assert_int_equal(-EKEYEXPIRED, did_release(did));

	did.size = dev16_sz;
	did.value = 0xcaca;
	assert_int_equal(-EKEYEXPIRED, did_release(did));

	// create an 8-bit did
	assert_int_equal(0, did_create_from_data(&did, 0xca, dev08_sz));

	// release a 16-bit did with the same value
	did.value = 0xca;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, did_release(did));

	// release the 8-bit did
	did.size = dev08_sz;
	assert_int_equal(0, did_equal(did, 0xca, dev08_sz));
	assert_int_equal(0, did_release(did));

	// try and release it again
	assert_int_equal(0, did_equal(did, 0xca, dev08_sz));
	assert_int_equal(-EKEYEXPIRED, did_release(did));

	// create a 16-bit did
	assert_int_equal(0, did_create_from_data(&did, 0xf0, dev16_sz));

	// release 8-bit did with same value
	did.value = 0xf0;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, did_release(did));

	// release the 16-bit did
	did.size = dev16_sz;
	assert_int_equal(0, did_equal(did, 0xf0, dev16_sz));
	assert_int_equal(0, did_release(did));

	// try and release it again
	assert_int_equal(0, did_equal(did, 0xf0, dev16_sz));
	assert_int_equal(-EKEYEXPIRED, did_release(did));

	(void)state; // unused
}

void did_not_inuse_test(void **state)
{
	did_t did;

	did_reset();

	// invalid size
	did.value = 0x12;
	did.size = invld_sz;
	assert_int_equal(-EPERM, did_not_inuse(did));

	did.size = dev32_sz;
	assert_int_equal(-EPERM, did_not_inuse(did));

	// 0 is reserved - considered not in use
	did.size = dev08_sz;
	did.value = 0;
	assert_int_equal(-EINVAL, did_not_inuse(did));

	// 255 is reserved - considered not in use
	did.value = RIO_LAST_DEV8;
	assert_int_equal(-EINVAL, did_not_inuse(did));

	// valid 8-bit size, invalid value
	did.value = RIO_LAST_DEV8 + 1;
	assert_int_equal(-EINVAL, did_not_inuse(did));

	// valid 16-bit size, invalid value
	did.size = dev16_sz;
	did.value = RIO_LAST_DEV16;
	assert_int_equal(-EINVAL, did_not_inuse(did));

	// valid 8-bit size and value, not in use
	did.size = dev08_sz;
	did.value = 0x1;
	assert_int_equal(0, did_not_inuse(did));

	// valid 16-bit size and value, not in use
	did.size = dev16_sz;
	did.value = 0xcaca;
	assert_int_equal(0, did_not_inuse(did));

	// in use - create an 8-bit and 16-bit did, verify in use
	assert_int_equal(0, did_create_from_data(&did, 1, dev08_sz));
	assert_int_equal(1, did_not_inuse(did));
	assert_int_equal(0, did_create_from_data(&did, 0xcaca, dev16_sz));
	assert_int_equal(1, did_not_inuse(did));

	(void)state; // unused
}

void did_get_value_test(void **state)
{
	did_t did;

	// not actually creating them, just populate a structure with the correct
	// values and query
	did.size = dev16_sz;
	did.value = 0xcafe;
	assert_int_equal(0xcafe, did_get_value(did));

	did.size = dev16_sz;
	did.value = 0xbabe;
	assert_int_equal(0xbabe, did_get_value(did));

	did.size = dev16_sz;
	did.value = 0xdead;
	assert_int_equal(0xdead, did_get_value(did));

	did.size = dev16_sz;
	did.value = 0xbeef;
	assert_int_equal(0xbeef, did_get_value(did));

	// size doesn't matter
	did.size = invld_sz;
	did.value = 0xcafe;
	assert_int_equal(0xcafe, did_get_value(did));

	did.size = dev08_sz;
	did.value = 0xbabe;
	assert_int_equal(0xbabe, did_get_value(did));

	did.size = dev16_sz;
	did.value = 0xdead;
	assert_int_equal(0xdead, did_get_value(did));

	did.size = dev32_sz;
	did.value = 0xbeef;
	assert_int_equal(0xbeef, did_get_value(did));

	(void)state; // unused
}

void did_get_size_test(void **state)
{
	did_t did;

	// not actually creating them, just populate a structure with the correct
	// values and query
	did.value = 0xf0;
	did.size = invld_sz;
	assert_int_equal(invld_sz, did_get_size(did));

	did.value = 0xf0;
	did.size = dev08_sz;
	assert_int_equal(dev08_sz, did_get_size(did));

	did.value = 0xf0;
	did.size = dev16_sz;
	assert_int_equal(dev16_sz, did_get_size(did));

	did.value = 0xf0;
	did.size = dev32_sz;
	assert_int_equal(dev32_sz, did_get_size(did));

	// value doesn't matter
	did.value = 0xcafe;
	did.size = invld_sz;
	assert_int_equal(invld_sz, did_get_size(did));

	did.value = 0xbabe;
	did.size = dev08_sz;
	assert_int_equal(dev08_sz, did_get_size(did));

	did.value = RIO_LAST_DEV16 + 27;
	did.size = dev16_sz;
	assert_int_equal(dev16_sz, did_get_size(did));

	did.value = 0xdeadbeef;
	did.size = dev32_sz;
	assert_int_equal(dev32_sz, did_get_size(did));
	(void)state; // unused
}

int main(int argc, char** argv)
{
	(void)argv; // not used
	argc++; // not used

	const struct CMUnitTest tests[] = {
	cmocka_unit_test(assumptions),
	cmocka_unit_test(did_size_from_int_null_parms_test),
	cmocka_unit_test(did_size_from_int_test),
	cmocka_unit_test(did_create_null_parms_test),
	cmocka_unit_test(did_create_test),
	cmocka_unit_test(did_create_from_data_null_parms_test),
	cmocka_unit_test(did_create_from_data_test),
	cmocka_unit_test(did_get_null_parms_test),
	cmocka_unit_test(did_get_test),
	cmocka_unit_test(did_release_test),
	cmocka_unit_test(did_not_inuse_test),
	cmocka_unit_test(did_get_value_test),
	cmocka_unit_test(did_get_size_test)};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

#ifdef __cplusplus
}
#endif

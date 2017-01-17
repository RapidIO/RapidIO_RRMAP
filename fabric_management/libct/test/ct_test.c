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

#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"

#include "did_test.h"
#include "src/ct.c"

#ifdef __cplusplus
extern "C" {
#endif

void assumptions(void **state)
{
	// verify constants
	assert_int_equal(0, COMPTAG_UNSET);
	assert_int_equal(1 << 16, NUMBER_OF_CTS);
	assert_int_equal(0xffff0000, CT_NR_MASK);
	assert_int_equal(0x0000ffff, CT_DID_MASK);

	ct_reset();
	assert_int_equal(1 << 16, sizeof(ct_ids) / sizeof(ct_ids[0]));
	assert_int_equal(1, ct_idx);
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(0, ct_ids[1]);
	assert_int_equal(0, ct_ids[101]);
	assert_int_equal(0, ct_ids[(1 << 16)-1]);

	(void)state; // unused
}

void macros_test(void **state)
{
	// the macro doesn't care if you give it values out of range
	// technically not true, it will die if a value > 32 bits is
	// provided as the NR value
	assert_int_equal(0xcafebabe, CT_FROM_NR_DID(0xcafe, 0xbabe));
	assert_int_equal(0xdeadbeef, CT_FROM_NR_DID(0xdead, 0xbeef));
	assert_int_equal(0x5a5aa5a5, CT_FROM_NR_DID(0x5a5a, 0xa5a5));
	assert_int_equal(0xa5a55a5a, CT_FROM_NR_DID(0xa5a5, 0x5a5a));
	assert_int_equal(0x5a5aa5a5, CT_FROM_NR_DID((uint64_t)0x5a5a5a, 0xa5a5a5));
	assert_int_equal(0xa5a55a5a, CT_FROM_NR_DID((uint64_t)0xa5a5a5, 0x5a5a5a));
	assert_int_equal(0xcafebabe, CT_FROM_NR_DID(0xdeadcafe, 0xdeadbabe));

	(void)state; // unused
}

void ct_next_nr_test(void **state)
{
	ct_nr_t nr;

	// ct_idx doesn't change in the called procedure, nor
	// does it make the ct_idx as in use

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// first available nr at startup
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(1, nr);
	ct_ids[ct_idx++]++;

	// next
	assert_int_equal(2, ct_idx);
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(2, nr);
	ct_ids[ct_idx++]++;

	// leave a gap
	ct_idx = 100;
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(100, nr);
	ct_ids[ct_idx++]++;

	assert_int_equal(101, ct_idx);
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(101, nr);
	ct_ids[ct_idx++]++;

	// boundary condition
	ct_idx = NUMBER_OF_CTS - 1;
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(NUMBER_OF_CTS-1, nr);
	ct_ids[ct_idx++]++;

	// next would be at position 3
	assert_int_equal(0, ct_next_nr(&nr));
	assert_int_equal(3, nr);
	ct_idx = 3;
	ct_ids[ct_idx++]++;
	assert_int_equal(4, ct_idx);

	assert_int_equal(0, ct_next_nr(&nr));
	ct_ids[ct_idx++]++;
	assert_int_equal(4, nr);

	// overall state
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_ids[1]);
	assert_int_equal(1, ct_ids[2]);
	assert_int_equal(1, ct_ids[3]);
	assert_int_equal(1, ct_ids[4]);
	assert_int_equal(0, ct_ids[5]);
	assert_int_equal(0, ct_ids[99]);
	assert_int_equal(1, ct_ids[100]);
	assert_int_equal(1, ct_ids[101]);
	assert_int_equal(0, ct_ids[102]);
	assert_int_equal(0, ct_ids[NUMBER_OF_CTS-2]);
	assert_int_equal(1, ct_ids[NUMBER_OF_CTS-1]);

	(void)state; // unused
}
void ct_create_all_test_null_parms(void **state)
{
	ct_t ct;
	did_t did;

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	assert_int_equal(-EINVAL, ct_create_all(NULL, NULL, dev08_sz));

	// did is updated
	assert_int_equal(-EINVAL, ct_create_all(NULL, &did, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// ct is updated
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EINVAL, ct_create_all(&ct, NULL, dev08_sz));
	assert_int_equal(COMPTAG_UNSET, ct);

	(void)state; // unused
}

void ct_create_all_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	// valid 8-bit did request
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, ct_create_all(&ct, &did, dev08_sz));
	assert_int_equal(0x00010001, ct);
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	assert_int_equal(2, ct_idx);
	assert_int_equal(1, ct_ids[1]);

	// valid 16-bit did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_all(&ct, &did, dev16_sz));
	assert_int_equal(0x00020002, ct);
	assert_int_equal(0, did_equal(did, 2, dev16_sz));
	assert_int_equal(3, ct_idx);
	assert_int_equal(1, ct_ids[2]);

	// invalid size
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EPERM, ct_create_all(&ct, &did, dev32_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(3, ct_idx);
	assert_int_equal(0, ct_ids[3]);

	// invalid size
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EPERM, ct_create_all(&ct, &did, invld_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(3, ct_idx);
	assert_int_equal(0, ct_ids[3]);

	// rollover - move the index
	ct_idx = NUMBER_OF_CTS - 1;

	// valid 8-bit did request at upper bound
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_all(&ct, &did, dev08_sz));
	assert_int_equal(0xffff0003, ct);
	assert_int_equal(0, did_equal(did, 3, dev08_sz));
	assert_int_equal(0, ct_idx);
	assert_int_equal(1, ct_ids[0xffff]);

	// valid 8-bit did request, roll over to next available position
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_all(&ct, &did, dev08_sz));
	assert_int_equal(0x00030004, ct);
	assert_int_equal(0, did_equal(did, 4, dev08_sz));
	assert_int_equal(4, ct_idx);
	assert_int_equal(1, ct_ids[3]);

	// validate internal structure
	assert_int_equal(4, ct_idx);
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_ids[1]);
	assert_int_equal(1, ct_ids[2]);
	assert_int_equal(1, ct_ids[3]);
	assert_int_equal(0, ct_ids[4]);
	assert_int_equal(0, ct_ids[0xfffe]);
	assert_int_equal(1, ct_ids[0xffff]);

	(void)state; // unused
}

void ct_create_from_data_test_null_parms(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	assert_int_equal(-EINVAL,
			ct_create_from_data(NULL, &did, 0, 0xcafe, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_invalid(did));

	assert_int_equal(-EINVAL,
			ct_create_from_data(&ct, NULL, 0, 0xcafe, dev08_sz));
	assert_int_equal(COMPTAG_UNSET, ct);

	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EINVAL,
			ct_create_from_data(NULL, NULL, 0, 0xcafe, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);

	(void)state; // unused
}

void ct_create_from_data_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	// valid 8-bit request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x100, 0xca, dev08_sz));
	assert_int_equal(0x010000ca, ct);
	assert_int_equal(0, did_equal(did, 0xca, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x100]);

	// valid 16-bit request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x200, 0xbabe,
					dev16_sz));
	assert_int_equal(0x0200babe, ct);
	assert_int_equal(0, did_equal(did, 0xbabe, dev16_sz));
	assert_int_equal(1, ct_idx);

	// invalid size
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EPERM,
			ct_create_from_data(&ct, &did, 0x300, 0xcafe,
					dev32_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, ct_ids[0x300]);

	// valid 16-bit request, previously used nr for 8-bit request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x100, 0xcafe,
					dev16_sz));
	assert_int_equal(0x0100cafe, ct);
	assert_int_equal(0, did_equal(did, 0xcafe, dev16_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(2, ct_ids[0x100]);

	// rollover
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);

	// valid 8-bit request at upper bounds
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0xffff, 0x03, dev08_sz));
	assert_int_equal(0xffff0003, ct);
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, did_equal(did, 0x03, dev08_sz));
	assert_int_equal(1, ct_ids[0xffff]);

	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);

	// valid 8-bit request rolled over
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x03, 0x04, dev08_sz));
	assert_int_equal(0x00030004, ct);
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, did_equal(did, 0x04, dev08_sz));
	assert_int_equal(1, ct_ids[3]);

	// previous used nr with a previously used 8-bit did value, orig was same 8-bit value
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-ENOTUNIQ,
			ct_create_from_data(&ct, &did, 0x100, 0xca, dev08_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, ct_idx);
	assert_int_equal(2, ct_ids[0x100]);

	// previously used nr with a previously used 16-bit did value, orig was 8-bit value
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-ENOTUNIQ,
			ct_create_from_data(&ct, &did, 0x100, 0xbabe,
					dev16_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x200]);

	// previously used nr with a new 8-bit did value, orig was 16-bit value
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x200, 0xa1, dev08_sz));
	assert_int_equal(0x020000a1, ct);
	assert_int_equal(0, did_equal(did, 0xa1, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(2, ct_ids[0x200]);

	// previously used nr with a previously used 16-bit did value, orig was 16-bit value
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-ENOTUNIQ,
			ct_create_from_data(&ct, &did, 0x200, 0xcafe,
					dev16_sz));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_invalid(did));
	assert_int_equal(1, ct_idx);
	assert_int_equal(2, ct_ids[0x200]);

	// verify internal structure
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(0, ct_ids[0x2]);
	assert_int_equal(1, ct_ids[0x3]);
	assert_int_equal(0, ct_ids[0x4]);
	assert_int_equal(0, ct_ids[0xff]);
	assert_int_equal(2, ct_ids[0x100]);
	assert_int_equal(0, ct_ids[0x101]);
	assert_int_equal(0, ct_ids[0x1ff]);
	assert_int_equal(2, ct_ids[0x200]);
	assert_int_equal(0, ct_ids[0x201]);
	assert_int_equal(0, ct_ids[0x2ff]);
	assert_int_equal(0, ct_ids[0x300]);
	assert_int_equal(0, ct_ids[0x301]);

	(void)state; // unused
}

void ct_create_from_nr_and_did_null_parms_test(void **state)
{
	// ct_idx is not updated

	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	did.value = 0xcafebabe;
	did.size = invld_sz;
	assert_int_equal(-EINVAL, ct_create_from_nr_and_did(NULL, 0, did));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_equal(did, 0xcafebabe, invld_sz));

	did.value = 0x12;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, ct_create_from_nr_and_did(NULL, 2, did));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_equal(did, 0x12, dev08_sz));

	assert_int_equal(-EINVAL, ct_create_from_nr_and_did(NULL, 0, did));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_equal(did, 0x12, dev08_sz));

	did.value = 0xcaca;
	did.size = dev16_sz;
	assert_int_equal(-EINVAL, ct_create_from_nr_and_did(NULL, 0, did));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_equal(did, 0xcaca, dev16_sz));

	did.value = 0xcafebabe;
	did.size = dev32_sz;
	assert_int_equal(-EINVAL, ct_create_from_nr_and_did(NULL, 0, did));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_equal(did, 0xcafebabe, dev32_sz));

	assert_int_equal(1, ct_idx);

	(void)state; // unused
}

void ct_create_from_nr_and_did_test(void **state)
{
	// ct_idx is not updated

	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	did.value = 0x1234;
	did.size = dev16_sz;
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);

	// valid nr and 8-bit did request
	assert_int_equal(0, did_create_from_data(&did, 0xca, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x100, did));
	assert_int_equal(0x010000ca, ct);
	assert_int_equal(0, did_equal(did, 0xca, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x100]);

	// valid nr and 16-bit did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0, did_create_from_data(&did, 0xbabe, dev16_sz));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x200, did));
	assert_int_equal(0x0200babe, ct);
	assert_int_equal(0, did_equal(did, 0xbabe, dev16_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x200]);

	// valid nr, invalid did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	did.value = 0xcafe; // emulate an invalid did previously created
	did.size = dev32_sz;
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EPERM, ct_create_from_nr_and_did(&ct, 0x300, did));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_equal(did, 0xcafe, dev32_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, ct_ids[0x300]);

	// valid nr, invalid did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	did.value = 0xcafe; // emulate an invalid did previously created
	did.size = invld_sz;
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EPERM, ct_create_from_nr_and_did(&ct, 0x100, did));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_equal(did, 0xcafe, invld_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x100]);

	// rollover - not really, the ct_idx doesn't change by calls to this procedure

	// valid nr, valid did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0, did_create_from_data(&did, 0x03, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0xffff, did));
	assert_int_equal(0xffff0003, ct);
	assert_int_equal(0, did_equal(did, 0x03, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0xffff]);

	// valid nr, valid did request
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0, did_create_from_data(&did, 0x04, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x03, did));
	assert_int_equal(0x00030004, ct);
	assert_int_equal(0, did_equal(did, 0x04, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(0, ct_ids[0x300]);

	// previous nr value, different did value - which doesn't exist
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	did.value = 0xac;
	did.size = dev08_sz;
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EKEYEXPIRED,
			ct_create_from_nr_and_did(&ct, 0x100, did));
	assert_int_equal(COMPTAG_UNSET, ct);
	assert_int_equal(0, did_equal(did, 0xac, dev08_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(1, ct_ids[0x100]);

	// previous nr and did value combo
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	did.value = 0xbabe;
	did.size = dev16_sz;
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x200, did));
	assert_int_equal(0x0200babe, ct);
	assert_int_equal(0, did_equal(did, 0xbabe, dev16_sz));
	assert_int_equal(1, ct_idx);
	assert_int_equal(2, ct_ids[0x200]);

	// validate internal structure
	assert_int_equal(1, ct_idx);
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(0, ct_ids[0xff]);
	assert_int_equal(1, ct_ids[0x100]);
	assert_int_equal(0, ct_ids[0x101]);
	assert_int_equal(0, ct_ids[0x1ff]);
	assert_int_equal(2, ct_ids[0x200]);
	assert_int_equal(0, ct_ids[0x201]);
	assert_int_equal(0, ct_ids[0x2ff]);
	assert_int_equal(0, ct_ids[0x300]);
	assert_int_equal(0, ct_ids[0x301]);
	assert_int_equal(0, ct_ids[0xfffe]);
	assert_int_equal(1, ct_ids[0xffff]);

	(void)state; // unused
}

void ct_create_from_did_null_parms_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	ct = CT_FROM_NR_DID(0xdead, 0x0001);
	assert_int_equal(ct, 0xdead0001);

	// null ct
	assert_int_equal(-EINVAL, ct_create_from_did(NULL, did));
	assert_int_equal(ct, 0xdead0001);
	assert_int_equal(0, did_equal(did, 1, dev08_sz));

	(void)state; // unused
}

void ct_create_from_did_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	assert_int_equal(0, did_create(&did, dev08_sz));
	assert_int_equal(0, did_equal(did, 1, dev08_sz));
	ct = CT_FROM_NR_DID(0xdead, 0x0001);
	assert_int_equal(ct, 0xdead0001);

	// valid did
	assert_int_equal(0, ct_create_from_did(&ct, did));
	assert_int_equal(ct, 0x00010001);
	assert_int_equal(0, did_equal(did, 1, dev08_sz));

	// previously used did, creates a new ct at a new nr
	assert_int_equal(0, ct_create_from_did(&ct, did));
	assert_int_equal(ct, 0x00020001);
	assert_int_equal(0, did_equal(did, 1, dev08_sz));

	// fake out a did
	did.value = 0x2;
	did.size = dev08_sz;
	assert_int_equal(0, did_not_inuse(did));
	assert_int_equal(-EKEYEXPIRED, ct_create_from_did(&ct, did));
	assert_int_equal(ct, COMPTAG_UNSET);
	assert_int_equal(0, did_equal(did, 0x2, dev08_sz));

	(void)state; // unused
}

void ct_release_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// initial values, so can check return is updated appropriately
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(0, did_create_from_data(&did, 0x12, dev08_sz));

	// release an unused ct
	assert_int_equal(0, ct_ids[0xdead]);
	assert_int_equal(-EINVAL, ct_release(ct, did));
	assert_int_equal(0, ct_ids[0xdead]);
	assert_int_equal(0xdeadbeef, ct);

	// release an inuse ct
	assert_int_equal(0, ct_ids[0x0001]);
	assert_int_equal(0, ct_create_from_did(&ct, did));
	assert_int_equal(ct, 0x00010012);
	assert_int_equal(1, ct_ids[0x0001]);
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(-1, ct_ids[0x0001]);
	assert_int_equal(ct, 0x00010012);

	// release the same ct
	assert_int_equal(0, did_create_from_data(&did, 0x12, dev08_sz));
	assert_int_equal(-EINVAL, ct_release(ct, did));
	assert_int_equal(-1, ct_ids[0x0001]);
	assert_int_equal(ct, 0x00010012);

	// invalid nr value
	ct = CT_FROM_NR_DID(0x0, 0x12);
	assert_int_equal(1, did_not_inuse(did));
	assert_int_equal(-EINVAL, ct_release(ct, did));

	// valid nr, but did doesn't match
	assert_int_equal(1, did_not_inuse(did));
	assert_int_equal(0, did_equal(did, 0x12, dev08_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x100, did));
	assert_int_equal(0x01000012, ct);
	assert_int_equal(1, ct_ids[0x0100]);
	did.size = dev16_sz;
	assert_int_equal(0, did_equal(did, 0x12, dev16_sz));
	assert_int_equal(-EINVAL, ct_release(ct, did));
	assert_int_equal(1, ct_ids[0x0100]);

	// valid nr, but did previously released
	did.size = dev08_sz;
	assert_int_equal(0, did_release(did));
	assert_int_equal(-EKEYEXPIRED, ct_release(ct, did));
	assert_int_equal(1, ct_ids[0x0100]);

	// valid nr, multiple dids attached - release out of order
	assert_int_equal(0, did_create_from_data(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x400, did));
	assert_int_equal(1, ct_ids[0x0400]);
	assert_int_equal(0, did_create_from_data(&did, 0xbabe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x400, did));
	assert_int_equal(2, ct_ids[0x0400]);
	assert_int_equal(0, did_create_from_data(&did, 0xa1, dev08_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x400, did));
	assert_int_equal(3, ct_ids[0x0400]);
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(2, ct_ids[0x0400]);
	assert_int_equal(0, did_get(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(1, ct_ids[0x0400]);
	assert_int_equal(0, did_get(&did, 0xbabe, dev16_sz));
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(-1, ct_ids[0x0400]);

	// valid nr, multiple dids attached - release out of order - one not in set
	// illustrates lack of tracking between nr and did
	assert_int_equal(0, did_create_from_data(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(1, ct_ids[0x0500]);
	assert_int_equal(0, did_create_from_data(&did, 0xbabe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(2, ct_ids[0x0500]);
	assert_int_equal(0, did_create_from_data(&did, 0xa1, dev08_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(3, ct_ids[0x0500]);
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(2, ct_ids[0x0500]);
	assert_int_equal(0, did_get(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(1, ct_ids[0x0500]);
	assert_int_equal(0, did_create_from_data(&did, 0xa5, dev08_sz));
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(-1, ct_ids[0x0500]);

	(void)state; // unused
}

void ct_internal_create_release_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// create a valid ct, release it, try and add again
	assert_int_equal(0, did_create_from_data(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(1, ct_ids[0x0500]);
	assert_int_equal(0, did_create_from_data(&did, 0xbabe, dev16_sz));
	assert_int_equal(0, ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(2, ct_ids[0x0500]);

	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(1, ct_ids[0x0500]);
	assert_int_equal(0, did_get(&did, 0xcafe, dev16_sz));
	assert_int_equal(0, ct_release(ct, did));
	assert_int_equal(-1, ct_ids[0x0500]);

	assert_int_equal(-EKEYEXPIRED,
			ct_create_from_nr_and_did(&ct, 0x500, did));
	assert_int_equal(COMPTAG_UNSET, ct);

	(void)state; // unused
}

void ct_get_nr_test(void **state)
{
	// no verification if the ct is valid by this procedure call

	ct_t ct;
	ct_nr_t nr;

	// null parm
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EINVAL, ct_get_nr(NULL, ct));
	assert_int_equal(0xdeadbeef, ct);

	// valid ct
	assert_int_equal(0, ct_get_nr(&nr, ct));
	assert_int_equal(0xdead, nr);

	// invalid ct
	ct = CT_FROM_NR_DID(0x0000, 0xbabe);
	assert_int_equal(-EINVAL, ct_get_nr(&nr, ct));
	assert_int_equal(0, nr);

	(void)state; // unused
}

void ct_get_destid_test(void **state)
{
	// no verification if the ct is valid by this procedure call

	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();

	// null parm
	ct = CT_FROM_NR_DID(0xdead, 0xbeef);
	assert_int_equal(0xdeadbeef, ct);
	assert_int_equal(-EINVAL, ct_get_destid(NULL, ct, dev08_sz));
	assert_int_equal(0xdeadbeef, ct);

	// valid did
	assert_int_equal(0, did_create_from_data(&did, 0xbabe, dev16_sz));
	ct = CT_FROM_NR_DID(0xcafe, 0xbabe);
	did = DID_ANY_DEV8_ID;
	assert_int_equal(0, ct_get_destid(&did, ct, dev16_sz));
	assert_int_equal(0, did_equal(did, 0xbabe, dev16_sz));

	// invalid did - size
	did = DID_ANY_DEV8_ID;
	assert_int_equal(-EINVAL, ct_get_destid(&did, ct, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// invalid did - does not exist
	ct = CT_FROM_NR_DID(0x0000, 0xbabe);
	did.value = 0xa1;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, ct_get_destid(&did, ct, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	// invalid did - value too large
	did.value = 0xdead;
	did.size = dev08_sz;
	assert_int_equal(-EINVAL, ct_get_destid(&did, ct, dev08_sz));
	assert_int_equal(0, did_invalid(did));

	(void)state; // unused
}

void ct_not_inuse_test(void **state)
{
	ct_t ct;
	did_t did;

	ct_reset();
	did_reset();
	assert_int_equal(-1, ct_ids[0]);
	assert_int_equal(1, ct_idx);

	// nr == 0
	ct = CT_FROM_NR_DID(0x0000, 0xbabe);
	assert_int_equal(-EINVAL, ct_not_inuse(ct, dev16_sz));

	// nr not in use
	ct = CT_FROM_NR_DID(0xcafe, 0xbabe);
	assert_int_equal(0, ct_not_inuse(ct, dev16_sz));

	// nr in use, did exists
	assert_int_equal(0,
			ct_create_from_data(&ct, &did, 0x100, 0xa1, dev08_sz));
	assert_int_equal(1, ct_not_inuse(ct, dev08_sz));
	assert_int_equal(0, ct_not_inuse(ct, dev16_sz));

	(void)state; // unused
}

int main(int argc, char** argv)
{
	(void)argv; // not used
	argc++; // not used

	const struct CMUnitTest tests[] = {
	cmocka_unit_test(assumptions),
	cmocka_unit_test(macros_test),
	cmocka_unit_test(ct_next_nr_test),
	cmocka_unit_test(ct_create_all_test_null_parms),
	cmocka_unit_test(ct_create_all_test),
	cmocka_unit_test(ct_create_from_data_test_null_parms),
	cmocka_unit_test(ct_create_from_data_test),
	cmocka_unit_test(ct_create_from_nr_and_did_null_parms_test),
	cmocka_unit_test(ct_create_from_nr_and_did_test),
	cmocka_unit_test(ct_create_from_did_null_parms_test),
	cmocka_unit_test(ct_create_from_did_test),
	cmocka_unit_test(ct_release_test),
	cmocka_unit_test(ct_get_nr_test),
	cmocka_unit_test(ct_get_destid_test),
	cmocka_unit_test(ct_not_inuse_test),
	cmocka_unit_test(ct_internal_create_release_test), };

	return cmocka_run_group_tests(tests, NULL, NULL);
}

#ifdef __cplusplus
}
#endif

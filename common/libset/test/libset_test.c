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

#include "libset.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_initialize(struct set_t *set)
{
	set->capacity = 123;
	set->expand_size = 456;
	set->next = 789;
	set->arr = (uint32_t *)0xdeadbeef;
}

// Not used. Was causing a segmentation fault on the build system
// so broke into smaller pieces to isolate the issue
void set_create_test(void **state)
{
	int rc;
	struct set_t set;

	// array may not be null
	set_initialize(&set);
	rc = set_create(NULL, 1, 1);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// must allocate at least one item
	rc = set_create(&set, 0, UINT16_MAX);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// may not allocate more than UINT16_MAX items
	rc = set_create(&set, UINT16_MAX, 1); // size + gr
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	rc = set_create(&set, UINT16_MAX, 0);
	assert_int_equal(0, rc);
	assert_int_equal(UINT16_MAX, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(sizeof(uint32_t *), sizeof(set.arr));
	free(set.arr);

	// may not have a size + grow_by greater than UINT16_MAX items
	set_initialize(&set);
	rc = set_create(&set, 1, UINT16_MAX); // size + gr
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	rc = set_create(&set, 1, UINT16_MAX - 1);
	assert_int_equal(0, rc);
	assert_int_equal(1, set.capacity);
	assert_int_equal(UINT16_MAX-1, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(sizeof(uint32_t *), sizeof(set.arr));
	free(set.arr);

	// some more normal values
	set_initialize(&set);
	rc = set_create(&set, 1, 2);
	assert_int_equal(0, rc);
	assert_int_equal(1, set.capacity);
	assert_int_equal(2, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(sizeof(uint32_t *), sizeof(set.arr));
	free(set.arr);

	set_initialize(&set);
	rc = set_create(&set, 2, 1);
	assert_int_equal(0, rc);
	assert_int_equal(2, set.capacity);
	assert_int_equal(1, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(sizeof(uint32_t *), sizeof(set.arr));
	free(set.arr);

	set_initialize(&set);
	rc = set_create(&set, 5, 6);
	assert_int_equal(0, rc);
	assert_int_equal(5, set.capacity);
	assert_int_equal(6, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(sizeof(uint32_t *), sizeof(set.arr));
	free(set.arr);

	(void)state; // unused
}

void set_create_invalid_test(void **state)
{
	int rc;
	struct set_t set;

	// array may not be null
	set_initialize(&set);
	rc = set_create(NULL, 1, 1);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// must allocate at least one item
	set_initialize(&set);
	rc = set_create(&set, 0, UINT16_MAX);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// may not allocate more than UINT16_MAX items
	set_initialize(&set);
	rc = set_create(&set, UINT16_MAX, 1); // size + gr
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// may not have a size + grow_by greater than UINT16_MAX items
	set_initialize(&set);
	rc = set_create(&set, 1, UINT16_MAX); // size + gr
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	(void)state; // unused
}

void set_create_max_capacity_test(void **state)
{
	int rc;
	struct set_t set;

	set_initialize(&set);
	rc = set_create(&set, UINT16_MAX, 0);
	assert_int_equal(0, rc);
	assert_int_equal(UINT16_MAX, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	free(set.arr);

	(void)state; // unused
}

void set_create_max_expand_test(void **state)
{
	int rc;
	struct set_t set;

	set_initialize(&set);
	rc = set_create(&set, 1, UINT16_MAX - 1);
	assert_int_equal(0, rc);
	assert_int_equal(1, set.capacity);
	assert_int_equal(UINT16_MAX-1, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	free(set.arr);

	(void)state; // unused
}

void set_create_normal_test(void **state)
{
	int rc;
	struct set_t set;

	// some more normal values
	set_initialize(&set);
	rc = set_create(&set, 1, 2);
	assert_int_equal(0, rc);
	assert_int_equal(1, set.capacity);
	assert_int_equal(2, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	free(set.arr);

	set_initialize(&set);
	rc = set_create(&set, 2, 1);
	assert_int_equal(0, rc);
	assert_int_equal(2, set.capacity);
	assert_int_equal(1, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	free(set.arr);

	set_initialize(&set);
	rc = set_create(&set, 5, 6);
	assert_int_equal(0, rc);
	assert_int_equal(5, set.capacity);
	assert_int_equal(6, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	free(set.arr);

	(void)state; // unused
}

void set_destroy_test(void **state)
{
	int rc;
	struct set_t set;

	// array may be null
	set_initialize(&set);
	rc = set_destroy(NULL);
	assert_int_equal(0, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// create and destroy
	assert_int_equal(0, set_create(&set, 1, 3));
	assert_int_equal(1, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(0, set_destroy(&set));
	assert_int_equal(0, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_null(set.arr);

	(void)state; // unused
}

void set_add_test(void **state)
{
	int rc;
	struct set_t set;
	uint16_t i;

	// array may not be null
	set_initialize(&set);
	rc = set_add(NULL, 5);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// create and add to limit
	assert_int_equal(0, set_create(&set, 1, 3));
	assert_int_equal(1, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);
	assert_int_equal(0, set_add(&set, 123));
	assert_int_equal(1, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(1, set.next);
	assert_non_null(set.arr);

	// add the grow by size (beyond initial limit)
	assert_int_equal(0, set_add(&set, 456));
	assert_int_equal(4, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(2, set.next);
	assert_non_null(set.arr);

	assert_int_equal(0, set_add(&set, 789));
	assert_int_equal(4, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(3, set.next);
	assert_non_null(set.arr);

	assert_int_equal(0, set_add(&set, 987));
	assert_int_equal(4, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(4, set.next);
	assert_non_null(set.arr);

	// add beyond limit
	assert_int_equal(0, set_add(&set, 654));
	assert_int_equal(7, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	// add duplicate entry (first)
	assert_int_equal(-EEXIST, set_add(&set, 123));
	assert_int_equal(7, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	// duplicate mid entry
	assert_int_equal(-EEXIST, set_add(&set, 789));
	assert_int_equal(7, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	// duplicate last entry
	assert_int_equal(-EEXIST, set_add(&set, 654));
	assert_int_equal(7, set.capacity);
	assert_int_equal(3, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	assert_int_equal(0, set_destroy(&set));
	assert_int_equal(0, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_null(set.arr);

	// try to add beyond max
	assert_int_equal(0, set_create(&set, UINT16_MAX-4, 2));
	for (i = 0; i < UINT16_MAX - 4; i++) {
		assert_int_equal(0, set_add(&set, i));
	}
	assert_int_equal(UINT16_MAX-4, set.capacity);
	assert_int_equal(2, set.expand_size);
	assert_int_equal(UINT16_MAX-4, set.next);
	assert_non_null(set.arr);

	for (i = UINT16_MAX - 4; i < UINT16_MAX; i++) {
		assert_int_equal(0, set_add(&set, i));
	}
	assert_int_equal(UINT16_MAX, set.capacity);
	assert_int_equal(2, set.expand_size);
	assert_int_equal(UINT16_MAX, set.next);
	assert_non_null(set.arr);

	assert_int_equal(-EPERM, set_add(&set, UINT16_MAX));
	assert_int_equal(UINT16_MAX, set.capacity);
	assert_int_equal(2, set.expand_size);
	assert_int_equal(UINT16_MAX, set.next);
	assert_non_null(set.arr);

	// cleanup
	assert_int_equal(0, set_destroy(&set));
	assert_int_equal(0, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_null(set.arr);

	// 0 growth
	assert_int_equal(0, set_create(&set, 5, 0));
	for (i = 0; i < 5; i++) {
		assert_int_equal(0, set_add(&set, i));
	}
	assert_int_equal(5, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	assert_int_equal(-EPERM, set_add(&set, 6));

	// cleanup
	assert_int_equal(0, set_destroy(&set));

	(void)state; // unused
}

void set_remove_test(void **state)
{
	int rc;
	struct set_t set;
	uint16_t i;

	// array may not be null
	set_initialize(&set);
	rc = set_remove(NULL, 5);
	assert_int_equal(-EINVAL, rc);
	assert_int_equal(123, set.capacity);
	assert_int_equal(456, set.expand_size);
	assert_int_equal(789, set.next);
	assert_ptr_equal(0xdeadbeef, set.arr);

	// create and add entries
	assert_int_equal(0, set_create(&set, 5, 5));
	for (i = 0; i < 20; i++) {
		assert_int_equal(0, set_add(&set, i));
	}
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(20, set.next);
	assert_non_null(set.arr);

	// remove an item that is not there
	assert_int_equal(-ENOENT, set_remove(&set, 20));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(20, set.next);
	assert_non_null(set.arr);

	// remove the last item
	assert_int_equal(0, set_remove(&set, 19));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(19, set.next);
	assert_non_null(set.arr);

	// add it back in
	assert_int_equal(0, set_add(&set, 19));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(20, set.next);
	assert_non_null(set.arr);

	// remove the first item
	assert_int_equal(0, set_remove(&set, 0));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(19, set.next);
	assert_non_null(set.arr);

	// try that again
	assert_int_equal(-ENOENT, set_remove(&set, 0));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(19, set.next);
	assert_non_null(set.arr);

	// remove a random item
	assert_int_equal(0, set_remove(&set, 12));
	assert_int_equal(20, set.capacity);
	assert_int_equal(5, set.expand_size);
	assert_int_equal(18, set.next);
	assert_non_null(set.arr);

	// cleanup
	assert_int_equal(0, set_destroy(&set));
	assert_int_equal(0, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_null(set.arr);

	// add 5 items, remove them all
	// create and add entries
	assert_int_equal(0, set_create(&set, 5, 0));
	for (i = 0; i < 5; i++) {
		assert_int_equal(0, set_add(&set, i));
	}
	assert_int_equal(5, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(5, set.next);
	assert_non_null(set.arr);

	for (i = 0; i < 5; i++) {
		assert_int_equal(0, set_remove(&set, i));
	}
	assert_int_equal(5, set.capacity);
	assert_int_equal(0, set.expand_size);
	assert_int_equal(0, set.next);
	assert_non_null(set.arr);

	// cleanup
	assert_int_equal(0, set_destroy(&set));

	(void)state; // unused
}

void set_contains_test(void **state)
{
	struct set_t set;
	uint16_t i;

	// empty
	assert_int_equal(0, set_create(&set, 5, 5));
	assert_false(set_contains(set, 0));

	for (i = 0; i < 5; i++) {
		assert_int_equal(0, set_add(&set, i));
	}

	for (i = 0; i < 5; i++) {
		assert_true(set_contains(set, i));
	}

	for (i = 5; i < 10; i++) {
		assert_false(set_contains(set, i));
	}

	assert_int_equal(0, set_remove(&set, 0));
	assert_false(set_contains(set, 0));
	for (i = 1; i < 5; i++) {
		assert_true(set_contains(set, i));
	}

	assert_int_equal(0, set_remove(&set, 4));
	assert_false(set_contains(set, 4));
	for (i = 1; i < 4; i++) {
		assert_true(set_contains(set, i));
	}

	// cleanup
	assert_int_equal(0, set_destroy(&set));

	(void)state; // unused
}

void set_size_test(void **state)
{
	struct set_t set;
	uint16_t i;

	// empty
	assert_int_equal(0, set_create(&set, 5, 5));
	assert_int_equal(0, set_size(set));

	for (i = 0; i < 12; i++) {
		assert_int_equal(0, set_add(&set, i));
		assert_int_equal(i + 1, set_size(set));
	}

	// cleanup
	assert_int_equal(0, set_destroy(&set));

	(void)state; // unused
}

int main(int argc, char** argv)
{
	(void)argv; // not used
	argc++; // not used

	const struct CMUnitTest tests[] = {
	cmocka_unit_test(set_create_invalid_test),
	cmocka_unit_test(set_create_max_capacity_test),
	cmocka_unit_test(set_create_max_expand_test),
	cmocka_unit_test(set_create_normal_test),
	cmocka_unit_test(set_destroy_test),
	cmocka_unit_test(set_add_test),
	cmocka_unit_test(set_remove_test),
	cmocka_unit_test(set_contains_test),
	cmocka_unit_test(set_size_test), };

	return cmocka_run_group_tests(tests, NULL, NULL);
}

#ifdef __cplusplus
}
#endif


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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "libset.h"

#ifdef UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include "cmocka.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a new set
 *
 * @param[out] set the newly constructed set
 * @param[in] initial_capacity the initial size of the set
 * @param[in] expand_size the number of items to grow by when full
 * @retval 0 on success
 * @retval -ENOMEM insufficient memory to allocate the array
 * @retval -EINVAL the parameters are not valid
 */
int set_create(struct set_t *set, uint16_t initial_capacity,
		uint16_t expand_size)
{
	if (NULL == set) {
		return -EINVAL;
	}

	if (0 == initial_capacity) {
		return -EINVAL;
	}

	if ((initial_capacity + expand_size) > UINT16_MAX) {
		return -EINVAL;
	}

	memset(set, 0, sizeof(struct set_t));
	set->arr = (uint32_t *)malloc(initial_capacity * sizeof(uint32_t));
	if (NULL == set->arr) {
		return -ENOMEM;
	}

	set->capacity = initial_capacity;
	set->expand_size = expand_size;
	return 0;
}

/**
 * Destroy a set. If the set parameter is null this
 * function behaves as a noop.
 *
 * @param[in/out] set the set to be destroyed
 * @retval 0 on success
 */
int set_destroy(struct set_t *set)
{
	if (NULL == set) {
		return 0;
	}

	free(set->arr);
	memset(set, 0, sizeof(struct set_t));
	return 0;
}

/**
 * Add an item to the set. The set resizes if necessary to accommodate
 * the new item.
 *
 * @param[in/out] set the set
 * @param[in] item the item to be added
 * @retval 0 on success
 * @retval -EINVAL the set parameter is invalid
 * @retval -ENOMEM insufficient memory to allocate the set
 * @retval -EEXIST the element already exists in the set
 * @retval -EPERM the maximum number of items is exceeded
 */
int set_add(struct set_t *set, uint32_t item)
{
	uint16_t i;
	uint32_t *new_arr;
	uint32_t capacity;

	if (NULL == set) {
		return -EINVAL;
	}

	// search for the item
	for (i = 0; i < set->next; i++) {
		if (set->arr[i] == item) {
			return -EEXIST;
		}
	}

	// grow if necessary
	if (set->next == set->capacity) {
		if (0 == set->expand_size) {
			return -EPERM;
		}

		capacity = set->capacity + set->expand_size;
		if (capacity > UINT16_MAX) {
			return -EPERM;
		}

		new_arr = (uint32_t *)realloc(set->arr,
				capacity * sizeof(uint32_t));
		if (NULL == new_arr) {
			return -ENOMEM;
		}
		set->arr = new_arr;
		set->capacity = capacity;
	}
	set->arr[set->next++] = item;
	return 0;
}

/**
 * Remove an item from the set.
 *
 * @param[in/out] set the set
 * @param[in] item the item to be removed
 * @retval 0 on success
 * @retval -EINVAL the set parameter is invalid
 * @retval -ENOENT the item does not exist in the array
 */
int set_remove(struct set_t *set, uint32_t item)
{
	uint16_t i;

	if (NULL == set) {
		return -EINVAL;
	}

	// search for the item
	for (i = 0; i < set->next; i++) {
		if (set->arr[i] == item) {
			goto found;
		}
	}
	return -ENOENT;

	found: set->next--;
	if (i != set->next) {
		// fill in the hole
		set->arr[i] = set->arr[set->next];
	}
	return 0;
}

/**
 * Determine if an item is in the set
 *
 * @param[in] set the set
 * @param[in] item the item to look for
 * @retval true if the item is in the set, false otherwise
 */
bool set_contains(struct set_t set, uint32_t item)
{
	uint32_t i;

	// search for the item
	for (i = 0; i < set.next; i++) {
		if (set.arr[i] == item) {
			return true;
		}
	}
	return false;
}

/**
 * Answer the number of items in the set
 *
 * @param[in] set the set
 * @retval the number of items in the set
 */
int set_size(struct set_t set)
{
	return set.next;
}

#ifdef __cplusplus
}
#endif


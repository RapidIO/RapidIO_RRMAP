/*
 ****************************************************************************
 Copyright (c) 2015, Integrated Device Technology Inc.
 Copyright (c) 2015, RapidIO Trade Association
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

#ifndef __LIB_SET_H__
#define __LIB_SET_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A collection that contains no duplicate items.
 *
 * Items in the collection are not sorted and are unique. When adding an item
 * the entire collection is searched to ensure the item is already not in the
 * collection. When removing items, the entire collection may need to be
 * searched to locate the item being removed.
 *
 * The initial size of the set as well as the expansion size are specified at
 * time of construction.
 *
 * \note The maximum number of items is bounded by UINT16_MAX
 */

/*
 * Implementation details.
 * The set is currently backed by an array that grows as required.  Items are
 * generally added to the end of the array. When an item is removed from the
 * array the last item is moved to the position vacated by that item and the
 * indices adjust appropriately. Order of items is not guaranteed.
 *
 * All additions and removal of items require a linear search of the existing
 * items to ensure duplicates are not present.
 */
struct set_t {
	uint16_t capacity; // size of array
	uint16_t expand_size; // grow size
	uint16_t next; // position of next item to be added
	uint32_t *arr;
};

int set_create(struct set_t *set, uint16_t initial_capacity,
		uint16_t expand_size);
int set_destroy(struct set_t *set);
int set_add(struct set_t *set, uint32_t item);
int set_remove(struct set_t *set, uint32_t item);
bool set_contains(struct set_t set, uint32_t item);
int set_size(struct set_t set);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_SET_H__ */

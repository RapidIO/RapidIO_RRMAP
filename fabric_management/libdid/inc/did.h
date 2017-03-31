
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

#ifndef __DID_H__
#define __DID_H__

#include <stdint.h>
#include <stdbool.h>

#include "rio_route.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	invld_sz = 0, dev08_sz = 8, dev16_sz = 16, dev32_sz = 32,
} did_sz_t;

typedef struct {
	did_val_t value;
	did_sz_t size;
} did_t;

#define ANY_ID 0xff
#define DID_ANY_DEV8_ID ((did_t) {RIO_LAST_DEV8, dev08_sz})
#define DID_ANY_DEV16_ID ((did_t) {RIO_LAST_DEV16, dev16_sz})
#define DID_INVALID_ID ((did_t) {0, invld_sz})

int did_size_from_int(did_sz_t *size, uint32_t asInt);
int did_create(did_t *did, did_sz_t size);
int did_create_from_data(did_t *did, did_val_t value, did_sz_t size);
int did_get(did_t *did, did_val_t value);
int did_from_value(did_t *did, uint32_t value, uint32_t size);
int did_to_value(did_t did, uint32_t *value, uint32_t *size);
int did_release(did_t did);
int did_not_inuse(did_t did);

did_val_t did_get_value(did_t did);
did_sz_t did_get_size(did_t did);
bool did_equal(did_t did, did_t other);

#ifdef __cplusplus
}
#endif

#endif /* __DID_H__ */

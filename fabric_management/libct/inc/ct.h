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

#ifndef __CT_H__
#define __CT_H__

#include <stdint.h>

#include "rio_route.h"
#include "did.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t ct_nr_t;

/**
 * \brief A value of zero is an invalid component tag
 */
#define COMPTAG_UNSET 0x00000000

int ct_create_all(ct_t *ct, did_t *did, did_sz_t size);
int ct_create_from_data(ct_t *ct, did_t *did, ct_nr_t nr,
		did_val_t value, did_sz_t size);
int ct_create_from_nr_and_did(ct_t *ct, ct_nr_t nr, did_t did);
int ct_create_from_did(ct_t *ct, did_t did);
int ct_from_value(ct_t *ct, uint32_t ct_val);
int ct_release(ct_t ct, did_t did);

int ct_get_nr(ct_nr_t *nr, ct_t ct);
int ct_get_destid(did_t *did, ct_t ct);
int ct_not_inuse(ct_t ct);

#ifdef __cplusplus
}
#endif

#endif /* __CT_H__ */

/* REserved memory file parsing test */
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

#include <errno.h>
#include "librsvdmem.h"
#include "librsvdmem_private.h"
#include "libcli.h"
#include "rapidio_mport_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief test success case for all known tokens, 32 bit address and size
*/
int test_1(void)
{
	const char *test_file = "test/test_1.conf";
	uint64_t start_addr = 0;
	uint64_t size = 0;
	int rc;

	rc = get_phys_mem((const char *)test_file, (char *)RSVD_PHYS_MEM,
							&start_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x18000000 != start_addr) || (0x04000000 != size)) {
		goto fail;
	};

	start_addr = 0;
	size = 0;
	rc = get_phys_mem((const char *)test_file, (char *)RSVD_PHYS_MEM_RDMAD,
							&start_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x18000000 != start_addr) || (0x01000000 != size)) {
		goto fail;
	};

	start_addr = 0;
	size = 0;
	rc = get_phys_mem((const char *)test_file,
				RSVD_PHYS_MEM_RSKTD, &start_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x19000000 != start_addr) || (0x01000000 != size)) {
		goto fail;
	};

	start_addr = 0;
	size = 0;
	rc = get_phys_mem((const char *)test_file,
				RSVD_PHYS_MEM_DMA_TUN, &start_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1A000000 != start_addr) || (0x01000000 != size)) {
		goto fail;
	};

	start_addr = 0;
	size = 0;
	rc = get_phys_mem((const char *)test_file,
				RSVD_PHYS_MEM_FXFR, &start_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1B000000 != start_addr) || (0x01000000 != size)) {
		goto fail;
	};

	return 0;
fail:
	return 1;
};

/** \brief test success case for all known tokens, 64 bit address and size
*/
int test_2(void)
{
	const char *test_file = "test/test_2.conf";
	uint64_t st_addr;
	uint64_t size;
	int rc;

	rc = get_phys_mem((const char *)test_file, RSVD_PHYS_MEM, &st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1800000000000000 != st_addr) || (0x0400000000000000 != size)) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file, RSVD_PHYS_MEM_RDMAD, &st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1800000000000000 != st_addr) || (0x0100000000000000 != size)) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file, RSVD_PHYS_MEM_RSKTD, &st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1900000000000000 != st_addr) || (0x0100000000000000 != size)) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file, RSVD_PHYS_MEM_DMA_TUN, &st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1A00000000000000 != st_addr) || (0x0100000000000000 != size)) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file, RSVD_PHYS_MEM_FXFR, &st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1B00000000000000 != st_addr) || (0x0100000000000000 != size)) {
		goto fail;
	};

	return 0;
fail:
	return 1;
};

/** \brief test failure cases:
 * - empty file
 * - blank line
 * - Valid file, missing token
 * - missing address & size
 * - missing size
 * - Invalid address/size combination (misaligned)
*/
int test_3(void)
{
	const char *test_file1 = "test/test_3_empty.conf";
	const char *test_file2 = "test/test_3_blankline.conf";
	const char *test_file3 = "test/test_3_badformat.conf";
	uint64_t st_addr;
	uint64_t size;
	int rc;
	int i;

	rc = get_phys_mem((const char *)test_file1, RSVD_PHYS_MEM, &st_addr, &size);
	if (!rc) {
		goto fail;
	};
	if ((RIO_ANY_ADDR != st_addr) || size) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file2, RSVD_PHYS_MEM_RDMAD, &st_addr, &size);
	if (!rc) {
		goto fail;
	};
	if ((RIO_ANY_ADDR != st_addr) || size) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file3, (char *)"TOKEN_NOT_FOUND", &st_addr, &size);
	if (!rc) {
		goto fail;
	};
	if ((RIO_ANY_ADDR != st_addr) || size) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file3, (char *)"NO_ADDR_SZ", &st_addr, &size);
	if (!rc) {
		goto fail;
	};
	if ((RIO_ANY_ADDR != st_addr) || size) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file3, (char *)"NO_SZ",
							&st_addr, &size);
	if (!rc) {
		goto fail;
	};
	if ((RIO_ANY_ADDR != st_addr) || size) {
		goto fail;
	};

	rc = get_phys_mem((const char *)test_file3, (char *)"STUFF_BEFORE",
							&st_addr, &size);
	if (rc) {
		goto fail;
	};
	if ((0x1800000000000000 != st_addr) || (0x010000000000000 != size)) {
		goto fail;
	};

	for (i = 1; i <= 3; i++) {
		char keyword[30];
		memset(keyword, 0, 30);
		snprintf(keyword, 30, "MISALIGNED%d", i);

		st_addr = 0;
		size = 0;
		errno = 0;

		rc = get_phys_mem((const char *)test_file3,
				(char *)keyword, &st_addr, &size);
		if (!rc || (errno != EDOM)) {
			goto fail;
		};
		if (0x0100000000000000 != size) {
			goto fail;
		};

		switch (i) {
		case 1: if (0x1900000000000001 != st_addr) {
				goto fail;
			};
			break;
		case 2: if (0x1980000000000000 != st_addr) {
				goto fail;
			};
			break;
		case 3: if (0x1900008000000000 != st_addr) {
				goto fail;
			};
			break;
		default: goto fail;
		};

	};

	for (i = 1; i <= 6; i++) {
		char keyword[30];
		snprintf(keyword, 30, "ILLEGAL_CHARS%d", i);
		rc = get_phys_mem((const char *)test_file3,
				(char *)keyword, &st_addr, &size);
		if (!rc || (errno != EDOM)) {
			goto fail;
		};
	};

	return 0;
fail:
	return 1;
};


int main(int argc, char *argv[])
{
	if (0) {
		argv[0] = NULL;
		return argc;
	};

	if (test_1()) {
		printf("Test 1 failed.\n");
		goto fail;
	};
	printf("Test 1 passed.\n");
	
	if (test_2()) {
		printf("Test 2 failed.\n");
		goto fail;
	};
	printf("Test 2 passed.\n");
	
	if (test_3()) {
		printf("Test 3 failed.\n");
		goto fail;
	};
	printf("Test 3 passed.\n");
	
	return 0;
fail:
	return 1;
};


#ifdef __cplusplus
}
#endif


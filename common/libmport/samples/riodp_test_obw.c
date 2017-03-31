/*
 * Copyright 2014 Integrated Device Technology, Inc.
 *
 * User-space RapidIO MEMIO transfer test program.
 *
 * This program uses code fragments from Linux kernel dmaengine
 * framework test driver developed by Atmel and Intel. Please, see
 * drivers/dma/dmatest.c file in Linux kernel source code tree for more
 * details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * \file
 * \brief Test MEMIO data transfers to/from RapidIO device.
 *
 * This program can be invoked in two modes:
 *
 * 1. MEMIO transfer initiator (master).
 *
 * 2. Target inbound memory allocator.
 *
 * The program starts in Inbound Target Memory mode when option -i or -I
 * is specified. To avoid predictable MEMIO transfer failure, target inbound memory
 * must be created first. When started in inbound memory mode, riodp_test_dma
 * program will display RapidIO base address assigned to the inbound window.
 * This address should be used to define target address of DMA data transfers.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h> /* For size_t */
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#include "rio_misc.h"
#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_mgmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond
/*
 * Initialization patterns. All bytes in the source buffer has bit 7
 * set, all bytes in the destination buffer has bit 7 cleared.
 *
 * Bit 6 is set for all bytes which are to be copied by the DMA
 * engine. Bit 5 is set for all bytes which are to be overwritten by
 * the DMA engine.
 *
 * The remaining bits are the inverse of a counter which increments by
 * one for each byte address.
 */
#define PATTERN_SRC		0x80
#define PATTERN_DST		0x00
#define PATTERN_COPY		0x40
#define PATTERN_OVERWRITE	0x20
#define PATTERN_COUNT_MASK	0x1f

struct dma_async_wait_param {
	uint32_t token; /* DMA transaction ID token */
	int err; /* error code returned to caller */
};

#define U8P uint8_t*

/// @endcond

/// Maximum number of mismatched bytes in buffer to print.
#define MAX_ERROR_COUNT		32
/// Default size of source and destination data buffers
#define TEST_BUF_SIZE (256 * 1024)
/// Default size of RapidIO target inbound window 
#define DEFAULT_IBWIN_SIZE (2 * 1024 * 1024)

static riomp_mport_t mport_hnd;
static did_val_t tgt_did_val = 0;
static uint64_t tgt_addr;
static uint32_t offset = 0;
static uint16_t align = 0;
static uint32_t copy_size = TEST_BUF_SIZE;
static uint32_t ibwin_size;
static uint32_t tbuf_size = TEST_BUF_SIZE;
static int debug = 0;

static void usage(char *program)
{
	printf("%s - test MEMIO data transfers to/from RapidIO device\n",
			program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("options are:\n");
	printf("Common:\n");
	printf("  --help (or -h)\n");
	printf("  -M mport_id\n");
	printf("  --mport mport_id\n");
	printf("    local mport device index (default 0)\n");
	printf("  -v turn off buffer data verification\n");
	printf("  --debug (or -d)\n");
	printf("OBW mapping test mode only:\n");
	printf("  -D xxxx\n");
	printf("  --destid xxxx\n");
	printf("    destination ID of target RapidIO device (default any)\n");
	printf("  -A xxxx\n");
	printf("  --taddr xxxx\n");
	printf("    memory address in target RapidIO device (default 0)\n");
	printf("  -S xxxx\n");
	printf("  --size xxxx\n");
	printf("    data transfer size in bytes (default 0x%x)\n", copy_size);
	printf("  -O xxxx\n");
	printf("  --offset xxxx\n");
	printf("    offset in local data src/dst buffers (default 0)\n");
	printf("  -a n\n");
	printf("  --align n\n");
	printf("    data buffer address alignment (default 0)\n");
	printf("  -T n\n");
	printf("  --repeat n\n");
	printf("    repeat test n times (default 1)\n");
	printf(
			"  -B xxxx size of test buffer and OBW aperture (in MB, e.g -B2) (default 256 * 1024)\n");
	printf("  -r use random size and local buffer offset values\n");
	printf("Inbound Window mode only:\n");
	printf("  -i\n");
	printf(
			"    allocate and map inbound window (memory) using default parameters\n");
	printf("  -I xxxx\n");
	printf("  --ibwin xxxx\n");
	printf("    inbound window (memory) size in bytes (default 0)\n");
	printf("  -R xxxx\n");
	printf("  --ibbase xxxx\n");
	printf(
			"    inbound window base address in RapidIO address space (default any address)\n");
	printf("  -L xxxx\n");
	printf("  --laddr xxxx\n");
	printf("    physical address of reserved local memory to use\n");
	printf("\n");
}

static struct timespec timediff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

static void dmatest_init_srcs(uint8_t *buf, unsigned int start,
		unsigned int len, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < start; i++) {
		buf[i] = PATTERN_SRC | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < start + len; i++) {
		buf[i] = PATTERN_SRC | PATTERN_COPY | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < buf_size; i++) {
		buf[i] = PATTERN_SRC | (~i & PATTERN_COUNT_MASK);
	}
}

static void dmatest_init_dsts(uint8_t *buf, unsigned int start,
		unsigned int len, unsigned int buf_size)
{
	unsigned int i;

	for (i = 0; i < start; i++) {
		buf[i] = PATTERN_DST | (~i & PATTERN_COUNT_MASK);
	}
	for (; i < start + len; i++) {
		buf[i] = PATTERN_DST | PATTERN_OVERWRITE
				| (~i & PATTERN_COUNT_MASK);
	}
	for (; i < buf_size; i++) {
		buf[i] = PATTERN_DST | (~i & PATTERN_COUNT_MASK);
	}
}

static void dmatest_mismatch(uint8_t actual, uint8_t pattern,
		unsigned int index, unsigned int counter, int is_srcbuf)
{
	uint8_t diff = actual ^ pattern;
	uint8_t expected = pattern | (~counter & PATTERN_COUNT_MASK);

	if (is_srcbuf) {
		printf("srcbuf[0x%x] overwritten! Expected %02x, got %02x\n",
				index, expected, actual);
	} else if ((pattern & PATTERN_COPY)
			&& (diff & (PATTERN_COPY | PATTERN_OVERWRITE))) {
		printf("dstbuf[0x%x] not copied! Expected %02x, got %02x\n",
				index, expected, actual);
	} else if (diff & PATTERN_SRC) {
		printf("dstbuf[0x%x] was copied! Expected %02x, got %02x\n",
				index, expected, actual);
	} else {
		printf("dstbuf[0x%x] mismatch! Expected %02x, got %02x\n",
				index, expected, actual);
	}
}

static unsigned int dmatest_verify(uint8_t *buf, unsigned int start,
		unsigned int end, unsigned int counter, uint8_t pattern,
		int is_srcbuf)
{
	unsigned int i;
	unsigned int error_count = 0;
	uint8_t actual;
	uint8_t expected;
	unsigned int counter_orig = counter;

	counter = counter_orig;
	for (i = start; i < end; i++) {
		actual = buf[i];
		expected = pattern | (~counter & PATTERN_COUNT_MASK);
		if (actual != expected) {
			if (error_count < MAX_ERROR_COUNT) {
				dmatest_mismatch(actual, pattern, i, counter,
						is_srcbuf);
			}
			error_count++;
		}
		counter++;
	}

	if (error_count > MAX_ERROR_COUNT) {
		printf("%u errors suppressed\n", error_count - MAX_ERROR_COUNT);
	}
	return error_count;
}

static void *obwtest_buf_alloc(uint32_t size)
{
	void *buf_ptr = NULL;

	buf_ptr = malloc(size);
	if (NULL == buf_ptr) {
		perror("malloc");
	}
	return buf_ptr;
}

static void obwtest_buf_free(void *buf)
{
	free(buf);
}

/**
 * \brief This function is called by main() if Inbound Target Memory mode was
 * specified.
 * 
 *
 * \param[in] rio_base Base RapidIO address for inbound window
 * \param[in] ib_size Inbound window and buffer size in bytes
 * \param[in] loc_addr Physical address in reserved memory range
 * \param[in] verify Flag to enable/disable data verification on exit
 *
 * \return 0 if successfull or error code returned by mport API.
 *
 * Performs the following steps:
 *
 */
int do_ibwin_test(uint64_t rio_base, uint32_t ib_size, uint64_t loc_addr,
		int verify)
{
	int ret;
	uint64_t ib_handle = loc_addr;
	void *ibmap;

	/** - Request mport's inbound window mapping */
	ret = riomp_dma_ibwin_map(mport_hnd, &rio_base, ib_size, &ib_handle);
	if (ret) {
		printf("Failed to allocate/map IB buffer err=%d\n", ret);
		return ret;
	}

	/** - Map associated kernel buffer into process address space */
	ret = riomp_dma_map_memory(mport_hnd, ib_size, ib_handle, &ibmap);
	if (ret) {
		perror("mmap");
		goto out;
	}

	memset(ibmap, 0, ib_size);

	printf("\tSuccessfully allocated/mapped IB buffer (rio_base=0x%x_%x)\n",
			(uint32_t)(rio_base >> 32),
			(uint32_t)(rio_base & 0xffffffff));

	if (debug) {
		printf("\t(h=0x%x_%x, loc=%p)\n", (uint32_t)(ib_handle >> 32),
				(uint32_t)(ib_handle & 0xffffffff), ibmap);
	}
	printf("\t.... press Enter key to exit ....\n");

	/** - Pause until a user presses Enter key */
	getchar();

	/** - Verify data before exit (if requested) */
	if (verify) {
		dmatest_verify((U8P)ibmap, 0, ib_size, 0,
				PATTERN_SRC | PATTERN_COPY, 0);
	}

	/** - Unmap kernel-space data buffer */
	ret = riomp_dma_unmap_memory(ib_size, ibmap);
	if (ret) {
		perror("munmap");
	}

out:
	/** - Release mport's inbound mapping window */
	ret = riomp_dma_ibwin_free(mport_hnd, &ib_handle);
	if (ret) {
		printf("Failed to release IB buffer err=%d\n", ret);
	}
	return 0;
}

/**
 * \brief This function performs MEMIO write and read back to/from remote RapidIO
 * target device.
 *
 * Called by main() if MEMIO Master mode was specified.
 *
 * \param[in] random If non-zero, enables using random transfer size and offsets
 * within source and destination buffers.
 * \param[in] verify Flag to enable/disable data verification for each write-read cycle
 * \param[in] loop_count Number of write-read cycles to perform
 *
 * \return 0 if successfull or error code returned by mport API.
 *
 * Performs the following steps:
 */
int do_obwin_test(int random, int verify, int loop_count)
{
	void *buf_src = NULL;
	void *buf_dst = NULL;
	void *obw_ptr = NULL;
	unsigned int src_off, dst_off, len;
	uint64_t obw_handle = 0;
	int i, ret = 0;
	struct timespec wr_starttime, wr_endtime;
	struct timespec rd_starttime, rd_endtime;
	float totaltime;

	/* check specified DMA block size */
	if (copy_size > tbuf_size || copy_size == 0) {
		printf("ERR: invalid transfer size parameter\n");
		printf("     max allowed copy size: %d bytes\n", tbuf_size);
		return -1;
	}

	if (random) {
		printf(
				"\tRANDOM mode is selected for size/offset combination\n");
		printf("\t\tmax data transfer size: %d bytes\n", copy_size);
		srand(time(NULL));
	} else if (copy_size + offset > tbuf_size) {
		printf("ERR: invalid transfer size/offset combination\n");
		return -1;
	} else {
		printf("\tcopy_size=%d offset=0x%x\n", copy_size, offset);
	}

	/** * Allocate source and destination buffers */
	buf_src = obwtest_buf_alloc(tbuf_size);
	if (NULL == buf_src) {
		printf("DMA Test: error allocating SRC buffer\n");
		ret = -1;
		goto out;
	}

	buf_dst = obwtest_buf_alloc(tbuf_size);
	if (NULL == buf_dst) {
		printf("DMA Test: error allocating DST buffer\n");
		ret = -1;
		goto out;
	}

	/** * Request outbound window mapped to the specified target RapidIO device */
	ret = riomp_dma_obwin_map(mport_hnd, tgt_did_val, tgt_addr, tbuf_size,
			&obw_handle);
	if (ret) {
		printf("riomp_dma_obwin_map failed err=%d\n", ret);
		goto out;
	}

	printf("OBW handle 0x%x_%08x\n", (uint32_t)(obw_handle >> 32),
			(uint32_t)(obw_handle & 0xffffffff));

	/** * Map obtained outbound window into process address space */
	ret = riomp_dma_map_memory(mport_hnd, tbuf_size, obw_handle, &obw_ptr);
	if (ret) {
		perror("mmap");
		goto out_unmap;
	}

	for (i = 1; i <= loop_count; i++) { /// * Enter write-read cycle
		struct timespec time, rd_time;

		if (random) {
			len = rand() % copy_size + 1;
			len = (len >> align) << align;
			if (!len)
				len = 1 << align;
			src_off = rand() % (tbuf_size - len + 1);
			dst_off = rand() % (tbuf_size - len + 1);

			src_off = (src_off >> align) << align;
			dst_off = (dst_off >> align) << align;
		} else {
			len = copy_size;
			src_off = offset;
			dst_off = offset;
		}

		printf("<%d>: len=0x%x src_off=0x%x dst_off=0x%x\n", i, len,
				src_off, dst_off);

		/** - If data verification is requested, fill src and dst buffers
		 * with predefined data */
		if (verify) {
			dmatest_init_srcs((U8P)buf_src, src_off, len,
					tbuf_size);
			dmatest_init_dsts((U8P)buf_dst, dst_off, len,
					tbuf_size);
		}

		if (debug) {
			printf("\tWrite %d bytes from src offset 0x%x\n", len,
					src_off);
		}
		clock_gettime(CLOCK_MONOTONIC, &wr_starttime);

		/** - Write data from local source buffer to remote target inbound buffer */
		memcpy(obw_ptr, (U8P)buf_src + src_off, len);
		clock_gettime(CLOCK_MONOTONIC, &wr_endtime);
		if (debug) {
			printf("\tRead %d bytes to dst offset 0x%x\n", len,
					dst_off);
		}

		clock_gettime(CLOCK_MONOTONIC, &rd_starttime);
		/** - Read back data from remote target inbound buffer into local destination buffer */
		memcpy((U8P)buf_dst + dst_off, obw_ptr, len);
		clock_gettime(CLOCK_MONOTONIC, &rd_endtime);

		rd_time = timediff(rd_starttime, rd_endtime);

		/** - If data verification is requested, verify data transfer results */
		if (verify) {
			unsigned int error_count;

			if (debug) {
				printf("\tVerifying source buffer...\n");
			}
			error_count = dmatest_verify((U8P)buf_src, 0, src_off,
					0, PATTERN_SRC, 1);
			error_count += dmatest_verify((U8P)buf_src, src_off,
					src_off + len, src_off,
					PATTERN_SRC | PATTERN_COPY, 1);
			error_count += dmatest_verify((U8P)buf_src,
					src_off + len, tbuf_size, src_off + len,
					PATTERN_SRC, 1);

			if (debug) {
				printf("\tVerifying destination buffer...\n");
			}
			error_count += dmatest_verify((U8P)buf_dst, 0, dst_off,
					0, PATTERN_DST, 0);
			error_count += dmatest_verify((U8P)buf_dst, dst_off,
					dst_off + len, src_off,
					PATTERN_SRC | PATTERN_COPY, 0);
			error_count += dmatest_verify((U8P)buf_dst,
					dst_off + len, tbuf_size, dst_off + len,
					PATTERN_DST, 0);
			if (error_count) {
				printf(
						"\tBuffer verification failed with %d errors\n",
						error_count);
				break;
			} else {
				printf("\tBuffer verification OK!\n");
			}
		} else {
			printf("\tBuffer verification is turned off!\n");
		}

		time = timediff(wr_starttime, wr_endtime);
		totaltime =
				((double)time.tv_sec
						+ (time.tv_nsec / 1000000000.0));
		printf("\t\tWR time: %4f s @ %4.2f MB/s\n", totaltime,
				(len / totaltime) / (1024 * 1024));
		totaltime = ((double)rd_time.tv_sec
				+ (rd_time.tv_nsec / 1000000000.0));
		printf("\t\tRD time: %4f s @ %4.2f MB/s\n", totaltime,
				(len / totaltime) / (1024 * 1024));
		time = timediff(wr_starttime, rd_endtime);
		totaltime =
				((double)time.tv_sec
						+ (time.tv_nsec / 1000000000.0));
		printf("\t\tFull Cycle time: %4f s\n", totaltime);
	} /// - Repeat if loop_count > 1

	/** * Unmap outbound window from process address space */
	ret = riomp_dma_unmap_memory(tbuf_size, obw_ptr);
	if (ret) {
		perror("munmap");
	}

out_unmap:
	/** * Release outbound window */
	ret = riomp_dma_obwin_free(mport_hnd, &obw_handle);
	if (ret) {
		printf("Failed to release OB window err=%d\n", ret);
	}

out:
	/** * Free source and destination buffers */
	if (buf_src) {
		obwtest_buf_free(buf_src);
	}
	if (buf_dst) {
		obwtest_buf_free(buf_dst);
	}
	return ret;
}

/**
 * \brief Starting point for the test program
 *
 * \param[in] argc Command line parameter count
 * \param[in] argv Array of pointers to command line parameter null terminated
 *                 strings
 *
 * \retval 0 means success
 */
int main(int argc, char** argv)
{
	int c;
	char *program = argv[0];

	// command line parameters
	uint32_t mport_id = 0;
	int do_rand = 0;
	int verify = 1;
	unsigned int repeat = 1;
	uint64_t rio_base = RIOMP_MAP_ANY_ADDR;
	uint64_t loc_addr = RIOMP_MAP_ANY_ADDR;
	static const struct option options[] = {
			{"destid", required_argument, NULL, 'D'},
			{"taddr", required_argument, NULL, 'A'},
			{"size", required_argument, NULL, 'S'},
			{"offset", required_argument, NULL, 'O'},
			{"align", required_argument, NULL, 'a'},
			{"repeat", required_argument, NULL, 'T'},
			{"ibwin", required_argument, NULL, 'I'},
			{"ibbase", required_argument, NULL, 'R'},
			{"mport", required_argument, NULL, 'M'},
			{"laddr", required_argument, NULL, 'L'},
			{"faf", no_argument, NULL, 'F'},
			{"async", no_argument, NULL, 'Y'},
			{"debug", no_argument, NULL, 'd'},
			{"help", no_argument, NULL, 'h'},
	};

	struct riomp_mgmt_mport_properties prop;
	int rc = EXIT_SUCCESS;
	bool ibwin_set = false;

	/** Parse command line options, if any */
	while (-1
			!= (c = getopt_long_only(argc, argv,
					"rvdhia:A:D:I:O:M:R:S:T:B:L:", options,
					NULL))) {
		switch (c) {
		case 'A':
			if (tok_parse_ull(optarg, &tgt_addr, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Target memory address");
				exit(EXIT_FAILURE);
			}
			break;
		case 'L':
			if (tok_parse_ull(optarg, &loc_addr, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Local memory address");
				exit(EXIT_FAILURE);
			}
			break;
		case 'a':
			if (tok_parse_us(optarg, &align, 0)) {
				printf(TOK_ERR_US_HEX_MSG_FMT,
						"Data alignment");
				exit(EXIT_FAILURE);
			}
			break;
		case 'D':
			if (tok_parse_did(optarg, &tgt_did_val, 0)) {
				printf(TOK_ERR_DID_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'O':
			if (tok_parse_ul(optarg, &offset, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "Data offset");
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			if (tok_parse_ul(optarg, &copy_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Data transfer size");
				exit(EXIT_FAILURE);
			}
			break;
		case 'T':
			if (tok_parse_ul(optarg, &repeat, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Number of repetitions");
				exit(EXIT_FAILURE);
			}
			break;
		case 'B':
			if (tok_parse_ul(optarg, &tbuf_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Test buffer size");
				exit(EXIT_FAILURE);
			}
			tbuf_size = tbuf_size * 1024 * 1024;
			break;
		case 'r':
			do_rand = 1;
			break;
			/* Inbound Memory (window) Mode options */
		case 'I':
			if (ibwin_set) {
				printf(
						"Only one of \'-\' or \'ibwin\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			ibwin_set = true;
			if (tok_parse_ul(optarg, &ibwin_size, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Inbound window memory size");
				exit(EXIT_FAILURE);
			}
			break;
		case 'i':
			if (ibwin_set) {
				printf(
						"Only one of \'-\' or \'ibwin\' may be specified\n");
				usage(program);
				exit(EXIT_FAILURE);
			}
			ibwin_set = true;
			ibwin_size = DEFAULT_IBWIN_SIZE;
			break;
		case 'R':
			if (tok_parse_ull(optarg, &rio_base, 0)) {
				printf(TOK_ERR_ULL_HEX_MSG_FMT,
						"Inbound window base memory address");
				return (EXIT_FAILURE);
			}
			break;
			/* Options common for all modes */
		case 'M':
			if (tok_parse_mport_id(optarg, &mport_id, 0)) {
				printf(TOK_ERR_MPORT_MSG_FMT);
				return (EXIT_FAILURE);
			}
			break;
		case 'v':
			verify = 0;
			break;
		case 'd':
			debug = 1;
			break;
		case 'h':
			usage(program);
			exit(EXIT_SUCCESS);
			break;
//		case '?':
		default:
			/* Invalid command line option */
			if (isprint(optopt)) {
				printf("Unknown option '-%c\n", optopt);
			}
			usage(program);
			exit(EXIT_FAILURE);
		}
	}

	rc = riomp_mgmt_mport_create_handle(mport_id, 0, &mport_hnd);
	if (rc < 0) {
		printf("DMA Test: unable to open mport%d device err=%d\n",
				mport_id, rc);
		exit(EXIT_FAILURE);
	}

	if (!riomp_mgmt_query(mport_hnd, &prop)) {
		riomp_mgmt_display_info(&prop);

		if (prop.link_speed == 0) {
			printf("SRIO link is down. Test aborted.\n");
			rc = EXIT_FAILURE;
			goto out;
		}
	} else {
		printf("Failed to obtain mport information\n");
		printf("Using default configuration\n\n");
	}

	if (ibwin_size) {
		printf("+++ RapidIO Inbound Window Mode +++\n");
		printf("\tmport%d ib_size=0x%x PID:%d\n", mport_id, ibwin_size,
				(int)getpid());
		if (loc_addr != RIOMP_MAP_ANY_ADDR)
			printf("\tloc_addr=0x%llx\n",
					(unsigned long long)loc_addr);

		do_ibwin_test(rio_base, ibwin_size, loc_addr, verify);
	} else {
		printf("+++ RapidIO Outbound Window Mapping Test +++\n");
		printf("\tmport%d destID=%d rio_addr=0x%llx repeat=%d PID:%d\n",
				mport_id, tgt_did_val,
				(unsigned long long)tgt_addr, repeat,
				(int)getpid());
		printf("\tbuf_size=0x%x\n", tbuf_size);

		do_obwin_test(do_rand, verify, repeat);
	}

out:
	riomp_mgmt_mport_destroy_handle(&mport_hnd);
	exit(rc);
}

#ifdef __cplusplus
}
#endif

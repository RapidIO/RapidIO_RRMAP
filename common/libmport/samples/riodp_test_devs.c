/*
 * Copyright 2015 Integrated Device Technology, Inc.
 *
 * User-space RapidIO kernel device object creation/removal test program.
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License(GPL) Version 2, or the BSD-3 Clause license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file riodp_test_devs.c
 * \brief RapidIO kernel device object creation/removal test program.
 */

#include <stdio.h>
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

#include "string_util.h"
#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Max device name size in characters.
#define RIODP_MAX_DEV_NAME_SZ 20

static riomp_mport_t mport_hnd;
static did_val_t tgt_did_val = 0;
static hc_t tgt_hop = HC_MP;
static ct_t comptag = 0;

static char dev_name[RIODP_MAX_DEV_NAME_SZ + 1];

static void usage(char *program)
{
	printf("%s - test device object creation/removal\n", program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("Options are:\n");
	printf("  --help (or -h)\n");
	printf("  -M mport_id\n");
	printf("  --mport mport_id\n");
	printf("    local mport device index (default 0)\n");
	printf("  -c create device using provided parameters (-D, -H, -T and -N)\n");
	printf("  -d delete device using provided parameters (-D, -H, -T and -N)\n");
	printf("  -D xxxx\n");
	printf("  --destid xxxx\n");
	printf("    destination ID of target RapidIO device (default 0)\n");
	printf("  -H xxxx\n");
	printf("  --hop xxxx\n");
	printf("    hop count to target RapidIO device (default 0xff)\n");
	printf("  -T xxxx\n");
	printf("  --tag xxxx\n");
	printf("    component tag of target RapidIO device (default unset)\n");
	printf("  -N <device_name>\n");
	printf("  --name <device_name>\n");
	printf("    RapidIO device name (default the empty string)\n");
	printf("\n");
}

/**
 * \brief Called by main() when device object create operation is requested
 *
 * Calls mport API function to create kernel space device object. New device
 * object will be created in rapidio-specific sysfs location:
 * "/sys/bus/rapidio/devices".
 * The device object will have sysfs attributes compatible to ones created
 * by kernel mode enumeration. If device name is not provided as a command
 * line parameter it will be generated automatically according to rapidio
 * device name format defined by kernel enumeration.
 */
void test_create(void)
{
	int ret;

	ret = riomp_mgmt_device_add(mport_hnd, tgt_did_val, tgt_hop, comptag,
			(*dev_name == '\0') ? NULL : dev_name);
	if (ret) {
		printf("Failed to create device object, err=%d\n", ret);
	}
}

/**
 * \brief Called by main() when device object delete operation is requested
 *
 * Calls mport API function to delete kernel space device object. The device
 * object will be deleted regardless of how it was created: by kernel-space
 * enumerator or user-space application using create API (see test_create()).
 */
void test_delete(void)
{
	int ret;

	ret = riomp_mgmt_device_del(mport_hnd, tgt_did_val, tgt_hop, comptag,
			(*dev_name == '\0') ? NULL : dev_name);
	if (ret) {
		printf("Failed to delete device object, err=%d\n", ret);
	}
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

	// command line parameters, all optional
	uint32_t mport_id = 0;
	int do_create = 0;
	int do_delete = 0;
	int discovered = 0;
	uint32_t regval = 0;
	static const struct option options[] = {
			{"mport", required_argument, NULL, 'M'},
			{"destid", required_argument, NULL, 'D'},
			{"hop", required_argument, NULL, 'H'},
			{"tag", required_argument, NULL, 'T'},
			{"name", required_argument, NULL, 'N'},
			{"debug", no_argument, NULL, 'd'},
			{"help", no_argument, NULL, 'h'},
	};

	struct riomp_mgmt_mport_properties prop;
	int rc = EXIT_SUCCESS;
	int err;

	/** Parse command line options, if any */
	while (-1
			!= (c = getopt_long_only(argc, argv, "dhcM:D:H:T:N:",
					options, NULL))) {
		switch (c) {
		case 'M':
			if (tok_parse_mport_id(optarg, &mport_id, 0)) {
				printf(TOK_ERR_MPORT_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'D':
			if (tok_parse_did(optarg, &tgt_did_val, 0)) {
				printf(TOK_ERR_DID_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'H':
			if (tok_parse_hc(optarg, &tgt_hop, 0)) {
				printf(TOK_ERR_HC_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'T':
			if (tok_parse_ct(optarg, &comptag, 0)) {
				printf(TOK_ERR_CT_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'N':
			SAFE_STRNCPY(dev_name, optarg, sizeof(dev_name));
			break;
		case 'c':
			do_create = 1;
			break;
		case 'd':
			do_delete = 1;
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

	if (do_create && do_delete) {
		printf(
				"%s: Unable to create and delete device object simultaneously\n",
				program);
		exit(EXIT_FAILURE);
	}

	/** Open mport device and query RapidIO link status. Exit if link is not active */
	err = riomp_mgmt_mport_create_handle(mport_id, 0, &mport_hnd);
	if (err < 0) {
		printf("DMA Test: unable to open mport%d device err=%d\n",
				mport_id, err);
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

	err = riomp_mgmt_lcfg_read(mport_hnd, 0x13c, sizeof(uint32_t), &regval);
	if (err) {
		printf("Failed to read from PORT_GEN_CTL_CSR, err=%d\n", err);
		rc = EXIT_FAILURE;
		goto out;
	}

	if (regval & 0x20000000) {
		discovered = 1;
	} else {
		printf("ATTN: Port DISCOVERED flag is not set\n");
	}

	if (discovered && (RIO_LAST_DEV16 == prop.did_val)) {
		err = riomp_mgmt_lcfg_read(mport_hnd, RIO_DEVID,
				sizeof(uint32_t), &regval);
		if (err) {
			printf("Failed to read from RIO_DEVID, err=%d\n", err);
			rc = EXIT_FAILURE;
			goto out;
		}

		prop.did_val = (regval >> 16) & RIO_LAST_DEV8;
		err = riomp_mgmt_destid_set(mport_hnd, prop.did_val);
		if (err) {
			printf("Failed to update local destID, err=%d\n", err);
		} else {
			printf("Updated destID=0x%x\n", prop.did_val);
		}
	}

	/** - Perform the specified operation. */
	printf("[PID:%d]\n", (int)getpid());
	if (do_create) {
		printf("+++ Create RapidIO device object as specified +++\n");
		printf("\tmport%d destID=0x%x hop_count=%d CTag=0x%x", mport_id,
				tgt_did_val, tgt_hop, comptag);
		if (strlen(dev_name)) {
			printf(" name=%s\n", dev_name);
		} else {
			printf("\n");
		}

		test_create();

	} else if (do_delete) {
		printf("+++ Delete RapidIO device object as specified +++\n");
		printf("\tmport%d destID=0x%x hop_count=%d CTag=0x%x", mport_id,
				tgt_did_val, tgt_hop, comptag);
		if (strlen(dev_name)) {
			printf(" name=%s\n", dev_name);
		} else {
			printf("\n");
		}

		test_delete();
	} else
		printf("Please specify the action to perform\n");

out:
	riomp_mgmt_mport_destroy_handle(&mport_hnd);
	exit(rc);
}

#ifdef __cplusplus
}
#endif

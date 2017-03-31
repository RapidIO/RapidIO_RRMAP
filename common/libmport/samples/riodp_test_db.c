/*
 * Copyright 2015 Integrated Device Technology, Inc.
 *
 * User-space RapidIO DoorBell exchange test program.
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
 * \file riodp_test_db.c
 * \brief Test program that demonstrates sending and receiving RapidIO doorbells.
 */

#include <stdio.h>
#include <stdint.h>
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

#include "rio_misc.h"
#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_mgmt.h"

#ifdef __cplusplus
extern "C" {
#endif

static int debug = 0;
static int exit_no_dev;

static volatile sig_atomic_t rcv_exit;
static volatile sig_atomic_t report_status;

static void usage(char *program)
{
	printf("%s - test RapidIO DoorBell exchange\n", program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("Options are:\n");
	printf("  --help (or -h)\n");
	printf("Sender:\n");
	printf("  -D xxxx\n");
	printf("  --destid xxxx\n");
	printf("    destination ID of sending RapidIO device [mandatory]\n");
	printf("  -M mport_id\n");
	printf("  --mport mport_id\n");
	printf("    local mport device index (default 0)\n");
	printf("  -I xxxx\n");
	printf("    DoorBell Info field value (default 0x5a5a)\n");
	printf("Receiver:\n");
	printf("  -D xxxx\n");
	printf("  --destid xxxx\n");
	printf("    destination ID of receiving RapidIO device (default any)\n");
	printf("  -r run in DB receiver mode\n");
	printf("  -n run receiver in non-blocking mode\n");
	printf("  -S xxxx\n");
	printf("    start of doorbell range (default 0x5a5a)\n");
	printf("  -E xxxx\n");
	printf("    end of doorbell range (default 0x5a5a)\n");
	printf("\n");
}

static void db_sig_handler(int signum)
{
	switch (signum) {
	case SIGTERM:
	case SIGINT:
		rcv_exit = 1;
		break;
	case SIGUSR1:
		report_status = 1;
		break;
	}
}

/**
 * \brief Called by main() when DB receive mode is specified
 *
 * \param[in] hnd mport device handle
 * \param[in] did_val sender RapidIO destination ID
 * \param[in] start doorbell range start
 * \param[in] end doorbell range end
 *
 * \retval 0 means success
 * \retval Not 0 means failure
 *
 * Performs the following steps:
 *
 */
int do_dbrcv_test(riomp_mport_t hnd, did_val_t did_val, uint16_t start,
		uint16_t end)
{
	int ret;
	struct riomp_mgmt_event evt;

	/** - enable receiving doorbells in specified range */
	ret = riomp_mgmt_dbrange_enable(hnd, did_val, start, end);
	if (ret) {
		printf("Failed to enable DB range, err=%d\n", ret);
		return ret;
	}

	/** - until terminated receive and display doorbell information */
	while (!rcv_exit) {
		if (exit_no_dev) {
			printf(">>> Device removal signaled <<<\n");
			break;
		}

		ret = riomp_mgmt_get_event(hnd, &evt);
		if (ret < 0) {
			if (ret == -EAGAIN) {
				continue;
			}
			else {
				printf("Failed to read event, err=%d\n", ret);
				break;
			}
		}

		if (evt.header == RIO_EVENT_DOORBELL) {
			printf("\tDB 0x%04x from destID %d\n",
					evt.u.doorbell.payload,
					evt.u.doorbell.did_val);
		} else {
			printf("\tIgnoring event type %d)\n", evt.header);
		}
	}

	/** - on exit, disable specified doorbell range */
	ret = riomp_mgmt_dbrange_disable(hnd, did_val, start, end);
	if (ret) {
		printf("Failed to disable DB range, err=%d\n", ret);
		return ret;
	}
	return 0;
}

/**
 * \brief Called by main() when in DB send mode
 *
 * \param[in] hnd mport device handle
 * \param[in] did_val target's RapidIO destination ID
 * \param[in] dbval doorbell info field value
 *
 * \retval 0 means success
 * \retval Not 0 means failure
 *
 * Performs the following steps:
 *
 */
int do_dbsnd_test(riomp_mport_t hnd, did_val_t did_val, uint16_t dbval)
{
	struct riomp_mgmt_event evt;
	int ret = 0;

	evt.header = RIO_EVENT_DOORBELL;
	evt.u.doorbell.did_val = did_val;
	evt.u.doorbell.payload = dbval;

	/** - send a single doorbell message to a target device */
	ret = riomp_mgmt_send_event(hnd, &evt);
	if (ret < 0) {
		printf("Write DB event failed, err=%d\n", ret);
	}
	return ret;
}

static void test_sigaction(int UNUSED(sig), siginfo_t *siginfo,
		void *UNUSED(context))
{
	printf("SIGIO info PID: %ld, UID: %ld CODE: 0x%x BAND: 0x%lx FD: %d\n",
			(long)siginfo->si_pid, (long)siginfo->si_uid,
			siginfo->si_code, siginfo->si_band, siginfo->si_fd);
	exit_no_dev = 1;
}

/**
 * \brief Starting point for the test program
 *
 * \param[in] argc Command line parameter count
 * \param[in] argv Array of pointers to command line parameter null terminated
 *                 strings
 *
 * \retval 0 means success
 *
 * Parses command line parameters and performs doorbell send or receive operation
 * depending on specified mode.
 */
int main(int argc, char** argv)
{
	int c;
	char *program = argv[0];

	// command line parameters, all optional
	uint32_t mport_id = 0;
	did_val_t did_val = UINT32_MAX;
	uint32_t db_info = 0x5a5a;
	uint32_t db_start = 0x5a5a;
	uint32_t db_end = 0x5a5a;

	static const struct option options[] = {
			{"destid", required_argument, NULL, 'D'},
			{"mport", required_argument, NULL, 'M'},
			{"debug", no_argument, NULL, 'd'},
			{"help", no_argument, NULL, 'h'},
	};

	riomp_mport_t mport_hnd;
	int flags = 0;
	int do_dbrecv = 0;

	struct riomp_mgmt_mport_properties prop;
	struct sigaction action;
	int rc = EXIT_SUCCESS;

	/** Parse command line options, if any */
	while (-1
			!= (c = getopt_long_only(argc, argv, "rdhnD:I:M:S:E:",
					options, NULL))) {
		switch (c) {
		case 'D':
			if (tok_parse_did(optarg, &did_val, 0)) {
				printf(TOK_ERR_DID_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'r':
			do_dbrecv = 1;
			break;
		case 'n':
			flags = O_NONBLOCK;
			break;
		case 'I':
			if (tok_parse_ul(optarg, &db_info, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "DoorBell Info");
				exit(EXIT_FAILURE);
			}
			break;
		case 'M':
			if (tok_parse_mport_id(optarg, &mport_id, 0)) {
				printf(TOK_ERR_MPORT_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			if (tok_parse_ul(optarg, &db_start, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"DoorBell start");
				exit(EXIT_FAILURE);
			}
			break;
		case 'E':
			if (tok_parse_ul(optarg, &db_end, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "DoorBell end");
				exit(EXIT_FAILURE);
			}
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

	// set default for receive if value not provided
	if (do_dbrecv && (UINT32_MAX == did_val)) {
		did_val = RIO_LAST_DEV8;
	}
	if (did_val > RIO_LAST_DEV8) {
		printf("Please specify a %s destination Id\n",
				do_dbrecv ? "receive" : "transmit");
	}

	memset(&action, 0, sizeof(action));
	action.sa_sigaction = test_sigaction;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGIO, &action, NULL);

	rc = riomp_mgmt_mport_create_handle(mport_id, flags, &mport_hnd);
	if (rc < 0) {
		printf("DB Test: unable to open mport%d device err=%d\n",
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

	/* Trap signals that we expect to receive */
	signal(SIGINT, db_sig_handler);
	signal(SIGTERM, db_sig_handler);
	signal(SIGUSR1, db_sig_handler);

	riomp_mgmt_set_event_mask(mport_hnd, RIO_EVENT_DOORBELL);

	if (do_dbrecv) {
		printf("+++ RapidIO Doorbell Receive Mode +++\n");
		printf("\tmport%d PID:%d\n", mport_id, (int)getpid());
		printf("\tfilter: destid=%x start=%x end=%x\n", did_val,
				db_start, db_end);

		do_dbrcv_test(mport_hnd, did_val, db_start, db_end);
	} else {
		printf("+++ RapidIO Doorbell Send +++\n");
		printf("\tmport%d destID=%d db_info=0x%x\n", mport_id,
				did_val, db_info);

		do_dbsnd_test(mport_hnd, did_val, db_info);
	}

out:
	riomp_mgmt_mport_destroy_handle(&mport_hnd);
	exit(rc);
}

#ifdef __cplusplus
}
#endif

/*
 * Copyright 2015 Integrated Device Technology, Inc.
 *
 * User-space RapidIO Port-Write event test program.
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
 * \file
 * \brief RapidIO port-write notifications receive test program.
 *
 * This program receives inbound RapidIO port-write messages according to specified
 * filtering options and displays contents of the message. This program demonstrates
 * reading port-write events in both: blocking and non-blocking modes.
 */

#include <stdio.h>
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

#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

static int debug = 0;

static volatile sig_atomic_t rcv_exit;
static volatile sig_atomic_t report_status;

static void usage(char *program)
{
	printf("%s - test RapidIO PortWrite events\n", program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("options are:\n");
	printf("  --help (or -h)\n");
	printf("  -M mport_id\n");
	printf("  --mport mport_id\n");
	printf("    local mport device index (default 0)\n");
	printf("  -m xxxx\n");
	printf("    mask (default 0xffffffff)\n");
	printf("  -L xxxx\n");
	printf("    low filter value (default 0)\n");
	printf("  -H xxxx\n");
	printf("    high filter value (default 0xffffffff)\n");
	printf("  -n run in non-blocking mode\n");
	/*printf("  --debug (or -d)\n");*/
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
 * \brief This function executes RapidIO port-write event read-and-display loop until
 * termination by a signal.
 *
 * \param[in] hnd mport device handle
 * \param[in] mask Port-write source Component Tag (CT) mask
 * \param[in] low  Low boundary of masked CT range
 * \param[in] high Upper boundary of masked CT range
 *
 * \return 0 if successfull or error code returned by mport API.
 *
 * Performs the following steps:
 */
int do_pwrcv_test(riomp_mport_t hnd, uint32_t mask, uint32_t low, uint32_t high)
{
	int ret;
	struct riomp_mgmt_event evt;
	unsigned long pw_count = 0, ignored_count = 0;

	/** * Enable port-write events range (based ob source Component Tag filtering) */
	ret = riomp_mgmt_pwrange_enable(hnd, mask, low, high);
	if (ret) {
		printf("Failed to enable PW filter, err=%d\n", ret);
		return ret;
	}

	while (!rcv_exit) { /// * Enter read-and-display loop

		if (report_status) {
			printf("port writes count: %lu\n", pw_count);
			printf("ignored events count: %lu\n", ignored_count);
			report_status = 0;
		}

		/** - Read pending event */
		ret = riomp_mgmt_get_event(hnd, &evt);
		if (ret < 0) {
			if (ret == -EAGAIN) {
				continue;
			} else {
				printf("Failed to read event, err=%d\n", ret);
				break;
			}
		}

		/** - If new event is PW, display payload info. Ignore non-PW events. */
		if (evt.header == RIO_EVENT_PORTWRITE) {
			int i;

			printf("\tPort-Write message:\n");
			for (i = 0; i < 16; i += 4) {
				printf("\t0x%02x: 0x%08x %08x %08x %08x\n",
						i * 4,
						evt.u.portwrite.payload[i],
						evt.u.portwrite.payload[i + 1],
						evt.u.portwrite.payload[i + 2],
						evt.u.portwrite.payload[i + 3]);
			}
			printf("\n");
			pw_count++;
		} else {
			printf("\tIgnoring event type %d)\n", evt.header);
			ignored_count++;
		}
	} /// - Repeat loop until terminated

	/** * Disable port-write events range before exit */
	ret = riomp_mgmt_pwrange_disable(hnd, mask, low, high);
	if (ret) {
		printf("Failed to disable PW range, err=%d\n", ret);
		return ret;
	}
	return 0;
}

/**
 * \brief Starting point of the program
 *
 * \param[in] argc Command line parameter count
 * \param[in] argv Array of pointers to command line parameter null terminated
 *                 strings
 *
 * \retval 0 means success
 *
 * Performs the following steps:
 */
int main(int argc, char** argv)
{
	int c;
	char *program = argv[0];

	// command line parameters
	uint32_t mport_id = 0;
	uint32_t pw_mask = 0xffffffff;
	uint32_t pw_low = 0;
	uint32_t pw_high = 0xffffffff;
	int flags = 0;
	static const struct option options[] = {
			{"mport", required_argument, NULL, 'M'},
			{"debug", no_argument, NULL, 'd'},
			{"help",no_argument, NULL, 'h'},
	};

	struct riomp_mgmt_mport_properties prop;
	riomp_mport_t mport_hnd;
	unsigned int evt_mask;
	int err;
	int rc = EXIT_SUCCESS;

	/** Parse command line options, if any */
	while (-1
			!= (c = getopt_long_only(argc, argv, "dhnm:M:L:H:",
					options, NULL))) {
		switch (c) {
		case 'm':
			if (tok_parse_ul(optarg, &pw_mask, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "Mask");
				return (EXIT_FAILURE);
			}
			break;
		case 'n':
			flags = O_NONBLOCK;
			break;
		case 'M':
			if (tok_parse_mport_id(optarg, &mport_id, 0)) {
				printf(TOK_ERR_MPORT_MSG_FMT);
				return (EXIT_FAILURE);
			}
			break;
		case 'L':
			if (tok_parse_ul(optarg, &pw_low, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"Low filter value");
				return (EXIT_FAILURE);
			}
			break;
		case 'H':
			if (tok_parse_ul(optarg, &pw_high, 0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT,
						"High filter value");
				return (EXIT_FAILURE);
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

	/** - Create handle for selected mport */
	rc = riomp_mgmt_mport_create_handle(mport_id, flags, &mport_hnd);
	if (rc < 0) {
		printf("DB Test: unable to open mport%d device err=%d\n",
				mport_id, rc);
		exit(EXIT_FAILURE);
	}

	/** - Query mport status information and verify SRIO link activity */
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

	/** - Trap signals that we expect to receive */
	signal(SIGINT, db_sig_handler);
	signal(SIGTERM, db_sig_handler);
	signal(SIGUSR1, db_sig_handler);

	/** - Read currently active event mask */
	err = riomp_mgmt_get_event_mask(mport_hnd, &evt_mask);
	if (err) {
		printf("Failed to obtain current event mask, err=%d\n", err);
		rc = EXIT_FAILURE;
		goto out;
	}

	/** - Update event mask to enable port-write events */
	riomp_mgmt_set_event_mask(mport_hnd, evt_mask | RIO_EVENT_PORTWRITE);

	printf("+++ RapidIO PortWrite Event Receive Mode +++\n");
	printf("\tmport%d PID:%d\n", mport_id, (int)getpid());
	printf("\tfilter: mask=%x low=%x high=%x\n", pw_mask, pw_low, pw_high);

	/** - Execute port-write test function (until terminated) */
	do_pwrcv_test(mport_hnd, pw_mask, pw_low, pw_high);

	/** - Restore the original event mask */
	riomp_mgmt_set_event_mask(mport_hnd, evt_mask);

out:
	/** - Close the mport handle */
	riomp_mgmt_mport_destroy_handle(&mport_hnd);
	exit(rc);
}

#ifdef __cplusplus
}
#endif

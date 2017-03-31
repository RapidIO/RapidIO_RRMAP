/* Fabric Management Daemon Configuration file and options parsing support */
/*
****************************************************************************
Copyright (c) 2014, Integrated Device Technology Inc.
Copyright (c) 2014, RapidIO Trade Association
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
#include <ctype.h>
#include <stdint.h>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <errno.h>

#include "tok_parse.h"
#include "fmd_dd.h"
#include "cfg.h"
#include "libcli.h"
#include "liblog.h"
#include "fmd_opts.h"

#ifdef __cplusplus
extern "C" {
#endif

void fmd_print_help(void)
{
	printf("\nThe RapidIO Fabric Management Daemon (\"FMD\") manages a\n");
	printf("RapidIO fabric.\n");
	printf("Options are:\n");
	printf("-a, -A <port>: POSIX Ethernet socket for App connections.\n");
	printf("       Default is %d\n", FMD_DFLT_APP_PORT_NUM);
	printf("-c, -C <filename>: FMD configuration file name.\n");
	printf("       Default is \"%s\"\n", FMD_DFLT_CFG_FN);
	printf("-d, -D <filename>: Device directory Posix SM file name.\n");
	printf("       Default is \"%s\"\n", FMD_DFLT_DD_FN);
	printf("-h, -H, -?: Print this message.\n");
	printf("-i <interval>: Interval between Device Directory updates.\n");
	printf("       Default is %d\n", FMD_DFLT_MAST_INTERVAL);
	printf("-l, -L <level>: Set starting logging level.\n");
	printf("       Default is %x\n",FMD_DFLT_LOG_LEVEL); 
	printf("-m, -M <filename>: Device directory Mutex SM file name.\n");
	printf("       Default is \"%s\"\n", FMD_DFLT_DD_MTX_FN);
	printf("-n, -N: Do not start console CLI.\n");
	printf("-p <port>: POSIX Ethernet socket for remote CLI.\n");
	printf("       Default is %d\n", FMD_DFLT_CLI_SKT);
	printf("-s, -S: Simple initialization, do not populate device dir.\n");
	printf("       Default is %d\n", FMD_DFLT_INIT_DD);
	printf("-x, -X: Initialize and then immediately exit.\n");
}

struct fmd_opt_vals *fmd_parse_options(int argc, char *argv[])
{
	int c;

	char *dflt_fmd_cfg = (char *)FMD_DFLT_CFG_FN;
	struct fmd_opt_vals *opts;

	opts = (struct fmd_opt_vals *)calloc(1, sizeof(struct fmd_opt_vals));
	if (NULL == opts) {
		goto oom;
	}

	opts->init_err = 0;
	opts->init_and_quit = 0;
	opts->simple_init = 0;
	opts->print_help = 0;
	opts->cli_port_num = FMD_DFLT_CLI_SKT;
	opts->app_port_num = FMD_DFLT_APP_PORT_NUM;
	opts->run_cons = 1;
	opts->log_level = FMD_DFLT_LOG_LEVEL;
	opts->mast_mode = 0;
	opts->mast_interval = FMD_DFLT_MAST_INTERVAL;
	opts->mast_did = (did_t){FMD_DFLT_MAST_DEVID, dev08_sz};
	opts->mast_cm_port = FMD_DFLT_MAST_CM_PORT;

	if (update_string(&opts->fmd_cfg, dflt_fmd_cfg, strlen(dflt_fmd_cfg))) {
		goto oom;
	}

	while (-1 != (c = getopt(argc, argv, "hH?nNsSxXa:A:c:C:d:D:i:I:l:L:m:M:p:P:"))) {
		switch (c) {
		case 'a':
		case 'A':
			if (tok_parse_socket(optarg, &opts->app_port_num, 0)) {
				printf(TOK_ERR_SOCKET_MSG_FMT, "App connection port");
				exit(EXIT_FAILURE);
			}
			break;
		case 'c':
		case 'C':
			if (get_v_str(&opts->fmd_cfg, optarg, 0)) {
				printf("\nInvalid FMD configuration file name.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
		case 'D':
			if (get_v_str(&opts->dd_fn, optarg, 1)) {
				printf("\nInvalid device directory file name.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'h':
		case 'H':
			goto print_help;
		case 'i':
		case 'I':
			if (tok_parse_ul(optarg, &opts->mast_interval,
								0)) {
				printf(TOK_ERR_UL_HEX_MSG_FMT, "Interval");
				exit(EXIT_FAILURE);
			}
			break;
		case 'l':
		case 'L':
			if (tok_parse_log_level(optarg,
						&opts->log_level, 0)) {
				printf(TOK_ERR_LOG_LEVEL_MSG_FMT);
				exit(EXIT_FAILURE);
			}
			break;
		case 'm':
		case 'M':
			if (get_v_str(&opts->dd_mtx_fn, optarg, 1)) {
				printf("\nInvalid device directory mutex file name.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'n':
		case 'N':
			opts->run_cons = 0;
			break;

		case 'p':
		case 'P':
			if (tok_parse_socket(optarg,
					&opts->cli_port_num, 0)) {
				printf(TOK_ERR_SOCKET_MSG_FMT, "Remote CLI port");
				exit(EXIT_FAILURE);
			}
			break;
		case 's':
		case 'S':
			opts->simple_init = 1;
			break;
		case 'x':
		case 'X':
			opts->init_and_quit = 1;
			break;
//		case '?':
		default:
			/* Invalid command line option */
			if (!isprint(optopt)) {
				printf("Unknown option '-%c\n", optopt);
			}
			fmd_print_help();
			exit(EXIT_FAILURE);
		}
	}
	return opts;

print_help:
	opts->init_and_quit = 1;
	opts->print_help = 1;
	fmd_print_help();
	return opts;

oom:
	free(opts);
	return NULL;
}

#ifdef __cplusplus
}
#endif


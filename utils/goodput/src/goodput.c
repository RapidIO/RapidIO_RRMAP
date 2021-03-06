/*
****************************************************************************
Copyright (c) 2015, Integrated Device Technology Inc.
Copyright (c) 2015, RapidIO Trade Association
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#include "rio_misc.h"
#include "tok_parse.h"
#include "libcli.h"
#include "liblog.h"
#include "worker.h"
#include "goodput.h"
#include "goodput_cli.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"
#include "rapidio_mport_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

riomp_mport_t mp_h = NULL;
int mp_h_valid;
int mp_h_num = -1;
int mp_h_qresp_valid;
struct riomp_mgmt_mport_properties qresp;

struct worker wkr[MAX_WORKERS];

void goodput_thread_shutdown(struct cli_env *UNUSED(env))
{
	printf("\nGoodput Evaluation Application EXITING!!!!\n");
	exit(EXIT_SUCCESS);
}

int setup_mport(int mport_num)
{
	int rc;

	if (mp_h_valid) {
		riomp_mgmt_mport_destroy_handle(&mp_h);
		mp_h_valid = 0;
	}

	mp_h_num = mport_num;
	rc = riomp_mgmt_mport_create_handle(mport_num, 0, &mp_h);
	if (!rc)
		mp_h_valid = 1;

	rc = riomp_mgmt_query(mp_h, &qresp);
	if (!rc)
		mp_h_qresp_valid = 1;

	return rc;
}

void sig_handler(int signo)
{
	switch (signo) {
	case SIGINT:
	case SIGHUP:
	case SIGTERM:
	case SIGUSR1:
		goodput_thread_shutdown(NULL);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	uint32_t mport_num = 0;

	char* rc_script = NULL;
	struct cli_env t_env;

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);

	if ((argc > 1) && (tok_parse_mport_id(argv[1], &mport_num, 0))) {
		printf(TOK_ERR_MPORT_MSG_FMT);
		exit(EXIT_FAILURE);
	}

	for (int n = 2; n < argc; n++) {
		const char* arg = argv[n];
		if (!strcmp(arg, "--rc")) {
			if (n == (argc - 1)) {
				continue;
			}
			rc_script = argv[++n];
			continue;
		}
		if (!strstr(arg, "=")) {
			continue;
		}
		SetEnvVar((char *)arg);
	}

	rdma_log_init("goodput_log.txt", 1);
	if (setup_mport(mport_num)) {
		printf("\nCould not open mport %d, exiting\n", mport_num);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < MAX_WORKERS; i++)
		init_worker_info(&wkr[i], 1);

	riomp_sock_mbox_init();
	cli_init_base(goodput_thread_shutdown);
	bind_goodput_cmds();
	liblog_bind_cli_cmds();

	char script_path[10] = {0};
	snprintf(script_path, sizeof(script_path), "mport%d", mport_num);
	set_script_path(script_path);

	init_cli_env(&t_env);
	splashScreen(&t_env, (char *)"Goodput Evaluation Application");

	ConsoleRc_t crc;
	crc.prompt = (char *)"GoodPut> ";
	crc.script = rc_script;

	console_rc((void *)&crc);

	goodput_thread_shutdown(NULL);

	exit(EXIT_SUCCESS);
}

#ifdef __cplusplus
}
#endif

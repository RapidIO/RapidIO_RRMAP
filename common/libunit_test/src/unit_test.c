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

#include "libcli.h"
#include "liblog.h"
#include "libunit_test.h"
#include "libunit_test_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

struct worker wkr[MAX_WORKERS];

int drvr_valid = 0;
struct unit_test_driver drvr;

void config_unit_test(struct unit_test_driver *drv)
{
	int i;

	memcpy(&drvr, (void *)drv, sizeof(struct unit_test_driver));
	drvr_valid = 1;

	for (i = 0; i < MAX_WORKERS; i++) {
		init_worker_info(&wkr[i], 1);
		wkr[i].idx = i;
	};
};


void unit_test_thread_shutdown(struct cli_env *env)
{
	int i;

	if (0)
		env = env + 1;

	for (i = 0; i < MAX_WORKERS; i++)
		kill_worker_thread(&wkr[i]);
};

void sig_handler(int signo)
{
	printf("\nRx Signal %x\n", signo);
	if ((signo == SIGINT) || (signo == SIGHUP) || (signo == SIGTERM)) {
		printf("Shutting down\n");
		unit_test_thread_shutdown(NULL);
	};
};

void run_unit_test_cli(char *title_str, char *prompt_str)
{
	int rc = EXIT_FAILURE;
	char prompt[25];
	struct cli_env t_env;

	if ((NULL == title_str) || (NULL == prompt_str))
		return;

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);

        cli_init_base(unit_test_thread_shutdown);
        bind_unit_test_thread_cli_cmds();
	init_cli_env(&t_env);
	splashScreen(&t_env, title_str);

	snprintf(prompt, 24, "%10s >", prompt_str);

	console((void *)(prompt));

	unit_test_thread_shutdown(NULL);

	printf("\nUnit Test EXITING!!!!\n");
	rc = EXIT_SUCCESS;

	exit(rc);
}

#ifdef __cplusplus
}
#endif

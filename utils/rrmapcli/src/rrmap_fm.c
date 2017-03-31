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

/* Implementation of the RDMA Socket Daemon fabric management thread */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#include "rio_misc.h"
#include "libfmdd.h"
#include "liblog.h"
#include "libcli.h"
#include "libtime_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

sem_t fm_started;
uint32_t fm_alive;
uint32_t fm_must_die;
int fm_thread_valid;
pthread_t fm_thread;
fmdd_h dd_h;

sem_t did_sem;
uint32_t did_list_sz;
uint32_t *did_list;

/* Sends requests and responses to all apps */
void *fm_loop(void *unused)
{
	const struct timespec delay = {3 * FMD_DFLT_MAST_INTERVAL, 0}; // seconds

	uint32_t l_did_list_sz;
	uint32_t *l_did_list;
	uint32_t rc;
	char my_name[16];

	if (-1 == sem_init(&did_sem, 0, 1)) {
		CRIT("fm_loop: sem_init()");
		pthread_exit(0);
	}

	memset(my_name, 0, 16);
	snprintf(my_name, 15, "RRMAP_FMDD");
	pthread_setname_np(fm_thread, my_name);

	fm_alive = 1;
	INFO("FM is alive!\n");

	sem_post(&fm_started);

	do {
		dd_h = fmdd_get_handle((char *)"RRMAPCLI", FMDD_RRMAP_CLI_FLAG);
		if (dd_h == NULL) {
			INFO("Sleeping...");
			time_sleep(&delay);
			INFO("Waking up and trying again...");
			continue;
		}

		do {
			rc = fmdd_get_did_list(dd_h, &l_did_list_sz,
								&l_did_list);
			if (rc) {
				CRIT("Failed to get DID list from FM.\n");
				break;
			} else {
				INFO("Got DID list, size %d", did_list_sz);
			}
			sem_wait(&did_sem);
			fmdd_free_did_list(dd_h, &did_list);
			did_list = l_did_list;
			did_list_sz = l_did_list_sz;
			sem_post(&did_sem);
	
			INFO("Waiting for FM to report change to RSKTD\n");
				if (fmdd_wait_for_dd_change(dd_h)) {
				ERR("fmdd_wait_for_dd_change\n");
				break;
			}
		} while (!fm_must_die && (NULL != dd_h));
	} while(!fm_must_die);

	fm_alive = 0;
	pthread_exit(unused);
}

int start_fm_thread(void)
{
	int ret;

	/* Prepare and start library connection handling threads */

	sem_init(&fm_started, 0, 0);
	fm_alive = 0;
	fm_must_die = 0;
	fm_thread_valid = 0;

	ret = pthread_create(&fm_thread, NULL, fm_loop, NULL);
	if (ret) {
		printf("Error - fm_thread rc: %d\n", ret);
	} else {
		sem_wait(&fm_started);
		fm_thread_valid = 1;
	}

	return ret;
}

void halt_fm_thread(void)
{
	fm_must_die = 1;
	fmdd_destroy_handle(&dd_h);
	if (fm_thread_valid) {
		fm_thread_valid = 0;
		pthread_kill(fm_thread, SIGUSR1);
		pthread_join(fm_thread, NULL);
	}
}

void rrmap_fm_custom_quit(struct cli_env *env)
{
	(void)env;
	fmdd_destroy_handle(&dd_h);
	exit(EXIT_SUCCESS);
}

#define MIN(a, b) ((a < b) ? a : b)

// Display differences between current did list and previously cached list
int CLIDIDListCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	uint32_t l_did_list_sz;
	uint32_t *l_did_list;
	uint32_t size;
	uint32_t idx;
	bool no_changes = true;
	int rc;

	rc = fmdd_get_did_list(dd_h, &l_did_list_sz, &l_did_list);
	if (rc) {
		LOGMSG(env, "\nFailed to get DID list from FM.\n");
		goto exit;
	}

	sem_wait(&did_sem);
	size = MIN(l_did_list_sz, did_list_sz);
	for (idx = 0; idx < size; idx++) {
		if (did_list[idx] != l_did_list[idx]) {
			LOGMSG(env,
					"\nPrevious DID[%u] %0x08x != Current DID[%u] %0x08x\n",
					idx, did_list[idx], idx,
					l_did_list[idx]);
			no_changes = false;
		}
	}

	if ((l_did_list_sz == did_list_sz) && no_changes) {
		LOGMSG(env, "\%u DIDs in use, no changes\n", did_list_sz);
	} else if (l_did_list_sz > did_list_sz) {
		for (idx = did_list_sz; idx < l_did_list_sz; idx++) {
			LOGMSG(env, "\nNew DID[%u] %0x08x\n", idx,
					l_did_list[idx]);
		}
	} else if (l_did_list_sz < did_list_sz) {
		for (idx = l_did_list_sz; idx < did_list_sz; idx++) {
			LOGMSG(env, "\nOld DID[%u] %0x08x\n", idx,
					did_list[idx]);
		}
	}
	fmdd_free_did_list(dd_h, &l_did_list);
	sem_post(&did_sem);

exit:
	return 0;
}


struct cli_cmd CLIDIDList = {
(char *)"dl",
2,
0,
(char *)"(No parameters) Display DID list changes.\n",
(char *)"<No Parameters>\n"
	"Display DID list changes.\n",
CLIDIDListCmd,
ATTR_NONE
};
struct cli_cmd *cli_cmd_arr = &CLIDIDList;

void rrmap_fm_bind_cli_cmds()
{
	add_commands_to_cmd_db(1, &cli_cmd_arr);
}

#ifdef __cplusplus
}
#endif

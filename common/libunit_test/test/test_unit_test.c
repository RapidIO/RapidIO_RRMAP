/*
 * ****************************************************************************
 * Copyright (c) 2014, Integrated Device Technology Inc.
 * Copyright (c) 2014, RapidIO Trade Association
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * *************************************************************************
 * */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

#include "libtime_utils.h"
#include "libunit_test.h"
#include "test_unit_test.h"
#include "liblog.h"
#include "libcli.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @brief Start up one worker thread
 * @return int, 0 for success, 1 for failure
 */
int test_case_1A(void)
{	
	int i = 0;
	int num_cpus = getCPUCount();
	struct test_worker_info *work = NULL;
	char name[16], exp_name[16];

	if (start_worker_thread(&wkr[i], i % num_cpus)) {
		ERR("Worker failed to start.");
		goto fail;
	};

	if (wait_for_worker_status(&wkr[i], worker_halted)) {
		ERR("Worker failed to halt.");
		goto fail;
	};

	if (wkr[i].idx != i)
		goto fail;
	if (wkr[i].stat != worker_halted)
		goto fail;
	if (wkr[i].stop_req != worker_running)
		goto fail;
	if (wkr[i].action != UNIT_TEST_NO_ACTION)
		goto fail;

	memset(exp_name, 0, 16);
	memset(name, 0, 16);
	snprintf(exp_name, 16, "WORKER_%d", wkr[i].idx);
	if (pthread_getname_np(wkr[i].wkr_thr.thr, name, 16))
		goto fail;

	if (strncmp(name, exp_name, 16))
		goto fail;
	
	if (wkr[i].priv_info == NULL)
		goto fail;
	work = (struct test_worker_info *)wkr[i].priv_info;
	if (work->inc_dec_test || work->inc_dec_amt || work->ts_sel)
		goto fail;
	if (work->ts_m)
		goto fail;
	if (work->shared_inc_dec_test != &shared_inc_dec)
		goto fail;
	if (work->shared_sem != &shared_sem)
		goto fail;


	return 0;
fail:
	return 1;
};

/** @brief Run some simple increment/decrement operations within each thread
 * @return int, 0 for success, 1 for failure
 */
int test_case_2A(void)
{
	int i = 0, loop, loop_max = 100;
	struct test_worker_info *work = NULL;

	work = (struct test_worker_info *)wkr[i].priv_info;
	work->inc_dec_amt = i+5;

	for (loop = 0; loop < loop_max; loop++) {
		run_worker_action(&wkr[i], ACTION_INC_VAL);
		sem_wait(&work->action_done);
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
	};

	if (wait_for_worker_status(&wkr[i], worker_halted)) {
		ERR("Worker %d not halted", i);
		goto fail;
	};
	work = (struct test_worker_info *)wkr[i].priv_info;
	if (work->inc_dec_test != (i+5) * loop_max) {
		ERR("Worker %d total %d not %d", i,
			work->inc_dec_test, (i+5) * 1000);
		goto fail;
	};

	for (loop = 0; loop < loop_max; loop++) {
		run_worker_action(&wkr[i], ACTION_DEC_VAL);
		sem_wait(&work->action_done);
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
	};

	if (wait_for_worker_status(&wkr[i], worker_halted)) {
		ERR("Worker %d not halted", i);
		goto fail;
	};
	work = (struct test_worker_info *)wkr[i].priv_info;
	if (work->inc_dec_test) {
		ERR("Worker %d total %d not %d", i,
			work->inc_dec_test, 0);
		goto fail;
	};

	kill_worker_thread(&wkr[i]);
	if (wait_for_worker_status(&wkr[i], worker_dead)) {
		ERR("Worker %d not halted", i);
		goto fail;
	};

	return 0;
fail:
	return 1;
};

int test_case_1(void)
{	
	int i;
	int num_cpus = getCPUCount();
	struct test_worker_info *work = NULL;

	for (i = 0; i < MAX_WORKERS; i++) {
		if (start_worker_thread(&wkr[i], i % num_cpus)) {
			ERR("Worker failed to start.");
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++) {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker failed to halt.");
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++) {
		if (wkr[i].idx != i)
			goto fail;
		if (wkr[i].stat != worker_halted)
			goto fail;
		if (wkr[i].stop_req != worker_running)
			goto fail;
		if (wkr[i].action != UNIT_TEST_NO_ACTION)
			goto fail;
		if (wkr[i].priv_info == NULL)
			goto fail;
		if (wkr[i].priv_info == NULL)
			goto fail;
		work = (struct test_worker_info *)wkr[i].priv_info;
		if (work->inc_dec_test || work->inc_dec_amt || work->ts_sel)
			goto fail;
		if (work->ts_m)
			goto fail;
		if (work->shared_inc_dec_test != &shared_inc_dec)
			goto fail;
		if (work->shared_sem != &shared_sem)
			goto fail;
	};

	return 0;
fail:
	return 1;
};

/** @brief Run some simple increment/decrement operations within each thread
 * @return int, 0 for success, 1 for failure
 */
int test_case_2(void)
{
	int i, loop;
	struct test_worker_info *work = NULL;

	for (i = 0; i < MAX_WORKERS; i++)  {
		work = (struct test_worker_info *)wkr[i].priv_info;
		work->inc_dec_amt = i+5;
	};

	for (loop = 0; loop < 1000; loop++) {
		for (i = 0; i < MAX_WORKERS; i++) {
			work = (struct test_worker_info *)wkr[i].priv_info;
			run_worker_action(&wkr[i], ACTION_INC_VAL);
			sem_wait(&work->action_done);
			if (wait_for_worker_status(&wkr[i], worker_halted)) {
				ERR("Worker %d not halted", i);
				goto fail;
			};
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		work = (struct test_worker_info *)wkr[i].priv_info;
		if (work->inc_dec_test != (i+5) * 1000) {
			ERR("Worker %d total %d not %d", i,
				work->inc_dec_test, (i+5) * 1000);
			goto fail;
		};
	};

	for (loop = 0; loop < 1000; loop++) {
		for (i = 0; i < MAX_WORKERS; i++) {
			work = (struct test_worker_info *)wkr[i].priv_info;
			run_worker_action(&wkr[i], ACTION_DEC_VAL);
			sem_wait(&work->action_done);
			if (wait_for_worker_status(&wkr[i], worker_halted)) {
				ERR("Worker %d not halted", i);
				goto fail;
			};
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		work = (struct test_worker_info *)wkr[i].priv_info;
		if (work->inc_dec_test) {
			ERR("Worker %d total %d not %d", i,
				work->inc_dec_test, 0);
			goto fail;
		};
	};

	return 0;
fail:
	return 1;
};

/** @brief Run some simple parallel increment/decrement operations 
 * @return int, 0 for success, 1 for failure
 */
int test_case_3(void)
{
	int i, loop;
	struct test_worker_info *work = NULL;
	int total = 0;

	for (i = 0; i < MAX_WORKERS; i++)  {
		work = (struct test_worker_info *)wkr[i].priv_info;
		work->inc_dec_amt = i+5;
		work->shared_inc_dec_loop = 1;
	};

	for (loop = 0; loop < 1000; loop++) {
		for (i = 0; i < MAX_WORKERS; i++) {
			run_worker_action(&wkr[i], ACTION_SHR_INC);
		};
		for (i = 0; i < MAX_WORKERS; i++) {
			work = (struct test_worker_info *)wkr[i].priv_info;
			sem_wait(&work->action_done);
			if (wait_for_worker_status(&wkr[i], worker_halted)) {
				ERR("Worker %d not halted", i);
				goto fail;
			};
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		total += ((i+5) * 1000);
	};
	if (shared_inc_dec != total) {
		ERR("Total %d not %d", shared_inc_dec, total);
		goto fail;
	};

	for (loop = 0; loop < 1000; loop++) {
		for (i = 0; i < MAX_WORKERS; i++) {
			run_worker_action(&wkr[i], ACTION_SHR_DEC);
		};
		for (i = 0; i < MAX_WORKERS; i++) {
			work = (struct test_worker_info *)wkr[i].priv_info;
			sem_wait(&work->action_done);
			if (wait_for_worker_status(&wkr[i], worker_halted)) {
				ERR("Worker %d not halted", i);
				goto fail;
			};
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
	};

	if (shared_inc_dec) {
		ERR("Total %d not %d", shared_inc_dec, 0);
		goto fail;
	};

	return 0;
fail:
	return 1;
};
	
/** @brief Test correct operation of time_difference
 * @return int, 0 for success, 1 for failure
 */
int test_case_4(void)
{
	int i;
	int num_cpus = getCPUCount();

	for (i = 0; i < MAX_WORKERS; i++)
		migrate_worker_thread(&wkr[i], (i+1) % num_cpus);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		if (wkr[i].wkr_thr.cpu_req != (i+1) % num_cpus) {
			ERR("Worker %d cpu req %d should be %d", i,
				wkr[i].wkr_thr.cpu_req, (i+1) % num_cpus);
			goto fail;
		};
		if (wkr[i].wkr_thr.cpu_run != wkr[i].wkr_thr.cpu_req) {
			ERR("Worker %d cpu req %d run %d WRONG", i,
				wkr[i].wkr_thr.cpu_req, 
				wkr[i].wkr_thr.cpu_run);
			goto fail;
		};
	};
	for (i = 0; i < MAX_WORKERS; i++)
		migrate_worker_thread(&wkr[i], -1);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		if (wkr[i].wkr_thr.cpu_req != -1) {
			ERR("Worker %d cpu req %d should be %d", i,
				wkr[i].wkr_thr.cpu_req, -1);
			goto fail;
		};
		if (wkr[i].wkr_thr.cpu_run != wkr[i].wkr_thr.cpu_req) {
			ERR("Worker %d cpu req %d run %d WRONG", i,
				wkr[i].wkr_thr.cpu_req, 
				wkr[i].wkr_thr.cpu_run);
			goto fail;
		};
	};

	return 0;
fail:
	return 1;
};
	
/** @brief Test correct operation of halt request
 * @return int, 0 for success, 1 for failure
 */
int test_case_5(void)
{
	int i;

	for (i = 0; i < MAX_WORKERS; i++)
		run_worker_action(&wkr[i], ACTION_HLT_REQ);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_running)) {
			ERR("Worker %d not running", i);
			goto fail;
		};
	};
	for (i = 0; i < MAX_WORKERS; i++)
		halt_worker_thread(&wkr[i]);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)
		run_worker_action(&wkr[i], ACTION_HLT_REQ);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_running)) {
			ERR("Worker %d not running", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)
		kill_worker_thread(&wkr[i]);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_dead)) {
			ERR("Worker %d not dead", i);
			goto fail;
		};
		if (wkr[i].priv_info != NULL) {
			ERR("Worker %d priv info still valid?", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++) {
		if (start_worker_thread(&wkr[i], -1)) {
			ERR("Worker %d could not be restarted", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_halted)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
		if (wkr[i].priv_info == NULL) {
			ERR("Worker %d priv info invalid?", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)
		kill_worker_thread(&wkr[i]);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_dead)) {
			ERR("Worker %d not dead", i);
			goto fail;
		};
		if (wkr[i].priv_info != NULL) {
			ERR("Worker %d priv info still valid?", i);
			goto fail;
		};
	};

	return 0;
fail:
	return 1;
};
	
/** @brief No cleanup required for cli exit.
 */

void no_cli_cleanup(struct cli_env *env)
{
	if (NULL == env)
		return;
	return;
};

/** @brief test of CLI entry and exit.
 */
int test_case_6(void)
{
	struct cli_env env;
	char *test_script = (char *)"scripts/tut.txt";

	cli_init_base(no_cli_cleanup);
	bind_unit_test_thread_cli_cmds();

	env.sess_socket = 0;
	env.fout = NULL;
	snprintf(env.prompt, PROMPTLEN, (char *)"TUT_SCR >");
	env.DebugLevel = 0;
	env.progressState = 0;
	env.h = NULL;
	env.cmd_prev = NULL;

	cli_script(&env, test_script, 0);

	return 0;
};
	
/* Test killing worker on signal...
 */
int test_case_7(void)
{
	int i;

	for (i = 0; i < MAX_WORKERS; i++) {
		if (start_worker_thread(&wkr[i], -1)) {
			ERR("Worker %d could not be restarted", i);
			goto fail;
		};
	};

	for (i = 0; i < MAX_WORKERS; i++)
		run_worker_action(&wkr[i], ACTION_HLT_SEM);

	
	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_running)) {
			ERR("Worker %d not halted", i);
			goto fail;
		};
	};
	
	for (i = 0; i < MAX_WORKERS; i++)
		kill_worker_thread(&wkr[i]);

	for (i = 0; i < MAX_WORKERS; i++)  {
		if (wait_for_worker_status(&wkr[i], worker_dead)) {
			ERR("Worker %d not dead", i);
			goto fail;
		};
		if (wkr[i].stop_req != worker_dead) {
			ERR("Worker %d stop_req not dead", i);
			goto fail;
		};
		if (wkr[i].priv_info != NULL) {
			ERR("Worker %d priv info still valid?", i);
			goto fail;
		};
	};

	return 0;
fail:
	return 1;
};
	
char *action_str(int action)
{
	char *rc = (char *)unknown_str;

	if (action <= ACTION_SHR_DEC)
		rc =  action_strings[action];
	else
		rc = (char *)unknown_str;

	return rc;
};
	
int ts_sel(char *parm)
{
	int rc = -1;

	if (NULL == parm)
		return rc;

	switch (parm[0]) {
	case 'a':
	case 'A':  rc = 0;
		break;
	case 'b':
	case 'B':  rc = 1;
		break;
	case 'c':
	case 'C':  rc = 2;
		break;
	default: rc = -1;
	};

	return rc;
};

int worker_body(struct worker *info)
{
	struct test_worker_info *work = NULL;
	int i, rc;
	sem_t local_sem;

	sem_init(&local_sem, 0, 0);

	if (NULL == info) {
		ERR("Worker info NULL.");
		goto fail;
	}

	if (NULL == info->priv_info) {
		ERR("Worker priv info NULL. Thread exiting.");
		info->stop_req = worker_dead;
		goto fail;
	};

	work = (struct test_worker_info *)info->priv_info;

	switch (info->action) {
	case ACTION_DEC_VAL:
			work->inc_dec_test -= work->inc_dec_amt;
			sem_post(&work->action_done);
			break;
	case ACTION_INC_VAL:
			work->inc_dec_test += work->inc_dec_amt;
			sem_post(&work->action_done);
			break;
	case ACTION_SHR_INC:
			for (i = 0; i < work->shared_inc_dec_loop; i++) {
				ts_now_mark(&info->ts[work->ts_sel], work->ts_m);
				sem_wait(work->shared_sem);
				ts_now_mark(&info->ts[work->ts_sel],
								work->ts_m + 1);
				*work->shared_inc_dec_test += work->inc_dec_amt;
				ts_now_mark(&info->ts[work->ts_sel],
								work->ts_m + 2);
				INFO("Shared_inc_dec %d", *work->shared_inc_dec_test);
				ts_now_mark(&info->ts[work->ts_sel],
								work->ts_m + 3);
				sem_post(work->shared_sem);
				ts_now_mark(&info->ts[work->ts_sel],
								work->ts_m + 4);
			};
			sem_post(&work->action_done);
			break;
	case ACTION_SHR_DEC:
			for (i = 0; i < work->shared_inc_dec_loop; i++) {
				ts_now_mark(&info->ts[work->ts_sel],
							work->ts_m + 10);
				sem_wait(work->shared_sem);
				ts_now_mark(&info->ts[work->ts_sel],
							work->ts_m + 11);
				*work->shared_inc_dec_test -= work->inc_dec_amt;
				ts_now_mark(&info->ts[work->ts_sel],
							work->ts_m + 12);
				INFO("Shared_inc_dec %d", *work->shared_inc_dec_test);
				ts_now_mark(&info->ts[work->ts_sel],
							work->ts_m + 13);
				sem_post(work->shared_sem);
				ts_now_mark(&info->ts[work->ts_sel],
							work->ts_m + 14);
			};
			sem_post(&work->action_done);
			break;
	case ACTION_HLT_REQ: while (info->stop_req == worker_running)
				sleep(0);
			break;
	case ACTION_HLT_SEM: 
			do {
				rc = sem_wait(&local_sem);
			} while  (rc && (errno == EINTR) &&
				(info->stop_req == worker_running));
			if (rc && errno != EINTR)
				goto fail;
			break;
	default: CRIT("Unknown command %d", info->action);
		 break;
	};

	return 0;
fail:
	return 1;
};

void create_priv(struct worker *info)
{
	struct test_worker_info *work = NULL;

	if (NULL == info) {
		ERR("Worker info NULL.");
		goto fail;
	}

	if (NULL != info->priv_info) {
		ERR("Worker priv info NOT NULL. Freeing memroy.");
		free((void *)info->priv_info);
	};

	info->priv_info = (void *)malloc(sizeof(struct test_worker_info));

	work = (struct test_worker_info *)info->priv_info;

	work->inc_dec_test = 0;
	work->inc_dec_amt = 0;
	work->ts_sel = 0;
	work->ts_m = 0;
	sem_init(&work->action_done, 0, 0);
	work->shared_inc_dec_test = &shared_inc_dec;
	work->shared_sem = &shared_sem;
	work->shared_inc_dec_loop = 0;
	INFO("Worker priv info allocated.");
fail:
	return;
};

void destroy_priv(struct worker *info)
{
	if (NULL == info) {
		ERR("Worker info NULL.");
		goto fail;
	}

	if (NULL == info->priv_info) {
		ERR("Worker priv info ALREADY NULL.");
		goto fail;
	};

	free((void *)info->priv_info);
	info->priv_info = NULL;
fail:
	return;
};

struct unit_test_driver test_unit_test_drv = {
        action_str,
        ts_sel,
        worker_body,
        create_priv,
        destroy_priv
};
	
int shared_inc_dec;
sem_t shared_sem;

int main(int argc, char *argv[])
{
	int rc = EXIT_FAILURE;

	if (0)
		argv[0][0] = argc;

	rdma_log_init("test_unit_test.log", 1);

	g_level = RDMA_LL_HIGH;

	config_unit_test(&test_unit_test_drv);

	sem_init(&shared_sem, 0, 1);
	shared_inc_dec = 0;

	if (test_case_1A()) {
		printf("\nTest_case_1A FAILED.");
		goto fail;
	};
	printf("\nTest_case_1A passed.");

	if (test_case_2A()) {
		printf("\nTest_case_2A FAILED.");
		goto fail;
	};
	printf("\nTest_case_2A passed.");


	if (test_case_1()) {
		printf("\nTest_case_1 FAILED.");
		goto fail;
	};
	printf("\nTest_case_1 passed.");

	if (test_case_2()) {
		printf("\nTest_case_2 FAILED.");
		goto fail;
	};
	printf("\nTest_case_2 passed.");

	if (test_case_3()) {
		printf("\nTest_case_3 FAILED.");
		goto fail;
	};
	printf("\nTest_case_3 passed.");

	if (test_case_4()) {
		printf("\nTest_case_4 FAILED.");
		goto fail;
	};
	printf("\nTest_case_4 passed.");

	if (test_case_5()) {
		printf("\nTest_case_5 FAILED.");
		goto fail;
	};
	printf("\nTest_case_5 passed.\n");

	if (test_case_6()) {
		printf("\nTest_case_6 FAILED.");
		goto fail;
	};
	printf("\nTest_case_6 passed.");

	if (test_case_7()) {
		printf("\nTest_case_7 FAILED.");
		goto fail;
	};
	printf("\nTest_case_7 passed.");

	rc = EXIT_SUCCESS;
fail:
	printf("\n");
	if (rc != EXIT_SUCCESS)
		rdma_log_dump();
	printf("\n");
	rdma_log_close();
	exit(rc);
};

#ifdef __cplusplus
}
#endif

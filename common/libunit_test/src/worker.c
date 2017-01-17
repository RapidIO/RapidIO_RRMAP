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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include <dirent.h>
#include <errno.h>

#include <pthread.h>
#include <sched.h>

#include "libcli.h"
#include "liblog.h"
#include "libtime_utils.h"
#include "libunit_test_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct unit_test_driver drvr;

void init_worker_info(struct worker *info, int first_time)
{
	int i;

	if (first_time) {
        	sem_init(&info->started, 0, 0);
        	sem_init(&info->run, 0, 0);
		info->priv_info = NULL;
	};

	info->stat = worker_dead;
	info->stop_req = worker_dead;
	info->wkr_thr.cpu_req = -1;
	info->wkr_thr.cpu_run = -1;
	info->action = 0;
	
	for (i = 0; i < MAX_UNIT_TEST_TS_IDX; i++)
		init_seq_ts(&info->ts[i], MAX_TIMESTAMPS);
};

void init_worker(struct worker *info)
{
        init_worker_info(info, 0);
};

int getCPUCount()
{
	FILE* f = fopen("/proc/cpuinfo", "rt");
	int count = 0;

	if (NULL == f) {
		CRIT("Could not open /proc/cpuinfo\n")
		return 1;
	}

	while (! feof(f)) {
		char buf[257] = {0};
		if (NULL == fgets(buf, 256, f))
			break;
		if (buf[0] == '\0')
			break;
		if (strstr(buf, "processor\t:")) count++;
	}

	fclose(f);

	return count;
}

int migrate_thread_to_cpu(struct thread_cpu *info)
{
        cpu_set_t cpuset;
        int chk_cpu_lim = 10;
	int rc;

	const int cpu_count = getCPUCount();

	if (-1 == info->cpu_req) {
        	CPU_ZERO(&cpuset);

		for(int c = 0; c < cpu_count; c++) CPU_SET(c, &cpuset);
	} else {
		if (info->cpu_req >= cpu_count) {
			ERR("\n\tInvalid cpu %d cpu count is %d\n",
				info->cpu_req, cpu_count);
			return 1;
		}
        	CPU_ZERO(&cpuset);
        	CPU_SET(info->cpu_req, &cpuset);
	};

        rc = pthread_setaffinity_np(info->thr, sizeof(cpu_set_t), &cpuset);
	if (rc) {
		ERR("pthread_setaffinity_np rc %d:%s\n",
					rc, strerror(errno));
                return 1;
	};

	if (-1 == info->cpu_req) {
		info->cpu_run = info->cpu_req;
		return 0;
	};
		
        rc = pthread_getaffinity_np(info->thr, sizeof(cpu_set_t), &cpuset);
	if (rc) {
		ERR("pthread_getaffinity_np rc %d:%s\n",
					rc, strerror(errno));
                return 1;
	};

        info->cpu_run = sched_getcpu();
        while ((info->cpu_run != info->cpu_req) && chk_cpu_lim) {
                usleep(1);
                info->cpu_run = sched_getcpu();
                chk_cpu_lim--;
        };
	rc = info->cpu_run != info->cpu_req;
	if (rc) {
		ERR("Unable to schedule thread on cpu %d\n", info->cpu_req);
                return 1;
	};
	return rc;
};

int migrate_worker_thread(struct worker *info, int cpu)
{
	/* Can only migrate a worker thread that is halted. */
	if ((info->stat == worker_dead) || (info->stat == worker_running))
		return 1;

	info->wkr_thr.cpu_req = cpu;
	info->stop_req = worker_running;
	info->action = UNIT_TEST_NO_ACTION;

	sem_post(&info->run);

	return 0;
};

void kill_worker_thread(struct worker *info)
{
        if (worker_dead != info->stat) {
                info->action = UNIT_TEST_SHUTDOWN;
                info->stop_req = worker_dead;
		if (worker_halted == info->stat) {
                	sem_post(&info->run);
			sleep(0);
		};
		pthread_kill(info->wkr_thr.thr, SIGUSR1);
                pthread_join(info->wkr_thr.thr, NULL);
        };

        init_worker(info);
};

void halt_worker_thread(struct worker *info)
{
        if (worker_running == info->stat)
                info->stop_req = worker_halted;
};

int wait_for_worker_status(struct worker *info, enum worker_stat desired)
{
	int count_down;
	struct timespec one_usec = {0, 1000};

	for (count_down = 0; (count_down < 10000) && (info->stat != desired);
								count_down++) 
		nanosleep(&one_usec, NULL);

	return info->stat != desired;
};

void worker_sig_handler(int sig)
{
        if (sig)
                return;
}

void *worker_thread(void *parm)
{
	struct worker *info = (struct worker *)parm;
	char my_name[16];
	struct sigaction sigh;

	info->stop_req = worker_running;

	memset(my_name, 0, 16);
	snprintf(my_name, 15, "WORKER_%d", info->idx);
	if (pthread_setname_np(info->wkr_thr.thr, my_name)) {
		info->action = UNIT_TEST_SHUTDOWN;
		info->stop_req = worker_dead;
	};

	if ((drvr_valid) && (NULL != drvr.create_priv))
		drvr.create_priv(info);
	else {
		info->action = UNIT_TEST_SHUTDOWN;
		info->stop_req = worker_dead;
	};

	memset(&sigh, 0, sizeof(sigh));
	sigh.sa_handler = worker_sig_handler;
	sigaction(SIGUSR1, &sigh, NULL);

	info->stat = worker_running;
	sem_post(&info->started);

	while (worker_running == info->stat) {
		if (info->wkr_thr.cpu_req != info->wkr_thr.cpu_run)
			migrate_thread_to_cpu(&info->wkr_thr);

		switch (info->action) {
		case UNIT_TEST_NO_ACTION: break;
		case UNIT_TEST_SHUTDOWN:
				info->stop_req = worker_dead;
				break;
		default:
			if (NULL == drvr.worker_body) {
				info->action = UNIT_TEST_SHUTDOWN;
				info->stop_req = worker_dead;
				sem_post(&info->run);
			} else {
				drvr.worker_body(info);
			};
		};

		info->stat = worker_halted;
		if (info->stop_req == worker_dead)
			break;
		info->stop_req = worker_running;
		sem_wait(&info->run);
		info->stat = worker_running;
	};

	info->stat = worker_dead;

	if ((drvr_valid) && (NULL != drvr.destroy_priv))
		drvr.destroy_priv(info);

	init_worker(info);
	pthread_exit(parm);
};

int start_worker_thread(struct worker *info, int cpu)
{
        int rc = 1;

        if (info->stat != worker_dead)
                goto fail;

        init_worker(info);

        info->wkr_thr.cpu_run = -1;
        info->wkr_thr.cpu_req = cpu;

        rc = pthread_create(&info->wkr_thr.thr, NULL, worker_thread,
                                                                (void *)info);
        if (!rc)
                sem_wait(&info->started);
fail:
        return rc;
};

int run_worker_action(struct worker *info, int action)
{
        if (info->stat != worker_halted)
                goto fail;

	info->action = action;
        sem_post(&info->run);

        return 0;
fail:
        return 1;
};
#ifdef __cplusplus
}
#endif

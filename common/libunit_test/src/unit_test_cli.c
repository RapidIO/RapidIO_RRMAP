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

#include <map>
#include <iostream>
#include <sstream>
#include <vector>

#include "tok_parse.h"
#include "string_util.h"
#include "libunit_test.h"
#include "libunit_test_priv.h"
#include "libtime_utils.h"
#include "libcli.h"
#include "liblog.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct unit_test_driver drvr;

int check_idx(struct cli_env *env, int idx, int want_halted)
{
	int rc = 1;

	if ((idx < 0) || (idx >= MAX_WORKERS)) {
		LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
		goto exit;
	};

	if (want_halted && (2 != wkr[idx].stat)) {
		LOGMSG(env, "\nWorker not halted...\n");
		goto exit;
	};

	if (!want_halted && (2 == wkr[idx].stat)) {
		LOGMSG(env, "\nWorker halted...\n");
		goto exit;
	};
	rc = 0;
exit:
	return rc;
};

int get_cpu(struct cli_env *env, char *dec_parm, int *cpu)
{
	int rc = 1;

	*cpu = GetDecParm(dec_parm, 0);

	const int MAX_GOODPUT_CPU = getCPUCount() - 1;

	if ((*cpu < -1) || (*cpu > MAX_GOODPUT_CPU)) {
		LOGMSG(env, "\nCPU must be 0 to %d...\n", MAX_GOODPUT_CPU);
		goto exit;
	};

	rc = 0;
exit:
	return rc;
};

int ThreadCmd(struct cli_env *env, int argc, char **argv)
{
	int idx, cpu;

	if (0)
		cpu = argc;

	idx = getDecParm(argv[0], 0);
	if (get_cpu(env, argv[1], &cpu))
		goto exit;

	if ((idx < 0) || (idx >= MAX_WORKERS)) {
		LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
		goto exit;
	};

	if (wkr[idx].stat) {
		LOGMSG(env, "\nWorker %d already running...\n", idx);
		goto exit;
	};

	start_worker_thread(&wkr[idx], cpu);
exit:
        return 0;
};

struct cli_cmd Thread = {
"thread",
1,
2,
"Start a thread on a cpu",
"start <idx> <cpu>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) "\n"
	"<cpu> is a cpu number, or -1 to indicate no cpu affinity\n",
ThreadCmd,
ATTR_NONE
};

int KillCmd(struct cli_env *env, int argc, char **argv)
{
	int st_idx = 0, end_idx = MAX_WORKERS-1, i;

	if (0)
		argv[0][0] = argc;

	if (strncmp(argv[0], "all", 3)) {
		st_idx = getDecParm(argv[0], 0);
		end_idx = st_idx;

		if ((st_idx < 0) || (st_idx >= MAX_WORKERS)) {
			LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
			goto exit;
		};
	};

	for (i = st_idx; i <= end_idx; i++) {
		kill_worker_thread(&wkr[i]);
	};
exit:
        return 0;
};

struct cli_cmd Kill = {
"kill",
4,
1,
"Kill a thread",
"kill <idx>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) ", or \"all\"\n",
KillCmd,
ATTR_NONE
};

int HaltCmd(struct cli_env *env, int argc, char **argv)
{
	unsigned int st_idx = 0, end_idx = MAX_WORKERS-1, i;

	if (0)
		argv[0][0] = argc;

	if (strncmp(argv[0], "all", 3)) {
		st_idx = getDecParm(argv[0], 0);
		end_idx = st_idx;

		if (st_idx >= MAX_WORKERS) {
			LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
			goto exit;
		};
	};

	for (i = st_idx; i <= end_idx; i++)
		halt_worker_thread(&wkr[i]);

exit:
        return 0;
};

struct cli_cmd Halt = {
"halt",
2,
1,
"Halt execution of a thread command",
"halt <idx>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) ", or \"all\"\n",
HaltCmd,
ATTR_NONE
};

int MoveCmd(struct cli_env *env, int argc, char **argv)
{
	int idx, cpu;

	if (0)
		cpu = argc;

	idx = getDecParm(argv[0], 0);
	if (get_cpu(env, argv[1], &cpu))
		goto exit;

	if ((idx < 0) || (idx >= MAX_WORKERS)) {
		LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
		goto exit;
	};

	if (migrate_worker_thread(&wkr[idx], cpu)) {
		LOGMSG(env, "\nFAILED: Could not move thread %d..\n", idx);
	} else {
		LOGMSG(env, "\nThread %d moving to cpu %d..\n", idx, cpu);
	}
exit:
	return 0;
};

struct cli_cmd Move = {
"move",
2,
2,
"Move a thread to a different CPU",
"move <idx> <cpu>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX)",\n" 
	"<cpu> is a cpu number, or -1 to indicate no cpu affinity\n",
MoveCmd,
ATTR_NONE
};

#define THREAD_STR(x) (char *)((worker_dead == x)?"---": \
				(worker_running == x)?"Run": \
				(worker_halted == x)?"Hlt":"UKN")

int WaitCmd(struct cli_env *env, int argc, char **argv)
{
	int idx;
	enum worker_stat state;

	if (0)
		argv[0][0] = argc;

	idx = getDecParm(argv[0], 0);
	switch (argv[1][0]) {
	case '0':
	case 'd':
	case 'D': state = worker_dead;
		break;
	case '1':
	case 'r':
	case 'R': state = worker_running;
		break;
	case '2':
	case 'h':
	case 'H': state = worker_halted;
		break;
	default: state = (enum worker_stat)-1;
		break;
	};

	if ((idx < 0) || (idx >= MAX_WORKERS)) {
		LOGMSG(env, "\nIndex must be 0 to %d...\n", MAX_WORKERS);
		goto exit;
	};

	if ((state < worker_dead) || (state > worker_running)) {
		LOGMSG(env, "\nState must be 0|d|D, 1|r|R , or 2|h|H\n")
		goto exit;
	};

	if (wait_for_worker_status(&wkr[idx], state)) {
		LOGMSG(env, "\nFAILED, Worker %d is now %s\n", idx,
				THREAD_STR(wkr[idx].stat));
	} else {
		LOGMSG(env, "\nPassed, Worker %d is now %s\n", idx,
				THREAD_STR(wkr[idx].stat));
	}

exit:
	return 0;
};

struct cli_cmd Wait = {
"wait",
2,
2,
"Wait until a thread reaches a particular state",
"wait <idx> <state>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) "\n"
	"<state> 0|d|D - Dead, 1|r|R - Run, 2|h|H - Halted\n",
WaitCmd,
ATTR_NONE
};

int SleepCmd(struct cli_env *env, int argc, char **argv)
{
	if (0)
		argv[0][0] = argc;

	float sec = GetFloatParm(argv[0], 0);
	if(sec > 0) {
		LOGMSG(env, "\nSleeping %f sec...\n", sec);
		const long usec = sec * 1000000;
		usleep(usec);
	}
	return 0;
}

int getIsolCPU(std::vector<std::string>& cpus)
{
  FILE* f = popen("awk '{for(i=1;i<NF;i++){if($i~/isolcpus/){is=$i}}}END{split(is,a,/=/);c=a[2];n=split(c,b,/,/); for(i in b){print b[i]}}' /proc/cmdline", "re");
	if(f == NULL) {
		return -1;
	}

	int count = 0;
	while(! feof(f)) {
		char buf[257] = {0};
		if (NULL == fgets(buf, 256, f)) {
			break;
		}

		int N = strlen(buf);
		if (N <= 0)
			break;
		if (buf[N-1] == '\n') {
			buf[--N] = '\0';
		}
		if (N) {
    			if (buf[N-1] == '\r') {
				buf[--N] = '\0';
			}
		}

		cpus.push_back(buf);
		count++;
  	}
	pclose(f);

	return count;
}

int IsolcpuCmd(struct cli_env *env, int argc, char **argv)
{
        int minisolcpu = 0;

        int n = 0; // this be a trick from X11 source tree ;)

        if (argc > 0)
                minisolcpu = GetDecParm(argv[n++], 0);

        std::vector<std::string> cpus;

        const int NI = getIsolCPU(cpus);

        if(minisolcpu > 0 && NI < minisolcpu) {
                CRIT("\n\tMinimum number of isolcpu cores (%d) not met, got %d. Bailing out!\n", minisolcpu, NI);
                return -1;
        }

        int c = 0;
        char clist[140] = {0};
        std::vector<std::string>::iterator it = cpus.begin();
        for(; it != cpus.end(); it++) {
                char tmp[15] = {0};
                snprintf(tmp, 8, "cpu%d=%s", ++c, it->c_str());
                if (SetEnvVar(tmp)) {
                        CRIT("IsolcpuCmd: Out of memory %s\n", tmp);
                        return -1;
                }
                strncat(clist, it->c_str(), 128);
                strncat(clist, " ", 128);
        }

        LOGMSG(env, "\nIsolcpus: %s\n", clist);

        return 0;
}

struct cli_cmd Isolcpu = {
"isolcpu",
4,
0,
"Returns the number of islcpus and sets the cpu1...cpuN env vars",
"<minisolcpu>\n"
        "<minisolcpu> [optional] STOP execution if minimum number of isolcpus not met\n",
IsolcpuCmd,
ATTR_NONE
};

struct cli_cmd Sleep = {
"sleep",
2,
1,
"Sleep for a number of seconds (fractional allowed)",
"sleep <sec>\n",
SleepCmd,
ATTR_NONE
};

#define PROC_STAT_PFMT "\nTot CPU Jiffies %lu %lu %lu %lu %lu %lu %lu\n"

#define CPUOCC_BUFF_SIZE 1024

void cpu_occ_parse_proc_line(char *file_line, 
				uint64_t *proc_new_utime,
				uint64_t *proc_new_stime)
{
	char *tok;
	char *saveptr;
	char *delim = (char *)" ";
	int tok_cnt = 0;
	char fl_cpy[CPUOCC_BUFF_SIZE];
	
	SAFE_STRNCPY(fl_cpy, file_line, sizeof(fl_cpy));
	tok = strtok_r(file_line, delim, &saveptr);
	while ((NULL != tok) && (tok_cnt < 13)) {
		tok = strtok_r(NULL, delim, &saveptr);
		tok_cnt++;
	};

	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, proc_new_utime, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, proc_new_stime, 0))
		goto error;

	return;
error:
	ERR("\nFAILED: proc_line \"%s\"\n", fl_cpy);
};

void cpu_occ_parse_stat_line(char *file_line,
			uint64_t *p_user,
			uint64_t *p_nice,
			uint64_t *p_system,
			uint64_t *p_idle,
			uint64_t *p_iowait,
			uint64_t *p_irq,
			uint64_t *p_softirq)
{
	char *tok, *saveptr;
	char *delim = (char *)" ";
	char fl_cpy[CPUOCC_BUFF_SIZE];
	
	SAFE_STRNCPY(fl_cpy, file_line, sizeof(fl_cpy));
	
	/* Throw the first token away. */
	tok = strtok_r(file_line, delim, &saveptr);
	if (NULL == tok)
		goto error;
	
	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_user, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_nice, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_system, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_idle, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_iowait, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_irq, 0))
		goto error;

	tok = strtok_r(NULL, delim, &saveptr);
	if (NULL == tok)
		goto error;
	if (tok_parse_ull(tok, p_softirq, 0))
		goto error;
	
	return;
error:
	ERR("\nFAILED: stat_line \"%s\"\n", fl_cpy);
};

int cpu_occ_valid;

uint64_t old_tot_jifis;
uint64_t old_proc_kern_jifis;
uint64_t old_proc_user_jifis;
uint64_t new_tot_jifis;
uint64_t new_proc_kern_jifis;
uint64_t new_proc_user_jifis;

float    cpu_occ_pct;

int cpu_occ_set(uint64_t *tot_jifis, 
		uint64_t *proc_kern_jifis,
		uint64_t *proc_user_jifis)
{
	pid_t my_pid = getpid();
	FILE *stat_fp, *cpu_stat_fp;
	char filename[256];
	char file_line[CPUOCC_BUFF_SIZE];
	uint64_t p_user = 1, p_nice = 1, p_system = 1, p_idle = 1;
	uint64_t p_iowait = 1, p_irq = 1, p_softirq = 1;
	int rc = 1;

	memset(filename, 0, 256);
	snprintf(filename, 255, "/proc/%d/stat", my_pid);

	stat_fp = fopen(filename, "r" );
	if (NULL == stat_fp) {
		ERR( "FAILED: Open proc stat file \"%s\": %d %s\n",
			filename, errno, strerror(errno));
		goto exit;
	};

	cpu_stat_fp = fopen("/proc/stat", "r");
	if (NULL == cpu_stat_fp) {
		ERR("FAILED: Open file \"/proc/stat\": %d %s\n",
			errno, strerror(errno));
		goto exit;
	};

	memset(file_line, 0, sizeof(file_line));
	if (NULL == fgets(file_line, sizeof(file_line), stat_fp)) {
		ERR("FAILED: reading line 1 returned NULL, errno %d: %s",
			errno, strerror(errno));
		goto exit;
	};

	cpu_occ_parse_proc_line(file_line, proc_user_jifis, proc_kern_jifis);

		
	memset(file_line, 0, sizeof(file_line));
	if (NULL == fgets(file_line, sizeof(file_line), cpu_stat_fp)) {
		ERR("FAILED: reading line 2 returned NULL, errno %d: %s",
			errno, strerror(errno));
		goto exit;
	};

	cpu_occ_parse_stat_line(file_line, &p_user, &p_nice, &p_system,
			&p_idle, &p_iowait, &p_irq, &p_softirq);

	*tot_jifis = p_user + p_nice + p_system + p_idle +
			p_iowait + p_irq + p_softirq;
	fclose(stat_fp);
	fclose(cpu_stat_fp);
	
	rc = 0;
exit:
	return rc;
};

int CPUOccSetCmd(struct cli_env *env, int argc, char **argv)
{
	if (0)
		argv[0][0] = argc;

	if (cpu_occ_set(&old_tot_jifis, &old_proc_kern_jifis,
			&old_proc_user_jifis)) {
		LOGMSG(env, "\nFAILED: Could not get proc info \n");
		goto exit;
	};
	LOGMSG(env, "\nSet CPU Occ measurement start point\n");

	cpu_occ_valid = 1;
exit:
	return 0;
};

struct cli_cmd CPUOccSet = {
"oset",
2,
0,
"Set CPU Occupancy measurement start point.",
"ost\n"
	"No parameters\n", 
CPUOccSetCmd,
ATTR_NONE
};

int cpu_occ_saved_idx;

int CPUOccDisplayCmd(struct cli_env *env, int argc, char **argv)
{
	char pctg[24];
	int cpus = getCPUCount();

	if (0)
		argv[0][0] = argc;

	if (!cpus)
		cpus = 1;

	if (!cpu_occ_valid) {
		LOGMSG(env, "\nFAILED: CPU OCC measurement start not set\n");
		goto exit;
	};

	if (cpu_occ_set(&new_tot_jifis, &new_proc_kern_jifis,
			&new_proc_user_jifis)) {
		LOGMSG(env, "\nFAILED: Could not get proc info \n");
		goto exit;
	};


	cpu_occ_pct = (((float)(new_proc_kern_jifis + new_proc_user_jifis -
				 old_proc_kern_jifis - old_proc_user_jifis)) /
		((float)(new_tot_jifis - old_tot_jifis))) * 100.0 * cpus;
	sprintf(pctg, "%4.2f", cpu_occ_pct);
	LOGMSG(env, "\n-Kernel- ProcUser ProcKern CPU_Occ\n");
	LOGMSG(env, "%8ld %8ld %8ld %7s\n", new_tot_jifis - old_tot_jifis,
			new_proc_user_jifis - old_proc_user_jifis,
			new_proc_kern_jifis - old_proc_kern_jifis, pctg);
exit:
	return 0;
};

struct cli_cmd CPUOccDisplay = {
"odisp",
3,
0,
"Display cpu occupancy",
"odisp <idx>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) "\n",
CPUOccDisplayCmd,
ATTR_RPT
};

void display_cpu(struct cli_env *env, int cpu)
{
	if (-1 == cpu) {
		LOGMSG(env, "Any ");
	} else {
		LOGMSG(env, "%3d ", cpu);
	}
};

void display_status(struct cli_env *env)
{
	int i;
	char *action_str;

	LOGMSG(env, "\n W STS CPU RUN ACTION\n");

	for (i = 0; i < MAX_WORKERS; i++) {
		LOGMSG(env, "%2d %3s ", i, THREAD_STR(wkr[i].stat));
		display_cpu(env, wkr[i].wkr_thr.cpu_req);
		display_cpu(env, wkr[i].wkr_thr.cpu_run);

		action_str = (char *)"UNKNOWN";
		switch (wkr[i].action) {
		case UNIT_TEST_NO_ACTION: action_str = (char *)"NO_ACTION";
					break;
		case UNIT_TEST_SHUTDOWN: action_str = (char *)"SHUTDOWN";
					break;
		default: if (drvr_valid &&  (NULL != drvr.action_str))
				action_str = drvr.action_str(wkr[i].action);
			else
				action_str = (char *)"INVALID";
		};
		
		LOGMSG(env, "%7s\n", action_str);
	};
};

int StatusCmd(struct cli_env *env, int argc, char **argv)
{
	if (0)
		argv[0][0] = argc;

	display_status(env);

        return 0;
};

struct cli_cmd Status = {
"status",
2,
0,
"Display status of all threads",
"status {No parameters}\n",
StatusCmd,
ATTR_RPT
};

int UTimeCmd(struct cli_env *env, int argc, char **argv)
{
	int idx, st_i = 0, end_i = MAX_TIMESTAMPS-1;
	struct seq_ts *ts_p = NULL;
	uint64_t lim = 0;
	int got_one = 0;
	int ts_idx = 0;
	struct timespec diff, min, max, tot;

	idx = GetDecParm(argv[0], 0);
	if (check_idx(env, idx, 0))
		goto exit;

	if (!drvr_valid || (NULL == drvr.ts_sel)) {
		LOGMSG(env, "Cannot parse ts_idx, using 0\n");
	} else {
		ts_idx = drvr.ts_sel(argv[1]);
		if ((ts_idx < 0) || (ts_idx >= MAX_UNIT_TEST_TS_IDX)) {
			LOGMSG(env, "FAILED: Unknown ts_idx.\n");
			goto exit;
		};
	};
	ts_p = &wkr[idx].ts[ts_idx];

	switch (argv[2][0]) {
	case 's':
	case 'S':
		init_seq_ts(ts_p, MAX_TIMESTAMPS);
		break;
	case '-':
		if (argc > 4) {
			st_i = GetDecParm(argv[3], 0);
			end_i = GetDecParm(argv[4], 0);
		} else {
			LOGMSG(env, "\nFAILED: Must enter two indices\n");
			goto exit;
		}

		if ((end_i < st_i) || (st_i < 0) || (end_i >= MAX_TIMESTAMPS)) {
			LOGMSG(env, "FAILED: Index range 0 to %d.\n",
					MAX_TIMESTAMPS-1);
			goto exit;
		}

		if (ts_p->ts_idx < MAX_TIMESTAMPS - 1) {
			LOGMSG(env, "\nWARNING: Last valid timestamp is %d\n",
					ts_p->ts_idx);
		}

		diff = time_difference(ts_p->ts_val[st_i], ts_p->ts_val[end_i]);
		LOGMSG(env, "\n---->> Sec<<---- Nsec---MMMuuuNNN\n");
		LOGMSG(env, "%16ld %16ld\n", diff.tv_sec, diff.tv_nsec);
		break;

	case 'p':
	case 'P':
		if (argc > 3)
			st_i = GetDecParm(argv[3], 0);
		if (argc > 4)
			end_i = GetDecParm(argv[4], 0);

		if ((end_i < st_i) || (st_i < 0) || (end_i >= MAX_TIMESTAMPS)) {
			LOGMSG(env, "FAILED: Index range 0 to %d.\n",
					MAX_TIMESTAMPS-1);
			goto exit;
		}

		if (ts_p->ts_idx < MAX_TIMESTAMPS - 1) {
			LOGMSG(env, "\nWARNING: Last valid timestamp is %d\n",
					ts_p->ts_idx);
		}

		LOGMSG(env, "\n Idx ---->> Sec<<---- Nsec---mmmuuunnn Marker\n");
		for (idx = st_i; idx <= end_i; idx++) {
			LOGMSG(env, "%4d %16ld %16ld %d\n", idx,
					ts_p->ts_val[idx].tv_sec,
					ts_p->ts_val[idx].tv_nsec,
					ts_p->ts_mkr[idx]);
		}
		break;
			
	case 'l':
	case 'L':
		if (argc > 3)
			lim = GetDecParm(argv[3], 0);
		else
			lim = 0;

		for (idx = st_i; idx < end_i; idx++) {
			time_track(idx, ts_p->ts_val[idx], ts_p->ts_val[idx+1],
				&tot, &min, &max);
			diff = time_difference(ts_p->ts_val[idx],
						ts_p->ts_val[idx+1]);
			if ((uint64_t)diff.tv_nsec < lim)
				continue;
			if (!got_one) {
				LOGMSG(env,
						"\n Idx ---->> Sec<<---- Nsec---MMMuuuNNN Marker\n");
				got_one = 1;
			}
			LOGMSG(env, "%4d %16ld %16ld %d -> %d\n", idx,
					diff.tv_sec, diff.tv_nsec,
					ts_p->ts_mkr[idx],
					ts_p->ts_mkr[idx + 1]);
		};

		if (!got_one) {
			LOGMSG(env, "\nNo delays found bigger than %ld\n", lim);
		}
		LOGMSG(env, "\n==== ---->> Sec<<---- Nsec---MMMuuuNNN\n");
		LOGMSG(env, "Min: %16ld %16ld\n", min.tv_sec, min.tv_nsec);
		diff = time_div(tot, end_i - st_i);
		LOGMSG(env, "Avg: %16ld %16ld\n", diff.tv_sec, diff.tv_nsec);
		LOGMSG(env, "Max: %16ld %16ld\n", max.tv_sec, max.tv_nsec);
		break;
	default:
		LOGMSG(env, "FAILED: <cmd> not 's','p' or 'l'.\n");
	};
exit:
        return 0;
};

struct cli_cmd UTime = {
"utime",
2,
3,
"Timestamp buffer command",
"<idx> <type> <cmd> <parms>\n"
	"<idx> is a worker index from 0 to " STR(MAX_WORKER_IDX) "\n"
	"<type> is:\n"
	"      'd' - descriptor timestamps\n"
	"      'f' - FIFO (descriptor completions)\n"
	"      'm' - measurement (development only)\n"
	"<cmd> is the command to perform on the buffer, one of:\n"
	"      's' - sample timestamps again\n"
	"      '-' - return difference in two timestamp idices\n"
	"            Note: Must enter two timestamp indexes\n"
	"      'p' - print the existing counter values\n"
	"            Note: optionally enter start and end indexes.\n"
	"      'l' - locate differences greater than x nsec\n"
	"            Note: Must enter the number of nanoseconds in decimal.\n",
UTimeCmd,
ATTR_NONE
};

struct cli_cmd *goodput_cmds[] = {
	&Thread,
	&Kill,
	&Halt,
	&Move,
	&Wait,
	&Sleep,
	&Isolcpu,
	&CPUOccSet,
	&CPUOccDisplay,
	&Status
};

void bind_unit_test_thread_cli_cmds(void)
{
	cpu_occ_saved_idx = 0;

	cpu_occ_valid = 0;
	new_proc_user_jifis = 0;
	new_proc_kern_jifis = 0;
	old_proc_user_jifis = 0;
	old_proc_kern_jifis = 0;
	old_tot_jifis = 1;
	new_tot_jifis = 2;
	cpu_occ_pct = 0.0;

        add_commands_to_cmd_db(sizeof(goodput_cmds)/sizeof(goodput_cmds[0]),
                                goodput_cmds);
};

#ifdef __cplusplus
}
#endif

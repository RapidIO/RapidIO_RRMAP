/* Fabric Management Daemon Device Directory CLI Commands Implementation */
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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include "fmd_dd_priv.h"


// #ifdef __LINUX__
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
// #endif

#include "rio_misc.h"
#include "fmd_dd.h"
#include "liblog.h"
#include "libcli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_dd **cli_dd;
struct fmd_dd_mtx **cli_dd_mtx;

char *cli_dd_fn;
char *cli_dd_mtx_fn;

extern struct cli_cmd CLIDDDump;

int CLIDDDumpCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	uint32_t i;
	uint32_t found = 0;
	did_val_t did_val;
	uint32_t did_sz;

	if (NULL == *cli_dd) {
		LOGMSG(env, "\nDevice Directory not available.\n");
		goto exit;
	}

	LOGMSG(env, "\nTime %lld.%.9ld ChgIdx: 0x%8x\n",
			(long long )(*cli_dd)->chg_time.tv_sec,
			(*cli_dd)->chg_time.tv_nsec, (*cli_dd)->chg_idx);
	LOGMSG(env, "fmd_dd: md_ct %x num_devs %d\n", (*cli_dd)->md_ct,
			(*cli_dd)->num_devs);

	if ((*cli_dd)->num_devs > 0) {
		LOGMSG(env, "Idx ---CT--- -destID- SZ HC MP FL Name\n");
		for (i = 0; (i < (*cli_dd)->num_devs) && (i < FMD_MAX_DEVS); i++) {
			did_to_value((*cli_dd)->devs[i].did, &did_val, &did_sz);
			LOGMSG(env, "%3d %8x %8x %2x %2x %2s %2x %30s\n", i,
					(*cli_dd)->devs[i].ct, did_val, did_sz,
					(*cli_dd)->devs[i].hc,
					(*cli_dd)->devs[i].is_mast_pt ?
							"MP" : "..",
					(*cli_dd)->devs[i].flag,
					(*cli_dd)->devs[i].name);
		}
	}

	if (NULL == (*cli_dd_mtx)) {
		LOGMSG(env, "\nDevice Directory Mutex not available.\n");
		goto exit;
	}

	LOGMSG(env, "Mutex: mtx_ref_cnt %x dd_ref_cnt %x init_done %x\n",
			(*cli_dd_mtx)->mtx_ref_cnt, (*cli_dd_mtx)->dd_ref_cnt,
			(*cli_dd_mtx)->init_done);

	found = 0;
	for (i = 0; i < FMD_MAX_APPS; i++) {
		if (!(*cli_dd_mtx)->dd_ev[i].in_use)
			continue;

		if (!found) {
			LOGMSG(env, "\nIdx --Proc-- Waiting\n");
			found = 1;
		}

		LOGMSG(env, "%3d %8d %d\n", i, (*cli_dd_mtx)->dd_ev[i].proc,
				(*cli_dd_mtx)->dd_ev[i].waiting);
	}

	if (!found) {
		LOGMSG(env, "\nNo applications connected.\n");
	}

exit:
	return 0;
}

struct cli_cmd CLIDDDump = {
(char *)"dddump",
3,
0,
(char *)"Device Directory Dump, no parameters.",
(char *)"Prints current state of the the device directory and mutex.",
CLIDDDumpCmd,
ATTR_RPT
};

extern struct cli_cmd CLIDDInc;

int CLIDDIncCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	if (NULL == *cli_dd) {
		LOGMSG(env, "\nDevice Database is not available.\n");
		goto exit;
	}

	fmd_dd_incr_chg_idx((*cli_dd), 1);
	LOGMSG(env, "\nIncrement idx value: 0x%8x\n",
			fmd_dd_get_chg_idx(*cli_dd));

exit:
	return 0;
}

struct cli_cmd CLIDDInc = {
(char *)"inc",
3,
0,
(char *)"Increment change index, no parameters.",
(char *)"Increments and prints current change index for the device database.",
CLIDDIncCmd,
ATTR_NONE
};

extern struct cli_cmd CLIClean;

int CLICleanCmd(struct cli_env *env, int argc, char **argv)
{

	if ((NULL == *cli_dd) || (NULL == *cli_dd_mtx) || (NULL == cli_dd_fn)
			|| (NULL == cli_dd_mtx_fn)) {
		LOGMSG(env, "\nState pointer is null.\n");
		goto exit;
	}

	argv[0] = NULL;
	if (argc) {
		LOGMSG(env, "\nFreeing Mutex, current state:\n");
		if (NULL == (*cli_dd_mtx)) {
			LOGMSG(env, "\ndd_mtx is NULL\n");
		} else {
			LOGMSG(env, "dd_ref_cnt   : %x\n",
					(*cli_dd_mtx)->dd_ref_cnt);
			LOGMSG(env, "mtx_ref_cnt: %x\n",
					(*cli_dd_mtx)->mtx_ref_cnt);
			LOGMSG(env, "init_done : %x\n",
					(*cli_dd_mtx)->init_done);
		}
		shm_unlink(cli_dd_mtx_fn);
	} else {
		LOGMSG(env, "\nFreeing dd, current state:\n");
		if (NULL == *cli_dd) {
			LOGMSG(env, "\ndd is NULL\n");
		} else {
			LOGMSG(env, "dd_ref_cnt   : %x\n",
					(*cli_dd_mtx)->dd_ref_cnt);
			LOGMSG(env, "mtx_ref_cnt: %x\n",
					(*cli_dd_mtx)->mtx_ref_cnt);
			LOGMSG(env, "init_done : %x\n",
					(*cli_dd_mtx)->init_done);
		}
		shm_unlink(cli_dd_fn);
	}
exit:
	return 0;
}

struct cli_cmd CLIClean = {
(char *)"clean",
3,
0,
(char *)"Drops shared memory blocks.",
(char *)"No parms drops sm block, any part drops mutex.",
CLICleanCmd,
ATTR_NONE
};

struct cli_cmd *dd_cmds[3] = 
	{&CLIDDDump, 
	 &CLIDDInc,
	 &CLIClean };

void bind_dd_cmds(struct fmd_dd **dd, struct fmd_dd_mtx **dd_mtx,
			char *dd_fn, char *dd_mtx_fn)
{
	cli_dd = dd;
	cli_dd_mtx = dd_mtx;
	cli_dd_fn = dd_fn;
	cli_dd_mtx_fn = dd_mtx_fn;
	add_commands_to_cmd_db(sizeof(dd_cmds)/sizeof(dd_cmds[0]), &dd_cmds[0]);
}

#ifdef __cplusplus
}
#endif

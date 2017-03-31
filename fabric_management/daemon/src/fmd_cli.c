/* Fabric Management Daemon Thread CLI Commands */
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


#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "rio_misc.h"
#include "tok_parse.h"
#include "rio_ecosystem.h"
#include "liblog.h"
#include "libcli.h"
#include "cfg.h"
#include "fmd_dd.h"
#include "fmd_app_msg.h"
#include "fmd_state.h"
#include "fmd.h"
#include "fmd_app.h"
#include "fmd_app.h"
#include "fmd_master.h"
#include "fmd_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

void display_apps_dd(struct cli_env *env)
{
	int found_one = 0, i;
	for (i = 0; i < FMD_MAX_APPS; i++) {
		if (app_st.apps[i].alloced) {
			if (!found_one) {
				LOGMSG(env,
						"         Idx V Fd A D ProcNum- Name\n");
			}
			found_one = 1;
			LOGMSG(env, "         %3d %1d %2d %1d %1d %8d %s\n",
					app_st.apps[i].index,
					app_st.apps[i].alloced,
					app_st.apps[i].app_fd,
					app_st.apps[i].alive,
					app_st.apps[i].i_must_die,
					app_st.apps[i].proc_num,
					app_st.apps[i].app_name);
		}
	}

	if (!found_one) {
		LOGMSG(env, "         No apps connected...\n");
	}
}
extern struct cli_cmd CLIStatus;

int CLIStatusCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	int app_cnt = 0, i;
	struct fmd_peer *peer;
	struct l_item_t *li;

	LOGMSG(env, "Rlogin  Alive: %1d Skt %5d\n\n", fmd->rlogin_alive,
		fmd->opts->cli_port_num); 
	for (i = 0; i < FMD_MAX_APPS; i++) {
		if (app_st.apps[i].alloced)
			app_cnt++;
	}
	LOGMSG(env, "AppMgmt Alive: %1d Exit: %1d  NumApps: %4d Skt: %5d\n",
			app_st.loop_alive, app_st.all_must_die, app_cnt,
			app_st.port);
	LOGMSG(env, "\nThread   A D Conn\n");
	display_apps_dd(env);

	if (!fmp.mode) {
		LOGMSG(env, "\nPeerMgmt Alive: %1d Exit: %1d SLAVE %5d %s\n",
				fmp.slv.slave_alive, fmp.slv.slave_must_die,
				fmp.slv.mast_skt_num,
				fmp.slv.m_h_resp_valid ? "OK" : "No Hello Rsp");
		goto exit;
	}

	LOGMSG(env, "\nPeerMgmt Alive %1d Exit %1d PeerCnt %4d MASTER %5d\n",
			fmp.acc.acc_alive, fmp.acc.acc_must_die,
			l_size(&fmp.peers), fmp.acc.cm_skt_num);

	if (!l_size(&fmp.peers)) {
		LOGMSG(env, "No connected peers.\n");
		goto exit;
	}

	LOGMSG(env, "\n         ---CT--- ---DID-- HC A D I R\n");

	peer = (struct fmd_peer *)l_head(&fmp.peers, &li);
	while (NULL != peer) {
		LOGMSG(env, "         %8x %8x %2x %1d %1d %1d %1d %s\n",
				peer->p_ct, did_get_value(peer->p_did),
				peer->p_hc, peer->rx_alive, peer->rx_must_die,
				peer->init_cplt, peer->restart_init,
				peer->peer_name);
		peer = (struct fmd_peer *)l_next(&li);
	}

exit:
	return 0;
}

struct cli_cmd CLIStatus  = {
(char *)"status",
2,
0,
(char *)"Print FMD thread status.",
(char *)"<No Parameters>\n",
CLIStatusCmd,
ATTR_RPT
};

extern struct cli_cmd CLIApp;

int CLIAppCmd(struct cli_env *env, int argc, char **argv)
{
	uint32_t idx;

	if (argc) {
		if (tok_parse_ulong(argv[0], &idx, 0, FMD_MAX_APPS, 0)) {
			LOGMSG(env, TOK_ERR_ULONG_MSG_FMT, "Maximum apps",
					0, FMD_MAX_APPS);
			goto exit;
		}
		if (app_st.apps[idx].alive) {
			app_st.apps[idx].i_must_die = 1;
			pthread_kill(app_st.apps[idx].app_thr, SIGHUP);
			pthread_join(app_st.apps[idx].app_thr, NULL);
		}
		app_st.apps[idx].alive = 0;
		app_st.apps[idx].i_must_die = 0;
		app_st.apps[idx].proc_num = 0;
		memset(app_st.apps[idx].app_name, 0, MAX_APP_NAME + 1);
		app_st.apps[idx].alloced = 0;
	}
	display_apps_dd(env);

exit:
	return 0;
}

struct cli_cmd CLIApp  = {
(char *)"app",
3,
0,
(char *)"Print FMD Application connections status",
(char *)"{{idx>}\n"
	"<idx> : Optionally shutdown and clear application at this index.\n",
CLIAppCmd,
ATTR_NONE
};

extern struct cli_cmd CLINotify;

int CLINotifyCmd(struct cli_env *env, int argc, char **argv)
{
	if (0) {
		argv[0][0] = argc;
		(void) env;
	}

	fmd_notify_apps();
	return 0;
}

struct cli_cmd CLINotify  = {
(char *)"notify",
3,
0,
(char *)"Notifies all applications of a change in the device directory",
(char *)"<No Parameters>\n",
CLINotifyCmd,
ATTR_NONE
};

struct cli_cmd *fmd_mgmt_cli_cmds[3] = {
	&CLIStatus,
	&CLIApp,
	&CLINotify
};

void fmd_bind_mgmt_dbg_cmds(void)
{
	add_commands_to_cmd_db(sizeof(fmd_mgmt_cli_cmds)/
			sizeof(fmd_mgmt_cli_cmds[0]), 
					&fmd_mgmt_cli_cmds[0]);
}

#ifdef __cplusplus
}
#endif

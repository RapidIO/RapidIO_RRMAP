/* Fabric Management Daemon Device Directory Library */
/* CLI Commands Implementation */
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
#include "fmd_dd.h"
#include "fmd_app_msg.h"
#include "liblog.h"
#include "libfmdd_info.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct cli_cmd CLIDDLCheckCT;

int CLIDDLCheckCTCmd(struct cli_env *env, int UNUSED(argc), char **argv)
{
	ct_t ct;
	uint16_t flag;
	int rc;

	if (tok_parse_ct(argv[0], &ct, 0)) {
		LOGMSG(env, "\n");
		LOGMSG(env, TOK_ERR_CT_MSG_FMT);
		goto exit;
	}

	if (tok_parse_ushort(argv[1], &flag, 0, 0x10, 0)) {
		LOGMSG(env, "\n");
		LOGMSG(env, TOK_ERR_USHORT_HEX_MSG_FMT, "flag", 0, 0x10);
		goto exit;
	}

	LOGMSG(env, "\nChecking ct 0x%8x flag 0x%2x\n", ct, flag);
	rc = fmdd_check_ct(&fml, ct, (uint8_t)flag);
	LOGMSG(env, "Return code was      : 0x%8x\n", rc);

exit:
	return 0;
}

struct cli_cmd CLIDDLCheckCT = {
(char *)"ddlct",
5,
2,
(char *)"Device Directory Library CT check.",
(char *)"<ct> <flag>\n"
	"Checks whether an entered component tag is present\n"
	"<flag> is a bitmask with the following values: \n"
	"0x01 - Device is OK\n"
	"0x02 - Device is local master port\n"
	"0x08 - RSKT application is active\n"
	"0x10 - RDMA application is active\n",
CLIDDLCheckCTCmd,
ATTR_NONE
};

extern struct cli_cmd CLIDDLCheckDID;

int CLIDDLCheckDIDCmd(struct cli_env *env, int UNUSED(argc), char **argv)
{
	did_val_t did_val;
	uint16_t flag;
	int rc;

	if (tok_parse_did(argv[0], &did_val, 0)) {
		LOGMSG(env, "\n");
		LOGMSG(env, TOK_ERR_DID_MSG_FMT);
		goto exit;
	}

	if (tok_parse_ushort(argv[1], &flag, 0, 0x10, 0)) {
		LOGMSG(env, "\n");
		LOGMSG(env, TOK_ERR_USHORT_HEX_MSG_FMT, "flag", 0, 0x10);
		goto exit;
	}

	LOGMSG(env, "\nChecking Device ID : 0x%8x flag 0x%2x\n", did_val, flag);
	rc = fmdd_check_did(&fml, did_val, (uint8_t)flag);
	LOGMSG(env, "Return code was    : 0x%8x\n", rc);

exit:
	return 0;
}

struct cli_cmd CLIDDLCheckDID = {
(char *)"ddldid",
5,
2,
(char *)"Device Directory Library Device ID check.",
(char *)"<did> <flag>\n"
	"Checks whether an entered device ID is present\n"
	"<flag> is a bitmask with the following values: \n"
	"0x01 - Device is OK\n"
	"0x02 - Device is local master port\n"
	"0x08 - RSKT application is active\n"
	"0x10 - RDMA application is active\n",
CLIDDLCheckDIDCmd,
ATTR_NONE
};

extern struct cli_cmd CLIDDStatus;

int CLIDDStatusCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	LOGMSG(env, "\nFMD Port number    : %d", fml.portno);
	LOGMSG(env, "\nInit ok            : %d", fml.init_ok);
	LOGMSG(env, "\nMy name            : %s", fml.app_name);
	LOGMSG(env, "\nMy flag            : 0x%x", fml.flag);
	LOGMSG(env, "\nSocket file number : %d", fml.fd);
	LOGMSG(env, "\nFMD Update period  : %d", fml.fmd_update_period);
	LOGMSG(env, "\nMonitor alive      : %d", fml.mon_alive);
	LOGMSG(env, "\nFMD is alive       : %d", !fml.fmd_dead);

	LOGMSG(env, "\nDevice Dir FN      : %s",
			(NULL == fml.dd ? "(nil)" : fml.dd_fn));
	LOGMSG(env, "\nDevice Dir PTR     : %p", fml.dd);
	LOGMSG(env, "\nDevice Dir Mtx FN  : %s",
			(NULL == fml.dd_mtx ? "(nil)" : fml.dd_mtx_fn));
	LOGMSG(env, "\nDevice Dir Mtx PTR : %p", fml.dd_mtx);
	LOGMSG(env, "\nDevice Dir Mtx IDX : %d", fml.app_idx);

	LOGMSG(env, "\n");

	return 0;
}

struct cli_cmd CLIDDStatus = {
(char *)"ddstat",
3,
0,
(char *)"Display Device Directory connection status.\n",
(char *)"<No Parameters>\n"
	"Display Device Directory status of connection to DD and to host.\n",
CLIDDStatusCmd,
ATTR_NONE
};

extern struct cli_cmd CLIDDList;

int CLIDDListCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	uint32_t did_list_sz;
	uint32_t i;
	did_val_t *did_list;
	int rc;

	rc = fmdd_get_did_list(&fml, &did_list_sz, &did_list);

	// did_list can be null on success
	LOGMSG(env, "\nfmdd_get_did_list returned : %d\n", rc);
	if (rc) {
		goto exit;
	}

	LOGMSG(env, "\nNumber of device IDs: %d\n", did_list_sz);

	// klocwork analysis led to paranoid NULL check
	if ((!did_list_sz) || (NULL == did_list)) {
		goto exit;
	}

	LOGMSG(env, "\nIdx ---DID--\n");

	for (i = 0; i < did_list_sz; i++) {
		LOGMSG(env, "%d %8x\n", did_list_sz, did_list[i]);
	}

exit:
	free(did_list);
	return 0;
}

struct cli_cmd CLIDDList = {
(char *)"ddlli",
5,
0,
(char *)"Display Device Directory device ID list.\n",
(char *)"<No Parameters>\n"
	"Display Device Directory device ID list.\n",
CLIDDListCmd,
ATTR_NONE
};

struct cli_cmd *libfmdd_cmds[4] = {
	&CLIDDStatus,
	&CLIDDLCheckCT,
	&CLIDDLCheckDID,
	&CLIDDList
};

void fmdd_bind_dbg_cmds(void)
{
	add_commands_to_cmd_db(sizeof(libfmdd_cmds) / sizeof(libfmdd_cmds[0]),
				&libfmdd_cmds[0]);
	bind_dd_cmds(&fml.dd, &fml.dd_mtx, fml.dd_fn, fml.dd_mtx_fn);
}

#ifdef __cplusplus
}
#endif

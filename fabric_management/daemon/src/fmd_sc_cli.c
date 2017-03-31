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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rio_misc.h"
#include "rio_ecosystem.h"
#include "fmd.h"
#include "fmd_dev_rw_cli.h"
#include "liblog.h"
#include "libcli.h"
#include "riocp_pe_internal.h"
#include "pe_mpdrv_private.h"
#include "string_util.h"

#include "rio_standard.h"
#include "RXS2448.h"
#include "RapidIO_Routing_Table_API.h"

#ifdef __cplusplus
extern "C" {
#endif

int CLICountReadCmd(struct cli_env *env, int argc, char **argv)
{
	uint32_t rc;
	struct mpsw_drv_private_data *priv = NULL;
	DAR_DEV_INFO_t *dev_h = NULL;

	rio_sc_read_ctrs_in_t sc_in;
	rio_sc_read_ctrs_out_t sc_out;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

        if (0) {
            argv[0][0] = argc;
	}

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	priv = (struct mpsw_drv_private_data *)(pe_h->private_data);
	if (NULL == priv) {
		LOGMSG(env, "\nCannot retrieve device state...\n");
		goto exit;
	}

	dev_h = &priv->dev_h;
	sc_in.ptl.num_ports = RIO_ALL_PORTS;
	sc_in.dev_ctrs = &priv->st.sc_dev;
	rc = rio_sc_read_ctrs(dev_h, &sc_in, &sc_out);

	if (RIO_SUCCESS == rc) {
		LOGMSG(env,"\nCounters read successfully\n");
	} else {
		LOGMSG(env, "\nFAILED: rc: %d imp rc 0x%x\n",
							rc, sc_out.imp_rc);
	}
exit:
	return 0;
}

struct cli_cmd CLICountRead = {
(char *)"scread",
4,
0,
(char *)"Statistics counter read for all ports",
(char *)"scread (no parameters)\n",
CLICountReadCmd,
ATTR_NONE
};

int CLICountDisplayCmd(struct cli_env *env, int argc, char **argv)
{
        struct mpsw_drv_private_data *priv = NULL;
	DAR_DEV_INFO_t *dev_h = NULL;

        riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);
	uint32_t val_p, cntr;
	rio_sc_dev_ctrs_t *dc;
	const char *oth_name;
	bool got_one = false;
	bool chk_rx_tx = false;
	bool sc_tx = false;
	bool chk_rio_oth = false;
	bool sc_srio = true;
	bool chk_flags = true;
	uint32_t flags = 0;
	int i = 0;

        if (NULL == pe_h) {
                LOGMSG(env, "\nNo Device Selected...\n");
                goto exit;
 	}

        priv = (struct mpsw_drv_private_data *)(pe_h->private_data);
	if (NULL == priv) {
		LOGMSG(env, "\nCannot retrieve device state...\n");
		goto exit;
	}
	dev_h = &priv->dev_h;
 	dc = &priv->st.sc_dev;
	
	if (rio_sc_other_if_names(dev_h, &oth_name)) {
		oth_name = "??????";
	}

	while (i < argc) {
		switch(parm_idx(argv[i], (char *)SC_GEN_FLAG_NAMES)) {
		case SC_F_TX: chk_rx_tx = true;
				sc_tx = true;
				break;
		case SC_F_RX: chk_rx_tx = true;
				sc_tx = false;
				break;
		case SC_F_SRIO: chk_rio_oth = true;
				sc_srio = true;
				break;
		case SC_F_OTH: chk_rio_oth = true;
				sc_srio = false;
				break;
		default:
			switch(parm_idx(argv[i], (char *)SC_FLAG_NAMES)) {
			case sc_f_DROP: flags |= SC_F_DROP;
				chk_flags = true;
				break;
			case sc_f_ERR: flags |= SC_F_ERR;
				chk_flags = true;
				break;
			case sc_f_RTY: flags |= SC_F_RTY;
				chk_flags = true;
				break;
			case sc_f_CS: flags |= SC_F_CS;
				chk_flags = true;
				break;
			case sc_f_PKT: flags |= SC_F_PKT;
				chk_flags = true;
				break;
			case sc_f_DATA: flags |= SC_F_DATA;
				chk_flags = true;
				break;
			default:
				LOGMSG(env, "\nUnknown flag \"%s\"\n", argv[i]);
				goto exit;
			}
		}
		i++;
	}

	for (val_p = 0; val_p < dc->valid_p_ctrs; ++val_p) {
		for (cntr = 0; cntr < dc->p_ctrs[val_p].ctrs_cnt; cntr++) {
			rio_sc_ctr_val_t *stctr = &dc->p_ctrs[val_p].ctrs[cntr];
			// Never print counters that are zero
			if (!stctr->last_inc && !stctr->total) {
				continue;
			}

			if (chk_rx_tx && (stctr->tx != sc_tx)) {
					continue;
			}

			if (chk_rio_oth && (stctr->srio != sc_srio)) {
				continue;
			}

			if (chk_flags && ((flags & SC_FLAG(stctr->sc)) != flags)) {
				continue;
			}

			if (!got_one) {
				got_one = true;
				LOGMSG(env, "\nPt   Counter             Last_Inc   Total\n ");
			}

			LOGMSG(env,
				"\n%2d  %2s %8s %6s 0x%8x 0x%16llx",
				dc->p_ctrs[val_p].pnum,
				stctr->tx ?  (char *)"TX" : (char *)"RX",
				SC_NAME(stctr->sc),
				stctr->srio?  (char *)" SRIO ":oth_name,
				stctr->last_inc,
				stctr->total);
		}
	}
	if (!got_one) {
		LOGMSG(env, "\nAll counters are 0 or no counters selected.\n");
	}

	LOGMSG(env, "\n");
exit:
        return 0;
}

struct cli_cmd CLICountDisplay = {
(char *)"scdisplay",
3,
0,
(char *)"display statistics counter values for a device",
(char *)"{flags}\n"
	"Display selected statistics counters (default is all)\n"
	"{flags} optional, selects the type of counters to display\n"
	"        Values are: " SC_GEN_FLAG_NAMES " and \n"
	"        " SC_FLAG_NAMES "\n",
CLICountDisplayCmd,
ATTR_NONE
};

int CLICountCfgCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
        riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);
        struct mpsw_drv_private_data *priv = NULL;

        if (NULL == pe_h) {
                LOGMSG(env, "\nNo Device Selected...\n");
                goto exit;
        }

        priv = (struct mpsw_drv_private_data *)(pe_h->private_data);
	if (NULL == priv) {
		LOGMSG(env, "\nCannot retrieve device state...\n");
		goto exit;
	}

exit:
        return 0;
}

struct cli_cmd CLICountCfg = {
(char *)"scconfig",
3,
0,
(char *)"Statistics counter configuration",
(char *)"scconfig\n",
CLICountCfgCmd,
ATTR_NONE
};

struct cli_cmd *sc_cmd_list[] = {
&CLICountRead,
&CLICountDisplay,
&CLICountCfg
};

void fmd_bind_dev_sc_cmds(void)
{
	// Init globals used by repeatable commands

	add_commands_to_cmd_db(sizeof(sc_cmd_list)/
			sizeof(struct cli_cmd *), sc_cmd_list);
}

#ifdef __cplusplus
}
#endif

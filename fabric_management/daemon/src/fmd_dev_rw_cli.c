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
#ifdef WINDOWS
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rio_route.h"
#include "tok_parse.h"
#include "did.h"
#include "ct.h"
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

void set_prompt(struct cli_env *e)
{
	riocp_pe_handle pe_h;
	ct_t comptag = 0;
	const char *name = NULL;
	uint16_t pe_did = 0;
	struct cfg_dev cfg_dev;

	if (NULL == e) {
		return;
	}

	if (NULL == e->h) {
		SAFE_STRNCPY(e->prompt, "HUNINIT> ", sizeof(e->prompt));
		return;
	}

	pe_h = (riocp_pe_handle)(e->h);

	if (riocp_pe_get_comptag(pe_h, &comptag)) {
		comptag = 0xFFFFFFFF;
	}
	pe_did = comptag & 0x0000FFFF;

	if (cfg_find_dev_by_ct(comptag, &cfg_dev)) {
		name = riocp_pe_handle_get_device_str(pe_h);
	} else {
		name = (char *)cfg_dev.name;
	}

	snprintf(e->prompt, PROMPTLEN,  "%s.%03x >", name, pe_did);
}

// Globals used by repeatable commands
static uint32_t store_address;
static uint32_t store_numbytes;
static uint32_t store_numacc;
static uint32_t store_data;

static uint32_t mstore_address;
static uint32_t mstore_numbytes;
static uint32_t mstore_numacc;
static uint32_t mstore_data;
static did_t mstore_did;
static hc_t mstore_hc;

void aligningAddress(struct cli_env *env, uint32_t address)
{
	LOGMSG(env,
			"\nNote: Converting address 0x%08x to multiple of %d bytes\n",
			address, 4);
}

void failedReading(struct cli_env *env, uint32_t address, uint32_t rc)
{
	LOGMSG(env, "\nFAILED reading, Address 0x%08x, rc 0x%08x\n", address,
			rc);
}

void failedWrite(struct cli_env *env, uint32_t address, uint32_t data,
		uint32_t rc)
{
	LOGMSG(env,
			"\nFAILED writing, Address 0x%08x, Data 0x%08x, rc = 0x%08x\n",
			address, data, rc);
}

int mport_read(riocp_pe_handle pe_h, uint32_t offset, uint32_t *data)
{
	int rc;
	uint32_t temp_data;

	rc = riocp_drv_reg_rd(pe_h, offset, &temp_data);
	if (!rc)
		*data = temp_data;
	return rc;
}

int mport_write(riocp_pe_handle pe_h, uint32_t offset, uint32_t data)
{
	return riocp_drv_reg_wr(pe_h, offset, data);
}

int select_device(struct cli_env *env, size_t pes_count, riocp_pe_handle *pes,
		char *tok)
{
	size_t i;
	int ret = 1;
	ct_t comptag = 0;
	ct_t pe_ct;

	// selecting a device - set the prompt
	for (i = 0; i < pes_count; i++) {
		// try as a device name
		if (!strcmp(pes[i]->sysfs_name, tok)
				&& (strlen(pes[i]->sysfs_name)
						== strlen(tok))) {
			env->h = pes[i];
			set_prompt(env);
			LOGMSG(env, "\nFound device named \"%s\"\n", tok);
			ret = 0;
			goto exit;
		}
	}

	// try again with a comptag
	if (tok_parse_ct(tok, &comptag, 0)) {
		LOGMSG(env, "\n");
		LOGMSG(env, TOK_ERR_CT_MSG_FMT);
		goto exit;
	}

	for (i = 0; i < pes_count; i++) {
		int rc = riocp_pe_get_comptag(pes[i], &pe_ct);
		if (rc) {
			LOGMSG(env, "\nFailed reading CT: %d\n", rc);
			goto exit;
		}
		if (comptag == pe_ct) {
			env->h = pes[i];
			set_prompt(env);
			LOGMSG(env, "\nFound device for CT 0x%08x\n", pe_ct);
			ret = 0;
			goto exit;
		}
	}
exit:
	return ret;
}

int CLIDevSelCmd(struct cli_env *env, int argc, char **argv)
{
	riocp_pe_handle *pes = NULL;
	size_t pes_count, i;
	int rc;
	ct_t comptag = 0;
	ct_t pe_ct;
	uint32_t num_switches;
	uint32_t num_endpoints;
	const char *dev_name, *vend_name, *sysfs_name;

	rc = riocp_mport_get_pe_list(mport_pe, &pes_count, &pes);
	if (rc) {
		LOGMSG(env, "\nCould not get PE list\n");
		goto exit;
	}

	if (argc && select_device(env, pes_count, pes, argv[0])) {
		goto exit;
	}

	if (NULL != env->h) {
		rc = riocp_pe_get_comptag((riocp_pe_handle)env->h, &comptag);
		if (rc) {
			LOGMSG(env, "\nFailed reading CT: %d\n", rc);
			goto exit;
		}
	}

	if (!pes_count) {
		LOGMSG(env, "\nNo PEs discovered!\n");
		goto exit;
	}

	num_switches = 0;
	num_endpoints = 0;
	for (i = 0; i < pes_count; i++) {
		if (RIOCP_PE_IS_SWITCH(pes[i]->cap)) {
			num_switches++;
		} else {
			num_endpoints++;
		}
	}
	LOGMSG(env, "Number of switches: %d\n", num_switches);
	LOGMSG(env, "Number of endpoints: %d\n", num_endpoints);

	LOGMSG(env,
			"\n  CompTag -->Sysfs Name<-- ----------->> Vendor <<-------------------  Device\n");
	for (i = 0; i < pes_count; i++) {
		pe_ct = pes[i]->comptag;
		sysfs_name = riocp_pe_get_sysfs_name(pes[i]);
		dev_name = riocp_pe_get_device_name(pes[i]);
		vend_name = riocp_pe_get_vendor_name(pes[i]);

		LOGMSG(env, "%s%08x %16s %42s %10s\n",
				(pe_ct == comptag) ? "*" : " ", pe_ct,
				sysfs_name, vend_name, dev_name);
	}

exit:
	rc = riocp_mport_free_pe_list(&pes);
	if (rc) {
		LOGMSG(env, "\nFailed freeing PE list %d\n", rc);
	}
	return 0;
}

struct cli_cmd CLIDevSel = {
(char *)"devsel",
3,
0,
(char *)"display available devices or select a device",
(char *)"{<comptag>}\n"
	"<comptag> Optional parameter, used to select a device as\n"
	"          the target for register reads and writes.\n"
	"          Can be component tag value of device name.\n",
CLIDevSelCmd,
ATTR_RPT
};


/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegReadCmd(struct cli_env *env, int argc, char **argv)
{
	int errorStat = 0;
	uint32_t address;
	uint32_t data, prevRead;
	uint32_t numReads, i;
	int rc;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	numReads = (argc ? 1 : store_numacc);

	switch (argc) {
	case 2:
		if (tok_parse_ul(argv[1], &numReads, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<repeat>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_numacc = numReads;

	for (i = 0; i < numReads; i++) {
		rc = mport_read(pe_h, address, &data);

		if (rc) {
			failedReading(env, address, rc);
			goto exit;
		}
		if (!i) {
			LOGMSG(env, "\t0x%08x\t0x%08x\n", address, data);
		} else if (data != prevRead) {
			LOGMSG(env, "\t0x%08x\t0x%08x (iteration 0x%x)*\n",
					address, data, i);
		}
		prevRead = data;
	}
exit:
	return errorStat;
}

struct cli_cmd CLIRegRead = {
(char *)"read",
1,
1,
(char *)"read register",
(char *)"<address> {<repeat>}\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<repeat>  Optional, number of times to read <address>\n"
	"          Default <repeat> is 1, can be up to 0xFFFFFFFF.\n",
CLIRegReadCmd,
ATTR_RPT
};

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegWriteCmd(struct cli_env *env, int argc, char **argv)

{
	int errorStat = 0;
	uint32_t address;
	uint32_t data;
	uint32_t rc;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	data = store_data;

	switch (argc) {
	case 2:
		if (tok_parse_ul(argv[1], &data, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<data>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_data = data;

	/* Command arguments are syntactically correct - do write */
	rc = mport_write(pe_h, address, data);
	if (0 != rc) {
		failedWrite(env, address, data, rc);
		goto exit;
	}

	/* read data back */
	rc = mport_read(pe_h, address, &data);
	if (0 != rc) {
		failedReading(env, address, rc);
		goto exit;
	} else {
		LOGMSG(env, "\nRead back %08x\n", data);
	}

exit:
	return errorStat;
}

struct cli_cmd CLIRegWrite = {
(char *)"write",
1,
2,
(char *)"write register, then read back updated register value",
(char *)"<address> <data>\n"
	"Write <data> at <address> for current device.\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<data>    a 4 byte value.\n",
CLIRegWriteCmd,
ATTR_RPT
};

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegReWriteCmd(struct cli_env *env, int argc, char **argv)
{
	int errorStat = 0;
	uint32_t address;
	uint32_t data;
	uint32_t repeat, i;
	uint32_t rc;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	data = store_data;
	repeat = (argc ? 1 : store_numacc);

	switch (argc) {
	case 3:
		if (tok_parse_ul(argv[2], &repeat, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<repeat>");
			goto exit;
		}
		// no break
	case 2:
		if (tok_parse_ul(argv[1], &data, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<data>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_data = data;
	store_numacc = repeat;

	for (i = 0; i < repeat; i++) {
		rc = mport_write(pe_h, address, data);
		if (0 != rc) {
			failedWrite(env, address, data, rc);
			goto exit;
		}
	}

	rc = mport_read(pe_h, address, &data);
	if (0 != rc) {
		failedReading(env, address, rc);
		goto exit;
	} else {
		LOGMSG(env, "\nRead back 0x%08x\n", data);
	}
exit:
	return errorStat;
}

struct cli_cmd CLIRegReWrite = {
(char *)"REWrite",
3,
3,
(char *)"write register repeatedly, then read back updated value",
(char *)"<address> <data> <repeat>\n"
"Write <data> at <address> for current device <repeat> times.\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<data>    a 4 byte value.\n"
	"<repeat>  can be up to 0xFFFFFFFF\n",
CLIRegReWriteCmd,
ATTR_RPT
};

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegWriteNoReadbackCmd(struct cli_env *env, int argc, char **argv)

{
	int errorStat = 0;
	uint32_t address;
	uint32_t data;
	uint32_t rc;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	data = store_data;

	switch (argc) {
	case 2:
		if (tok_parse_ul(argv[1], &data, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<data>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_data = data;

	/* Command arguments are syntactically correct - do write */
	rc = mport_write(pe_h, address, data);
	if (0 != rc) {
		failedWrite(env, address, data, rc);
		goto exit;
	} else {
		LOGMSG(env, "\nWrite successfull\n");
	}
exit:
	return errorStat;
}

struct cli_cmd CLIRegWriteNoReadback = {
(char *)"Write",
1,
2,
(char *)"write register",
(char *)"<address> <data>\n"
"Write <data> at <address> for current device\n"
"<address> Register offset. Must be 4 byte aligned.\n"
"<data>    a 4 byte value.\n",
CLIRegWriteNoReadbackCmd,
ATTR_RPT
};

int expect(struct cli_env *env, int argc, char **argv, int inverse)
{
	int errorStat = 0;
	uint32_t address;
	uint32_t data, expdata;
	uint32_t rc;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	expdata = store_data;

	switch (argc) {
	case 2:
		if (tok_parse_ul(argv[1], &expdata, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<data>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_data = expdata;

	rc = mport_read(pe_h, address, &data);
	if (0 != rc) {
		failedReading(env, address, rc);
		goto exit;
	}

	if (((data == expdata) && (!inverse))
			|| ((data != expdata) && (inverse))) {
		LOGMSG(env,
				"\nPASSED: Address: 0x%08x Data 0x%08x ExpData 0x%08x\n",
				address, data, expdata);
	} else {
		LOGMSG(env,
				"\nFAILED: Address: 0x%08x Data 0x%08x ExpData 0x%08x\n",
				address, data, expdata);
	}

exit:
	return errorStat;
}

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegExpectNotCmd(struct cli_env *env, int argc, char **argv)
{
	return expect(env, argc, argv, 1);
}

struct cli_cmd CLIRegExpectNot = {
(char *)"expnot",
4,
2,
(char *)"check register does not match specified value",
(char *)"<address> <data>\n"
	"Read register at <address> on current device, compare to <data>\n"
	"Print an error message if value read is equal to <data>\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<data>    a 4 byte value.\n",
CLIRegExpectNotCmd,
ATTR_RPT
};

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegExpectCmd(struct cli_env *env, int argc, char **argv)
{
	return expect(env, argc, argv, 0);
}

struct cli_cmd CLIRegExpect = {
(char *)"expect",
2,
2,
(char *)"check register matches expected value",
(char *)"<address> <data>\n"
	"Read register at <address> on current device, compare to <data>\n"
	"Print an error message if value read is not equal to <data>\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<data>    a 4 byte value\n",
CLIRegExpectCmd,
ATTR_RPT
};

/* If the structure or syntax of this command changes,
 * please update the Help structure following the procedure.
 */

int CLIRegDumpCmd(struct cli_env *env, int argc, char **argv)

{
	int errorStat = 0;
	uint32_t address, data;
	uint32_t numbytes;
	uint32_t i;
	uint32_t rc;
	static uint32_t store_address, store_numbytes;
	riocp_pe_handle pe_h = (riocp_pe_handle)(env->h);

	if (NULL == pe_h) {
		LOGMSG(env, "\nNo Device Selected...\n");
		goto exit;
	}

	address = store_address;
	numbytes = store_numbytes;

	switch (argc) {
	case 2:
		if (tok_parse_ul(argv[1], &numbytes, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<numbytes>");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	store_address = address;
	store_numbytes = numbytes;

	/* Dump column data for 16 bytes at a time.
	 * First get the output aligned for the entered address
	 */
	LOGMSG(env, "\nAddress  00____03 04____07 08____0B 0C____0F");
	LOGMSG(env, "\n%8x", address & 0xFFFFFFF0);
	for (i = 0; i < (address & 0xF); i += 4) {
		LOGMSG(env, "         ");
	}
	for (i = 0; i < numbytes; i += 4) {
		rc = mport_read(pe_h, address + i, &data);
		if (0 != rc) {
			failedReading(env, address, rc);
			goto exit;
		}
		LOGMSG(env, " %08x", data);
		if ((0xC == ((address + i) & 0xF)) && ((i + 4) < numbytes)) {
			LOGMSG(env, "\n%8x", (address + i + 4) & 0xFFFFFFF0);
		}
	}
	LOGMSG(env, "\n");

	/* store data for continuous dump command */
	store_address = address + numbytes;
	store_numbytes =  numbytes;

exit:
	return errorStat;
}

struct cli_cmd CLIRegDump = {
(char *)"dump",
1,
2,
(char *)"display a block of memory/registers",
(char *)"<address> <numbytes>\n"
"Read 4 byte registers starting at <address> on current device\n"
	"<address>  will be rounded down to 4 byte alignment.\n"
	"<numbytes> will be rounded up to 4 byte alignment.\n",
CLIRegDumpCmd,
ATTR_RPT
};

int CLIMRegReadCmd(struct cli_env *env, int argc, char **argv)
{
	int errorStat = 0;
	uint32_t address;
	uint32_t data, prevRead;
	uint32_t numReads, i;
	did_val_t did_val;
	did_t did;
	hc_t hc;
	int rc;

	address = mstore_address;
	did = mstore_did;
	hc = mstore_hc;
	numReads = (argc ? 1 : mstore_numacc);

	switch(argc) {
	case 4:
		if (tok_parse_ul(argv[3], &numReads, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<repeat>");
			goto exit;
		}
		// no break
	case 3:
		if (tok_parse_hc(argv[2], &hc, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_HC_MSG_FMT);
			goto exit;
		}
		// no break
	case 2:
		if (tok_parse_did(argv[1], &did_val, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_DID_MSG_FMT);
			goto exit;
		}
		did = (did_t){did_val, dev08_sz};
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "<address>");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	mstore_address = address;
	mstore_did = did;
	mstore_hc = hc;
	mstore_numacc = numReads;

	for (i = 0; i < numReads; i++) {
		rc = riocp_drv_raw_reg_rd((riocp_pe_handle)env->h,
			did, hc, address, &data);

		if (rc) {
			failedReading(env, address, rc);
			goto exit;
		}
		if (!i) {
			LOGMSG(env, "\t0x%08x\t0x%08x\n", address, data);
		} else if (data != prevRead) {
			LOGMSG(env, "\t0x%08x\t0x%08x (iteration 0x%x)*\n",
					address, data, i);
		}
		prevRead = data;
	}
exit:
	return errorStat;
}

struct cli_cmd CLIMRegRead = {
(char *)"mread",
2,
1,
(char *)"maintenance read register",
(char *)"<address> {<devid> <hc> {<repeat>}}\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<did>     Optional, device ID to read.\n"
	"          If not present, use last entered <did>.\n"
	"<hc>      Optional, hop count. Specify 0xFF to access mport.\n"
	"          If not present, use last entered <hc>.\n"
	"<repeat>  Optional, number of times to read <address>\n"
	"          Default <repeat> is 1, can be up to 0xFFFFFFFF.\n",
CLIMRegReadCmd,
ATTR_RPT
};

int CLIMRegWriteCmd(struct cli_env *env, int argc, char **argv)
{
	int errorStat = 0;
	uint32_t address;
	did_val_t did_val;
	did_t did;
	hc_t hc;
	uint32_t data;
	uint32_t rc;

	address = mstore_address;
	data = mstore_data;
	did = mstore_did;
	hc = mstore_hc;

	switch(argc) {
	case 4:
		if (tok_parse_hc(argv[3], &hc, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_HC_MSG_FMT);
			goto exit;
		}
		// no break
	case 3:
		if (tok_parse_did(argv[2], &did_val, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_DID_MSG_FMT);
			goto exit;
		}
		did = (did_t){did_val, dev08_sz};
		// no break
	case 2:
		if (tok_parse_ul(argv[1], &data, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "data");
			goto exit;
		}
		// no break
	case 1:
		if (tok_parse_ul(argv[0], &address, 0)) {
			LOGMSG(env, "\n");
			LOGMSG(env, TOK_ERR_UL_HEX_MSG_FMT, "address");
			goto exit;
		}
		if ((address % 4) != 0) {
			aligningAddress(env, address);
			address = address - (address % 4);
		}
		// no break
	case 0:
		break;
	default:
		LOGMSG(env, "\nInvalid number of arguments: %d\n", argc);
		goto exit;
	}

	mstore_address = address;
	mstore_data = data;
	mstore_did = did;
	mstore_hc = hc;

	/* Command arguments are syntactically correct - do write */

	rc = riocp_drv_raw_reg_wr((riocp_pe_handle)env->h,
		did, hc, address, data);

	if (0 != rc) {
		failedWrite(env, address, data, rc);
		goto exit;
	}

	/* read data back */
	rc = riocp_drv_raw_reg_rd((riocp_pe_handle)env->h,
		did, hc, address, &data);

	if (0 != rc) {
		failedReading(env, address, rc);
		goto exit;
	} else {
		LOGMSG(env, "\nRead back %08x\n", data);
	}

exit:
	return errorStat;
}

struct cli_cmd CLIMRegWrite = {
(char *)"mwrite",
2,
2,
(char *)"write register, then read back updated register value",
(char *)"<address> <data> {<devid> <hc>}\n"
	"Write <data> at <address> for device <devid> at <hc> hops away.\n"
	"<address> Register offset. Must be 4 byte aligned.\n"
	"<data>    4 byte value to write.\n"
	"<did>     Optional, device ID to access.\n"
	"          If not present, use last <did> entered.\n"
	"<hc>      Optional, hop count. Use 0xFF to access local mport registers.\n"
	"          If not present, use last <hc> entered.\n",
CLIMRegWriteCmd,
ATTR_RPT
};

struct cli_cmd *reg_cmd_list[] = {
&CLIRegRead,
&CLIRegWrite,
&CLIRegReWrite,
&CLIRegWriteNoReadback,
&CLIRegExpect,
&CLIRegExpectNot,
&CLIRegDump,
&CLIMRegRead,
&CLIMRegWrite,
&CLIDevSel
};

void fmd_bind_dev_rw_cmds(void)
{
	// Init globals used by repeatable commands
	store_address = 0;
	store_numbytes = 4;
	store_numacc = 1;
	store_data = 0;

	mstore_address = 0;
	mstore_did = DID_INVALID_ID;
	mstore_hc = HC_MP;
	mstore_numbytes = 4;
	mstore_numacc = 1;
	mstore_data = 0;

	add_commands_to_cmd_db(sizeof(reg_cmd_list)/
			sizeof(struct cli_cmd *), reg_cmd_list);
};

#ifdef __cplusplus
}
#endif

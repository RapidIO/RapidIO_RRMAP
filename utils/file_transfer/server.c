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

#include "rio_route.h"
#include "string_util.h"
#include "tok_parse.h"
#include "rio_misc.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"
#include "rapidio_mport_dma.h"
#include "rrmap_config.h"
#include "libcli.h"
#include "liblog.h"
#include "libfxfr_private.h"
#include "librsvdmem.h"

#define MAX_IBWIN 8

#ifdef __cplusplus
extern "C" {
#endif

uint8_t debug;

void print_server_help(void)
{
	printf("\nThe file_xfer server accepts requests to send files.\n");
	printf("The following options are supported:\n");
	printf("-h, -H, ?: Displays an explanation of parameters and syntax\n");
	printf("-d	 : Disables display of debug messages. This is the \n");
	printf("	   default operating mode for the server.\n");
	printf("-D	 : Displays debug/trace/error messages\n");
	printf("-c <skt> : Supports remote console connectivity using"
							" <skt>.\n");
	printf("	 The default <skt> value is %d.\n", FXFR_DFLT_CLI_SKT);
	printf("-m <mport>: Accept requests on <mport>.\n");
	printf("        0-%u is the range of valid mport values.\n",
						RIO_MAX_MPORTS - 1);
	printf("	 The default mport value is 0. \n");
	printf("-n, -N: Disables the command line interface for this"
							" server.\n");
	printf("	 Note that a server command line can still be\n");
	printf("	 accessed using Ethernet sockets (see the \"-c\""
							" parameter).\n");
	printf("-S <size>: Size in kilobytes of each inbound RDMA window.\n");
	printf("	Valid values: 128, 256, 512, 1024, 2048, and 4096.\n");
	printf("	The default value is %d.\n", TOTAL_TX_BUFF_SIZE/1024);
	printf("	The larger the window, the faster the transfer.\n");

	printf("-W <buffers>: Attempts to create <buffers> separate \n");
	printf("         buffers/file receive worker threads.\n");
	printf("	 The default value is 1.\n");
	printf("	 The maximum number of parallel file transfers is\n");
	printf("	 equal to the number of buffers. The\n");
	printf("	 server will continue to operate as long as it has\n");
	printf("	 at least one buffer.\n");
	printf("-X <cm_skt>: Server RapidIO Channel Manager socket.\n");
	printf("	 The default value is %d.\n", FXFR_DFLT_SVR_CM_PORT);
	printf("	 The cm_skt and mport value must be correct to\n");
	printf("	 successfully connect .\n");
}

void parse_options(int argc, char *argv[], 
		int *cons_skt,
		int *print_help,
		uint8_t *mport_num,
		int *run_cons,
		int *win_size,
		int *num_buffs, 
		int *xfer_skt)
{
	int idx;
	uint32_t tmp32;
	uint16_t tmp16;

	*cons_skt = FXFR_DFLT_CLI_SKT;
	*print_help = 0;
	*mport_num = 0;
	*run_cons = 1;
	*win_size = TOTAL_TX_BUFF_SIZE/1024;
	*num_buffs = 1;
	*xfer_skt = FXFR_DFLT_SVR_CM_PORT;

	for (idx = 0; (idx < argc) && !*print_help; idx++) {
		if (strnlen(argv[idx], 4) < 2)
			continue;

		if ('-' == argv[idx][0]) {
			switch(argv[idx][1]) {
			case 'd':
				debug = 0;
				break;
			case 'D':
				debug = 1;
				break;
			case 'c': 
			case 'C':
				idx++;
				if (argc < idx) {
					  printf("\n<skt> not specified\n");
					  *print_help = 1;
					  goto exit;
				}
				if (tok_parse_socket(argv[idx], &tmp16, 0)) {
					printf("\n");
					printf(TOK_ERR_SOCKET_MSG_FMT, "<skt>");
					*print_help = 1;
					goto exit;
				}
				*cons_skt = (int)tmp16;
				break;
			case '?': 
			case 'h': 
			case 'H':
				*print_help = 1;
				break;
			case 'm': 
			case 'M': 
				idx++;
				if (argc < idx) {
					printf("\n<mport> not specified\n");
					*print_help = 1;
					goto exit;
				}
				if (tok_parse_mport_id(argv[idx], &tmp32, 0)) {
					printf("\n");
					printf(TOK_ERR_MPORT_MSG_FMT);
					*print_help = 1;
					goto exit;
				}
				*mport_num = (int)tmp32;
				break;
			case 'n': 
			case 'N':
				*run_cons = 0;
				break;
			case 's': 
			case 'S':
				idx++;
				if (argc < idx) {
					printf("\n<size> not specified\n");
					*print_help = 1;
					goto exit;
				}
				if (tok_parse_ulong(argv[idx], &tmp32, 0, TOTAL_TX_BUFF_SIZE/1024, 0)) {
					printf("\n");
					printf(TOK_ERR_ULONG_HEX_MSG_FMT, "<size>", 0, TOTAL_TX_BUFF_SIZE/1024);
					*print_help = 1;
					goto exit;
				}
				*win_size = (int)tmp32;
				break;
			case 'w':
			case 'W':
				idx++;
				if (argc < idx) {
					printf("\n<buffers> not specified\n");
					*print_help = 1;
					goto exit;
				}
				if (tok_parse_ulong(argv[idx], &tmp32, 0, 9, 0)) {
					printf("\n");
					printf(TOK_ERR_ULONG_HEX_MSG_FMT, "<buffers>", 0, 9);
					*print_help = 1;
					goto exit;
				}
				*num_buffs = (int)tmp32;
				break;
			case 'x': 
			case 'X':
				idx++;
				if (argc < idx) {
					printf("\n<skt> not specified\n");
					*print_help = 1;
					goto exit;
				}
				if (tok_parse_socket(argv[idx], &tmp16, 0)) {
					printf("\n");
					printf(TOK_ERR_SOCKET_MSG_FMT, "<skt>");
					*print_help = 1;
					goto exit;
				}
				*xfer_skt = (int)tmp16;
				break;
			default:
				printf("\nUnknown parm: \"%s\"\n", argv[idx]);
				*print_help = 1;
			}
		}
	}

exit:
	*win_size = *win_size * 1024;
}

pthread_t conn_thread, console_thread;
int cli_session_portno;
int cli_session_alive;

void set_prompt(struct cli_env *e)
{
	if (e != NULL) {
		SAFE_STRNCPY(e->prompt, "FXServer> ", sizeof(e->prompt));
	}
}

uint8_t mp_h_mport_num;
struct riomp_mgmt_mport_properties qresp;
riomp_mport_t mp_h;
int mp_h_valid;

// buff_cnt is the number of rx_bufs[] that are allocated.
int buff_cnt = 1;
struct buffer_info *rx_bufs = NULL;

struct req_list_t {
	struct req_list_t *next;
	riomp_sock_t *skt;
};

/* NOTE: list is empty when HEAD == NULL.
 * When HEAD == NULL, tail is invalid.
 */

struct req_list_head_t {
	struct req_list_t *head;
	struct req_list_t *tail;
};

riomp_sock_t *pop_conn_req(struct req_list_head_t *list);

volatile int all_must_die;
sem_t conn_reqs_mutex;
struct req_list_head_t conn_reqs;
uint16_t num_conn_reqs;
uint8_t pause_file_xfer_conn;
uint8_t max_file_xfer_queue;

sem_t conn_loop_started;
int conn_loop_alive;
int conn_skt_num;
extern struct cli_cmd FXStatus;

int FXStatusCmd(struct cli_env *env, int argc, char **argv)
{
	int	idx, st_idx = 0, max_idx = buff_cnt;

	if (argc) {
		st_idx = getDecParm(argv[0], 0);
		max_idx = st_idx+1;
		if ((st_idx >= buff_cnt) || (argc > 2))
			goto show_help;
		if (argc > 1)
			rx_bufs[st_idx].debug = getDecParm(argv[1], 0)?1:0;
	}

	
	LOGMSG(env,
			"\nWin V   RapidIO Addr    Size    Memory Space PHYS TV D C       RC I\n");
	for (idx = st_idx; idx < max_idx; idx++) {
		LOGMSG(env, "%2d  %1d %16lx %8lx  %16lx  %1d %1d %1d %8x %1s\n",
				idx, rx_bufs[idx].valid,
				(long unsigned int )rx_bufs[idx].rio_base,
				(long unsigned int )rx_bufs[idx].length,
				(long unsigned int )rx_bufs[idx].handle,
				rx_bufs[idx].thr_valid, rx_bufs[idx].debug,
				rx_bufs[idx].completed, rx_bufs[idx].rc,
				rx_bufs[idx].is_an_ibwin ? "Y" : "N");
	}
	LOGMSG(env, "\nall_must_die status : %d\n", all_must_die);
	LOGMSG(env, "Server destID       : %d\n", qresp.did_val);
	LOGMSG(env, "Server cm_skt       : %d\n", conn_skt_num);
	LOGMSG(env, "Server Accept Lp OK : %d\n", conn_loop_alive);
	LOGMSG(env, "Pending conn reqs   : %s\n",
			(conn_reqs.head) ? "AVAILABLE" : "None");
	LOGMSG(env, "\nCLI Sessions Alive : %d", cli_session_alive);
	if (!cli_session_alive) {
		LOGMSG(env, "  FAILED! MULTIPLE FXFR_SERVER PROCESSES?");
	}
	LOGMSG(env, "\nCLI Socket #       : %d", cli_session_portno);
	LOGMSG(env, "\n\nServer mport       : %d\n", mp_h_mport_num);

	return 0;

show_help:
	LOGMSG(env, "\nFAILED: Extra parms or invalid values\n");
	cli_print_help(env, &FXStatus);

	return 0;
}

struct cli_cmd FXStatus = {
"status",
2,
0,
"File transfer status command.",
"{IDX {DBG}}\n" 
	"Dumps the status of the file transfer state database.\n"
	"<IDX> : 0-7, Optionally limits display to a single process.\n"
	"<DBG> : 0|1  Disables (0) or enables (1) debug statements.\n",
FXStatusCmd,
ATTR_RPT
};

void fxfr_server_shutdown(void);

int FXShutdownCmd(struct cli_env *env, int UNUSED(argc), char **UNUSED(argv))
{
	fxfr_server_shutdown();
	LOGMSG(env, "Shutdown initiated...\n");
	return 0;
}

struct cli_cmd FXShutdown = {
"shutdown",
8,
0,
"Shutdown server.",
"No Parameters\n" 
	"Shuts down all threads, including CLI.\n",
FXShutdownCmd,
ATTR_NONE
};

int FXMpdevsCmd(struct cli_env *env, int argc, char **argv)
{
	mport_list_t *mport_list = NULL;
	mport_list_t *list_ptr;
	uint8_t  number_of_mports = RIO_MAX_MPORTS;
	uint8_t mport_id;

	did_val_t *ep_list = NULL;
	uint32_t number_of_eps = 0;
	uint32_t ep;
	int i;
	int ret;

	if (argc) {
		LOGMSG(env, "FAILED: Extra parameters ignored: %s\n", argv[0]);
	}

	ret = riomp_mgmt_get_mport_list(&mport_list, &number_of_mports);
	if (ret) {
		LOGMSG(env, "ERR: riomp_mport_get_mport_list() ERR %d\n", ret);
		return 0;
	}

	printf("\nAvailable %d local mport(s):\n", number_of_mports);
	if (number_of_mports > RIO_MAX_MPORTS) {
		LOGMSG(env, "WARNING: Only %d out of %d have been retrieved\n",
				RIO_MAX_MPORTS, number_of_mports);
	}

	list_ptr = mport_list;
	for (i = 0; i < number_of_mports; i++, list_ptr++) {
		mport_id = *list_ptr >> 16;
		LOGMSG(env, "+++ mport_id: %u dest_id: %u\n", mport_id,
				*list_ptr & 0xffff);

		/* Display EPs for this MPORT */

		ret = riomp_mgmt_get_ep_list(mport_id, &ep_list,
				&number_of_eps);
		if (ret) {
			LOGMSG(env, "ERR: riomp_ep_get_list() ERR %d\n", ret);
			break;
		}

		LOGMSG(env, "\t%u Endpoints (dest_ID): ", number_of_eps);
		for (ep = 0; ep < number_of_eps; ep++) {
			LOGMSG(env, "%u ", *(ep_list + ep));
		}

		LOGMSG(env, "\n");

		ret = riomp_mgmt_free_ep_list(&ep_list);
		if (ret) {
			LOGMSG(env, "ERR: riomp_ep_free_list() ERR %d\n", ret);
		}

	}

	LOGMSG(env, "\n");

	ret = riomp_mgmt_free_mport_list(&mport_list);
	if (ret) {
		LOGMSG(env, "ERR: riomp_ep_free_list() ERR %d\n", ret);
	}

	return 0;
}

struct cli_cmd FXMpdevs = {
"mpdevs",
2,
0,
"Query mport info.",
"No Parameters\n" 
	"Displays available mports, and associated target destigation IDs.\n",
FXMpdevsCmd,
ATTR_NONE
};

void print_file_xfer_status(struct cli_env *env)
{
	LOGMSG(env, "\nFile transfer status: %s\n",
			pause_file_xfer_conn ? "PAUSED" : "running");

	if (pause_file_xfer_conn) {
		LOGMSG(env, "Maximum Q length: %d\n", max_file_xfer_queue);
		LOGMSG(env, "Current Q length: %d\n", num_conn_reqs);
	}
}
		
void batch_start_connections(void)
{
	int i;

	pause_file_xfer_conn = 0;

	for (i = 0; (i < buff_cnt) && num_conn_reqs; i++) {
		if (rx_bufs[i].valid && rx_bufs[i].completed) {
			rx_bufs[i].req_skt = pop_conn_req(&conn_reqs);
			rx_bufs[i].completed = 0;
			sem_post(&rx_bufs[i].req_avail);
		}
	}
}
	
	
int FXPauseCmd(struct cli_env *env, int argc, char **argv)
{
	if (argc) {
		pause_file_xfer_conn = getDecParm(argv[0], 0)?1:0;
		max_file_xfer_queue = 8;
	}
	if (argc > 1) {
		max_file_xfer_queue = getDecParm(argv[1], 0);
	}

	if ((num_conn_reqs >= max_file_xfer_queue) ||
		(!pause_file_xfer_conn && num_conn_reqs)) {

		sem_wait(&conn_reqs_mutex);
		batch_start_connections();
		sem_post(&conn_reqs_mutex);
	}

	print_file_xfer_status(env);

	return 0;
}

struct cli_cmd FXPause = {
"pause",
5,
0,
"Pause file request transfer processing",
"pause {<PAUSE> {<NUMREQS>}}\n"
	"PAUSE is either 0 (Transfer files) or 1 (Halt file transfers).\n"
	"NUMREQS: 0-255, maximum number of requests pending before PAUSE is\n"
	"	 set back to 0.\n"
	"	 Default value is 8 when PAUSE is set to 1.\n",
FXPauseCmd,
ATTR_NONE
};

struct cli_cmd *server_cmds[] = 
	{ &FXStatus,
	  &FXShutdown,
	  &FXMpdevs,
	  &FXPause
	};

void bind_server_cmds(void)
{
	all_must_die = 0;
	num_conn_reqs = 0;
	pause_file_xfer_conn = 0;
	max_file_xfer_queue = 0;
	add_commands_to_cmd_db(sizeof(server_cmds)/sizeof(server_cmds[0]), 
				server_cmds);

}

#define RIO_MPORT_DEV_PATH "/dev/rio_mport"

void add_conn_req( struct req_list_head_t *list, riomp_sock_t *new_socket)
{
	struct req_list_t *new_req;

	new_req = (struct req_list_t *)(malloc(sizeof(struct req_list_t)));
	if (NULL == new_req) {
		return;
	}

	new_req->skt = new_socket;
	new_req->next = NULL;

	if (NULL == list->head) {
		list->head = list->tail = new_req;
	} else {
		list->tail->next = new_req;
		list->tail = new_req;
	}
	num_conn_reqs++;
}

riomp_sock_t *pop_conn_req(struct req_list_head_t *list)
{
	riomp_sock_t *skt = NULL;
	struct req_list_t *temp;

	if (NULL != list->head) {
		skt = list->head->skt;
		temp = list->head;
		list->head = list->head->next;
		free(temp);
		num_conn_reqs--;
	}
	return skt;
}

void prep_info_for_xfer(struct buffer_info *info)
{
	memset(info->file_name, 0, MAX_FILE_NAME);
	info->rc = 0;
	info->fd = -1;
	info->bytes_rxed = 0;
}

void *xfer_loop(void *ibwin_idx)
{
	struct buffer_info *info;
	int idx = *(int *)(ibwin_idx);
	char thr_name[16] = {0};

	free(ibwin_idx);
	info = &rx_bufs[idx];

	snprintf(thr_name, 15, "XFER_%02x", idx);
	pthread_setname_np(rx_bufs[idx].xfer_thread, thr_name);

	info->req_skt = NULL;
	info->completed = 1;

	if (debug)
		printf("\nxfer_loop idx %d starting\n", idx);

	while(!all_must_die) {
		prep_info_for_xfer(info);
		if (NULL == info->req_skt)
			sem_wait(&info->req_avail);

		if (info->debug)
			printf("Thread %d processing connect request.\n", idx);
		info->rc = rx_file(info, &all_must_die);

		if (NULL != info->req_skt) {
			riomp_sock_close(info->req_skt);
			free(info->req_skt);
			info->req_skt = NULL;
		}

		if (!pause_file_xfer_conn) {
			sem_wait(&conn_reqs_mutex);
			info->req_skt = pop_conn_req(&conn_reqs);
			if (NULL == info->req_skt)
				info->completed = 1;
			sem_post(&conn_reqs_mutex);
		}
	}

	if (info->is_an_ibwin) {
		riomp_dma_ibwin_free(mp_h, &info->handle);
	}
	info->thr_valid = 0;
	info->valid = 0;

	if (info->debug || debug)
		printf("\nxfer_loop idx %d EXITING\n", idx);

	pthread_exit(EXIT_SUCCESS);
}

riomp_mailbox_t conn_mb;
riomp_sock_t conn_skt;

void *conn_loop(void *ret)
{
	int rc;
	riomp_sock_t *new_socket = NULL;
	uint8_t found_one = 0;
	int i;
	int xfer_skt_num;
	char thr_name[16] = {0};
	
	conn_loop_alive = 0;

	if (NULL == ret) {
		if (debug)
			printf("conn_loop: Parameter is null, exiting\n");
		goto exit;
	}

	xfer_skt_num = *(int *)(ret);
	free(ret);
	conn_skt_num = xfer_skt_num;
	rc = riomp_sock_mbox_create_handle(mp_h_mport_num, 0, &conn_mb);
	if (rc) {
		if (debug)
			printf("conn_loop: riomp_mbox_create_handle ERR %d\n", 
				rc);
		goto close_mb;
	}

	rc = riomp_sock_socket(conn_mb, &conn_skt);
	if (rc) {
		if (debug)
			printf("conn_loop: riomp_sock_socket ERR %d\n", rc);
		goto close_mb;
	}

	rc = riomp_sock_bind(conn_skt, xfer_skt_num);
	if (rc) {
		if (debug)
			printf("conn_loop: riomp_sock_bind() ERR %d\n", rc);
		goto close_skt;
	}

	rc = riomp_sock_listen(conn_skt);
	if (rc) {
		if (debug) {
			printf("conn_loop: riomp_sock_listen() ERR %d\n", rc);
		}
		goto close_skt;
	}

	if (debug) {
		printf("\nSERVER File Transfer bound to socket %d\n", 
			xfer_skt_num);
	}
	conn_loop_alive = 1;
	sem_post(&conn_loop_started);

	snprintf(thr_name, 15, "CONN_MGR");
	pthread_setname_np(conn_thread, thr_name);

	while (!all_must_die) {
		if (NULL == new_socket) {
			new_socket = (riomp_sock_t *)
				(malloc(sizeof(riomp_sock_t)));
			if (NULL == new_socket) {
				break;
			}
			rc = riomp_sock_socket(conn_mb, new_socket);
			if (rc) {
				if (debug) {
					printf("socket() ERR %d\n", rc);
				}
				break;
			}
		}

		rc = riomp_sock_accept(conn_skt, new_socket, 1000,
								&all_must_die);
		if (rc) {
			if (debug) {
				printf("riomp_accept() ERR %d\n", rc);
			}
			break;
		}

		sem_wait(&conn_reqs_mutex);

		if (all_must_die) {
			riomp_sock_close(new_socket);
			sem_post(&conn_reqs_mutex);
			continue;
		}

		found_one = 0;
		for (i = 0; (i < buff_cnt) && !pause_file_xfer_conn; i++) {
			if (rx_bufs[i].valid && rx_bufs[i].completed) {
				rx_bufs[i].req_skt = new_socket;
				rx_bufs[i].completed = 0;
				sem_post(&rx_bufs[i].req_avail);
				found_one = 1;
				break;
			}
		}

		if (!found_one) {
			//@sonar:off - c:S3584 Allocated memory not released
			// It is not supposed to be released, the request
			// must be queued for later processing.
			add_conn_req( &conn_reqs, new_socket);
			//@sonar:on
		}

		if (pause_file_xfer_conn && 
				(num_conn_reqs >= max_file_xfer_queue)) {
			batch_start_connections();
		}

		sem_post(&conn_reqs_mutex);
		new_socket = NULL;
	}

	/* Make sure all connection requests are cleaned up */
	free(new_socket);
	new_socket = pop_conn_req(&conn_reqs);

	while ((NULL != new_socket) && (*new_socket)) {
		rc = riomp_sock_close(new_socket);
		if (rc && debug)
			printf("conn_loop: nskt riomp_socket_close ERR %d\n", 
				rc);
		new_socket = pop_conn_req(&conn_reqs);
	}
		
close_skt:
	rc = riomp_sock_close(&conn_skt);
	if (rc && debug)
		printf("conn_loop: riomp_socket_close ERR %d\n", rc);

close_mb:
	rc = riomp_sock_mbox_destroy_handle(&conn_mb);
	if (rc && debug)
		printf("conn_loop: riomp_mbox_shutdown ERR: %d\n", rc);

exit:
	if (debug)
		printf("\nSERVER File Transfer EXITING\n");
	conn_loop_alive = 0;
	sem_post(&conn_loop_started);
	pthread_exit(0);
}

int setup_ibwins(int num_buffs, uint32_t buff_size)
{
	int i, rc;

	if (num_buffs > MAX_IBWIN) {
		WARN("Only %d windows created, %d requested.",
							MAX_IBWIN, num_buffs);
		num_buffs = MAX_IBWIN;
	}

	buff_cnt = num_buffs;

        for (i = 0; i < num_buffs; i++) {
                rx_bufs[i].rio_base = RIO_ANY_ADDR;
                rx_bufs[i].handle = RIO_ANY_ADDR;
                rx_bufs[i].length = buff_size;
                if (riomp_dma_ibwin_map(mp_h, &rx_bufs[i].rio_base,
				rx_bufs[i].length, &rx_bufs[i].handle)) {
                        rc = 5;
                        rx_bufs[i].length = 0;
                        printf("\nCould not map ibwin %d...\n", i);
                        goto close_ibwin;
                }

                rc = riomp_dma_map_memory(mp_h, rx_bufs[i].length,
                                rx_bufs[i].handle, (void **)&rx_bufs[i].ib_mem);
                if (rc) {
                        CRIT("riomp_dma_map_memory");
                        goto close_ibwin;
                }

                rx_bufs[i].valid = 1;
        }

        return 0;

close_ibwin:
        for (i = 0; i < buff_cnt; i++) {
                if (rx_bufs[i].length) {
                        if (rx_bufs[i].ib_mem != MAP_FAILED) {
                                riomp_dma_unmap_memory(rx_bufs[i].length,
                                        rx_bufs[i].ib_mem);
                                rx_bufs[i].ib_mem = (char *)MAP_FAILED;
                        }
                        riomp_dma_ibwin_free(mp_h, &rx_bufs[i].handle);
                        rx_bufs[i].length = 0;
                }
        }
	return rc;
}

int setup_buffers(int num_buffs, uint32_t buff_size)
{
	int i;
	int rc;
	uint64_t rsvd_addr, rsvd_size;
	uint64_t req_size = (uint64_t)buff_size * (uint64_t)num_buffs;
	
	rx_bufs = (struct buffer_info *)calloc(num_buffs, sizeof(struct buffer_info));
	if (NULL == rx_bufs) {
		rc = -ENOMEM;
		goto exit;
	}

	for (i = 0; i < num_buffs; i++) {
		sem_init(&rx_bufs[i].req_avail, 0, 0);
		rx_bufs[i].ib_mem = (char *)MAP_FAILED;
	}

	rc = get_rsvd_phys_mem(RSVD_PHYS_MEM_FXFR, &rsvd_addr, &rsvd_size);
	if (rc) {
		rc = setup_ibwins(num_buffs, buff_size);
		goto exit;
	}
	
	/* Reserved memory exists, so map it... */

	if (rsvd_size < req_size) {
		int new_num_buffs = rsvd_size / buff_size;
		WARN("Only %d buffers created, %d requested.",
						new_num_buffs, num_buffs);
		num_buffs = new_num_buffs;
	}
	if (!num_buffs) {
		CRIT("\nRsvd memory size less than one buffer! Exiting\n");
		rc = -1;
		goto exit;
	}

	buff_cnt = num_buffs;

	rx_bufs[0].rio_base = RIO_ANY_ADDR;
	rx_bufs[0].handle = rsvd_addr;
	rx_bufs[0].length = rsvd_size;

	rc = riomp_dma_ibwin_map(mp_h, &rx_bufs[0].rio_base, 
					rx_bufs[0].length, &rx_bufs[0].handle);
	if (rc) {
		printf("\nriomp_dma_ibwin_map could not map handle. Exiting\n");
		printf("rc %d errno %d : %s\n", rc, errno, strerror(errno));
		rc = -1;
		goto exit;
	}
	rc = riomp_dma_map_memory(mp_h, rx_bufs[0].length,
				rx_bufs[0].handle, (void **)&rx_bufs[0].ib_mem);
	if (rc) {
		printf("riomp_dma_map_memory failed. Exiting");
		printf("rc %d errno %d : %s\n", rc, errno, strerror(errno));
		goto exit;
	}
	rx_bufs[0].is_an_ibwin = true;

	for (i = 0; i < num_buffs; i++) {
		rx_bufs[i].rio_base = rx_bufs[0].rio_base + (buff_size * i);
		rx_bufs[i].ib_mem = rx_bufs[0].ib_mem + (buff_size * i);
		rx_bufs[i].handle = rsvd_addr + (buff_size * i);
		rx_bufs[i].length = buff_size;
		rx_bufs[i].is_an_ibwin = true;
		rx_bufs[i].valid = 1;
	}

	return 0;
exit:
	return rc;
	
}

int setup_mport(uint8_t mport_num, int num_buffs, uint32_t win_size)
{
	int rc;

	all_must_die = 0;

	mp_h_mport_num = mport_num;
	rc = riomp_mgmt_mport_create_handle(mport_num, 0, &mp_h);
	if (rc) {
		printf("\nUnable to open mport %d...\n", mport_num);
		goto close_mport;
	}
	mp_h_valid = 1;

	if (riomp_mgmt_query(mp_h, &qresp)) {
		printf("\nUnable to query mport %d...\n", mport_num);
		goto close_mport;
	}

	if (debug)
		riomp_mgmt_display_info(&qresp);

	if (!(qresp.flags & RIO_MPORT_DMA)) {
		printf("\nMport %d has no DMA support...\n", mport_num);
		goto close_mport;
	}

	rc = setup_buffers(num_buffs, win_size);
	if (!rc) {
		return 0;
	}
close_mport:
	if (mp_h_valid) {
		riomp_mgmt_mport_destroy_handle(&mp_h);
		mp_h_valid = 0;
	}

	return rc;
}

int spawned_threads;
void fxfr_server_shutdown_cli(struct cli_env *env);

//@sonar:off - c:S3584 Allocated memory not released
//    conn_loop_rc is freed by conn_thread
//    pass_idx is freed by rx_bufs[i].xfer_thread
//    rlp is freed by the remote_login_thread
//    all are freed by this routine in the appropriate error conditions
void spawn_threads(int cons_skt, int xfer_skt, int run_cons)
{
	struct remote_login_parms *rlp;
	int conn_ret;
	int cli_ret;
	int cons_ret = 0;
	int *conn_loop_rc;
	int i;

	all_must_die = 0;
	conn_loop_alive = 0;
	spawned_threads = 1;

	cli_init_base(fxfr_server_shutdown_cli);
	bind_server_cmds();
	liblog_bind_cli_cmds();

	/* Prepare and start console thread */
	if (run_cons) {
		char thr_name[16] = {0};
		struct cli_env t_env;

		init_cli_env(&t_env);
		splashScreen(&t_env, (char *)
			"RTA File Transfer Server Command Line Interface");

		cons_ret = pthread_create( &console_thread, NULL, 
				console, (void *)((char *)"FXServer> "));
		if(cons_ret) {
			printf("\nError cons_thread rc: %d\n", cons_ret);
			exit(EXIT_FAILURE);
		}

		snprintf(thr_name, 15, "CONSOLE");
		pthread_setname_np(console_thread, thr_name);
	}

	/* Prepare and start CM connection handling thread */
	sem_init(&conn_reqs_mutex, 0, 0);
	conn_reqs.head = NULL;
	conn_reqs.tail = NULL;
	sem_post(&conn_reqs_mutex);

	sem_init(&conn_loop_started, 0, 0);

	conn_loop_rc = (int *)(malloc(sizeof(int)));
	if (NULL == conn_loop_rc) {
		printf("Error - failed to allocate memory\n");
		exit(EXIT_FAILURE);
	}
	*conn_loop_rc = xfer_skt;
	conn_ret = pthread_create(&conn_thread, NULL, conn_loop, 
				(void *)(conn_loop_rc));
	if (conn_ret) {
		printf("Error - conn_thread rc: %d\n", conn_ret);
		free(conn_loop_rc);
		exit(EXIT_FAILURE);
	}
 
	for (i = 0; i < buff_cnt; i++) {
		int *pass_idx;
		int ibwin_ret;

		if (!rx_bufs[i].valid) {
			continue;
		}

		pass_idx = (int *)(malloc(sizeof(int)));
		if (NULL == pass_idx) {
			fprintf(stderr,"Error - could not allocate pass_idx\n");
			exit(EXIT_FAILURE);
		}

		*pass_idx = i;
		ibwin_ret = pthread_create( &rx_bufs[i].xfer_thread, NULL, 
				xfer_loop, (void *)(pass_idx));
		if (ibwin_ret) {
			printf("\nCould not create fxfr rx thread %d, rc=%d\n", 
					i, ibwin_ret);
			free(pass_idx);
		}
		rx_bufs[i].thr_valid = 1;
	}

	/* Start remote_login_thread, enabling remote debug over Ethernet */
	rlp = (struct remote_login_parms *)
				malloc(sizeof(struct remote_login_parms));
	if (NULL == rlp) {
		printf("\nCould not allocate memory for login parameters\n");
		exit(EXIT_FAILURE);
	}

	rlp->portno = cons_skt;
	cli_session_portno = cons_skt;
	SAFE_STRNCPY(rlp->thr_name, "FXFR_SVR_RLOG", sizeof(rlp->thr_name));
	rlp->status = &cli_session_alive;
	*rlp->status = 0;

	cli_ret = pthread_create( &remote_login_thread, NULL, remote_login, 
				(void *)(rlp));
	if(cli_ret) {
		fprintf(stderr,"Error - remote_login_thread rc: %d\n",cli_ret);
		free(rlp);
		exit(EXIT_FAILURE);
	}

	if (debug) {
		printf("pthread_create() for conn_loop returns: %d\n",
			conn_ret);
		printf("pthread_create() for remote_login_thread returns: %d\n",
			cli_ret);
		if (run_cons) {
			printf("pthread_create() for console returns: %d\n", 
				cons_ret);
		}
	}
}
//@sonar:on

void fxfr_server_shutdown(void) {
	int idx;

	if (!all_must_die && spawned_threads && conn_loop_alive) {
		pthread_kill(conn_thread, SIGHUP);
	}

	all_must_die = 1;
	
	/* Make sure all request processing threads die... */
	for (idx = 0; (idx < buff_cnt) && spawned_threads; idx++) {
		sem_post(&conn_reqs_mutex);
		if (rx_bufs[idx].valid) {
			if (rx_bufs[idx].thr_valid) {
				sem_post(&rx_bufs[idx].req_avail);
			} else {
				riomp_dma_ibwin_free(mp_h, &rx_bufs[idx].handle);
			}
		}
	}
}

void fxfr_server_shutdown_cli(struct cli_env *UNUSED_PARM(env))
{
	fxfr_server_shutdown();
}

void sig_handler(int signo)
{
	printf("\nRx Signal %x\n", signo);
	if ((signo == SIGINT) || (signo == SIGHUP) || (signo == SIGTERM)) {
		printf("Shutting down\n");
		fxfr_server_shutdown();
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	int cons_skt, print_help;
	uint8_t mport_num = 0;
	int rc = EXIT_FAILURE;
	int i;
	int run_cons, win_size, num_buffs, xfer_skt;

	debug = 0;

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGPIPE, sig_handler);

	parse_options(argc, argv, &cons_skt, &print_help, &mport_num, &run_cons,
		&win_size, &num_buffs, &xfer_skt);   
	
	if (print_help) {
		print_server_help();
		goto exit;
	}

	rdma_log_init("fxfr_server.log", 1);
	if (setup_mport(mport_num, num_buffs, win_size)) {
		goto exit;
	}

	spawn_threads(cons_skt, xfer_skt, run_cons);

	sem_wait(&conn_loop_started);

	if (!conn_loop_alive)
		fxfr_server_shutdown();

	pthread_join(conn_thread, NULL);
	for (i = 0; i < num_buffs; i++) {
		if (rx_bufs[i].thr_valid)
			pthread_join(rx_bufs[i].xfer_thread, NULL);
	}

	if (run_cons)
		pthread_join(console_thread, NULL);
 
	if (mp_h_valid) {
		riomp_mgmt_mport_destroy_handle(&mp_h);
		mp_h_valid = 0;
	}

	printf("\nFILE TRANSFER SERVER EXITING!!!!\n");
	rc = EXIT_SUCCESS;
exit:
	rdma_log_close();
	exit(rc);
}

#ifdef __cplusplus
}
#endif

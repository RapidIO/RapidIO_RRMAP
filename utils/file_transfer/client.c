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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include "string_util.h"
#include "rio_route.h"
#include "tok_parse.h"
#include "rrmap_config.h"
#include "libfxfr.h"
#include "rapidio_mport_sock.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_dma.h"

char src_fs[MAX_FILE_NAME + 1];
char rem_fs[MAX_FILE_NAME + 1];

void print_client_help(void)
{
	printf("\nThe file transfer client sends a file to a server.\n");
	printf("Syntax for invoking the client is as follows:\n");
	printf("./rftp <src_file> <dest_file> <destID> "
					"<cm_skt> <mport> <dbg> <k_buf>\n");
	printf("<src_file>: Name of the local file to be transferred.\n");
	printf(" 	Local_src_file must exist on a file system\n");
	printf("	accessible from the command line where the\n");
	printf("	./rftp command is entered.\n");
	printf("<dest_file>: Name of the transferred file on the\n");
	printf(" 	target machine.\n");
	printf("<destID>: RapidIO destination ID of the target server.\n");
	printf("	Execute the \"status\" command on the target server\n");
	printf("	to determine the available mports and destination IDs.\n");
	printf("<cm_skt>: Server RapidIO Channel Manager socket.\n");
	printf("	Default value is %d.\n", FXFR_DFLT_SVR_CM_PORT);
	printf("\n");
	printf("-->> Execute the \"status\" command on the target server\n");
	printf("-->> to display <destID> and <cm_skt> values.\n");
	printf("\n");
	printf("<mport> : The index of the mport number to be used to send\n");
	printf("	the request. \n");
	printf("	The default mport number is 0. \n");
	printf("	Enter a non-zero <dbg> value to see <mport> and \n");
	printf("	<destID> values available to this client.\n");
	printf("<dbg>   : A non-zero <dbg> value displays error/debug/trace\n");
	printf("	messages for the file transfer. -1 is ultraquiet\n\n");
	printf("<k_buf> : A zero <k_buf> value means ordinary user space\n");
	printf("	memory will be used for file transferr. This is\n");
	printf("	slower than the default kernel mode buffers.\n\n");
	printf("Entering ./rftp with no parameters displays this message.\n");
	printf("The rftp client performs the file transfer and reports\n");
	printf("elapsed time for the transfer.\n");
}

struct timespec time_difference( struct timespec start, struct timespec end )
{
        struct timespec temp;

        if ((end.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = end.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = end.tv_sec-start.tv_sec;
                temp.tv_nsec = end.tv_nsec-start.tv_nsec;
        }
        return temp;
} /* time_difference() */

int parse_options(int argc, char *argv[], char **src_name, char **rem_name,
		did_val_t *server_did_val, int *xfer_skt, uint8_t *mport_num,
		int *debug, uint8_t *k_buffs)
{
	uint16_t tmp16;
	uint32_t tmp32;
	memset(src_fs, 0, sizeof(src_fs));
	memset(rem_fs, 0, sizeof(rem_fs));
	*src_name = src_fs;
	*rem_name = rem_fs;
	*server_did_val = 0;
	*xfer_skt = FXFR_DFLT_SVR_CM_PORT;
	*mport_num = 0;
	*debug = 0;
	*k_buffs = 1;
	
	if (argc < 3)
		goto print_help;

	SAFE_STRNCPY(src_fs, argv[1], sizeof(src_fs));
	SAFE_STRNCPY(rem_fs, argv[2], sizeof(rem_fs));

	if ((argc > 3) && (tok_parse_did(argv[3], server_did_val, 0))) {
		printf(TOK_ERR_DID_MSG_FMT);
		goto print_help;
	}

	if (argc > 4) {
		if (tok_parse_socket(argv[4], &tmp16, 0)) {
			printf(TOK_ERR_SOCKET_MSG_FMT, "Socket number");
			goto print_help;
		}
		*xfer_skt = (int)tmp16;
	}

	if (argc > 5) {
		if (tok_parse_mport_id(argv[5], &tmp32, 0)) {
			printf(TOK_ERR_MPORT_MSG_FMT);
			goto print_help;
		}
		*mport_num = (uint8_t)(tmp32 & UINT8_MAX);
	}

	if (argc > 6) {
		if (tok_parse_ushort(argv[6], &tmp16, 0, 1, 0)) {
			printf(TOK_ERR_USHORT_HEX_MSG_FMT, "debug", 0, 1);
			goto print_help;
		}
		*debug = (uint8_t)(tmp16 & UINT8_MAX);
	}

	if (argc > 7) {
		if (tok_parse_ushort(argv[7], &tmp16, 0, 1, 0)) {
			printf(TOK_ERR_USHORT_HEX_MSG_FMT, "k_buf", 0, 1);
			goto print_help;
		}
		*k_buffs = (uint8_t)(tmp16 & UINT8_MAX);
	}

	return 0;

print_help:
	print_client_help();
	return 1;
}

void sig_handler(int signo)
{
        printf("\nRx Signal %x\n", signo);
        if ((signo == SIGINT) || (signo == SIGHUP) || (signo == SIGTERM)) {
                printf("Shutting down\n");
                exit(0);
        }
}

int main(int argc, char *argv[])
{
	int rc;
	char *src_name; /* Name of file to send */
	char *rem_name; /* Name of file received */
	uint8_t mport_num; /* Master port number to use on this node */
	did_val_t did_val; /* DestID where fxfr server is running */
	int svr_skt; /* Socket fxfr server is accepting requests */
	int debug = 0;
	uint8_t k_buff = 0;
	struct timespec req_time, st_time, end_time, duration;
	uint64_t bytes_sent;

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);

	if (parse_options(argc, argv, &src_name, &rem_name, &did_val,
		&svr_skt, &mport_num, &debug, &k_buff))
		goto exit;

	if (debug > 0) {
		printf("\nLocal  file: \"%s\"\n", src_name);
		printf("\nRemote file: \"%s\"\n", rem_name);
	}
	printf("\nDestID     : %d\n", did_val);
	if (debug > 0) {
		printf("\nSocket     : %d\n", svr_skt);
		printf("\nMport      : %d\n", mport_num);
		printf("\nDebug      : %d\n", debug);
		printf("\nKernel Buff: %d\n", k_buff);
	}

	clock_gettime(CLOCK_MONOTONIC, &req_time);
	rc = send_file(src_name, rem_name, did_val, svr_skt, mport_num,
			(debug > 0 ? debug : 0), &st_time, &bytes_sent, k_buff);
	clock_gettime(CLOCK_MONOTONIC, &end_time);

	if (rc) {
		printf("\nFile transfer FAILED.\n");
	} else if (debug > 0) {
		printf("\nFile transfer Passed.\n");
	}

	if (!rc) {
		duration = time_difference(st_time, end_time);
		float throughput, time, xfer_size;
		
		if (debug >= 0) {
			printf("Req   time: %10d sec %10d nsec\n", 
				(uint32_t)req_time.tv_sec, (uint32_t)req_time.tv_nsec);
			printf("Start time: %10d sec %10d nsec\n", 
				(uint32_t)st_time.tv_sec, (uint32_t)st_time.tv_nsec);
			printf("End   time: %10d sec %10d nsec\n", 
				(uint32_t)end_time.tv_sec, (uint32_t)end_time.tv_nsec);
			printf("Duration  : %10d sec %10d nsec\n\n", 
				(uint32_t)duration.tv_sec, (uint32_t)duration.tv_nsec);
		}

		time = (float)duration.tv_nsec + 
			((float)(duration.tv_sec) * 1000000000.0);
		time = time/1000000000.0;
		xfer_size = (float)(bytes_sent);
		throughput = xfer_size/time/(1024.0*1024.0);

		if (debug >= 0) {
			printf("Bytes sent  : %ld\n", (long int)bytes_sent);
			printf("Throughput  : %12.3f Mbps\n", throughput*8.0);
		}

		printf("Throughput  : %12.3f MBps\n", throughput);
	} else {
		return 69;
	}

exit:
	exit(EXIT_SUCCESS);
}

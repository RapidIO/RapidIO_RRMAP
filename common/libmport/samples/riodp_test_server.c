/*
 * Copyright 2014, 2015 Integrated Device Technology, Inc.
 *
 * User-space RIO_CM server test program.
 *
 * This software is available to you under a choice of one of two licenses.
 * You may choose to be licensed under the terms of the GNU General Public
 * License(GPL) Version 2, or the BSD-3 Clause license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Server part of client/server test program for RapidIO channelized messaging.
 *
 * Listens for connection requests from remote peers and start a child process for new connection.
 * Each child process receives a message from a client and echoes it back to the sender. This echo loop
 * continues until the client closes connection (or server program is terminated by a signal).  
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> /* for sleep() */
#include <sys/wait.h>
#include <stdint.h> /* For size_t */
#include <fcntl.h>
#include <sys/ioctl.h>

#include "rio_route.h"
#include "tok_parse.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

struct args {
	uint32_t mport_id;
	uint16_t loc_channel;
};
/// @endcond

static volatile sig_atomic_t srv_exit;
static volatile sig_atomic_t report_status;

static int srv_debug;

static void usage(char *program)
{
	printf("%s - library test server\n", program);
	printf("Usage:\n");
	printf("  %s [options]\n", program);
	printf("Options are:\n");
	printf("    <mport> mport device index\n");
	printf("    <channel> channel number to listen for connection requests\n");
	printf("    [debug] enable debug output\n");
	printf("\n");
}

static void srv_sig_handler(int signum)
{
	switch (signum) {
	case SIGTERM:
	case SIGINT:
		srv_exit = 1;
		break;
	case SIGUSR1:
		report_status = 1;
		break;
	}
}

/**
 * \brief Display available devices
 */
void show_rio_devs(void)
{
	mport_list_t *mport_list = NULL;
	mport_list_t *list_ptr;
	uint8_t number_of_mports = RIO_MAX_MPORTS;
	uint8_t mport_id;

	did_val_t *ep_list = NULL;
	uint32_t number_of_eps = 0;
	uint32_t ep;
	int i;
	int ret;

	/** - request from driver list of available local mport devices */
	ret = riomp_mgmt_get_mport_list(&mport_list, &number_of_mports);
	if (ret) {
		printf("ERR: riomp_mgmt_get_mport_list() ERR %d\n", ret);
		return;
	}

	printf("\nAvailable %d local mport(s):\n", number_of_mports);
	if (number_of_mports > RIO_MAX_MPORTS) {
		printf("WARNING: Only %d out of %d have been retrieved\n",
		RIO_MAX_MPORTS, number_of_mports);
	}

	/** - for each local mport display list of remote RapidIO devices */
	list_ptr = mport_list;
	for (i = 0; i < number_of_mports; i++, list_ptr++) {
		mport_id = *list_ptr >> 16;
		printf("+++ mport_id: %u dest_id: %u\n", mport_id,
				*list_ptr & 0xffff);

		/* Display EPs for this MPORT */

		ret = riomp_mgmt_get_ep_list(mport_id, &ep_list,
				&number_of_eps);
		if (ret) {
			printf("ERR: riodp_ep_get_list() ERR %d\n", ret);
			break;
		}

		printf("\t%u Endpoints (dest_ID): ", number_of_eps);
		for (ep = 0; ep < number_of_eps; ep++) {
			printf("%u ", *(ep_list + ep));
		}
		printf("\n");

		ret = riomp_mgmt_free_ep_list(&ep_list);
		if (ret) {
			printf("ERR: riodp_ep_free_list() ERR %d\n", ret);
		}

	}

	printf("\n");

	ret = riomp_mgmt_free_mport_list(&mport_list);
	if (ret) {
		printf("ERR: riodp_ep_free_list() ERR %d\n", ret);
	}
}

/**
 * \brief Child process created by main() to service an individual connection.
 *
 * \param[in] new_socket Socket associated with this connection

 * In a loop performs the following steps until terminated:
 */
void doprocessing(riomp_sock_t new_socket)
{
	int ret = 0;
	rapidio_mport_socket_msg *msg_rx = NULL;

	printf("CM_SERVER processing child(%d) started\n", (int)getpid());

	// use malloc to get rx buffer, because buffers aren't managed yet
	msg_rx = (rapidio_mport_socket_msg *) malloc(sizeof(rapidio_mport_socket_msg));
	if (msg_rx == NULL) {
		printf("CM_SERVER(%d): error allocating rx buffer\n",
				(int)getpid());
		exit(1);
	}

	while (!srv_exit) {
		/** - Receive an inbound message */
		ret = riomp_sock_receive(new_socket, &msg_rx,
				60 * 1000, &srv_exit);
		//@sonar:off - c:S3584 Allocated memory not released
		if (ret) {
			printf("CM_SERVER(%d): riomp_sock_receive() ERR=%d\n",
					(int)getpid(), ret);
			break;
		}
		//@sonar:on

		if (srv_debug) {
			printf("CM_SERVER(%d): RX_MSG=%s\n", (int)getpid(),
					(char *)msg_rx->msg.payload);
		}

		/** - Send  a message back to the client */
		ret = riomp_sock_send(new_socket, msg_rx,
				sizeof(*msg_rx), &srv_exit);
		if (ret) {
			printf("CM_SERVER(%d): riomp_sock_send() ERR %d (%d)\n",
					(int)getpid(), ret, errno);
			break;
		}
	}

	riomp_sock_release_receive_buffer(new_socket, msg_rx);
	/** - Close a socket assigned to this process if connection closed by
	 * a client or process terminated */
	ret = riomp_sock_close(&new_socket);
	//@sonar:off - c:S3584 Allocated memory not released
	if (ret) {
		printf("CM_SERVER(%d): riomp_sock_close() ERR %d\n",
				(int)getpid(), ret);
	}
	//@sonar:on

	if (ret && ret != ECONNRESET) {
		exit(1);
	}
}

/**
 * \brief Starting point for the program
 *
 * \param[in] argc Command line parameter count
 * \param[in] argv Array of pointers to command line parameter null terminated
 *                 strings
 *
 * \retval 0 means success
 *
 * When running without expected number of arguments displays list of available local
 * and remote devices.
 *
 * Performs the following steps:
 */
int main(int argc, char** argv)
{
	int ret = 0;
	struct args arg;
	riomp_mailbox_t mailbox = NULL;
	riomp_sock_t socket = NULL;
	riomp_sock_t new_socket = NULL;
	pid_t pid, wpid;
	int status = 0;
	int tmp;

	/** - Parse console arguments */
	/** - If number of arguments is less than expected display help message and list of devices. */
	if (argc < 3) {
		usage(argv[0]);
		show_rio_devs();
		exit(EXIT_FAILURE);
	}

	if (tok_parse_mport_id(argv[1], &arg.mport_id, 0)) {
		printf(TOK_ERR_MPORT_MSG_FMT);
		exit(EXIT_FAILURE);
	}
	if (tok_parse_socket(argv[2], &arg.loc_channel, 0)) {
		printf(TOK_ERR_SOCKET_MSG_FMT, "Channel number");
		exit(EXIT_FAILURE);
	}

	if (argc == 4) {
		srv_debug = 1;
	}

	/** - Trap signals that we expect to receive */
	signal(SIGINT, srv_sig_handler);
	signal(SIGTERM, srv_sig_handler);
	signal(SIGUSR1, srv_sig_handler);

	printf("CM_SERVER(%d): Running on RapidIO mport_%d\n", (int)getpid(),
			arg.mport_id);

	/** - Create rapidio_mport_mailbox control structure */
	ret = riomp_sock_mbox_create_handle(arg.mport_id, 0, &mailbox);
	if (ret) {
		printf("CM_SERVER: riodp_mbox_init() ERR %d\n", ret);
		goto out;
	}

	/** - Create a socket  structure associated with given mailbox */
	ret = riomp_sock_socket(mailbox, &socket);
	if (ret) {
		printf("CM_SERVER: riomp_sock_socket() ERR %d\n", ret);
		goto out;
	}

	/** - Bind the listen channel to opened MPORT device */
	ret = riomp_sock_bind(socket, arg.loc_channel);
	if (ret) {
		printf("CM_SERVER: riomp_sock_bind() ERR %d\n", ret);
		goto out;
	}

	/** - Initiate LISTEN on the specified channel */
	ret = riomp_sock_listen(socket);
	if (ret) {
		printf("CM_SERVER: riomp_sock_listen() ERR %d\n", ret);
		goto out;
	}

	/** - Create child process for each accepted request */
	while (!srv_exit) {

		/* Create new socket object for accept */
		ret = riomp_sock_socket(mailbox, &new_socket);
		if (ret) {
			printf("CM_SERVER(%d): riomp_sock_socket() ERR %d\n",
					(int)getpid(), ret);
			break;
		}

		while (1) {
			wpid = waitpid(-1, &status, WNOHANG);
			if (wpid != -1 && wpid != 0) {
				printf(
						"CM_SERVER(%d): terminated with status %d\n",
						wpid, status);
			} else {
				break;
			}
		}

		ret = riomp_sock_accept(socket, &new_socket, 3 * 60000,
				&srv_exit); /* Time out = 3 min */
		if (ret) {
			printf("CM_SERVER(%d): riomp_sock_accept() ERR %d\n",
					(int)getpid(), ret);
			riomp_sock_close(&new_socket);
			srv_exit = 2;
			break;
		}

		// Create child process
		pid = fork();
		if (pid < 0) {
			perror("CM_SERVER: ERROR on fork()\n");
			exit(1);
		}

		if (pid == 0) {
			// This is the child process
			doprocessing(new_socket);
			exit(0);
		} else {
			// TBD
		}
		free(new_socket);
	}

out:
	/** - Exit closing listening channel */
	tmp = riomp_sock_close(&socket);
	if (tmp) {
		printf("CM_SERVER(%d): riomp_sock_close() ERR %d\n",
				(int)getpid(), tmp);
	}

	/** - Release rapidio_mport_mailbox control structure */
	tmp = riomp_sock_mbox_destroy_handle(&mailbox);
	if (tmp) {
		printf("riodp_mbox_shutdown error: %d\n", tmp);
	}

	return ret;
}

#ifdef __cplusplus
}
#endif

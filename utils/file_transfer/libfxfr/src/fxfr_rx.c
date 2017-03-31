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
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#define CONFIG_RAPIDIO_DMA_ENGINE 1
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>


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

#include "string_util.h"
#include "rapidio_mport_dma.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"
#include "libfxfr_private.h"
#include "fxfr_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

int init_message_buffers(struct buffer_info *info)
{
	info->msg_rx = (rapidio_mport_socket_msg *) malloc(
			sizeof(rapidio_mport_socket_msg));
	if (info->msg_rx == NULL) {
		printf("File RX: malloc rx msg failed\n");
		goto fail;
	}

	info->msg_tx = (rapidio_mport_socket_msg *) malloc(
			sizeof(rapidio_mport_socket_msg));
	if (info->msg_tx == NULL) {
		printf("File RX: malloc tx msg failed\n");
		goto fail;
	}

	info->rxed_msg =
			(struct fxfr_client_to_svr_msg *)info->msg_rx->msg.payload;
	info->tx_msg =
			(struct fxfr_svr_to_client_msg *)info->msg_tx->msg.payload;

	return 0;
fail:
	return 1;
}

int process_rxed_msg(struct buffer_info *info, int *abort_flag)
{
	if (info->rxed_msg->fail_abort)
		goto fail;

	/* Make sure we have a file to write to */
	if ('\0' == info->file_name[0]) {
		SAFE_STRNCPY(info->file_name, info->rxed_msg->rx_file_name,
			sizeof(info->file_name));
		info->fd = open(info->file_name, O_WRONLY | O_CREAT, 0644);
		if (-1 == info->fd) {
			*abort_flag = 1;
			perror("open");
		}
	} else {
		if (strncmp(info->file_name, info->rxed_msg->rx_file_name,
			sizeof(info->file_name))) {
			*abort_flag = 1;
			printf("Attempt to write to wrong file!!!\n");
		}
	}

	if (!*abort_flag && info->rxed_msg->bytes_tx_now) {
		uint64_t offset = info->rxed_msg->rapidio_addr -
				  info->rio_base;
		uint64_t writn = write (info->fd, &info->ib_mem[offset], 
				(ssize_t) info->rxed_msg->bytes_tx_now);
		if (writn != info->rxed_msg->bytes_tx_now) {
			*abort_flag = 1;
			printf("Could not write all bytes!!!\n");
		}
		info->bytes_rxed += info->rxed_msg->bytes_tx_now;
	}

	if (info->rxed_msg->end_of_file) {
		close(info->fd);
		return 1;
	}

	return 0;
fail:
	return 1;
}
				
int send_server_msg(struct buffer_info *info, int fail_abort, volatile int *abort_flag)
{ 
	/* Server sends first message to client */

	info->tx_msg->rapidio_addr = htobe64(info->rio_base);
	info->tx_msg->rapidio_size = htobe64(info->length);
	info->tx_msg->tot_bytes_rx = htobe64(info->bytes_rxed);
	info->tx_msg->fail_abort = htobe64(fail_abort || *abort_flag);
		
	memcpy(info->tx_msg->rx_file_name, 
		info->file_name, MAX_FILE_NAME);
	info->tx_msg->rx_file_name[MAX_FILE_NAME] = '\0';

	if (info->debug) {
		printf("	Server: Tx to Client\n");
		printf("	rapidio_addr = %16lx %16lx\n",
			(long unsigned int)info->rio_base,
			(long unsigned int)info->tx_msg->rapidio_addr);
		printf("	size         = %16lx %16lx\n",
			(long unsigned int)info->length,
			(long unsigned int)info->tx_msg->rapidio_size);
		printf("	tot_bytes_rx = %16lx %16lx\n",
			(long unsigned int)info->bytes_rxed,
			(long unsigned int)info->tx_msg->tot_bytes_rx);
		printf("	fail_abort   = %16lx\n",
			(long unsigned int)info->tx_msg->fail_abort);
		printf("	file name    = %s\n",
				info->tx_msg->rx_file_name);
	}

	/* Send a message to the client */
	return riomp_sock_send(*info->req_skt, 
			info->msg_tx, sizeof(*info->msg_tx), abort_flag);
}

int receive_client_msg(struct buffer_info *info)
{
	int ret = riomp_sock_receive(*info->req_skt, &info->msg_rx, 0, NULL);
	if (ret) {
		printf("File RX: riomp_socket_receive() ERR %d (%d)\n",
			ret, errno);
		return ret;
	}

	info->rxed_msg->rapidio_addr = be64toh(info->rxed_msg->rapidio_addr);
	info->rxed_msg->bytes_tx_now = be64toh(info->rxed_msg->bytes_tx_now);
	info->rxed_msg->tot_bytes_tx = be64toh(info->rxed_msg->tot_bytes_tx);
	info->rxed_msg->end_of_file = be64toh(info->rxed_msg->end_of_file); 
	info->rxed_msg->fail_abort = be64toh(info->rxed_msg->fail_abort); 

	if (info->debug) {
		printf("Server: RX from Client\n");
		printf("rapidio_addr = %16lx\n", 
			(long unsigned int)info->rxed_msg->rapidio_addr);
		printf("bytes_tx_now = %16lx\n", 
			(long unsigned int)info->rxed_msg->bytes_tx_now);
		printf("tot_bytes_tx = %16lx\n", 
			(long unsigned int)info->rxed_msg->tot_bytes_tx);
		printf("end_of_file  = %16lx\n",
			(long unsigned int)info->rxed_msg->end_of_file);
		printf("fail_abort   = %16lx\n",
			(long unsigned int)info->rxed_msg->fail_abort);
		printf("file name    = %s\n",
			info->rxed_msg->rx_file_name);
	}

	return ret;
}

int rx_file(struct buffer_info *info, volatile int *abort_flag)
{
        int ret = 0;
	int end_of_file = 0;
	int fail_abort = 0;
	int done = 0;

	if (info->debug)
        	printf("\nFile RX started\n");

	init_message_buffers(info);

	if (info->debug)
		printf("\nRX_FILE starting while loop %d %d\n", 
			*abort_flag, fail_abort);

	while (!*abort_flag && !fail_abort && !done && !end_of_file) {
                /* Server sends first message to client */
                ret = send_server_msg(info, fail_abort, abort_flag);
                if (ret) {
			if (info->debug)
                        	printf("File RX: socket_send() ERR %d (%d)\n",
                                	ret, errno);
                        break;
                }

		if (end_of_file || *abort_flag)
			break;

                ret = receive_client_msg(info);
		if (ret)
			break;

		if (info->rxed_msg->fail_abort)
			fail_abort = 1;
		else
			end_of_file = process_rxed_msg(info, &fail_abort);
        }

	if (info->req_skt) {
        	riomp_sock_release_receive_buffer(*info->req_skt, 
							info->msg_rx);
        	riomp_sock_release_send_buffer(*info->req_skt, info->msg_tx);
        	ret = riomp_sock_close(info->req_skt);
		info->req_skt = NULL;
	}

        if (ret)
                printf("File RX: riomp_socket_close() ERR %d\n", ret);
	return ret;
}

#ifdef __cplusplus
}
#endif

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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#include "string_util.h"
#include "rio_route.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"
#include "rapidio_mport_dma.h"
#include "libfxfr_private.h"
#include "fxfr_msg.h"

#define MAX_TX_SEGMENTS 2

#ifdef __cplusplus
extern "C" {
#endif

struct fxfr_tx_state {
	uint8_t fail_abort; 	/* 0 - Proceed with transfer.
				* 1 - Abort transfer
				*/
	uint8_t done; 	/* 0 - Transfer continuing
			* 1 - Transfer successfull completion
			*/
	uint8_t debug;
	/* MPORT selection and data */
	uint8_t mport_num;
	riomp_mport_t mp_h;
	int mp_h_valid;
	/* Mailbox data */
	riomp_mailbox_t req_mb;
	riomp_sock_t req_skt;
	int svr_skt;
	rapidio_mport_socket_msg *msg_rx;
	rapidio_mport_socket_msg *msg_tx;
	struct fxfr_svr_to_client_msg *rxed_msg;
	struct fxfr_client_to_svr_msg *tx_msg;
	/* File name data */
	char src_name[MAX_FILE_NAME+1];
	int src_fd;
	char dest_name[MAX_FILE_NAME+1];
	uint8_t end_of_file;
	/* RapidIO target/rx data */
	did_val_t did_val; /* DestID of fxfr server */
	uint8_t use_kbuf; /* 1 => Use kernel buffers, 0 => use malloc/free */
	uint64_t buf_h; /* Handle for DMA buffer, if using Kernel Buffs */
	uint8_t *buffers[MAX_TX_SEGMENTS]; /* Data to DMA to fxfr server */
	int next_buff_idx;
	uint64_t bytes_txed; /* Bytes transmitted by this message */
	uint64_t tot_bytes_txed; /* Total bytes transmitted so far */
	uint64_t bytes_rxed; /* Bytes received/acknowledged by this message */
	uint64_t tot_bytes_rxed; /* Total bytes rxed by fxfr server */
	uint64_t rx_rapidio_addr; /* Base address of fxfr server window */
	uint64_t rx_rapidio_size; /* Size of fxfr server window */
};

static volatile sig_atomic_t srv_exit;

void process_msg_from_server(struct fxfr_tx_state *info)
{
	if (info->fail_abort)
		return;

	if (info->rxed_msg->tot_bytes_rx || info->rxed_msg->rx_file_name[0]) {
		/* Process transfer acknowledgement message from server.  
		 */
		if ((info->rxed_msg->tot_bytes_rx > info->tot_bytes_txed) ||
	     		!(info->rxed_msg->tot_bytes_rx) ||
			strncmp(info->rxed_msg->rx_file_name, 
				info->dest_name, MAX_FILE_NAME))
			goto fail;

		info->tot_bytes_rxed = info->rxed_msg->tot_bytes_rx;

		// Note: This clause is never true, as the server closes
		// the connection on receipt of a message with "end_of_file"
		// set.
		if ((info->tot_bytes_txed == info->rxed_msg->tot_bytes_rx) && 
		     info->end_of_file)
			info->done = 1;
	} else {
		/* Connection acknowledgement message from server.  
		 * Set up RapidIO DMA addresses and DMA transfer sizes
		 */
		if (!info->rxed_msg->rapidio_addr || 
		    !info->rxed_msg->rapidio_size ||
		     info->rxed_msg->fail_abort)
			goto fail;

		info->rx_rapidio_addr = info->rxed_msg->rapidio_addr;
		if (info->rxed_msg->rapidio_size > 
					(MAX_TX_SEGMENTS*MAX_TX_BUFF_SIZE))
			info->rx_rapidio_size = MAX_TX_BUFF_SIZE;
		else
			info->rx_rapidio_size = 
				(info->rxed_msg->rapidio_size/MAX_TX_SEGMENTS)
				& ((2*MAX_TX_BUFF_SIZE) - 0x1000);
	}

	return;
fail:
	info->fail_abort = 1;
}

void rx_msg_from_server(struct fxfr_tx_state *info)
{
	int ret = riomp_sock_receive(info->req_skt, &info->msg_rx, 0, NULL);
	if (ret) {
		if (info->tx_msg->end_of_file) {
			info->done = 1;
		} else {
			printf("File TX: riomp_sock_receive() ERR %d (%d)\n",
				ret, errno);
			info->fail_abort = 1;
			info->rxed_msg->fail_abort = 1;
		}
		return;
	}

	info->rxed_msg->rapidio_addr = be64toh(info->rxed_msg->rapidio_addr);
	info->rxed_msg->rapidio_size = be64toh(info->rxed_msg->rapidio_size);
	info->rxed_msg->tot_bytes_rx = be64toh(info->rxed_msg->tot_bytes_rx);
	info->rxed_msg->fail_abort = be64toh(info->rxed_msg->fail_abort);

	if (info->debug) {
		printf("Client: RX from Server\n");
		printf("rapidio_addr = %16lx\n", 
			(long unsigned int)info->rxed_msg->rapidio_addr);
		printf("rapidio_size = %16lx\n", 
			(long unsigned int)info->rxed_msg->rapidio_size);
		printf("tot_bytes_rx = %16lx\n", 
			(long unsigned int)info->rxed_msg->tot_bytes_rx);
		printf("fail_abort   = %16lx\n", 
			(long unsigned int)info->rxed_msg->fail_abort);
		printf("file name    = %s\n", info->rxed_msg->rx_file_name);
	}

	process_msg_from_server(info);
}

void fill_dma_buffer(struct fxfr_tx_state *info, int idx)
{
	info->bytes_txed = read(info->src_fd, 
				info->buffers[idx], info->rx_rapidio_size);
	if (info->bytes_txed < info->rx_rapidio_size)
		info->end_of_file = 1;
}

void send_dma_buffer(struct fxfr_tx_state *info, int idx)
{
	int rc = 0;
       
	if (info->bytes_txed) {
		if (info->use_kbuf) {
			rc = riomp_dma_write_d(info->mp_h, 
				info->did_val,
				info->rx_rapidio_addr +
					(idx * info->rx_rapidio_size),
				info->buf_h,
				idx * MAX_TX_BUFF_SIZE,
				info->bytes_txed,
				RIO_DIRECTIO_TYPE_NWRITE,
				RIO_DIRECTIO_TRANSFER_SYNC,
				NULL);
		} else {
			rc = riomp_dma_write(info->mp_h, 
				info->did_val,
				info->rx_rapidio_addr +
					(idx * info->rx_rapidio_size),
				info->buffers[idx], 
				info->bytes_txed,
				RIO_DIRECTIO_TYPE_NWRITE,
				RIO_DIRECTIO_TRANSFER_SYNC,
				NULL);
		}
	}

	if (rc < 0) {
		info->fail_abort = 1;
		info->bytes_txed = 0;
	} else {
		info->tot_bytes_txed += info->bytes_txed;
	}
}

void send_transfer_msg(struct fxfr_tx_state *info, int idx)
{
	int ret;

	info->tx_msg->rapidio_addr = htobe64(info->rx_rapidio_addr + 
			(idx * info->rx_rapidio_size));
	info->tx_msg->bytes_tx_now = htobe64(info->bytes_txed);
	info->tx_msg->tot_bytes_tx = htobe64(info->tot_bytes_txed);
	info->tx_msg->end_of_file = htobe64(info->end_of_file);
	info->tx_msg->fail_abort = htobe64(info->fail_abort);
	SAFE_STRNCPY(info->tx_msg->rx_file_name, info->dest_name,
			sizeof(info->tx_msg->rx_file_name));

	if (info->debug) {
		printf("	Client: TX to Server\n");
		printf("	rapidio_addr = %16lx %16lx\n", 
			(long unsigned int)(info->rx_rapidio_addr 
					+ (idx * info->rx_rapidio_size)),
			(long unsigned int)info->tx_msg->rapidio_addr);
		printf("	bytes_tx_now = %16lx %16lx\n", 
			(long unsigned int)info->bytes_txed,
			(long unsigned int)info->tx_msg->bytes_tx_now);
		printf("	tot_bytes_tx = %16lx %16lx\n", 
			(long unsigned int)info->tot_bytes_txed,
			(long unsigned int)info->tx_msg->tot_bytes_tx);
		printf("	end_of_file  = %16lx %16lx\n", 
			(long unsigned int)info->end_of_file,
			(long unsigned int)info->tx_msg->end_of_file);
		printf("	fail_abort   = %16lx %16lx\n", 
			(long unsigned int)info->fail_abort,
			(long unsigned int)info->tx_msg->fail_abort);
		printf("	file name    = %s\n",
			info->tx_msg->rx_file_name);
	}

	/* Send  a message back to the client */
	ret = riomp_sock_send(info->req_skt, info->msg_tx,
		sizeof(*info->msg_tx), NULL);
	if (ret) {
		printf("File TX(%d): riomp_sock_send() ERR %d (%d)\n",
			(int)getpid(), ret, errno);
		info->fail_abort = 1;
	}
}

void send_msgs_to_server(struct fxfr_tx_state *info, struct timespec *st_time)
{
	if (info->fail_abort) {
	       	if (!info->rxed_msg->fail_abort) {
			info->bytes_txed = 0;
			send_transfer_msg(info, 0);
		}
	} else {
		uint64_t diff = info->tot_bytes_txed - info->tot_bytes_rxed;
		uint64_t max_diff = MAX_TX_SEGMENTS*info->rx_rapidio_size;
		
		while (!info->fail_abort && !info->done && !info->end_of_file
				&& (diff < max_diff)) {
			fill_dma_buffer(info, info->next_buff_idx);
			if (NULL != st_time)
				clock_gettime(CLOCK_MONOTONIC, st_time);
			st_time = NULL;

			send_dma_buffer(info, info->next_buff_idx);
			send_transfer_msg(info, info->next_buff_idx);
			diff = info->tot_bytes_txed - info->tot_bytes_rxed;
			info->next_buff_idx = 
				(info->next_buff_idx + 1) % MAX_TX_SEGMENTS;
		}
	}
}

int init_info_vals(struct fxfr_tx_state *info)
{	
	int i;

	info->fail_abort = 0;
	info->done = 0;
	info->debug = 0;

	/* MPORT data */
	info->mport_num = -1;
	info->mp_h = NULL;

	/* Mailbox data */
	info->req_mb = NULL;
	info->req_skt = NULL;
	info->svr_skt = 0;
	info->msg_rx = NULL;
	info->msg_tx = NULL;
	info->rxed_msg = NULL;
	info->tx_msg = NULL;
	/* File name data */
	memset(info->src_name, 0, MAX_FILE_NAME);
	memset(info->dest_name, 0, MAX_FILE_NAME);
	info->src_fd = -1;
	info->end_of_file = 0;
	/* RapidIO target/rx data */
	info->buf_h = 0;
	info->use_kbuf = 1;
	info->did_val = (did_val_t)(-1);
	for (i = 0; i < MAX_TX_SEGMENTS; i++) 
		info->buffers[i] = NULL; 
	info->next_buff_idx = 0;
	info->bytes_txed = 0;
	info->tot_bytes_txed = 0; /* Total bytes transmitted so far */
	info->bytes_rxed = 0; /* Bytes received/acknowledged by this message */
	info->tot_bytes_rxed = 0; /* Total bytes rxed by fxfr server */
	info->rx_rapidio_addr = 0; /* Base address of fxfr server window */
	info->rx_rapidio_size = 0; /* Size of fxfr server window */

	return 0;
}

int init_file_info(struct fxfr_tx_state *info, char *src_name, char *dst_name)
{
	SAFE_STRNCPY(info->dest_name, dst_name, sizeof(info->dest_name));
	SAFE_STRNCPY(info->src_name, src_name, sizeof(info->src_name));

	info->src_fd = open(info->src_name, O_RDONLY);
	if (info->src_fd == -1) {
		perror ("open");
		printf("\nFile \"%s\" open read-only failed.\n", 
			info->src_name);
		return 1;
	}

	return 0;
}

void cleanup_file_info(struct fxfr_tx_state *info)
{
	if (info->src_fd > 0)
		close(info->src_fd);
}

int init_message_buffers(struct fxfr_tx_state *info)
{
	info->msg_rx = (rapidio_mport_socket_msg *) malloc(sizeof(rapidio_mport_socket_msg));
	if (info->msg_rx == NULL) {
		printf("File TX: malloc rx msg failed\n");
		return 1;
	}

	info->msg_tx = (rapidio_mport_socket_msg *) malloc(sizeof(rapidio_mport_socket_msg));
	if (info->msg_tx == NULL) {
		printf("File TX: malloc tx msg failed\n");
		return 1;
	}

	info->rxed_msg =
			(struct fxfr_svr_to_client_msg *) info->msg_rx->msg.payload;
	info->tx_msg =
			(struct fxfr_client_to_svr_msg *) info->msg_tx->msg.payload;

	return 0;
}

void cleanup_msg_buffers(struct fxfr_tx_state *info)
{
	riomp_sock_release_receive_buffer(info->req_skt, info->msg_rx);
	riomp_sock_release_send_buffer(info->req_skt, info->msg_tx);
}

int init_server_connect(struct fxfr_tx_state *info, uint8_t mport_num,
		did_val_t did_val, int svr_skt, uint8_t k_buff)
{
        int rc;
	int i;
        struct riomp_mgmt_mport_properties qresp;

        info->mport_num = mport_num;
	info->svr_skt = svr_skt;
	info->use_kbuf = k_buff;
	info->mp_h_valid = 0;

        rc = riomp_mgmt_mport_create_handle(info->mport_num, 0, &info->mp_h);
        if (rc) {
                printf("\nUnable to create mport handle  for mport %d...\n",
			info->mport_num);
                goto fail;
        }

	info->mp_h_valid = 1;

        if (riomp_mgmt_query(info->mp_h, &qresp)) {
                printf("\nUnable to query mport %d...\n", info->mport_num);
                goto fail;
        }

	if (info->debug)
        	riomp_mgmt_display_info(&qresp);
	
        if (!(qresp.flags & RIO_MPORT_DMA)) {
                printf("\nMport %d has no DMA support...\n", info->mport_num);
                goto fail;
        }

        rc = riomp_sock_mbox_create_handle(info->mport_num, 0, &info->req_mb);
        if (rc) {
                printf("riomp_mbox_create_handle ERR %d\n", rc);
                goto fail;
        }

        rc = riomp_sock_socket(info->req_mb, &info->req_skt);
        if (rc) {
                printf("riomp_sock_socket ERR %d\n", rc);
                goto fail;
        }

	info->did_val = did_val;
	rc = riomp_sock_connect(info->req_skt, info->did_val, info->svr_skt,
			NULL);
        if (rc) {
                printf("riomp_sock_connect ERR %d\n", rc);
                goto fail;
        }

	if (info->use_kbuf) {
		info->buf_h = RIO_ANY_ADDR;
                rc = riomp_dma_dbuf_alloc(info->mp_h, TOTAL_TX_BUFF_SIZE,
				&info->buf_h);
                if (rc) {
			printf("riomp_dbuf_alloc failed err=%d\n", rc);
			goto fail;
		}
		rc = riomp_dma_map_memory(info->mp_h, 
			(size_t)TOTAL_TX_BUFF_SIZE, info->buf_h,
			(void **)&info->buffers[0]);
                if (rc) {
			if (info->debug)
                        	perror("mmap");
                        info->buffers[0] = NULL;
                        rc = riomp_dma_dbuf_free(info->mp_h, &info->buf_h);
                        if (rc && info->debug)
                                printf("riomp_dbuf_free failed err=%d\n", rc);
		} else {
			for (i = 1; i < MAX_TX_SEGMENTS; i++) {
                        	info->buffers[i] = info->buffers[0] +
					(i * MAX_TX_BUFF_SIZE);
			}
		}
	} else {
		for (i = 0; i < MAX_TX_SEGMENTS; i++) {
			info->buffers[i] = (uint8_t *)malloc(TOTAL_TX_BUFF_SIZE);
        		if (info->buffers[i] == NULL) {
                		printf("File TX: malloc tx buffer %d failed\n",
					i);
                		goto fail;
			}
        	}
	}

	return 0;
fail:
	return 1;
}

void cleanup_server_connect(struct fxfr_tx_state *info)
{
	int i, rc;

	if (info->use_kbuf && info->buffers[0]) {
		rc = riomp_dma_dbuf_free(info->mp_h, &info->buf_h);
		if (rc)
			printf("riomp_dbuf_free() ERR: =%d\n", rc);
		info->buf_h = 0;
		info->buffers[0] = NULL;
	}

	if (!info->use_kbuf) {
		for (i = 0; i < MAX_TX_SEGMENTS; i++) {
			if (NULL != info->buffers[i]) {
				free(info->buffers[i]);
				info->buffers[i] = NULL;
			}
		}
	}

	if (info->req_skt) {
        	rc = riomp_sock_close(&info->req_skt);
        	if (rc)
                	printf("riomp_sock_close() ERR %d\n", rc);
		info->req_skt = NULL;
	}

        if (info->req_mb) {
		rc = riomp_sock_mbox_destroy_handle(&info->req_mb);
        	if (rc)
                	printf("riomp_mbox_shutdown() ERR: %d\n", rc);
		info->req_mb = NULL;
	}


	if (info->mp_h_valid) {
                riomp_mgmt_mport_destroy_handle(&info->mp_h);
		info->mp_h_valid = 0;
	}
}

void cleanup_all(struct fxfr_tx_state *info)
{
	cleanup_file_info(info);
	cleanup_msg_buffers(info);
	cleanup_server_connect(info);
}

int init_info(struct fxfr_tx_state *info, char *src_name, char *dest_name,
		did_val_t did_val, int svr_skt, uint8_t mport_num,
		uint8_t debug, uint8_t k_buff)
{
	init_info_vals(info);
	
	info->debug = debug;

	if (init_file_info(info, src_name, dest_name))
		goto fail;

	if (init_message_buffers(info))
		goto fail;

	if (init_server_connect(info, mport_num, did_val, svr_skt, k_buff))
		goto fail;

	return 0;
fail:
	cleanup_all(info);
	return 1;
}

static void srv_sig_handler(int signum)
{
	switch (signum) {
	case SIGTERM:
	case SIGINT:
	case SIGUSR1:
		srv_exit = 1;
		break;
	}
}

int send_file(char *src_name, /* Local source file name */
		char *dest_name, /* Requested destination file name */
		did_val_t did_val, /* DestID of fxfr server */
		uint16_t skt_num, /* Channel # of fxfr server */
		uint8_t mport_num, /* MPORT index to use */
		uint8_t debug, /* MPORT index to use */
		struct timespec *st_time, uint64_t *bytes_sent, uint8_t k_buff)
{
	struct fxfr_tx_state info;

        /* Trap signals that we expect to receive */
        signal(SIGINT, srv_sig_handler);
        signal(SIGTERM, srv_sig_handler);
        signal(SIGUSR1, srv_sig_handler);

	/* Confirm local file, connectivity to remote mport/socket etc. */
	if (init_info(&info, src_name, dest_name, did_val, skt_num,
			mport_num, debug, k_buff))
		return -errno;

	while (!srv_exit && !info.fail_abort && !info.done) {
		rx_msg_from_server(&info);
		send_msgs_to_server(&info, st_time);
		st_time = NULL;
	}

	*bytes_sent = info.tot_bytes_txed;
	cleanup_all(&info);

	return info.fail_abort;
}

#ifdef __cplusplus
}
#endif

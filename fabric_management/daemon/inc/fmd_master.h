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

#ifndef __FMD_MASTER_H__
#define __FMD_MASTER_H__

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>

#include "rio_ecosystem.h"
#include "fmd_slave.h"
#include "liblist.h"
#include "fmd_state.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_peer {
	uint32_t cm_skt;

	int skt_h_valid;
	riomp_sock_t cm_skt_h;
	char peer_name[MAX_P_NAME+1];
	uint32_t p_pid;
	did_t p_did;
	ct_t p_ct;
	hc_t p_hc;
	struct l_item_t *li; /* Position of this peer in the fmp.peers list */

	pthread_t rx_thr; /* Thread listening for responses */
	sem_t started; /* Wait in this sema to ensure thread starts */
	sem_t do_the_free; /* Starter tells thread to free memory by posting this thread */
	int got_hello; /* 0 until received hello request */
	int rx_alive; /* 1 - RX thread is alive, 0 - RX thread is dead */
			/* Not 1 until hello response is received */
	sem_t init_cplt_mtx;
	int init_cplt; /* 1 - initial set of messages sent to peer */
	int restart_init; /* 1 - Additions/deletions occurred during init */
	int rx_must_die; /* 1 - RX thread should die */

	int tx_buff_used;
	int tx_rc;
	sem_t tx_mtx; /* Sender waits on mutex to get access to tx_buff, 
			then sends message. */
	union {
		rapidio_mport_socket_msg *tx_buff;
		struct fmd_mast_to_slv_msg *m2s; /* alias for tx_buff */
	};
	int rx_buff_used;
	int rx_rc;
	union {
		rapidio_mport_socket_msg *rx_buff;
		struct fmd_slv_to_mast_msg *s2m; /* alias for rx_buff */
	};
};

/* Data for thread accepting CM Connections */
struct fmd_mast_acc {
	uint32_t mp_num;
	uint32_t mb_valid;
	riomp_mailbox_t mb;
	sem_t mb_mtx; /* FIXME: Do we need this? */
	uint32_t cm_skt_num; /* Socket number to perform the accept on */
	uint32_t cm_acc_valid; 
	riomp_sock_t cm_acc_h; /* Listening socket */

	pthread_t acc; /* Listening thread for responses */
	sem_t started; /* Wait in ths sema to ensure thread starts */
	int acc_alive; /* 1 - Accepting thread is alive, 0 - thread is dead */
	int acc_must_die; /* 1 - Accepting thread should die */
};

struct fmd_pw_mgmt {
	pthread_t pw_mgr; /* Mgmt thread removes items from peers */
			/* Trigger is port-writes */
	sem_t started; 
	int alive;
	int pw_mgr_must_die;
};

struct fmd_mgmt {
	int mode; /* 1 = master, 0 = slave */
	struct fmd_slave slv;
	struct fmd_pw_mgmt pw_mgr;
	struct fmd_mast_acc acc; /* acc thread adds items to peers */
	sem_t peers_mtx;
	struct l_head_t peers; /* List of fmd_peer, ordered by did */
};

extern struct fmd_mgmt fmp;

int start_peer_mgmt(uint32_t mast_acc_skt_num, uint32_t mp_num, did_t mast_did,
		uint32_t master);

void update_peer_flags(void);

#ifdef __cplusplus
}
#endif

#endif /* __FMD_MASTER_H__ */

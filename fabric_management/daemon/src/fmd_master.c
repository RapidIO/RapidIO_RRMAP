/* Management implementation for FMDs operating in Master mode */
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

#include "fmd_master.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#include <rapidio_mport_mgmt.h>
#include <rapidio_mport_sock.h>

#include "string_util.h"
#include "rio_ecosystem.h"
#include "libcli.h"
#include "fmd.h"
#include "cfg.h"
#include "fmd_app.h"
#include "liblog.h"
#include "fmd_dd.h"
#include "fmd_slave.h"
#include "libfmdd.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_mgmt fmp;

void send_m2s_flag_update(struct fmd_peer *peer, struct fmd_dd_dev_info *dev)
{
	uint8_t flag;

	sem_wait(&peer->tx_mtx);
	peer->m2s->msg_type = htonl(FMD_P_REQ_FSET);
	peer->m2s->dest_did = htonl(peer->p_did);
	peer->m2s->fset.did = htonl(dev->destID);
	peer->m2s->fset.did_sz = htonl(dev->destID_sz);
	peer->m2s->fset.ct = htonl(dev->ct);
	flag = dev->flag;

	if (dev->is_mast_pt)
		flag &= ~FMDD_FLAG_MP;

	peer->m2s->fset.flag = htonl(flag);

	INFO("TX M2S FLAG UPDATE to did 0x%x for did 0x%x flag 0x%x\n",
		peer->p_did, dev->destID, flag);

	peer->tx_buff_used = 1;
	peer->tx_rc = riomp_sock_send(peer->cm_skt_h, 
				peer->tx_buff, FMD_P_M2S_CM_SZ);
	sem_post(&peer->tx_mtx);
	INFO("Sent M2S update to %s for %s, flags 0x%2x, tx rc 0x%x\n",
		peer->peer_name, dev->name, flag, peer->tx_rc);
	if (peer->tx_rc) {
		ERR("Failed M2S update to %s for %s, flags 0x%2x, tx rc 0x%x\n",
			peer->peer_name, dev->name, flag, peer->tx_rc);
	};
};

void send_add_dev_msg(struct fmd_peer *peer, struct fmd_dd_dev_info *dev)
{
	uint8_t flag;

	sem_wait(&peer->tx_mtx);
 	flag = (dev->flag & ~FMDD_FLAG_OK_MP) | FMDD_FLAG_OK;

	peer->m2s->msg_type = htonl(FMD_P_REQ_MOD);
	peer->m2s->dest_did = htonl(peer->p_did);
	peer->m2s->mod_rq.op = htonl(FMD_P_OP_ADD);
	peer->m2s->mod_rq.did = htonl(dev->destID);
	peer->m2s->mod_rq.did_sz = htonl(dev->destID_sz);
	peer->m2s->mod_rq.hc_long = htonl(HC_MP);
	peer->m2s->mod_rq.ct = htonl(dev->ct);
	peer->m2s->mod_rq.is_mp = 0;

	if (dev->is_mast_pt) {
		SAFE_STRNCPY(peer->m2s->mod_rq.name, FMD_SLAVE_MASTER_NAME, 
			sizeof(peer->m2s->mod_rq.name));
	} else {
		if (dev->destID == peer->p_did) {
			SAFE_STRNCPY(peer->m2s->mod_rq.name, FMD_SLAVE_MPORT_NAME,
				sizeof(peer->m2s->mod_rq.name));
			peer->m2s->mod_rq.is_mp = htonl(1);
			flag |= FMDD_FLAG_OK_MP;
		} else {
			struct l_item_t *li;
			struct fmd_peer *t_peer;

			sem_wait(&fmp.peers_mtx);
			t_peer = (struct fmd_peer *)
				l_find(&fmp.peers, dev->destID, &li);
			sem_post(&fmp.peers_mtx);

			/* Only tell peers about other connected peers */
			if (NULL == t_peer)
				goto exit;
			SAFE_STRNCPY(peer->m2s->mod_rq.name, dev->name, 
				sizeof(peer->m2s->mod_rq.name));
		};
	};
	INFO("TX ADD DEV MSG to did 0x%x Name %s Adding did 0x%x ct 0x%x\n",
		peer->p_did, peer->m2s->mod_rq.name, dev->destID, dev->ct);

	peer->m2s->mod_rq.flag = htonl(flag);
	peer->tx_buff_used = 1;
	peer->tx_rc = riomp_sock_send(peer->cm_skt_h, 
				peer->tx_buff, FMD_P_M2S_CM_SZ);
exit:
	sem_post(&peer->tx_mtx);
};

void send_del_dev_msg(struct fmd_peer *peer, struct fmd_peer *del_peer)
{
	sem_wait(&peer->tx_mtx);

	peer->m2s->msg_type = htonl(FMD_P_REQ_MOD);
	peer->m2s->dest_did = htonl(peer->p_did);
	peer->m2s->mod_rq.op = htonl(FMD_P_OP_DEL);
	peer->m2s->mod_rq.did = htonl(del_peer->p_did);
	peer->m2s->mod_rq.did_sz = htonl(del_peer->p_did_sz);
	peer->m2s->mod_rq.hc_long = htonl(del_peer->p_hc);
	peer->m2s->mod_rq.ct = htonl(del_peer->p_ct);
	peer->m2s->mod_rq.is_mp = 0;
	memcpy(peer->m2s->mod_rq.name, del_peer->peer_name, MAX_P_NAME+1);

	INFO("TX DEL DEV MSG to did 0x%x Dropping did 0x%x ct 0x%x\n",
		peer->p_did, del_peer->p_did, del_peer->p_ct);

	peer->m2s->mod_rq.flag = 0;
	peer->tx_buff_used = 1;
	peer->tx_rc = riomp_sock_send(peer->cm_skt_h, 
				peer->tx_buff, FMD_P_M2S_CM_SZ);
	sem_post(&peer->tx_mtx);
};

void update_all_peer_dd_and_flags(uint32_t add_dev)
{
	uint32_t src, tgt;
	struct fmd_peer *t_peer = NULL;
	struct l_item_t *li = NULL;
	uint32_t num_devs;

	struct fmd_dd_dev_info devs[FMD_MAX_DEVS];
	memset(devs, 0, sizeof(devs));

	if (0 >= fmd_dd_atomic_copy(fmd->dd, fmd->dd_mtx, &num_devs, devs,
				FMD_MAX_DEVS))
		return;

	for (src = 0; src < num_devs; src++) {
		INFO("\nSRC DestID 0x%x %s\n", devs[src].destID, devs[src].name);
		for (tgt = 0; tgt < num_devs; tgt++) {
			INFO("    TGT DestID 0x%x %s\n",
				devs[tgt].destID, devs[tgt].name);
			if (src == tgt) {
				// INFO("\n         Skip, SRC == TGT\n");
				continue;
			};
			if (devs[tgt].is_mast_pt) {
				// INFO("\n         Skip, TGT is mast port\n");
				continue;
			};
			t_peer = (struct fmd_peer *) 
				l_find(&fmp.peers, devs[tgt].destID, &li);
			if (NULL == t_peer) {
				ERR("\nNo addr for %s, can not add %s\n",
					devs[tgt].name, devs[src].name);
				continue;
			};
			if (add_dev) {
				send_add_dev_msg(t_peer, &devs[src]);
			} else {
				send_m2s_flag_update(t_peer, &devs[src]);
			}
		};
	};
};

/* Assumes that peer has already been removed from fmp.peers... */
void send_peer_removal_messages(struct fmd_peer *del_peer)
{
	struct fmd_peer *t_peer = NULL;
	struct l_item_t *li = NULL;

	sem_wait(&fmp.peers_mtx);

	t_peer = (struct fmd_peer *)l_head(&fmp.peers, &li);
	while (NULL != t_peer) {
		send_del_dev_msg(t_peer, del_peer);
		t_peer = (struct fmd_peer *)l_next(&li);
	};

	sem_post(&fmp.peers_mtx);
};

void master_process_hello_peer(struct fmd_peer *peer)
{
	riocp_pe_handle peer_pe;
	int add_to_list = 0;
	int peer_not_found;

	INFO("Peer(%x) RX HELLO Req %s 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		peer->p_ct, peer->s2m->hello_rq.peer_name,
		ntohl(peer->s2m->hello_rq.pid),
		ntohl(peer->s2m->hello_rq.did),
		ntohl(peer->s2m->hello_rq.did_sz),
		ntohl(peer->s2m->hello_rq.ct),
		ntohl(peer->s2m->hello_rq.hc_long));

	peer->p_pid = ntohl(peer->s2m->hello_rq.pid);
	peer->p_did = ntohl(peer->s2m->hello_rq.did);
	peer->p_did_sz = ntohl(peer->s2m->hello_rq.did_sz);
	peer->p_ct = ntohl(peer->s2m->hello_rq.ct);
	peer->p_hc = ntohl(peer->s2m->hello_rq.hc_long);
	SAFE_STRNCPY(peer->peer_name, peer->s2m->hello_rq.peer_name,
		sizeof(peer->peer_name));

	peer_not_found = riocp_pe_find_comptag(*fmd->mp_h, peer->p_ct, &peer_pe);

	if (peer_not_found) {
		DBG("Could not find configured peer ct 0x%x\n", peer->p_ct);
	}

	sem_wait(&peer->tx_mtx);

	peer->m2s->msg_type = htonl(FMD_P_RESP_HELLO);
	memset(peer->m2s->hello_rsp.peer_name, 0, MAX_P_NAME+1);
	if (peer_not_found) {
		snprintf(peer->m2s->hello_rsp.peer_name, (size_t)MAX_P_NAME,
			"%s", "REQUEST_DENIED!");
		peer->m2s->hello_rsp.pid = htonl(0);
		peer->m2s->hello_rsp.did = htonl(0);
		peer->m2s->hello_rsp.did_sz = htonl(0);
		peer->m2s->hello_rsp.ct = htonl(0);
		peer->m2s->hello_rsp.hc_long = htonl(0);
	} else {
		SAFE_STRNCPY(peer->m2s->hello_rsp.peer_name, peer_pe->sysfs_name,
			sizeof(peer->m2s->hello_rsp.peer_name));
		peer->m2s->hello_rsp.pid = htonl(getpid());
		peer->m2s->hello_rsp.did = htonl(fmd->opts->mast_devid);
		peer->m2s->hello_rsp.did_sz = htonl(fmd->opts->mast_devid_sz);
		peer->m2s->hello_rsp.ct = htonl(peer_pe->comptag);
		peer->m2s->hello_rsp.hc_long = htonl(0);
		add_to_list = 1;
		peer->p_hc = HC_MP;
	};
	peer->tx_buff_used = 1;
	peer->tx_rc = riomp_sock_send(peer->cm_skt_h, peer->tx_buff,
				FMD_P_M2S_CM_SZ);
	sem_post(&peer->tx_mtx);

	if (!peer->tx_rc && add_to_list) {
		peer->rx_alive = 2;
		peer->got_hello = 1;
		sem_wait(&fmp.peers_mtx);
		peer->li = l_add(&fmp.peers, peer->p_did, peer);
		sem_post(&fmp.peers_mtx);
		add_device_to_dd(peer->p_ct, peer->p_did, peer->p_did_sz,
			peer->p_hc, 0, FMDD_FLAG_OK,
			(char *)peer_pe->sysfs_name);
		HIGH("New Peer 0x%x: Updating all dd and flags\n", peer->p_ct);
		update_all_peer_dd_and_flags(1);
	};
};

void master_process_flag_set(struct fmd_peer *peer)
{
	uint32_t did;
	ct_t ct;
	uint8_t flag;
	uint32_t i;
	int tell_peers = 0;

	if ((NULL == fmd->dd) || (NULL == fmd->dd_mtx))
		return;

	INFO("Peer(0x%x) RX FLAG Set 0x%x 0x%x 0x%x\n",
		peer->p_ct, ntohl(peer->s2m->fset.did),
		ntohl(peer->s2m->fset.ct),
		ntohl(peer->s2m->fset.flag));

	did = ntohl(peer->s2m->fset.did);
	ct = ntohl(peer->s2m->fset.ct);
	flag = ntohl(peer->s2m->fset.flag);

	if ((did != peer->p_did) || (ct != peer->p_ct))
		return;

	sem_wait(&fmd->dd_mtx->sem);

	for (i = 0; i < fmd->dd->num_devs; i++) {
		if ((did == fmd->dd->devs[i].destID) &&
				(ct == fmd->dd->devs[i].ct)) {
			fmd->dd->devs[i].flag = flag;
			tell_peers = 1;
			break;
		};
	};
	sem_post(&fmd->dd_mtx->sem);

	if (tell_peers) {
		HIGH("Peer 0x%x FLAG SET 0x%x: Updating all dd and flags\n",
			peer->p_ct, flag);

		update_all_peer_dd_and_flags(0);
		fmd_notify_apps();
	};
};

void peer_rx_req(struct fmd_peer *peer)
{
	peer->rx_buff_used = 1;
	do {
		peer->rx_rc = riomp_sock_receive(peer->cm_skt_h, 
			&peer->rx_buff, FMD_P_S2M_CM_SZ, 3*60*1000);
	} while ((peer->rx_rc) && ((errno == EINTR) || (errno == ETIME)));

	if (peer->rx_rc) {
		ERR("PEER RX(0x%x): %d (%d:%s)\n",
			peer->p_ct, peer->rx_rc, errno, strerror(errno));
		peer->rx_must_die = 1;
	};
};

void cleanup_peer(struct fmd_peer *peer) 
{
	peer->rx_alive = 0;

	if (NULL != peer->li) {
		sem_wait(&fmp.peers_mtx);
		l_lremove(&fmp.peers, peer->li);
		peer->li = NULL;
		sem_post(&fmp.peers_mtx);
	};

	if (!del_device_from_dd(peer->p_ct, peer->p_did)) {
		send_peer_removal_messages(peer);
	};

	if (peer->tx_buff_used) {
		riomp_sock_release_send_buffer(peer->cm_skt_h,
						peer->tx_buff);
		peer->tx_buff = NULL;
		peer->tx_buff_used = 0;
	};
	
	if (peer->rx_buff_used) {
		riomp_sock_release_receive_buffer(peer->cm_skt_h,
						peer->rx_buff);
		peer->rx_buff = NULL;
		peer->rx_buff_used = 0;
	};

	if (peer->skt_h_valid && (NULL != peer->cm_skt_h)) {
		int rc = riomp_sock_close(&peer->cm_skt_h);
		if (rc) {
			ERR("socket close rc %d: %s\n", rc, strerror(errno));
		};
		peer->cm_skt_h = NULL;
		peer->skt_h_valid= 0;
	};
};

void *peer_rx_loop(void *p_i)
{
	struct fmd_peer *peer = (struct fmd_peer *)p_i;

	peer->rx_alive = 1;
	sem_post(&peer->started);

	while (!peer->rx_must_die && !peer->tx_rc && !peer->rx_rc) {
		peer_rx_req(peer);

		if (peer->rx_must_die || peer->rx_rc || peer->tx_rc)
			break;

		switch (ntohl(peer->s2m->msg_type)) {
		case FMD_P_REQ_HELLO:
			master_process_hello_peer(peer);
			break;
		case FMD_P_RESP_MOD:
			/* Nothing to do for a modification response */
			INFO(
			"Peer(0x%x) RX MOD Resp 0x%x 0x%x 0x%x 0x%x 0x%x rc %d\n",
				peer->p_ct,
				ntohl(peer->s2m->mod_rsp.did),
				ntohl(peer->s2m->mod_rsp.did_sz),
				ntohl(peer->s2m->mod_rsp.ct),
				ntohl(peer->s2m->mod_rsp.hc_long),
				ntohl(peer->s2m->mod_rsp.is_mp),
				ntohl(peer->s2m->mod_rsp.rc));
			break;
		case FMD_P_REQ_FSET:
			master_process_flag_set(peer);
			break;
		default:
			WARN("Peer(0x%x) RX Msg type 0x%x\n", peer->p_ct,
					ntohl(peer->s2m->msg_type));
			break;
		};
	};

	cleanup_peer(peer);

	INFO("Peer(0x%x) EXITING\n", peer->p_ct);
	pthread_exit(NULL);
};

int start_new_peer(riomp_sock_t new_skt)
{
	int rc;
	struct fmd_peer *peer = NULL;

	peer = (struct fmd_peer *) calloc(1, sizeof(struct fmd_peer));
	if (NULL == peer) {
		goto fail;
	}

	peer->skt_h_valid= 1;
	peer->cm_skt_h = new_skt;
	sem_init(&peer->init_cplt_mtx, 0, 1);
	sem_init(&peer->tx_mtx, 0, 1);
	sem_init(&peer->started, 0, 0);
	peer->rx_buff = calloc(1, 4096);

	if (riomp_sock_request_send_buffer(new_skt, &peer->tx_buff)) {
		riomp_sock_close(&new_skt);
		goto fail;
	};

        rc = pthread_create(&peer->rx_thr, NULL, peer_rx_loop, (void*)peer);
	if (rc) {
		cleanup_peer(peer);
		goto fail;
	};

	sem_wait(&peer->started);

	if (rc || !peer->rx_alive) {
		goto fail;
	};

	return 0;
fail:
	return 1;
};

void cleanup_acc_handler(void)
{
	fmp.acc.acc_alive = 0;
	if (fmp.acc.cm_acc_valid) {
		riomp_sock_close(&fmp.acc.cm_acc_h);
		memset(&fmp.acc.cm_acc_h, 0, sizeof(fmp.acc.cm_acc_h));
		fmp.acc.cm_acc_valid = 0;
	};

	if (fmp.acc.mb_valid) {
		riomp_sock_mbox_destroy_handle(&fmp.acc.mb);
		memset(&fmp.acc.mb, 0, sizeof(fmp.acc.mb));
		fmp.acc.mb_valid = 0;
	};
};

void *mast_acc(void *unused)
{
	int rc = 1;
	riomp_sock_t new_skt = NULL;

        char my_name[16] = {0};

        snprintf(my_name, 15, "MAST_PEER_ACC");
        pthread_setname_np(fmp.acc.acc, my_name);

        pthread_detach(fmp.acc.acc);

	fmp.acc.mb_valid = 0;
	fmp.acc.cm_acc_valid = 0;
	rc = riomp_sock_mbox_create_handle(fmp.acc.mp_num, 0, &fmp.acc.mb);
	if (rc) {
		ERR("riodp_mbox_create ERR %d\n", rc);
		goto exit;
	};
	fmp.acc.mb_valid = 1;
	sem_init(&fmp.acc.mb_mtx, 0, 1);

	rc = riomp_sock_socket(fmp.acc.mb, &fmp.acc.cm_acc_h);
	if (rc) {
		ERR("riomp_sock_socket ERR %d %d: %s\n", rc, errno,
			strerror(errno));
		goto exit;
	};

	rc = riomp_sock_bind(fmp.acc.cm_acc_h, fmp.acc.cm_skt_num);
	if (rc) {
		ERR("riomp_sock_bind() ERR %d errno %d: %s\n", rc, errno,
			strerror(errno));
		goto exit;
	};
	fmp.acc.cm_acc_valid = 1;

	rc = riomp_sock_listen(fmp.acc.cm_acc_h);
	if (rc) {
		ERR("riomp_sock_listen() ERR %d %d: %s\n", rc, errno,
			strerror(errno));
		goto exit;
	};
	fmp.acc.acc_alive = 1;
	sem_post(&fmp.acc.started);

	while (!fmp.acc.acc_must_die) {
		if (NULL == new_skt) {
			rc = riomp_sock_socket(fmp.acc.mb, &new_skt);
			if (rc) {
				ERR("socket() ERR %d\n", rc);
				break;
			};
		};
		do {
			errno = 0;
			rc = riomp_sock_accept(fmp.acc.cm_acc_h,
				&new_skt, 3*60*1000);
		} while (rc && ((errno == ETIME) || (errno == EINTR)));

		if (rc) {
			ERR("riodp_accept() ERR %d\n", rc);
			break;
		};

		if (fmp.acc.acc_must_die) {
			riomp_sock_close(&new_skt);
			continue;
		};

		if (start_new_peer(new_skt)) {
			WARN("Could not start peer after accept\n");
		};
			
		new_skt = NULL;
	}

	if (NULL != new_skt)
		free((void *)new_skt);

exit:
	cleanup_acc_handler();

	CRIT("\nFMD Peer Connection Handler EXITING\n");
	sem_post(&fmp.acc.started);
	pthread_exit(unused);
};

int start_peer_mgmt_master(uint32_t mast_acc_skt_num, uint32_t mp_num)
{
	uint32_t rc;

	fmp.acc.cm_skt_num = mast_acc_skt_num;
	fmp.acc.mp_num = mp_num;

	sem_init(&fmp.acc.started, 0, 0);
	sem_init(&fmp.peers_mtx, 0, 1);

        rc = pthread_create(&fmp.acc.acc, NULL, mast_acc, NULL);
        if (rc)
                goto fail;

        sem_wait(&fmp.acc.started);
	if (!fmp.acc.acc_alive)
		goto fail;

	return 0;
fail:
	return 1;
};

int start_peer_mgmt(uint32_t mast_acc_skt_num, uint32_t mp_num, 
		uint32_t mast_did, uint32_t master)
{
	uint32_t rc = 0;

	fmp.acc.cm_skt_num = mast_acc_skt_num;
	fmp.acc.mp_num = mp_num;
	fmp.mode = master;

	if (master) {
		rc = start_peer_mgmt_master(mast_acc_skt_num, mp_num);
	} else {
		rc = start_peer_mgmt_slave(mast_acc_skt_num, mast_did, mp_num, 
			&fmp.slv);
	};
	return rc;
};
		
void update_peer_flags(void)
{
	if (fmp.mode)
		update_all_peer_dd_and_flags(0);
	else
		update_master_flags_from_peer();
};

#ifdef __cplusplus
}
#endif

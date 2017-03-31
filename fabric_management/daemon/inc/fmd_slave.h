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

#ifndef __FMD_SLAVE_H__
#define __FMD_SLAVE_H__

/**
 * @file fmd_slave.h
 * Data Structure for connection to FMD in slave mode
 */

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>

#include "rio_route.h"
#include "rio_ecosystem.h"
#include "fmd_peer_msg.h"
#include "did.h"
#include "rapidio_mport_mgmt.h"
#include "rapidio_mport_sock.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_slave {
	riomp_mport_t mp_hnd; /* MPORT handle for register access */
	pthread_t slave_thr; /* Slave FMDR, handles Master FMD cmds */
	sem_t started; 
	int slave_alive;
	int slave_must_die;

        uint32_t mp_num;
	did_t mast_did;
        uint32_t mast_skt_num; /* Socket number to connect to */
        uint32_t mb_valid;
        riomp_mailbox_t mb;
	uint32_t skt_valid;
        riomp_sock_t skt_h; /* Connected socket */
	sem_t tx_mtx;
        int tx_buff_used;
        int tx_rc;
        union {
                rapidio_mport_socket_msg *tx_buff;
                struct fmd_slv_to_mast_msg *s2m; /* alias for tx_buff */
        };
        int rx_buff_used;
        int rx_rc;
        union {
                rapidio_mport_socket_msg *rx_buff;
                struct fmd_mast_to_slv_msg *m2s; /* alias for rx_buff */
        };
	int m_h_resp_valid;
	struct fmd_p_hello m_h_rsp;
};

int start_peer_mgmt_slave(uint32_t mast_acc_skt_num, did_t mast_did,
			uint32_t mp_num, struct fmd_slave *slave);

int add_device_to_dd(ct_t ct, did_t did, hc_t hc, uint32_t is_mast_pt,
		uint8_t flag, char *name);

int del_device_from_dd(ct_t ct, did_t did);

void update_master_flags_from_peer(void);

#ifdef __cplusplus
}
#endif

#endif /* __FMD_SLAVE_H__ */

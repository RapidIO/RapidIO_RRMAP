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

/* Implementation of Fabric Management Device Directory Library */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <netinet/in.h>
#include <assert.h>

#include "rio_misc.h"
#include "string_util.h"
#include "liblist.h"
#include "libcli.h"
#include "fmd_app_msg.h"
#include "fmd_dd.h"
#include "liblog.h"
#include "libfmdd_info.h"
#include "libfmdd.h"
#include "libtime_utils.h"
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fml_globals fml;

int open_socket_to_fmd(void)
{
	if (!fml.fd) {
		fml.addr_sz = sizeof(fml.addr);
		memset(&fml.addr, 0, fml.addr_sz);

		fml.addr.sun_family = AF_UNIX;
		snprintf(fml.addr.sun_path, sizeof(fml.addr.sun_path) - 1,
				FMD_APP_MSG_SKT_FMT, fml.portno);

		// If the file does not exist, the FMD is not running.
		// Do not try to connect.
		if (-1 == access(fml.addr.sun_path, F_OK)) {
			INFO("Could not access FMD Socket @ \"%s\", errno %d",
				fml.addr.sun_path, errno);
			goto fail;
		}

		fml.fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
		if (-1 == fml.fd) {
			goto fail;
		}

		if (connect(fml.fd, (struct sockaddr *)&fml.addr,
				fml.addr_sz)) {
			goto fail;
		}
	}
	return 0;

fail:
	return -1;
}

int get_dd_names_from_fmd(void)
{
	fml.req.msg_type = htonl(FMD_REQ_HELLO);
	fml.req.hello_req.flag = htonl(fml.flag);
	fml.req.hello_req.app_pid = htonl(getpid());
	SAFE_STRNCPY(fml.req.hello_req.app_name, fml.app_name,
			sizeof(fml.req.hello_req.app_name));

	if (send(fml.fd, (void *)&(fml.req), sizeof(fml.req), MSG_EOR) < 0) {
		CRIT(LOC_SOCKET_FAIL, fml.addr.sun_path, errno);
		goto fail;
	}

	if (recv(fml.fd, (void *)&(fml.resp), sizeof(fml.resp), 0) < 0) {
		ERR("Failed to recv(): %s\n", strerror(errno));
		goto fail;
	}

	if (FMD_RESP_HELLO != ntohl(fml.resp.msg_type)) {
		ERR("Non-HELLO message type!\n");
		goto fail;
	}

	fml.app_idx = ntohl(fml.resp.hello_resp.sm_dd_mtx_idx);
	if ((fml.app_idx < 0) || (fml.app_idx >= FMD_MAX_APPS)) {
		ERR("fml.ap_idx out of range!\n");
		goto fail;
	}

	fml.fmd_update_period = ntohl(fml.resp.hello_resp.fmd_update_pd);
	return 0;

fail:
	return -1;
}

int open_dd(void)
{
	SAFE_STRNCPY(fml.dd_fn, fml.resp.hello_resp.dd_fn, sizeof(fml.dd_fn));
	SAFE_STRNCPY(fml.dd_mtx_fn, fml.resp.hello_resp.dd_mtx_fn,
			sizeof(fml.dd_mtx_fn));

	if (fmd_dd_mtx_open((char *)&fml.dd_mtx_fn, &fml.dd_mtx_fd,
			&fml.dd_mtx)) {
		ERR("fmd_dd_mtx_open failed\n");
		goto fail;
	}

	if (fmd_dd_open((char *)&fml.dd_fn, &fml.dd_fd, &fml.dd, fml.dd_mtx)) {
		ERR("fmd_dd_mtx_open failed\n");
		goto fail;
	}
	return 0;

fail:
	return -1;
}

void notify_app_of_events(void);

void shutdown_fml(fmdd_h dd_h)
{
	if (dd_h == &fml) {
		DBG("Notifying app of events\n");
		notify_app_of_events();
	}

	if ((NULL != fml.dd_mtx) && (fml.dd_mtx->dd_ev[fml.app_idx].waiting)) {
		fml.dd_mtx->dd_ev[fml.app_idx].waiting = 0;
		fml.dd_mtx->dd_ev[fml.app_idx].proc = 0;
		fml.dd_mtx->dd_ev[fml.app_idx].in_use = 0;
		sem_post(&fml.dd_mtx->dd_ev[fml.app_idx].dd_event);
	}

	fmd_dd_cleanup(fml.dd_mtx_fn, &fml.dd_mtx_fd, &fml.dd_mtx, fml.dd_fn,
			&fml.dd_fd, &fml.dd, 0);

	fml.dd_mtx = NULL;
	fml.dd = NULL;

	if (fml.dd_fd) {
		close(fml.dd_fd);
		fml.dd_fd = 0;
	}

	if (fml.dd_mtx_fd) {
		close(fml.dd_mtx_fd);
		fml.dd_mtx_fd = 0;
	}

	if (fml.fd) {
		close(fml.fd);
		fml.fd = 0;
	}
}

void init_devid_status(void)
{
	uint32_t i;

	for (i = 0; i < FMD_MAX_DEVS; i++) {
		fml.devid_status[i] = FMDD_FLAG_NOK;
	}
}

int update_devid_status(void)
{
	uint32_t i, j, found;
	uint32_t changed = 0;
	did_val_t did_val;

	for (i = 0; i <= FMD_MAX_DEVID; i++) {
		found = 0;
		for (j = 0; j < fml.num_devs; j++) {
			did_val = did_get_value(fml.devs[j].did);
			if (did_val > FMD_MAX_DEVID) {
				ERR("Devid 0x%x, out of range, MAX is 0x%x",
						did_val, FMD_MAX_DEVID);
				continue;
			}

			if (did_val == i) {
				uint8_t temp_flag = FMDD_FLAG_OK;
				temp_flag |= fml.devs[j].flag;

				if (fml.devs[j].is_mast_pt) {
					temp_flag |= FMDD_FLAG_OK_MP;
				}

				if (fml.devid_status[i] != temp_flag) {
					fml.devid_status[i] = temp_flag;
					changed = 1;
				}
				found = 1;
				break;
			}
		}

		if (!found && (FMDD_FLAG_NOK != fml.devid_status[i])) {
			fml.devid_status[i] = FMDD_FLAG_NOK;
			changed = 1;
		}
	}
	return changed;
}

void notify_app_of_events(void)
{
	sem_t *wt = NULL;

	sem_wait(&fml.pend_waits_mtx);

	wt = (sem_t *)l_pop_head(&fml.pend_waits);
	if (NULL == wt) {
		DBG("wt == NULL\n");
	}

	while (NULL != wt) {
		sem_post(wt);
		wt = (sem_t *)l_pop_head(&fml.pend_waits);
	}

	sem_post(&fml.pend_waits_mtx);
}

// Very simple monitor:  If anything goes sideways on the socket connection
// to the FMD, die!
void *fmd_socket_mon(void *UNUSED(unused))
{
	char buf[1];

	sem_post(&fml.fmd_mon_started);
	recv(fml.fd, (void *)&buf, sizeof(buf), MSG_WAITALL);

	close(fml.fd);
	fml.fd = 0;
	sem_post(&fml.dd_mtx->dd_ev[fml.app_idx].dd_event);
	pthread_exit(NULL);
}

void *mon_loop(void *parms)
{
	const struct timespec delay = {3 * fml.fmd_update_period, 0}; // seconds
	const struct timespec loop_delay = {1, 0}; // seconds

	int rc;
	struct fmd_dd_ticks new_ticks = {0, {0,0}};
	struct fmd_dd_ticks old_ticks = {0, {0,0}};
	bool display_msg = true;

	fml.mon_alive = 1;
	sem_post(&fml.mon_started);

	// Wait until the application information is set before
	// attempting to connect to the FMD.
	sem_wait(&fml.app_info_set);

	do {
		if (open_socket_to_fmd()) {
			if (display_msg) {
				CRIT(LOC_SOCKET_FAIL, fml.addr.sun_path, errno);
				display_msg = false;
			}
			goto cleanup;
		} else {
			display_msg = true;
		}

		if (get_dd_names_from_fmd()) {
			CRIT(DEV_DB_FAIL, "");
			goto cleanup;
		}
		if (open_dd()) {
			CRIT(DEV_DB_FAIL, "");
			goto cleanup;
		}
		fml.dd_mtx->dd_ev[fml.app_idx].in_use = 1;
		fml.dd_mtx->dd_ev[fml.app_idx].proc = getpid();

		// monitoring the FMD socket
		sem_init(&fml.fmd_mon_started, 0, 0);
		if (pthread_create(&fml.fmd_mon_thr, NULL, fmd_socket_mon, NULL)) {
			goto cleanup;
		}
		sem_wait(&fml.fmd_mon_started);

		// While the FMD/DD is alive wait for updates
		fml.fmd_dead = false;
		do {
			fml.dd_mtx->dd_ev[fml.app_idx].waiting = 0;

			if (!fmd_dd_atomic_copy(fml.dd, fml.dd_mtx,
					&fml.num_devs, fml.devs,
					FMD_MAX_DEVS)) {
				break;
			}

			if (update_devid_status()) {
				notify_app_of_events();
			}
			fml.dd_mtx->dd_ev[fml.app_idx].waiting = 1;

			do {
				rc = sem_timedwait(&fml.dd_mtx->dd_ev[fml.app_idx].dd_event,
						&delay);
			} while (rc && ((EAGAIN == errno) || (EINTR == errno)));

			if (rc && (ETIME == errno)) {
				// If there's a problem accessing the DD, bail and disconnect.
				if (!fmd_dd_atomic_copy_ticks(fml.dd,
						fml.dd_mtx, &new_ticks)) {
					break;
				}

				if ((new_ticks.chg_idx == old_ticks.chg_idx)
						|| ((new_ticks.chg_time.tv_nsec
								== old_ticks.chg_time.tv_nsec)
								&& (new_ticks.chg_time.tv_sec
										== old_ticks.chg_time.tv_sec))) {
					// Heart beat is dead, bail and disconnect
					break;
				}
				old_ticks = new_ticks;
			}
		} while (fml.fd && !fml.mon_must_die);
		fml.fmd_dead = true;

cleanup:
		// Cannot access DD, cleanup
		shutdown_fml(&fml);
		time_sleep(&loop_delay);

	} while (!fml.mon_must_die);

	fml.mon_alive = 0;
	notify_app_of_events();
	shutdown_fml(&fml);
	return parms;
}

void libfmdd_init(void) {
	sem_init(&fml.app_info_set, 0, 0);
	sem_init(&fml.pend_waits_mtx, 0, 1);
	l_init(&fml.pend_waits);

	sem_init(&fml.mon_started, 0, 0);
	fml.all_must_die = 0;
	fml.mon_alive = 0;
	fml.fmd_dead = true;

	/* Startup the connection monitoring thread */
	if (pthread_create(&fml.mon_thr, NULL, mon_loop, NULL)) {
		fml.all_must_die = 1;
		CRIT(THREAD_FAIL, errno);
		return;
	}

	sem_wait(&fml.mon_started);
	INFO("Monitor thread started...\n");
}

fmdd_h fmdd_get_handle(char *my_name, uint8_t flag)
{
	DBG("ENTER with my_name = %s, flag = 0x%X\n", my_name, flag);

	// if an application calls fmdd_get_handle multiple times, the first
	// time should cause initialization and the second and subsequent
	// should return pointers to the handle that the first one is
	// initializing.
	fml.flag = flag;
	if (!fml.portno) {
		INFO("No portno specified, using default of %d\n",
				FMD_DFLT_APP_PORT_NUM);
		fml.portno = FMD_DFLT_APP_PORT_NUM;
		SAFE_STRNCPY(fml.app_name, my_name, sizeof(fml.app_name));
	} else {
		INFO("fml.portno = %d\n", fml.portno);
	}
	sem_post(&fml.app_info_set);

	// Only return a valid handle when connected to the FMD.
	if (fml.fd) {
		return (void *)&fml;
	}
	return NULL;
}

void fmdd_destroy_handle(fmdd_h *dd_h)
{
	shutdown_fml(dd_h);
	*dd_h = NULL;
}

uint8_t fmdd_check_ct(fmdd_h h, ct_t ct, uint8_t flag)
{
	uint32_t i;

	if (h != &fml) {
		goto fail;
	}

	for (i = 0; i < fml.num_devs; i++) {
		if (fml.devs[i].ct == ct) {
			return flag & fml.devid_status[did_get_value(fml.devs[i].did)];
		}
	}

fail:
	return FMDD_FLAG_NOK;
}

uint8_t fmdd_check_did(fmdd_h h, did_val_t did_val, uint8_t flag)
{
	if (h != &fml) {
		ERR("Bad FMDD handle\n");
		goto fail;
	}

	if (did_val > FMD_MAX_DEVID) {
		goto fail;
	}

	return flag & fml.devid_status[did_val];

fail:
	return FMDD_FLAG_NOK;
}

int fmdd_get_did_list(fmdd_h h, uint32_t *did_list_sz, did_val_t **did_list)
{
	did_val_t i;
	uint32_t cnt = 0, idx = 0;
	uint8_t flag = 0;

	DBG("Fetching DID list\n");

	if (h != &fml) {
		ERR("Invalid fmdd_h h(0x%X)\n", h);
		goto fail;
	}

	if (!fml.fd) {
		ERR("Not connected to FMD, fail\n", h);
		goto fail;
	}

	for (i = 0; i < FMD_MAX_DEVID; i++) {
		flag = fmdd_check_did(h, i, FMDD_FLAG_OK_MP);
		if (flag && (FMDD_FLAG_OK_MP != flag)) {
			cnt++;
		}
	}

	*did_list_sz = cnt;
	if (!cnt) {
		INFO("Returning empty(NULL) list since cnt==0\n");
		*did_list = NULL;
		goto exit;
	}

	*did_list = (did_val_t *)calloc(cnt, sizeof(did_val_t));
	if (NULL == *did_list) {
		goto fail;
	}

	for (i = 0; i <= FMD_MAX_DEVID; i++) {
		flag = fmdd_check_did(h, i, FMDD_FLAG_OK_MP);
		if (flag && (FMDD_FLAG_OK_MP != flag)) {
			DBG("Adding did %d index %d\n", i, idx);
			(*did_list)[idx] = i;
			idx++;
		}
	}

exit:
	return 0;
fail:
	return 1;
}

int fmdd_free_did_list(fmdd_h h, did_val_t **did_list)
{
	if (h != &fml) {
		return 1;
	}

	DBG("Freeing DID list...\n");
	if (NULL != *did_list) {
		free(*did_list);
	}
	*did_list = NULL;

	return 0;
}

int fmdd_wait_for_dd_change(fmdd_h h)
{
	sem_t *chg_sem = (sem_t *)calloc(1, sizeof(sem_t));
	int rc;

	if ((h != &fml) || fml.mon_must_die || !fml.mon_alive) {
		ERR("Bad handle, mon not alive or mon must die\n");
		goto fail;
	}

	sem_init(chg_sem, 0, 0);

	sem_wait(&fml.pend_waits_mtx);
	l_push_tail(&fml.pend_waits, (void *)chg_sem);
	sem_post(&fml.pend_waits_mtx);

	DBG("Waiting for change to device database\n");
	rc = sem_wait(chg_sem);

	DBG("Waking up after change to device database\n");

	sem_destroy(chg_sem);
	free(chg_sem);

	if (fml.mon_must_die || !fml.mon_alive || rc) {
		ERR("mon_must_die, !mon_alive or sem_wait() failed\n");
		goto fail;
	}
	return 0;

fail:
	return 1;
}

#ifdef __cplusplus
}
#endif


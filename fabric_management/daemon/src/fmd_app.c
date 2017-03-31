/* Implementation of the RDMA Socket Daemon side of the "librskt" library */
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

#include "fmd_app.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>

#include "rio_misc.h"
#include "string_util.h"
#include "fmd_state.h"
#include "fmd_dd.h"
#include "fmd_app_msg.h"
#include "fmd_master.h"
#include "liblog.h"
#include "libfmdd.h"
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct app_mgmt_globals app_st;

#define INIT_SEM 1
#define NO_SEM 0

void init_app_mgmt(struct fmd_app_mgmt_state *app, int init_sem)
{
	app->alloced = 0;
	app->app_fd = 0;
	app->addr_size = 0;
	memset((void *)&app->addr, 0, sizeof(struct sockaddr_un));
	app->alive = 0;

	if (NO_SEM != init_sem) {
		sem_init(&app->started, 0, 0);
	} else {
		if (!sem_destroy(&app->started))
			sem_init(&app->started, 0, 0);
	}

	app->i_must_die = 0;
	app->proc_num = 0;
	memset((void *)app->app_name, 0, MAX_APP_NAME+1);
	memset((void *)&app->req, 0, sizeof(struct libfmd_dmn_app_msg));
	memset((void *)&app->resp, 0, sizeof(struct libfmd_dmn_app_msg));
}

void init_app_mgmt_st(void)
{
	int i;

	app_st.port = -1;
	app_st.bklg = -1;
	app_st.loop_alive = 0;
	sem_init(&app_st.loop_started, 0, 0);
	app_st.all_must_die = 0;
	app_st.ct = 0;
	app_st.fd = 0;

	memset((void *)&app_st.addr, 0, sizeof(struct sockaddr_un));
	for (i = 0; i < FMD_MAX_APPS; i++) {
		init_app_mgmt(&app_st.apps[i], INIT_SEM);
		app_st.apps[i].index = i;
	}
	sem_init(&app_st.apps_avail, 0, FMD_MAX_APPS);
}

int handle_app_msg(struct fmd_app_mgmt_state *app)
{
	memset((void *)&app->resp, 0, sizeof(struct libfmd_dmn_app_msg));

	app->resp.msg_type = app->req.msg_type | htonl(FMD_APP_MSG_RESP);

	if (htonl(FMD_REQ_HELLO) != app->req.msg_type) {
		app->resp.msg_type |= htonl(FMD_APP_MSG_FAIL);
		return 0;
	}

	app->proc_num = ntohl(app->req.hello_req.app_pid);
	app->flag = ntohl(app->req.hello_req.flag);
	SAFE_STRNCPY(app->app_name, app->req.hello_req.app_name,
			sizeof(app->app_name));
	app->resp.hello_resp.sm_dd_mtx_idx = htonl(app->index);
	SAFE_STRNCPY(app->resp.hello_resp.dd_fn, app_st.dd_fn,
			sizeof(app->resp.hello_resp.dd_fn));
	SAFE_STRNCPY(app->resp.hello_resp.dd_mtx_fn, app_st.dd_mtx_fn,
			sizeof(app->resp.hello_resp.dd_mtx_fn));
	app->resp.hello_resp.fmd_update_pd = htonl(fmd->opts->mast_interval);
	INFO("APP %s Connected!\n", app->app_name);
	return 1;
}

void mod_dd_mp_flag(uint8_t flag, int add_it)
{
	uint32_t i;

	if ((NULL == fmd->dd) || (NULL == fmd->dd_mtx)) {
		return;
	}

	if ((flag == FMDD_NO_FLAG) || (flag == FMDD_ANY_FLAG)) {
		return;
	}

	i = fmd->dd->loc_mp_idx;
	if (i >= fmd->dd->num_devs) {
		return;
	}

	sem_wait(&fmd->dd_mtx->sem);

	if (fmd->dd->devs[i].is_mast_pt) { 
		if (add_it)
			fmd->dd->devs[i].flag |= flag;
		else
			fmd->dd->devs[i].flag &= ~flag;
	} else {
		ERR("DD Index %d is not master port!", i);
	}

	sem_post(&fmd->dd_mtx->sem);
}


/* Initializes and then monitors one application. */
/* This thread is responsible for managing the library connection if the 
 * app shuts down unexpectedly.
 */

void *app_loop(void *ip)
{
	struct fmd_app_mgmt_state *app = (struct fmd_app_mgmt_state *)ip;
	int msg_size = sizeof(struct libfmd_dmn_app_msg); 
	int rc;
	int update_reqd;

	memset((void *)&app->app_name, 0, MAX_APP_NAME);
	app->alive = 1;
	sem_post(&app->started);
	
	while (!app->i_must_die) {
		do {
			rc = recv(app->app_fd, &app->req, msg_size, 0);
		} while ((EINTR == errno) && !app->i_must_die);

		if ((rc <= 0) || app->i_must_die) {
			break;
		}

		update_reqd = handle_app_msg(app);

		rc = send(app->app_fd, &app->resp, msg_size, 0);
		if ((rc != msg_size) || app->i_must_die) {
			break;
		}

		if (update_reqd) {
			mod_dd_mp_flag(app->flag, 1);
			fmd_notify_apps();
			update_peer_flags();
		}
	}

	if (app->app_fd) {
		close(app->app_fd);
		app->app_fd = 0;
	}

	app->alloced = 0;
	app->alive = 0;
	sem_post(&app_st.apps_avail);
	INFO("APP %s DISCONNECTED!\n", app->app_name);
	mod_dd_mp_flag(app->flag, 0);
	fmd_notify_apps();
	update_peer_flags();

	pthread_exit(NULL);
}
	
int open_app_conn_socket(void)
{
	if (-1 == access(RRMAP_TEMP_DIR_PATH, F_OK)) {
		mkdir(RRMAP_TEMP_DIR_PATH, 0777);
	}
	app_st.addr.sun_family = AF_UNIX;

	snprintf(app_st.addr.sun_path, sizeof(app_st.addr.sun_path) - 1,
		FMD_APP_MSG_SKT_FMT, app_st.port);

	unlink(app_st.addr.sun_path);

	app_st.fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (-1 == app_st.fd) {
		CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
		goto fail;
	}

	if (remove(app_st.addr.sun_path) && (ENOENT != errno)) {
		CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
	}

	if (-1 == bind(app_st.fd, (struct sockaddr *) &app_st.addr, 
			sizeof(struct sockaddr_un))) {
		CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
		goto fail;
	}

	if (chmod(app_st.addr.sun_path, S_IRWXU | S_IRWXG | S_IRWXO)) {
		CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
		goto fail;
	}

	if (listen(app_st.fd, app_st.bklg) == -1) {
		CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
		goto fail;
	}
	return 0;

fail:
	return 1;
}

void *app_conn_loop( void *unused )
{
	int rc = open_app_conn_socket(); 
	char my_name[16];

	memset(my_name, 0, 16);
	snprintf(my_name, 15, "FMD_APP_CONN");
	pthread_setname_np(app_st.conn_thread, my_name);

	pthread_detach(app_st.conn_thread);

	/* Open Unix domain socket */
	app_st.loop_alive = (!rc);
	app_st.all_must_die = !app_st.loop_alive;
	sem_post(&app_st.loop_started);

	while (!app_st.all_must_die) {
		int rc, found, i, new_app_i = - 1;
		struct fmd_app_mgmt_state *new_app;

		sem_wait(&app_st.apps_avail);
		found = 0;
		for (i = 0; (i < FMD_MAX_APPS) && !found; i++) {
			if (!app_st.apps[i].alloced) {
				new_app_i = i;
				found = 1;
			}
		}

		if (!found) {
			CRIT("FMD: Maximum applications reached!");
			goto fail;
		}
		new_app = &app_st.apps[new_app_i];
		init_app_mgmt(new_app, NO_SEM);

		new_app->addr_size = sizeof(struct sockaddr_un);
		new_app->app_fd = accept(app_st.fd,
				(struct sockaddr *)&new_app->addr,
				&new_app->addr_size);

		if (-1 == new_app->app_fd) {
			CRIT(LOC_SOCKET_FAIL, app_st.addr.sun_path, errno);
			goto fail;
		}

		new_app->alloced = 2;
		rc = pthread_create(&new_app->app_thr, NULL, app_loop,
				(void *)new_app);
		if (rc) {
			ERR("Error - app_rx_loop rc: %d\n", rc);
		} else {
			sem_wait(&new_app->started);
			new_app->alloced = 1;
		}
	}

fail:
	CRIT("\nFMD Application Connection Thread Exiting\n");
	halt_app_handler();

	pthread_exit(unused);
	return unused;
}

int start_fmd_app_handler(uint32_t port, uint32_t backlog,
		char *dd_fn, char *dd_mtx_fn)
{
	int ret;

	init_app_mgmt_st();

	/* Prepare and start application connection handling threads */
	app_st.port = port;
	app_st.bklg = backlog;
	app_st.dd_fn = dd_fn;
	app_st.dd_mtx_fn = dd_mtx_fn;

	ret = pthread_create(&app_st.conn_thread, NULL, app_conn_loop, NULL);
	if (ret) {
		printf("Error - start_fmd_app_handler rc: %d\n", ret);
	} else {
		sem_wait(&app_st.loop_started);
	}

	return ret;
}

int app_handler_dead(void)
{
	return !app_st.loop_alive;
}

void halt_app_handler(void)
{
	cleanup_app_handler();
}

void cleanup_app_handler(void)
{
	if (app_st.fd > 0) {
		close(app_st.fd);
		unlink(app_st.addr.sun_path);
		app_st.fd = -1;
	}
}

void fmd_notify_apps (void)
{
	int i;

	if (NULL == fmd->dd_mtx)
		return;

	for (i = 0; i < FMD_MAX_APPS; i++) {
		if (!app_st.apps[i].alloced || !app_st.apps[i].alive) {
			continue;
		}
		if (!fmd->dd_mtx->dd_ev[i].in_use || !app_st.apps[i].proc_num) {
			continue;
		}
		if (fmd->dd_mtx->dd_ev[i].proc != app_st.apps[i].proc_num) {
			continue;
		}

		sem_post(&fmd->dd_mtx->dd_ev[i].dd_event); 
	}
}

#ifdef __cplusplus
}
#endif

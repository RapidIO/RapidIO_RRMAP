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

#ifndef __FMD_APP_H__
#define __FMD_APP_H__

/**
 * @file fmd_app.h
 * Application (library) routines for internal use by FMD
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

#include "did.h"
#include "ct.h"
#include "fmd_dd.h"
#include "fmd_app_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_app_mgmt_state {
	int index;
	int alloced;
	int app_fd;
	socklen_t addr_size;
	struct sockaddr_un addr;
	pthread_t app_thr;
	int alive;
	sem_t started;
	volatile int i_must_die;
	uint32_t proc_num;
	uint32_t flag;
	char app_name[MAX_APP_NAME + 1];
	struct libfmd_dmn_app_msg req;
	struct libfmd_dmn_app_msg resp;
};

struct app_mgmt_globals {
	int port;
	int bklg;
	char *dd_fn;
	char *dd_mtx_fn;

	pthread_t conn_thread;
	int loop_alive;
	sem_t loop_started;
	volatile int all_must_die;
	ct_t ct; /* Component tag of FMD mport */

	int fd; /* File number library instance connect to */
	struct sockaddr_un addr;
	sem_t apps_avail;
	struct fmd_app_mgmt_state apps[FMD_MAX_APPS];
};

int start_fmd_app_handler(uint32_t port, uint32_t backlog, char *dd_fn,
		char *dd_mtx_fn);

void halt_app_handler(void);

void cleanup_app_handler(void);

void fmd_notify_apps(void);

extern struct app_mgmt_globals app_st;

#ifdef __cplusplus
}
#endif

#endif /* __FMD_APP_H__ */

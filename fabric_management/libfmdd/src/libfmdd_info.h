/* Information structure of Fabric Management Device Directory Library */
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <netinet/in.h>

#include "libcli.h"
#include "fmd_app_msg.h"
#include "fmd_dd.h"
#include "liblog.h"
#include "liblist.h"
#include "libfmdd.h"
#include "rrmap_config.h"

#ifndef __LIBFMDD_INFO_H__
#define __LIBFMDD_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

struct fml_globals {
        int portno;     /* FMD port number to connect to */
        int init_ok;    /* Equal to portno when initialization is successful */
	char app_name[MAX_APP_NAME+1];
	uint8_t flag;   /* Flag for this app. FMDD_NO_FLAG means what is says */

        struct sockaddr_un addr; /* FMD Linux socket address */
        int addr_sz;    /* size of addr */
        int fd;         /* Connection to FMD */

	struct libfmd_dmn_app_msg req;
	struct libfmd_dmn_app_msg resp;

	char   dd_fn[MAX_DD_FN_SZ+1];
	char   dd_mtx_fn[MAX_DD_MTX_FN_SZ+1];
	int dd_fd;
	int dd_mtx_fd;
	struct fmd_dd *dd;
	struct fmd_dd_mtx *dd_mtx;

        int all_must_die; /* When non-zero, all threads exit immediately */
                        /* FMD must cleanup */

	int app_idx;
        pthread_t mon_thr;  /* Thread for monitoring DD */
	sem_t mon_started;
	int mon_must_die;
	int mon_alive;

	uint32_t num_devs;
	struct fmd_dd_dev_info devs[FMD_MAX_DEVS+1];
	uint8_t devid_status[FMD_MAX_DEVID+1];

	
	sem_t pend_waits_mtx;
	struct l_head_t pend_waits;
};
	
extern struct fml_globals fml;

#ifdef __cplusplus
}
#endif

#endif /* __LIBFMDD_INFO_H__ */


/* Fabric Management Daemon Device Directory Implementation */
/*
****************************************************************************
Copyright (c) 2014, Integrated Device Technology Inc.
Copyright (c) 2014, RapidIO Trade Association
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
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "rio_route.h"
#include "fmd_dd.h"
#include "liblog.h"
#include "riocp_pe.h"
#include "libcli.h"
#include "libfmdd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <time.h>
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

void fmd_dd_incr_chg_idx(struct fmd_dd *dd, int dd_rw)
{
	if ((NULL != dd) && dd_rw) {
		uint32_t next_idx = dd->chg_idx+1;
		if (!next_idx)
			next_idx = 1;
		dd->chg_idx = next_idx;
		clock_gettime(CLOCK_REALTIME, &dd->chg_time);
	}
}

int fmd_dd_open_rw(char *dd_fn, int *dd_fd, struct fmd_dd **dd,
					struct fmd_dd_mtx *dd_mtx)
{
	int rc, idx;

	*dd_fd = shm_open(dd_fn, O_RDWR | O_CREAT | O_EXCL,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (-1 == *dd_fd) {
		CRIT(DEV_DB_FAIL, dd_fn);
		goto fail;
	}

	rc = ftruncate(*dd_fd, sizeof(struct fmd_dd));
	if (-1 == rc) {
		CRIT(DEV_DB_FAIL, dd_fn);
		shm_unlink(dd_fn);
		goto fail;
	}

	*dd = (struct fmd_dd *) mmap(NULL, sizeof(struct fmd_dd),
		PROT_READ|PROT_WRITE, MAP_SHARED, *dd_fd, 0);

	if (MAP_FAILED == *dd) {
		CRIT(DEV_DB_FAIL, dd_fn);
		*dd = NULL;
		shm_unlink(dd_fn);
		goto fail;
	}

	(*dd)->chg_idx = 0;
	(*dd)->md_ct = 0;
	(*dd)->num_devs = 0;
	(*dd)->loc_mp_idx = FMD_MAX_DEVS;
	for (idx = 0; idx < FMD_MAX_DEVS; idx++) {
		(*dd)->devs[idx].ct = 0;
		(*dd)->devs[idx].did = (did_t){0, dev08_sz};
		(*dd)->devs[idx].hc = HC_MP;
		(*dd)->devs[idx].is_mast_pt = 0;
		(*dd)->devs[idx].flag = FMDD_NO_FLAG;
		memset((*dd)->devs[idx].name, 0, FMD_MAX_NAME+1);
	}
	fmd_dd_incr_chg_idx(*dd, 1);
	dd_mtx->dd_ref_cnt++;

	return 0;
fail:
	return -1;
}

int fmd_dd_open(char *dd_fn, int *dd_fd, struct fmd_dd **dd,
					struct fmd_dd_mtx *dd_mtx)
{
	*dd_fd = shm_open(dd_fn, O_RDONLY, 0);
	if (-1 == *dd_fd) {
		CRIT(DEV_DB_FAIL, dd_fn);
		goto exit;
	}

	*dd = (struct fmd_dd *)mmap(NULL, sizeof(struct fmd_dd), PROT_READ,
		MAP_SHARED, *dd_fd, 0);

	if (MAP_FAILED == *dd) {
		CRIT(DEV_DB_FAIL, dd_fn);
		*dd = NULL;
		goto exit;
	}

	if ((NULL != *dd) && (NULL != dd_mtx)) {
		dd_mtx->dd_ref_cnt++;
	}

	return 0;
exit:
	return -1;
}

int fmd_dd_mtx_open_priv(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx, bool open_by_fmd)
{
	int rc, i;
	bool new_mtx = false;
	char mutex_fn[FMD_MAX_SHM_FN_LEN];

	if ((NULL == dd_mtx_fn) || (NULL == dd_mtx_fd) || (NULL == dd_mtx)) {
		errno = -EINVAL;
		goto fail;
	}

	if (open_by_fmd) {
		// FMD owns the MTX file, so it should create the file.
		*dd_mtx_fd = shm_open(dd_mtx_fn, O_RDWR | O_CREAT | O_EXCL,
				S_IRUSR | S_IWUSR | S_IXUSR |
				S_IRGRP | S_IWGRP | S_IXGRP |
				S_IROTH | S_IWOTH | S_IXOTH);
		if (EEXIST == errno) {
			CRIT(DEV_DB_FAIL, dd_mtx_fn);
			goto fail;
		}
		snprintf(mutex_fn, FMD_MAX_SHM_FN_LEN,
			"%s%s", FMD_DFLT_SHM_DIR, dd_mtx_fn);
		if (chmod(mutex_fn, S_IRWXU | S_IRWXG | S_IRWXO)) {
			CRIT(DEV_DB_FAIL, dd_mtx_fn);
			goto fail;
		}

		new_mtx = true;
	} else {
		*dd_mtx_fd = shm_open(dd_mtx_fn, O_RDWR, S_IRUSR | S_IWUSR);
	}

	if (-1 == *dd_mtx_fd) {
		CRIT(DEV_DB_FAIL, dd_mtx_fn);
		goto fail;
	}

	rc = ftruncate(*dd_mtx_fd, sizeof(struct fmd_dd_mtx));
	if (-1 == rc) {
		CRIT(DEV_DB_FAIL, dd_mtx_fn);
		goto fail;
	}

	*dd_mtx = (struct fmd_dd_mtx *)mmap(NULL, sizeof(struct fmd_dd_mtx),
			PROT_READ|PROT_WRITE, MAP_SHARED, *dd_mtx_fd, 0);

	if (MAP_FAILED == *dd_mtx) {
		CRIT(DEV_DB_FAIL, dd_mtx_fn);
		*dd_mtx = NULL;
		goto fail;
	}

	if (new_mtx) {
		sem_init(&(*dd_mtx)->sem, 1, 0);
		(*dd_mtx)->mtx_ref_cnt = 0;
		(*dd_mtx)->init_done = TRUE;
		for (i = 0; i < FMD_MAX_APPS; i++) {
			(*dd_mtx)->dd_ev[i].in_use = 0;
			(*dd_mtx)->dd_ev[i].proc = 0;
			(*dd_mtx)->dd_ev[i].waiting = 0;
			sem_init(&(*dd_mtx)->dd_ev[i].dd_event, 1, 0);
		}
		sem_post(&(*dd_mtx)->sem);
	}
	(*dd_mtx)->mtx_ref_cnt++;

	return 0;
fail:
	return -1;
}

int fmd_dd_mtx_open(char *dd_mtx_fn, int *dd_mtx_fd,
		struct fmd_dd_mtx **dd_mtx)
{
	return fmd_dd_mtx_open_priv(dd_mtx_fn, dd_mtx_fd, dd_mtx, false);
}

int fmd_dd_init(char *dd_mtx_fn, int *dd_mtx_fd, struct fmd_dd_mtx **dd_mtx,
		char *dd_fn, int *dd_fd, struct fmd_dd **dd)
{
	int rc;

	rc = fmd_dd_mtx_open_priv(dd_mtx_fn, dd_mtx_fd, dd_mtx, true);
	if (rc) {
		goto fail;
	}
	rc = fmd_dd_open_rw(dd_fn, dd_fd, dd, *dd_mtx);
	if (rc) {
		goto fail;
	}

	return 0;
fail:
	return -1;
}

void fmd_dd_cleanup(char *dd_mtx_fn, int *dd_mtx_fd,
			struct fmd_dd_mtx **dd_mtx_p,
			char *dd_fn, int *dd_fd, struct fmd_dd **dd_p,
			int dd_rw)
{
	struct fmd_dd_mtx *dd_mtx = (NULL == dd_mtx_p)?NULL:*dd_mtx_p;
	struct fmd_dd *dd = (NULL == dd_p)?NULL:*dd_p;

	//@sonar:off - Collapsible "if" statements should be merged
	if ((NULL != dd) && (NULL != dd_mtx)) {
		if (dd->chg_idx && dd_mtx->dd_ref_cnt) {
			if (!--dd_mtx->dd_ref_cnt) {
				if (dd_rw)
					dd->chg_idx = 0;
				shm_unlink(dd_fn);
			}
		}

		if (*dd_fd) {
			close(*dd_fd);
			*dd_fd = 0;
		}
		*dd_p = NULL;
	}

	if (NULL != dd_mtx) {
		if (dd_mtx->init_done && dd_mtx->mtx_ref_cnt) {
			if (!--dd_mtx->mtx_ref_cnt) {
				sem_destroy(&dd_mtx->sem);
				dd_mtx->init_done = FALSE;
				shm_unlink(dd_mtx_fn);
			}
		}

		if (*dd_mtx_fd) {
			close(*dd_mtx_fd);
			*dd_mtx_fd = 0;
		}
		*dd_mtx_p = NULL;
	}
	//@sonar:on
}
	
uint32_t fmd_dd_get_chg_idx(struct fmd_dd *dd)
{
	if (NULL != dd)
		return dd->chg_idx;

	return 0;
}

/* Note that get_first_dev and get_next_dev will block until
 * enumeration has been completed.
 */

int fmd_dd_atomic_copy(struct fmd_dd *dd, struct fmd_dd_mtx *dd_mtx,
		uint32_t *num_devs, struct fmd_dd_dev_info *devs,
		uint32_t max_devs)
{
	uint32_t idx;

	if (sem_wait(&dd_mtx->sem)) {
		return -1;
	}

	*num_devs = dd->num_devs;
	if (*num_devs > max_devs) {
		*num_devs = max_devs;
	}

	for (idx = 0; idx < *num_devs; idx++) {
		devs[idx] = dd->devs[idx];
	}

	if (sem_post(&dd_mtx->sem)) {
		return -1;
	}
	return *num_devs;
}

int fmd_dd_atomic_copy_ticks(struct fmd_dd *dd, struct fmd_dd_mtx *dd_mtx,
		struct fmd_dd_ticks *ticks)
{
	if (sem_wait(&dd_mtx->sem)) {
		return -1;
	}

	ticks->chg_idx = dd->chg_idx;
	ticks->chg_time = dd->chg_time;

	if (sem_post(&dd_mtx->sem)) {
		return -1;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif

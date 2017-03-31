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
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <regex.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>

#include <linux/rio_mport_cdev.h>
#include "rio_standard.h"
#include "rio_ecosystem.h"
#include "rio_route.h"

#include "RapidIO_Device_Access_Routines_API.h"
#include "RapidIO_Utilities_API.h"

#include "string_util.h"
#include "libcli.h"
#include "libtime_utils.h"
#include "riocp_pe.h"
#include "fmd_dd.h"
#include "fmd_dd_priv.h"
#include "fmd_app_msg.h"
#include "liblist.h"
#include "liblog.h"
#include "ct.h"
#include "did.h"
#include "cfg.h"
#include "fmd_cfg_cli.h"
#include "fmd_state.h"
#include <rapidio_mport_mgmt.h>
#include "DSF_DB_Private.h"
#include "fmd_app.h"
#include "fmd_cli.h"
#include "fmd_dev_rw_cli.h"
#include "fmd_sc_cli.h"
#include "fmd_dev_conf_cli.h"
#include "fmd_master.h"
#include "fmd_net.h"
#include "fmd_opts.h"
#include "libfmdd.h"
#include "pe_mpdrv_private.h"
#include "RapidIO_Routing_Table_API.h"
#include "Tsi721.h"
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

riocp_pe_handle mport_pe;
DAR_DEV_INFO_t *dev_h = NULL;

struct fmd_opt_vals *opts = NULL;
struct fmd_state *fmd = NULL;

void custom_quit(struct cli_env *env)
{
	(void)env;

	fmd_dd_cleanup(fmd->dd_mtx_fn, &fmd->dd_mtx_fd, &fmd->dd_mtx,
			fmd->dd_fn, &fmd->dd_fd, &fmd->dd, fmd->fmd_rw);
	if (app_st.fd > 0) {
		close(app_st.fd);
		unlink(app_st.addr.sun_path);
		remove(app_st.addr.sun_path);
		app_st.fd = -1;
	}
	exit(EXIT_SUCCESS);
}

void sig_handler(int signo)
{
	INFO("\nRx Signal %x\n", signo);
	if ((signo == SIGINT) || (signo == SIGHUP) || (signo == SIGTERM)) {
		custom_quit(NULL);
		exit(EXIT_SUCCESS);
		kill(getpid(), SIGKILL);
	}
}

pthread_t poll_thread, console_thread;

sem_t cons_owner;

void *poll_loop(void *poll_interval)
{
	int wait_time = ((int *)(poll_interval))[0];
	int console = ((int*)(poll_interval))[1];
	char my_name[16] = {0};
	free(poll_interval);

	INFO("RIO_DEMON: Poll interval %d seconds\n", wait_time);
	sem_post(&cons_owner);

	memset(my_name, 0, 16);
	snprintf(my_name, 15, "FMD_DD_POLL_%02d", wait_time);
	pthread_setname_np(poll_thread, my_name);

	pthread_detach(poll_thread);

	while (TRUE) {
		fmd_dd_incr_chg_idx(fmd->dd, 1);
		sleep(wait_time);
		if (!console)
			INFO("\nTick!");
	}
	return NULL;
}

void spawn_threads(struct fmd_opt_vals *opts)
{
	int poll_ret, cli_ret, cons_ret;
	int *pass_poll_interval;
	int ret;
	struct remote_login_parms *rlp;

	rlp = (struct remote_login_parms *)malloc(
			sizeof(struct remote_login_parms));
	if (NULL == rlp) {
		printf("\nCould not allocate memory for login parameters\n");
		exit(EXIT_FAILURE);
	}

	sem_init(&cons_owner, 0, 0);
	pass_poll_interval = (int *)(calloc(2, sizeof(int)));
	if (NULL == pass_poll_interval) {
		free(rlp);
		CRIT(MALLOC_FAIL);
		exit(EXIT_FAILURE);
	}

	pass_poll_interval[0] = opts->mast_interval;
	pass_poll_interval[1] = opts->run_cons;

	cli_init_base(custom_quit);
	bind_dd_cmds(&fmd->dd, &fmd->dd_mtx, fmd->dd_fn, fmd->dd_mtx_fn);
	liblog_bind_cli_cmds();
	// fmd_bind_dbg_cmds();
	fmd_bind_mgmt_dbg_cmds();
	fmd_bind_dev_rw_cmds();
	fmd_bind_dev_sc_cmds();
	fmd_bind_dev_conf_cmds();

	/* Create independent threads each of which will execute function */
	poll_ret = pthread_create(&poll_thread, NULL, poll_loop,
			(void*)(pass_poll_interval));

	//@sonar:off - c:S3584 Allocated memory not released
	// pass_poll_interval is always deallocated by poll_thread/poll_loop
	// if it is successfully launched.  Disable the warning.
	// Remote_login_parms is always freed either by this thread in the
	// event of error, or by the remote login procedure.
	if (poll_ret) {
		free(pass_poll_interval);
		free(rlp);
		CRIT(THREAD_FAIL, poll_ret);
		exit(EXIT_FAILURE);
	}

	rlp->portno = fmd->opts->cli_port_num;
	SAFE_STRNCPY(rlp->thr_name, "FMD_RLOGIN", sizeof(rlp->thr_name));
	rlp->status = &fmd->rlogin_alive;
	*rlp->status = 0;

	cli_ret = pthread_create(&remote_login_thread, NULL, remote_login,
			(void*)(rlp));
	if (cli_ret) {
		free(rlp);
		CRIT(THREAD_FAIL, cli_ret);
		exit(EXIT_FAILURE);
	}

	if (opts->run_cons) {
		struct cli_env t_env;

		init_cli_env(&t_env);
		splashScreen(&t_env, (char *)"FMD  Command Line Interface");
		cons_ret = pthread_create(&console_thread, NULL, console,
				(void *)((char *)"FMD > "));
		if (cons_ret) {
			CRIT(THREAD_FAIL, cli_ret);
			exit(EXIT_FAILURE);
		}
	}
	//@sonar:on

	ret = start_fmd_app_handler(opts->app_port_num, 50, fmd->dd_fn,
			fmd->dd_mtx_fn);
	if (ret) {
		CRIT(THREAD_FAIL, ret);
		exit(EXIT_FAILURE);
	}

	ret = start_peer_mgmt(opts->mast_cm_port, 0, opts->mast_did,
			opts->mast_mode);
	if (ret) {
		CRIT(THREAD_FAIL, ret);
		exit(EXIT_FAILURE);
	}

	// Only enable all endpoints after we've got the peer management
	// threads going...
	//
	// Note that it is possible for nodes to fail between enumeration and
	// enabling.  Don't fail the fabric management daemon if this occurs,
	// keep trying and recover the nodes...

	ret = fmd_enable_all_endpoints(mport_pe);
	if (ret) {
		WARN("fmd_enable_all_endpoints rc: %d\n", ret);
	}
}

// cleanup the /sys/bus/rapidio/devices directory
int delete_sysfs_devices(riocp_pe_handle mport_pe, bool delete_all)
{
	DIR *dir;
	struct dirent *entry;
	struct stat st;

	struct l_head_t names_list;
	char *sysfs_name;
	regex_t regex;
	bool regex_allocated = false;

	int rc = 0;
	int tmp;
	struct mpsw_drv_pe_acc_info *p_acc;
	struct mpsw_drv_private_data *p_dat;
	struct rapidio_mport_handle *hnd;
	// int fd;

	p_dat = (struct mpsw_drv_private_data *)mport_pe->private_data;
	if (NULL == p_dat) {
		WARN("Could not access private data\n");
		return 1;
	}

	p_acc = (struct mpsw_drv_pe_acc_info *)p_dat->dev_h.accessInfo;
	if (NULL == p_acc) {
		WARN("Could not access device data\n");
		return 2;
	}

	hnd = (struct rapidio_mport_handle *)p_acc->maint;
	// fd = hnd->fd;

	dir = opendir(FMD_DFLT_DEV_DIR);
	if (NULL == dir) {
		WARN("Could not access %s\n", FMD_DFLT_DEV_DIR);
		return 3;
	}

	// wanted a more complex expression, but couldn't get it to work, so
	// back to basics
	rc = regcomp(&regex, "^[0-9][0-9]:[a-z]:[0-9][0-9][0-9][0-9]$", 0);
	if (rc) {
		return rc;
	}
	regex_allocated = true;

	l_init(&names_list);
	while (NULL != (entry = readdir(dir))) {
		if ((DT_DIR == entry->d_type) || (DT_LNK == entry->d_type)) {
			if ((0 == strcmp(".", entry->d_name))
					|| (0 == strcmp("..", entry->d_name))) {
				continue;
			}

			if (fstatat(dirfd(dir), entry->d_name, &st, 0)) {
				continue;
			}

			//@sonar:off - c:S3584 Allocated memory not released
			// The loop fails if memory was not allocated for
			// sysfs_name.  If memory was allocated, it must remain
			// allocated to track the devices.
			if (S_ISDIR(st.st_mode)) {
				// always delete kernel names (dd:a:dddd)
				if (!regexec(&regex, entry->d_name, 0, NULL,
						0)) {
					tmp = strlen(entry->d_name) + 1;
					sysfs_name = (char *)malloc(tmp);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					SAFE_STRNCPY(sysfs_name, entry->d_name, tmp);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}

				// delete_all: delete all names, asserted
				// on slave nodes and on master node when
				// configured as 'auto'
				if (delete_all) {
					tmp = strlen(entry->d_name) + 1;
					sysfs_name = (char *)malloc(tmp);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					SAFE_STRNCPY(sysfs_name, entry->d_name, tmp);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}

				// regular: only delete auto generated names
				if (!strncmp(entry->d_name, AUTO_NAME_PREFIX,
						strlen(AUTO_NAME_PREFIX))) {
					tmp = strlen(entry->d_name) + 1;
					sysfs_name = (char *)malloc(tmp);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					SAFE_STRNCPY(sysfs_name, entry->d_name, tmp);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}
			}
			//@sonar:on
		}
	}

cleanup:
	if (0 == l_size(&names_list)) {
		goto exit;
	}

	while ((sysfs_name = (char *)l_pop_head(&names_list))) {
		tmp = riomp_mgmt_device_del(hnd, 0, 0, 0,
				(const char *)sysfs_name);
		if (tmp) {
			// retain the original error
			rc = (rc == 0 ? tmp : rc);
			WARN("Failed to delete device %s, err=%d\n", sysfs_name,
					rc);
			// try and delete as many as possible
		}
		free(sysfs_name);
	}

exit:
	// Free memory allocated to the regex
	if (regex_allocated) {
		regfree(&regex);
	}
	closedir(dir);
	return rc;
}

int setup_mport_master(int mport)
{
	ct_t comptag;
	struct cfg_mport_info mp;
	struct cfg_dev cfg_dev;
	did_t did;
	char *name;

	if (cfg_find_mport(mport, &mp)) {
		CRIT("\nRequested mport %d does not exist, exiting\n", mport);
		return 1;
	}

	comptag = mp.ct;

	if (cfg_find_dev_by_ct(comptag, &cfg_dev) && !cfg_auto()) {
		CRIT(
				"\nRequested mport %d device component tag 0x%x does not exist\n.",
				mport, comptag);
		return 1;
	}

	name = (char *)calloc(1, FMD_MAX_NAME + 1);
	if (NULL == name) {
		CRIT(MALLOC_FAIL);
		return 1;
	}

	if ((COMPTAG_UNSET == comptag) && cfg_auto()) {
		if (did_create_from_data(&did, mp.devids[CFG_DEV08].did_val,
				dev08_sz)) {
			CRIT("\nCannot create dev08 did 0x%d, exiting...\n");
			return 1;
		}
		if (ct_create_from_did(&comptag, did)) {
			CRIT("\nMaster port DID duplicated...\n");
			return 1;
		}
		snprintf(name, FMD_MAX_NAME + 1, "MPORT%d", mport);
		name[sizeof(name) - 1] = '\0';
	} else {
		SAFE_STRNCPY(name, cfg_dev.name, FMD_MAX_NAME + 1);
	}

	if (riocp_pe_create_host_handle(&mport_pe, mport, 0, &comptag, name)) {
		CRIT("Cannot create host handle mport %d, exiting...", mport);
		riocp_pe_destroy_handle(&mport_pe);
		free(name);
		return 1;
	}

	free(name);
	delete_sysfs_devices(mport_pe, true);

	return fmd_traverse_network(mport_pe, &cfg_dev);
}

int slave_get_ct_and_name(int mport, ct_t *comptag, char *dev_name)
{
	const struct timespec delay = {0, 1000 * 1000}; // 1000 microseconds

	uint32_t mp_num = 0;
	struct cfg_mport_info mp;
	struct cfg_dev cfg_dev;
	struct mport_regs regs;
	bool check;

	if (!cfg_find_mport(mport, &mp)) {
		mp_num = mp.num;
		*comptag = mp.ct;
		if (!cfg_find_dev_by_ct(*comptag, &cfg_dev)) {
			SAFE_STRNCPY(dev_name, cfg_dev.name, FMD_MAX_DEV_FN);
			return 0;
		}
	}

	while (!riocp_get_mport_regs(mp_num, &regs)) {
		check = !(regs.disc & RIO_SP_GEN_CTL_DISC);
		check |= !(regs.disc & RIO_SP_GEN_CTL_MAST_EN);
		check |= !(regs.p_err_stat & RIO_SPX_ERR_STAT_OK);
		check |= !(regs.p_ctl1 & RIO_SPX_CTL_INP_EN);
		check |= !(regs.p_ctl1 & RIO_SPX_CTL_OTP_EN);
		if (check) {
			time_sleep(&delay);
			continue;
		}
		*comptag = regs.comptag;
		memset(dev_name, 0, FMD_MAX_DEV_FN);
		snprintf(dev_name, FMD_MAX_DEV_FN, "LOCAL_MP%d", mp_num);
		did_from_value(&fmd->opts->mast_did,
				GET_DEV8_FROM_PW_TGT_HW(regs.host_did_reg_val),
				FMD_DEV08);
		fmd->opts->mast_cm_port = regs.scratch_cm_sock;
		return 0;
	}

	return 1;
}

int setup_mport_slave(int mport)
{
	int rc, ret;
	ct_t comptag;
	did_val_t did_val;
	char mast_dev_fn[FMD_MAX_DEV_FN] = {0};
	struct mpsw_drv_private_data *p_dat = NULL;
	struct mpsw_drv_pe_acc_info *acc_p = NULL;
	char dev_name[FMD_MAX_DEV_FN];

	if (slave_get_ct_and_name(mport, &comptag, dev_name)) {
		CRIT("\nComponent tag/device name fetch failed for mport %d\n",
				mport);
		return 1;
	}

	if (riocp_pe_create_agent_handle(&mport_pe, mport, 0, &comptag,
			dev_name)) {
		CRIT("\nCannot create agent handle for mport %d\n", mport);
		return 1;
	}

	// Slave devices only know what the master tells them.
	// Delete all pre-existing sysfs devices.
	delete_sysfs_devices(mport_pe, true);

	ret = riocp_pe_handle_get_private(mport_pe, (void **)&p_dat);
	if (ret) {
		CRIT("\nAgent handle failed for mport %d, exiting...\n", mport);
		return 1;
	}

	acc_p = (struct mpsw_drv_pe_acc_info *)p_dat->dev_h.accessInfo;
	if ((NULL == acc_p) || !acc_p->maint_valid) {
		CRIT("\nAgent handle failed for mport %d, exiting...\n", mport);
		return 1;
	}

	/* Poll to add the FMD master devices until the master
	 * completes network initialization.
	 */
	memset(mast_dev_fn, 0, FMD_MAX_DEV_FN);
	snprintf(mast_dev_fn, FMD_MAX_DEV_FN - 1, "%s%s",
	FMD_DFLT_DEV_DIR, FMD_SLAVE_MASTER_NAME);
	do {
		if (access(mast_dev_fn, F_OK) != -1) {
			rc = 0;
		} else {
			did_val = did_get_value(fmd->opts->mast_did);
			rc = riomp_mgmt_device_add(acc_p->maint, did_val,
					HC_MP, did_val, FMD_SLAVE_MASTER_NAME);
		}
		if (rc) {
			CRIT("\nFMD Master inaccessible, wait & try again\n");
			sleep(5);
		}
	} while (EIO == rc);
	return rc;
}

void setup_mport(struct fmd_state *fmd)
{
	int rc = 1;
	int mport = 0;
	uint32_t dsf_rc;

	dsf_rc = RIO_bind_procs(SRIO_API_ReadRegFunc, SRIO_API_WriteRegFunc,
			SRIO_API_DelayFunc);
	if (dsf_rc) {
		CRIT(SOFTWARE_FAIL);
		goto fail;
	}

	fmd->mp_h = &mport_pe;

	if (fmd->opts->mast_mode) {
		rc = setup_mport_master(mport);
	} else {
		rc = setup_mport_slave(mport);
	}

fail:
	if (rc) {
		CRIT("\nNetwork initialization failed...\n");
		riocp_pe_destroy_handle(&mport_pe);
	}
}

int fmd_dd_update(riocp_pe_handle mp_h, struct fmd_dd *dd,
		struct fmd_dd_mtx *dd_mtx)
{
	did_t did;

	if (NULL == mp_h) {
		WARN("\nMaster port is NULL, device directory not updated\n");
		goto fail;
	}

	did_from_value(&did, mp_h->did_reg_val, FMD_DEV08);
	add_device_to_dd(mp_h->comptag, did, mp_h->hopcount,
			1, FMDD_FLAG_OK_MP, (char *)mp_h->sysfs_name);

	fmd_dd_incr_chg_idx(dd, 1);
	sem_post(&dd_mtx->sem);
	return 0;

fail:
	return 1;
}

int main(int argc, char *argv[])
{
	char log_file_name[FMD_MAX_LOG_FILE_NAME];
	char *cfg_dd_mtx_fn;
	char *cfg_dd_fn;

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGPIPE, sig_handler);

	opts = fmd_parse_options(argc, argv);
	if (NULL == opts) {
		goto fail;
	}

	snprintf(log_file_name, FMD_MAX_LOG_FILE_NAME,
			FMD_LOG_FILE_FMT, opts->app_port_num);
	rdma_log_init(log_file_name, 1);

	g_level = opts->log_level;
	if (opts->init_and_quit && opts->print_help) {
		goto fail;
	}

	fmd = (struct fmd_state *)calloc(1, sizeof(struct fmd_state));
	if (NULL == fmd) {
		goto fail;
	}

	fmd->opts = opts;
	fmd->fmd_rw = 1;

	// Parse the configuration file, continue no matter what errors are found.
	cfg_dd_mtx_fn = NULL;
	cfg_dd_fn = NULL;
	cfg_parse_file(opts->fmd_cfg, &cfg_dd_mtx_fn, &cfg_dd_fn,
			&fmd->opts->mast_did, &fmd->opts->mast_cm_port,
			&fmd->opts->mast_mode);

	// If the user specified the dd_mtx_fn or dd_fn name on the command
	// line then use those values (opts->dd_mtx_fn/dd_fn).
	//
	// If the user didn't specify a value and the config file does,
	// then use those valuee (cfg_dd_mtx_fn, cfg_dd_fn)
	//
	// else use default values
	if (NULL != opts->dd_mtx_fn) {
		if (update_string(&fmd->dd_mtx_fn, opts->dd_mtx_fn,
				strlen(opts->dd_mtx_fn))) {
			goto fail;
		}
	} else {
		if (NULL != cfg_dd_mtx_fn) {
			if (update_string(&fmd->dd_mtx_fn, cfg_dd_mtx_fn,
					strlen(cfg_dd_mtx_fn))) {
				goto fail;
			}
		} else {
			if (update_string(&fmd->dd_mtx_fn,
					(char *)FMD_DFLT_DD_MTX_FN,
					strlen((char *)FMD_DFLT_DD_MTX_FN))) {
				goto fail;
			}
		}
	}

	if (NULL != opts->dd_fn) {
		if (update_string(&fmd->dd_fn, opts->dd_fn,
				strlen(opts->dd_fn))) {
			goto fail;
		}
	} else {
		if (NULL != cfg_dd_fn) {
			if (update_string(&fmd->dd_fn, cfg_dd_fn,
					strlen(cfg_dd_fn))) {
				goto fail;
			}
		} else {
			if (update_string(&fmd->dd_fn,
					(char *)FMD_DFLT_DD_FN,
					strlen((char *)FMD_DFLT_DD_FN))) {
				goto fail;
			}
		}
	}

	if (fmd_dd_init(fmd->dd_mtx_fn, &fmd->dd_mtx_fd, &fmd->dd_mtx,
			fmd->dd_fn, &fmd->dd_fd, &fmd->dd)) {
		goto dd_cleanup;
	}

	setup_mport(fmd);

	if (!fmd->opts->simple_init
			&& fmd_dd_update(*fmd->mp_h, fmd->dd, fmd->dd_mtx)) {
		goto dd_cleanup;
	}

	if (fmd->opts->init_and_quit) {
		goto dd_cleanup;
	}
	spawn_threads(fmd->opts);

	if (fmd->opts->run_cons) {
		pthread_join(console_thread, NULL);
	}

	while (1) {
		sched_yield();
	}

dd_cleanup:
	fmd_dd_cleanup(fmd->dd_mtx_fn, &fmd->dd_mtx_fd,
			&fmd->dd_mtx, fmd->dd_fn, &fmd->dd_fd, &fmd->dd,
			fmd->fmd_rw);

fail:
	exit(EXIT_SUCCESS);
}

#ifdef __cplusplus
}
#endif

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

#include <stdint.h>
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
#include "DAR_DevDriver.h"
#include "rio_standard.h"
#include "rio_ecosystem.h"

#include "IDT_DSF_DB_Private.h"
#include "DAR_Utilities.h"

#include "string_util.h"
#include "rio_ecosystem.h"
#include "libcli.h"
#include "riocp_pe.h"
#include "DAR_DevDriver.h"
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
#include "IDT_Routing_Table_Config_API.h"
#include "IDT_Tsi721.h"
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
	(void) env;

	fmd_dd_cleanup(fmd->dd_mtx_fn, &fmd->dd_mtx_fd, &fmd->dd_mtx,
			fmd->dd_fn, &fmd->dd_fd, &fmd->dd, fmd->fmd_rw);
	exit(EXIT_SUCCESS);
};

void sig_handler(int signo)
{
	INFO("\nRx Signal %x\n", signo);
	if ((signo == SIGINT) || (signo == SIGHUP) || (signo == SIGTERM)) {
		custom_quit(NULL);
		exit(EXIT_SUCCESS);
		kill(getpid(), SIGKILL);
	};
};

pthread_t poll_thread, console_thread;

sem_t cons_owner;

void *poll_loop( void *poll_interval ) 
{
	int wait_time = ((int *)(poll_interval))[0];
	int console = ((int*)(poll_interval))[1];
        char my_name[16] = {0};
	free(poll_interval);

	INFO("RIO_DEMON: Poll interval %d seconds\n", wait_time);
	sem_post(&cons_owner);

        memset(my_name, 0, 16);
        snprintf(my_name, 15, "FMD_DD_POLL_%02d",wait_time);
        pthread_setname_np(poll_thread, my_name);

        pthread_detach(poll_thread);

	while(TRUE) {
		fmd_dd_incr_chg_idx(fmd->dd, 1);
		sleep(wait_time);
		if (!console)
			INFO("\nTick!");
	}
	return poll_interval;
}

void spawn_threads(struct fmd_opt_vals *cfg)
{
	int  poll_ret, cli_ret, cons_ret;
	int *pass_poll_interval;
	int *pass_cons_ret;
	int ret;
	struct remote_login_parms *rlp;

	rlp = (struct remote_login_parms *)
					malloc(sizeof(struct remote_login_parms));
	if (NULL == rlp) {
		printf("\nCould not allocate memory for login parameters\n");
		exit(EXIT_FAILURE);
	}

	sem_init(&cons_owner, 0, 0);
	pass_poll_interval = (int *)(calloc(2, sizeof(int)));
	pass_cons_ret = (int *)(calloc(1, sizeof(int))); /// \todo MEMLEAK
	if (!pass_poll_interval || !pass_cons_ret) {
		free(pass_cons_ret);
		free(pass_poll_interval);

		CRIT(MALLOC_FAIL);
		exit(EXIT_FAILURE);
	}

	pass_poll_interval[0] = cfg->mast_interval;
	pass_poll_interval[1] = cfg->run_cons;
	*pass_cons_ret = cfg->run_cons;

	cli_init_base(custom_quit);
	bind_dd_cmds(fmd->dd, fmd->dd_mtx, fmd->dd_fn, fmd->dd_mtx_fn);
	liblog_bind_cli_cmds();
	// fmd_bind_dbg_cmds();
	fmd_bind_mgmt_dbg_cmds();
	fmd_bind_dev_rw_cmds();
	fmd_bind_dev_sc_cmds();
	fmd_bind_dev_conf_cmds();

	/* Create independent threads each of which will execute function */
	poll_ret = pthread_create( &poll_thread, NULL, poll_loop, 
				(void*)(pass_poll_interval));
	if(poll_ret) {
		CRIT(THREAD_FAIL, poll_ret);
		exit(EXIT_FAILURE);
	}
 
	rlp->portno = fmd->opts->cli_port_num;
	SAFE_STRNCPY(rlp->thr_name, "FMD_RLOGIN", sizeof(rlp->thr_name));
	rlp->status = &fmd->rlogin_alive;
	*rlp->status = 0;

	cli_ret = pthread_create( &remote_login_thread, NULL, remote_login,
								(void*)(rlp));
	if(cli_ret) {
		CRIT(THREAD_FAIL, cli_ret);
		exit(EXIT_FAILURE);
	}

	if (cfg->run_cons) {
		struct cli_env t_env;

		init_cli_env(&t_env);
		splashScreen(&t_env, (char *)"FMD  Command Line Interface");
		cons_ret = pthread_create( &console_thread, NULL, 
			console, (void *)((char *)"FMD > "));
		if(cons_ret) {
			CRIT(THREAD_FAIL, cli_ret);
			exit(EXIT_FAILURE);
		}
	};
 
	ret = start_fmd_app_handler(cfg->app_port_num, 50,
					cfg->dd_fn, cfg->dd_mtx_fn); 
	if (ret) {
		CRIT(THREAD_FAIL, ret);
		exit(EXIT_FAILURE);
	}
	ret = start_peer_mgmt(cfg->mast_cm_port, 0, cfg->mast_devid, 
			cfg->mast_mode);
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
	};
}

// cleanup the /sys/bus/rapidio/devices directory
int delete_sysfs_devices(riocp_pe_handle mport_pe, bool auto_config)
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
	if(NULL == p_dat) {
		WARN("Could not access private data\n");
		return 1;
	}

	p_acc = (struct mpsw_drv_pe_acc_info *)p_dat->dev_h.accessInfo;
	if(NULL == p_acc) {
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
	if(rc) {
		return rc;
	}
	regex_allocated = true;

	l_init(&names_list);
	while(NULL != (entry = readdir(dir))) {
		if ((DT_DIR == entry->d_type) || (DT_LNK == entry->d_type)) {
			if ((0 == strcmp(".", entry->d_name))
					|| (0 == strcmp("..", entry->d_name))) {
				continue;
			}

			if (fstatat(dirfd(dir), entry->d_name, &st, 0)) {
				continue;
			}

			if(S_ISDIR(st.st_mode)) {
				// always delete kernel names (dd:a:dddd)
				if(!regexec(&regex, entry->d_name, 0, NULL, 0)) {
					sysfs_name =(char *)malloc(
						strlen(entry->d_name) + 1);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					strcpy(sysfs_name, entry->d_name);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}

				// auto: delete all names
				if(auto_config) {
					sysfs_name = (char *)malloc(
						strlen(entry->d_name) + 1);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					strcpy(sysfs_name, entry->d_name);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}

				// regular: only delete auto generated names
				if (!strncmp(entry->d_name, AUTO_NAME_PREFIX,
						strlen(AUTO_NAME_PREFIX))) {
					sysfs_name = (char *)malloc(
						strlen(entry->d_name) + 1);
					if (NULL == sysfs_name) {
						CRIT(MALLOC_FAIL);
						rc = -ENOMEM;
						goto cleanup;
					}
					strcpy(sysfs_name, entry->d_name);
					l_push_tail(&names_list,
							(void *)sysfs_name);
					continue;
				}
			}
		}
	}

cleanup:
	if (0 == l_size(&names_list)) {
		goto exit;
	}

	while ((sysfs_name = (char *) l_pop_head(&names_list))) {
		tmp = riomp_mgmt_device_del(hnd, 0, 0, 0,
						(const char *)sysfs_name);
		if(tmp) {
			// retain the original error
			rc = (rc == 0 ? tmp : rc);
			WARN("Failed to delete device %s, err=%d\n",
					sysfs_name, rc);
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
	/* TODO: Change this to support other master ports etc... */
	ct_t comptag;
	struct cfg_mport_info mp;
	struct cfg_dev cfg_dev;
	did_t did;
	char *name;

	if (cfg_find_mport(mport, &mp)) {
		CRIT("\nRequested mport %d does not exist, exiting\n", mport);
		return 1;
	};

	comptag = mp.ct;

	if (cfg_find_dev_by_ct(comptag, &cfg_dev) && !cfg_auto()) {
	CRIT("\nRequested mport %d device component tag 0x%x does not exist\n.",
			mport, comptag);
		return 1;
	};

	name = (char *)cfg_dev.name;

	if ((COMPTAG_UNSET == comptag) && cfg_auto()) {
		if (did_create_from_data(&did, mp.devids[CFG_DEV08].devid,
							dev08_sz)) {
			CRIT("\nCannot create dev08 did 0x%d, exiting...\n");
			return 1;
		}
		if (ct_create_from_did(&comptag, did)) {
			CRIT("\nMaster port DID duplicated...\n");
			return 1;
		}
		name = (char *)calloc(1,40);
		if (NULL == name) {
			CRIT(MALLOC_FAIL);
			return 1;
		} else {
			snprintf(name, 39, "MPORT%d", mport);
		}
	}

	if (riocp_pe_create_host_handle(&mport_pe, mport, 0, &pe_mpsw_rw_driver,
			&comptag, name)) {
		CRIT("Cannot create host handle mport %d, exiting...",
			mport);
		riocp_pe_destroy_handle(&mport_pe);
		return 1;
	};

	delete_sysfs_devices(mport_pe, cfg_auto());

	return fmd_traverse_network(mport_pe, &cfg_dev);
};

int slave_get_ct_and_name(int mport, uint32_t *comptag, char *dev_name)
{
	uint32_t mp_num = 0;
	struct cfg_mport_info mp;
	struct cfg_dev cfg_dev;
	struct mport_regs regs;

	if (!cfg_find_mport(mport, &mp)) {
		mp_num = mp.num;
		*comptag = mp.ct;
		if (!cfg_find_dev_by_ct(*comptag, &cfg_dev)) {
			SAFE_STRNCPY(dev_name, cfg_dev.name, FMD_MAX_DEV_FN);
			return 0;
		};
	};

	while (!riocp_get_mport_regs(mp_num, &regs)) {
		if (!(regs.disc & RIO_SP_GEN_CTL_DISC) ||
				!(regs.disc & RIO_SP_GEN_CTL_MAST_EN) ||
				!(regs.p_err_stat & RIO_SPX_ERR_STAT_OK) ||
				!(regs.p_ctl1 & RIO_SPX_CTL_INP_EN) ||
				!(regs.p_ctl1 & RIO_SPX_CTL_OTP_EN)) {
			usleep(1000);
			continue;
		};
		*comptag = regs.comptag;
		memset(dev_name, 0, FMD_MAX_DEV_FN);
		snprintf(dev_name, FMD_MAX_DEV_FN, "LOCAL_MP%d", mp_num);
		fmd->opts->mast_devid = GET_DEV8_FROM_PW_TGT_HW(
							regs.host_destID);
		fmd->opts->mast_cm_port = regs.scratch_cm_sock;
		return 0;
	};

	return 1;
};

int setup_mport_slave(int mport)
{
	int rc, ret;
	ct_t comptag;
	char mast_dev_fn[FMD_MAX_DEV_FN] = {0};
	struct mpsw_drv_private_data *p_dat = NULL;
	struct mpsw_drv_pe_acc_info *acc_p = NULL;
	char dev_name[FMD_MAX_DEV_FN];

	// TODO: Ideally, the devname used here is updated based on the
	//       hello response.  The devname for the MPORT is only used
	//       in the dd and libriocp_pe, it is not used by sysfs.
	if (slave_get_ct_and_name(mport, &comptag, dev_name)) {
		CRIT("\nComponent tag/device name fetch failed for mport %d\n",
								mport);
		return 1;
	};

	if (riocp_pe_create_agent_handle(&mport_pe, mport, 0,
			&pe_mpsw_rw_driver, &comptag, dev_name)) {
		CRIT("\nCannot create agent handle for mport %d\n", mport);
		return 1;
	};

	delete_sysfs_devices(mport_pe, cfg_auto());

	ret = riocp_pe_handle_get_private(mport_pe, (void **)&p_dat);
	if (ret) {
		CRIT("\nAgent handle failed for mport %d, exiting...\n", mport);
		return 1;
	};

	acc_p = (struct mpsw_drv_pe_acc_info *)p_dat->dev_h.accessInfo;
	if ((NULL == acc_p) || !acc_p->maint_valid) {
		CRIT("\nAgent handle failed for mport %d, exiting...\n", mport);
		return 1;
	};

	/* Poll to add the FMD master devices until the master
	* completes network initialization.
	*/
	memset(mast_dev_fn, 0, FMD_MAX_DEV_FN);
	snprintf(mast_dev_fn, FMD_MAX_DEV_FN-1, "%s%s",
                        FMD_DFLT_DEV_DIR, FMD_SLAVE_MASTER_NAME);
	do {
		if (access(mast_dev_fn, F_OK) != -1) {
                        rc = 0;
                } else {
			rc = riomp_mgmt_device_add(acc_p->maint,
				(uint16_t)fmd->opts->mast_devid,
				HC_MP, fmd->opts->mast_devid,
				FMD_SLAVE_MASTER_NAME);
		};
		if (rc) {
			CRIT("\nFMD Master inaccessible, wait & try again\n");
			sleep(5);
		};
	} while (EIO == rc);
	return rc;
};

void setup_mport(struct fmd_state *fmd)
{
	int rc = 1;
	int mport = 0;
	uint32_t dsf_rc;

        dsf_rc = IDT_DSF_bind_DAR_routines(SRIO_API_ReadRegFunc,
                                SRIO_API_WriteRegFunc,
                                SRIO_API_DelayFunc);
        if (dsf_rc) {
                CRIT(SOFTWARE_FAIL);
		goto fail;
        };

        if (riocp_bind_driver(&pe_mpsw_driver)) {
                CRIT(SOFTWARE_FAIL);
		goto fail;
        };

	fmd->mp_h = &mport_pe;

	if (fmd->opts->mast_mode)
		rc = setup_mport_master(mport);
	else
		rc = setup_mport_slave(mport);
fail:
	if (rc) {
		CRIT("\nNetwork initialization failed...\n");
		riocp_pe_destroy_handle(&mport_pe);
	};
}

int fmd_dd_update(riocp_pe_handle mp_h, struct fmd_dd *dd,
			struct fmd_dd_mtx *dd_mtx)
{
        if (NULL == mp_h) {
                WARN("\nMaster port is NULL, device directory not updated\n");
                goto fail;
        };

	add_device_to_dd(mp_h->comptag, mp_h->destid, FMD_DEV08,
			mp_h->hopcount, 1, FMDD_FLAG_OK_MP,
			(char *)mp_h->sysfs_name);

        fmd_dd_incr_chg_idx(dd, 1);
        sem_post(&dd_mtx->sem);
	return 0;
fail:
        return 1;
};

int main(int argc, char *argv[])
{

	signal(SIGINT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGPIPE, sig_handler);

	rdma_log_init("fmd.log", 1);

	opts = fmd_parse_options(argc, argv);
	if (NULL == opts) {
		goto fail;
	}

	g_level = opts->log_level;
	if ((opts->init_and_quit) && (opts->print_help)) {
		goto fail;
	}

	fmd = (struct fmd_state *)calloc(1, sizeof(struct fmd_state));
	if (NULL == fmd) {
		goto fail;
	}

	fmd->opts = opts;
	fmd->fmd_rw = 1;
	fmd->dd_mtx_fn = fmd->opts->dd_mtx_fn;
	fmd->dd_fn = fmd->opts->dd_fn;

	// Parse the configuration file, continue no matter what
	// errors are found.
	cfg_parse_file(opts->fmd_cfg, &fmd->dd_mtx_fn, &fmd->dd_fn,
			&fmd->opts->mast_devid, &fmd->opts->mast_cm_port,
			&fmd->opts->mast_mode);

	if (fmd_dd_init(opts->dd_mtx_fn, &fmd->dd_mtx_fd, &fmd->dd_mtx,
			opts->dd_fn, &fmd->dd_fd, &fmd->dd)) {
		goto dd_cleanup;
	}

	setup_mport(fmd);

	if (!fmd->opts->simple_init) {
		if (fmd_dd_update(*fmd->mp_h, fmd->dd, fmd->dd_mtx)) {
			goto dd_cleanup;
		}
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
	fmd_dd_cleanup(opts->dd_mtx_fn, &fmd->dd_mtx_fd, &fmd->dd_mtx,
			opts->dd_fn, &fmd->dd_fd, &fmd->dd, fmd->fmd_rw);
fail:
	exit(EXIT_SUCCESS);
}

#ifdef __cplusplus
}
#endif

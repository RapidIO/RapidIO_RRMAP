/*
 ****************************************************************************
 Copyright (c) 2016, Integrated Device Technology Inc.
 Copyright (c) 2016, RapidIO Trade Association
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
#include <string.h>
#include <errno.h>

#include <linux/rio_mport_cdev.h>
#include "fmd_net.h"
#include "did.h"
#include "ct.h"
#include "rio_ecosystem.h"
#include "riocp_pe.h"
#include "riocp_pe_internal.h"
#include "cfg.h"
#include "liblist.h"
#include "liblog.h"
#include "Tsi721.h"
#include "fmd_state.h"
#include "fmd_errmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmd_no_cfg {
	riocp_pe_handle curr_pe;
	rio_port_t pnum;
};

int fmd_traverse_network(riocp_pe_handle mport_pe, struct cfg_dev *c_dev)
{
	return fmd_traverse_network_from_pe_port(mport_pe, RIO_ANY_PORT, c_dev);
}

int fmd_traverse_network_from_pe_port(riocp_pe_handle pe, rio_port_t port_num,
		struct cfg_dev *c_dev)
{
	struct l_head_t sw_list;

	riocp_pe_handle new_pe, curr_pe;
	rio_port_t port_st, port_cnt, pnum;
	int conn_pt, rc;
	ct_t comptag;
	struct cfg_dev curr_dev, conn_dev;

	struct l_head_t no_cfg_list;
	struct fmd_no_cfg *no_cfg;

	l_init(&sw_list);
	l_init(&no_cfg_list);

	/* Enumerated device connected to master port */
	curr_pe = pe;
	curr_dev = *c_dev;

	if (RIO_ANY_PORT == port_num) {
		port_st = 0;
		port_cnt = RIOCP_PE_PORT_COUNT(curr_pe->cap);
	} else {
		port_st = port_num;
		port_cnt = port_num + 1;
	}

	// enumerate devices in the configuration files
	do {
		for (pnum = port_st; pnum < port_cnt; pnum++) {
			new_pe = NULL;

			if (cfg_get_conn_dev(curr_pe->comptag, pnum, &conn_dev,
					&conn_pt)) {
				HIGH("PE 0x%0x Port %d NO CONFIG\n",
						curr_pe->comptag, pnum);

				no_cfg = (struct fmd_no_cfg *)malloc(
						sizeof(struct fmd_no_cfg));
				if (NULL == no_cfg) {
					CRIT(MALLOC_FAIL);
					goto fail;
				}

				no_cfg->curr_pe = curr_pe;
				no_cfg->pnum = pnum;
				l_push_tail(&no_cfg_list, (void *)no_cfg);
				//@sonar:off - Dynamically allocated memory should be released
				continue;
				//@sonar:on
			}

			rc = riocp_pe_probe(curr_pe, pnum, &new_pe,
					&conn_dev.ct, (char *)conn_dev.name,
					true);

			if (rc) {
				if ((-ENODEV != rc) && (-EIO != rc)) {
					HIGH("PE 0x%0x Port %d probe failed %d",
							curr_pe->comptag, pnum,
							rc);
					goto fail;
				}
				HIGH("PE 0x%x Port %d NO DEVICE, expected %x\n",
						curr_pe->comptag, pnum,
						conn_dev.ct);
				continue;
			}

			if (NULL == new_pe) {
				HIGH("PE 0x%x Port %d ALREADY CONNECTED\n",
						curr_pe->comptag, pnum);
				continue;
			}

			rc = riocp_pe_get_comptag(new_pe, &comptag);
			if (rc) {
				HIGH("Get new comptag failed, rc %d\n", rc);
				goto fail;
			}

			if (comptag != conn_dev.ct) {
				DBG(
						"Probed ep ct 0x%x != 0x%x config ct port %d\n",
						comptag, conn_dev.ct, pnum);
				goto fail;
			}

			HIGH(
					"PE 0x%x Port %d Connected: DEVICE %s CT 0x%x DID 0x%x\n",
					curr_pe->comptag, pnum,
					new_pe->sysfs_name, new_pe->comptag,
					new_pe->did_reg_val);

			if (RIOCP_PE_IS_SWITCH(new_pe->cap)) {
				void *pe;
				struct l_item_t *li;
				bool found = false;

				pe = l_head(&sw_list, &li);
				while (NULL != pe && !found) {
					if (((struct riocp_pe *)pe)->comptag
							== new_pe->comptag) {
						found = true;
						continue;
					}
					pe = l_next(&li);
				}
				if (!found) {
					HIGH("Adding PE 0x%08x to search\n",
							new_pe->comptag);
					l_push_tail(&sw_list, (void *)new_pe);
				}
			}
		}

		curr_pe = (riocp_pe_handle)l_pop_head(&sw_list);
		if (NULL != curr_pe) {
			rc = cfg_find_dev_by_ct(curr_pe->comptag, &curr_dev);
			if (rc) {
				HIGH("cfg_find_dev_by_ct fail, ct 0x%x rc %d",
						curr_pe->comptag, rc);
				goto fail;
			}
			port_st = 0;
			port_cnt = RIOCP_PE_PORT_COUNT(curr_pe->cap);
			HIGH("Now probing PE CT 0x%08x ports %d to %d\n",
					curr_pe->comptag, port_st, port_cnt);
		}
	} while (NULL != curr_pe);

	/* enumerate devices not in the configuration */
	if (cfg_auto() && l_size(&no_cfg_list)) {
		int dev_number = 1;
		ct_t ct = COMPTAG_UNSET;
		did_t did;
		char sysfs_name[RIO_MAX_DEVNAME_SZ + 1 + 1];
		char sysfs_name_format[strlen(AUTO_NAME_PREFIX) + 3];

		memset(sysfs_name, 0, sizeof(sysfs_name_format));
		snprintf(sysfs_name_format, sizeof(sysfs_name_format), "%s%s",
				AUTO_NAME_PREFIX, "%d");
		while (1) {
			no_cfg = (struct fmd_no_cfg *)l_pop_head(&no_cfg_list);
			if (NULL == no_cfg) {
				if (COMPTAG_UNSET != ct) {
					ct_release(ct, did);
					ct = COMPTAG_UNSET;
				}
				break;
			}

			if (COMPTAG_UNSET == ct) {
				// create a comptag and a device name
				ct_create_all(&ct, &did, dev08_sz);
				memset(sysfs_name, 0, sizeof(sysfs_name));
				snprintf(sysfs_name, RIO_MAX_DEVNAME_SZ,
						sysfs_name_format,
						dev_number++);
			}

			new_pe = NULL;
			curr_pe = no_cfg->curr_pe;
			pnum = no_cfg->pnum;
			free(no_cfg);

			rc = riocp_pe_probe(curr_pe, pnum, &new_pe, &ct,
					sysfs_name, false);
			if (rc) {
				if ((-ENODEV != rc) && (-EIO != rc)) {
					HIGH("PE 0x%0x Port %d probe failed %d",
							curr_pe->comptag, pnum,
							rc);
					ct_release(ct, did);
					goto fail;
				}
				HIGH("PE 0x%x Port %d NO DEVICE, expected %x\n",
						curr_pe->comptag, pnum, ct);
				// note re-use of ct (and sysfs_name)
				continue;
			}

			if (NULL == new_pe) {
				HIGH("PE 0x%x Port %d ALREADY CONNECTED\n",
						curr_pe->comptag, pnum);
				// note re-use of ct (and sysfs_name)
				continue;
			}

			rc = riocp_pe_get_comptag(new_pe, &comptag);
			if (rc) {
				HIGH("Get new comptag failed, rc %d\n", rc);
				ct_release(ct, did);
				goto fail;
			}

			if (comptag == ct) {
				HIGH("PE 0x%x Port %d Connected: DEVICE %s CT 0x%x DID 0x%x\n",
					curr_pe->comptag, pnum,
					new_pe->sysfs_name,
					new_pe->comptag,
					new_pe->did_reg_val);
				// create a new ct (and name) next loop
				ct = COMPTAG_UNSET;

				if (RIOCP_PE_IS_SWITCH(new_pe->cap)) {
					// explore the other ports of the switch
					port_cnt = RIOCP_PE_PORT_COUNT(
							new_pe->cap);
					for (pnum = 0; pnum < port_cnt; pnum++) {
						//@sonar:off - c:S3584 Allocated memory not released
						// The only reason the memory is not released, is
						// that the port will be processed later before the
						// procedure exits.
						no_cfg = (struct fmd_no_cfg *)malloc(
								sizeof(struct fmd_no_cfg));
						if (NULL == no_cfg) {
							CRIT(MALLOC_FAIL);
							goto fail;
						}
						no_cfg->curr_pe = new_pe;
						no_cfg->pnum = pnum;
						l_push_tail(&no_cfg_list,
								(void *)no_cfg);
						//@sonar:on
					}
				}
			} else {
				DBG("Probed ep ct 0x%x != 0x%x config ct port %d\n",
					comptag, ct, pnum);
			}
		}

		if (COMPTAG_UNSET != ct) {
			ct_release(ct, did);
		}
	}

	while ((no_cfg = (struct fmd_no_cfg *)l_pop_head(&no_cfg_list))) {
		free(no_cfg);
	}
	return 0;

fail:
	while ((no_cfg = (struct fmd_no_cfg *)l_pop_head(&no_cfg_list))) {
		free(no_cfg);
	}
	return -1;
}

int fmd_enable_all_endpoints(riocp_pe_handle mp_pe)
{
	riocp_pe_handle *pes = NULL;
	size_t count;
	int rc = -1;
	size_t i;

	if (riocp_mport_get_pe_list(mp_pe, &count, &pes)) {
		return -1;
	}

	for (i = 0; i < count; i++) {
		uint32_t lockval;
		uint32_t cm_sock = fmd->opts->mast_cm_port;

		if (RIOCP_PE_IS_BRIDGE(pes[i]->cap)) {
			riocp_pe_maint_write(pes[i], TSI721_WHITEBOARD,
					cm_sock);
		}
		riocp_pe_maint_read(pes[i], RIO_HOST_LOCK, &lockval);
		if (RIO_HOST_LOCK_UNLOCKED != lockval) {
			riocp_pe_maint_write(pes[i], RIO_HOST_LOCK, lockval);
		}
		if (riocp_enable_pe(pes[i], RIO_ALL_PORTS)) {
			goto cleanup;
		}
	}
	rc = 0;

cleanup:
	if (riocp_mport_free_pe_list(&pes)) {
		goto fail;
	}
	return rc;

fail:
	return -1;
}

#ifdef __cplusplus
}
#endif

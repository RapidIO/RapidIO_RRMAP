/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file handle.c
 * Processing element handle management
 */
#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#include <inttypes.h>

#include <sys/ioctl.h>

#include <rapidio_mport_mgmt.h>

#include "inc/riocp_pe.h"
#include "inc/riocp_pe_internal.h"

#include "tok_parse.h"
#include "rio_ecosystem.h"
#include "rio_standard.h"
#include "handle.h"
#include "comptag.h"
#include "llist.h"
#include "pe.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/** List of created PE handles behind every MPort */
static struct riocp_pe_llist_item riocp_pe_mport_handles;

/**
 * Convert address string e.g: "0,4,10" to address
 */
int riocp_pe_handle_addr_aton(char *addr, uint8_t **address, size_t *address_len)
{
	unsigned int i = 0;
	size_t _address_len = 0;
	static uint8_t _address[255] = {0};
	uint16_t val;
	char *str = NULL;
	char *saveptr1 = NULL;
	char *token = NULL;
	char *tmp = NULL;

	*address_len = 0;

	if (addr == NULL)
		return -EINVAL;

	memset(_address, 0, sizeof(_address));

	tmp = strdup(addr);
	for (_address_len = 0, str = tmp; ; _address_len++, str = NULL) {
		token = strtok_r(str, ",", &saveptr1);
		if (token == NULL)
			break;
	}
	free(tmp);

	if (_address_len > sizeof(_address)/sizeof(*_address))
		return -ENOMEM;

	for (i = 0, str = addr; ; i++, str = NULL) {
		token = strtok_r(str, ",", &saveptr1);
		if (token == NULL)
			break;
		if (tok_parse_ushort(token, &val, 0, UINT8_MAX, 0)) {
			return -EINVAL;
		}
		_address[i] = (uint8_t)(val & UINT8_MAX);
	}

	*address = _address;
	*address_len  = _address_len;

	return 0;
}

/**
 * Convert address to string
 */
const char * riocp_pe_handle_addr_ntoa(uint8_t *address, size_t address_len)
{
	size_t i;
	static char buf[256];
	char *cur = buf;
	char * const end = buf + sizeof(buf);

	if (address == NULL || address_len == 0)
		return "";

	memset(buf, 0, sizeof(buf));

	for (i = 0; i < address_len; i++) {
		if (cur < end)
			cur += snprintf(cur, end - cur, "%" PRIu8, address[i]);
		if (i < address_len - 1) {
			if (cur < end)
				cur += snprintf(cur, end - cur, ",");
		}
	}

	return buf;
}

/**
 * Check handle for NULL and if it exists in riocp_pe_mport_handles or
 *  in the list of handles in one of the mport handles.
 * @param handle The handle to check
 * @retval 0 Handle is valid
 * @retval -EINVAL Handle is NULL
 * @retval -ENOENT Handle not known
 */
int RIOCP_SO_ATTR riocp_pe_handle_check(riocp_pe_handle handle)
{
	struct riocp_pe *mport = NULL;
	struct riocp_pe_llist_item *item = NULL;

	RIOCP_TRACE("Checking handle %p\n", handle);

	if (handle == NULL) {
		RIOCP_ERROR("null handle\n");
		return -EINVAL;
	}

	/* Search if handle is in mport handles list */
	if (riocp_pe_llist_find(&riocp_pe_mport_handles, handle))
		return 0;

	RIOCP_TRACE("Handle not found in mport handles list\n");

	/* Search handle in every mport handles list */
	riocp_pe_llist_foreach(item, &riocp_pe_mport_handles) {
		mport = (struct riocp_pe *)item->data;
		if (mport && mport->minfo) {
			RIOCP_TRACE("Search handle in mport %u\n", mport->minfo->id);
			if (riocp_pe_llist_find(&mport->minfo->handles, handle)) {
				RIOCP_TRACE("Handle found in mport %u\n", mport->minfo->id);
				return 0;
			}
			RIOCP_TRACE("Handle not found in mport %u\n", mport->minfo->id);
		}
	}

	RIOCP_ERROR("invalid handle %p\n", handle);

	return -ENOENT;
}

/**
 * Set private data to riocp_pe_handle
 * @param pe PE to set private data of
 * @param data Data to set
 */
int RIOCP_SO_ATTR riocp_pe_handle_set_private(struct riocp_pe *pe, void *data)
{
	int ret;

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_TRACE("Invalid pe handle\n");
		return -EINVAL;
	}

	pe->private_data = data;
	return 0;
}

/**
 * Get private data of riocp_pe_handle
 * @param pe PE to get private data from
 * @param data Pointer to private data
 */
int RIOCP_SO_ATTR riocp_pe_handle_get_private(struct riocp_pe *pe, void **data)
{
       if (riocp_pe_handle_check(pe)) {
                RIOCP_TRACE("Invalid pe handle\n");
                return -EINVAL;
        }

        if (pe->private_data == NULL) {
                RIOCP_TRACE("Private data output is NULL\n");
                return -EFAULT;
        }

        *data = pe->private_data;
        return 0;
}

/**
 * Set private data to riocp_pe
 * @param pe PE to set private data of
 * @param data Data to set
 */
int RIOCP_SO_ATTR riocp_mport_set_private(struct riocp_pe *pe, void *data)
{
	int ret;

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_TRACE("Invalid pe handle\n");
		return -EINVAL;
	}

	pe->minfo->private_data = data;
	return 0;
}

/**
 * Get private data of riocp_pe
 * @param pe PE to get private data from
 * @param data Pointer to private data
 */
int RIOCP_SO_ATTR riocp_mport_get_private(struct riocp_pe *pe, void **data)
{
	int ret;

	if (*data == NULL) {
		RIOCP_TRACE("Private data output is NULL\n");
		return -EINVAL;
	}

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_TRACE("Invalid pe handle\n");
		return -EINVAL;
	}

	*data = pe->minfo->private_data;
	return 0;
}

/**
 * Remove handle from mport or global handle list and free all data
 * @param handle PE handle to destroy
 */
static void riocp_pe_handle_destroy(struct riocp_pe **handle)
{
	struct riocp_pe *p = NULL;
	struct riocp_pe_llist_item *item = NULL, *next = NULL;

	if (*handle == NULL) {
		RIOCP_ERROR("Trying to destroy NULL handle\n");
		return;
	}

	/* When handle is of type mport free the mport info */
	if (RIOCP_PE_IS_MPORT(*handle)) {
		int ret;
		RIOCP_TRACE("Destroying mport handle %p (ct: 0x%08x)\n",
			*handle, (*handle)->comptag);
		riocp_pe_llist_foreach_safe(item, next, &(*handle)->minfo->handles) {
			p = (struct riocp_pe *)item->data;
			if (p)
				riocp_pe_handle_destroy(&p);
		}
		riocp_pe_llist_del(&riocp_pe_mport_handles, *handle);
		ret = riocp_drv_destroy_pe(*handle);
		if (ret)
			RIOCP_TRACE(
			"Drv err %d destroying PE hndl %p (ct: 0x%08x)\n",
				ret, *handle, (*handle)->comptag);
		free((*handle)->minfo->comptag_pool);
		free((*handle)->minfo);
	} else {
		RIOCP_TRACE("Destroying PE handle %p (ct: 0x%08x)\n",
			*handle, (*handle)->comptag);
		riocp_pe_llist_del(&(*handle)->mport->minfo->handles, *handle);
	}

	free((*handle)->port);
	free((*handle)->peers);
	free(*handle);

	*handle = NULL;
}

/**
 * Create processing element handle
 *
 * It will initialize the PE based on component tag/destination ID and if the originating
 *  handle is host/agent.
 * @param pe         Originating PE
 * @param handle     Create PE
 * @param hopcount   RapidIO hopcount to new PE
 * @param destid     RapidIO destination id for new PE
 * @param port       RapidIO port
 */
int riocp_pe_handle_create_pe(struct riocp_pe *pe, struct riocp_pe **handle, hc_t hopcount,
	uint32_t destid, uint8_t port, ct_t *comptag_in, char *name)
{
	struct riocp_pe *h = NULL;
	ct_nr_t comptag_nr = 0;
	uint8_t peer_port = 0;
	int ret = 0;

	RIOCP_TRACE("Creating new handle\n");

	/* Create new empty handle */
	h = (struct riocp_pe *)calloc(1, sizeof(struct riocp_pe));
	if (h == NULL)
		return -ENOMEM;

	/* Create port field */
	h->port = (struct riocp_pe_port *)calloc(1, sizeof(struct riocp_pe_port));
	if (h->port == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	/* Initialize handle attributes */
	h->version  = RIOCP_PE_HANDLE_REV;
	h->mport    = pe->mport;
	h->hopcount = hopcount;
	h->destid   = destid;

	/* Allocate space for address used to access this PE
		and copy port list from PE to new peer handle */
	if (hopcount > 0) {
		// address is a string, with entries for 2 digit port numbers
		// followed by a comma.  3 = 2 digits + a comma...
		//
		// There may be a few extra bytes as part of this allocation,
		// but it's not a big deal...
		h->address = (uint8_t *)calloc(3 * hopcount, sizeof(*h->address));
		if (h->address == NULL) {
			RIOCP_ERROR("Unable to allocate memory for h->address\n");
			ret = -ENOMEM;
			goto err;
		}

		if (hopcount > 1) {
			RIOCP_TRACE("Address copy of PE to Peer: %u\n", hopcount - 1);
			memcpy(h->address, pe->address, sizeof(*pe->address) * (hopcount - 1));
		}
		h->address[hopcount - 1] = port;

		RIOCP_DEBUG("Address to new PE (hc: %u): %s\n",
			h->hopcount,
			riocp_pe_handle_addr_ntoa(h->address, h->hopcount));
	}

	/* Add new handle to mport handle list BEFORE any maintenance access
		(which depends on checking for valid handle in list) */
	riocp_pe_llist_add(&h->mport->minfo->handles, h);
	h->comptag = *comptag_in;

	ret = riocp_drv_init_pe(h, comptag_in, pe, name);
	if (ret) {
		RIOCP_ERROR("Could not initialize PE with CT 0x%x\n",
				pe->comptag);
		goto err;
	}

	ret = riocp_pe_read_capabilities(h);
	if (ret) {
		RIOCP_ERROR("Capabilities could not be read\n");
		goto err;
	}
	peer_port = RIOCP_PE_SW_PORT(h->cap);

	ret = riocp_pe_read_features(h);
	if (ret) {
		RIOCP_ERROR("Features could not be read\n");
		goto err;
	}

	ret = riocp_pe_comptag_read(h, &h->comptag);
	if (ret) {
		RIOCP_ERROR("Could not read comptag\n");
		ret = -EIO;
		goto err;
	}

	/* Decide if the host needs to initialize comptag/destid the PE based
		on comptag unique number */
	comptag_nr = RIOCP_PE_COMPTAG_GET_NR(h->comptag);

	RIOCP_DEBUG("h->comptag: 0x%08x (comptag_nr: %u, 0x%08x)\n", h->comptag,
		comptag_nr, comptag_nr);

	/* Comptag unique number not set */
	if (comptag_nr == 0 && RIOCP_PE_IS_HOST(h) == true) {

		RIOCP_DEBUG("Initializing empty comptag/reset destid for h 0x%08x\n", h->comptag);

		ret = riocp_pe_comptag_init(h);
		if (ret) {
			RIOCP_ERROR("Could not initialize component tag\n");
			goto err;
		}

		if (!RIOCP_PE_IS_SWITCH(h->cap)) {
			ret = riocp_pe_maint_write(h, RIO_DEVID, RIO_DID_UNSET);
			if (ret) {
				RIOCP_ERROR("Could not write destid\n");
				ret = -EIO;
				goto err;
			}
		}
	} else {
		/* Add h to comptag_pool at comptag_nr */
		ret = riocp_pe_comptag_set_slot(h, comptag_nr);
		if (ret) {
			RIOCP_ERROR("Error adding handle at comptag pool slot %u\n",
				comptag_nr);
			goto err;
		}

		RIOCP_DEBUG("Added PE: ct 0x%08x at comptag_pool slot %u\n",
			h->comptag, comptag_nr);
	}

	/* Initialize switch port event mask and attach switch driver */
	if (!RIOCP_PE_IS_SWITCH(h->cap)) {
		ret = riocp_pe_get_destid(h, &h->destid);
		if (ret) {
			RIOCP_ERROR("Could not read destid\n");
			ret = -EIO;
			goto err;
		}
	}

	/* Create PE peers placeholders and connect new handle (h) to PE */
	h->peers = (struct riocp_pe_peer *)
		calloc(RIOCP_PE_PORT_COUNT(h->cap),
						sizeof(struct riocp_pe_peer));
	if (h->peers == NULL) {
		RIOCP_ERROR("Unable to allocate memory for h->peers\n");
		ret = -ENOMEM;
		goto err;
	}
	RIOCP_DEBUG("Allocated %zu peers for 0x%08x\n", RIOCP_PE_PORT_COUNT(h->cap), h->comptag);

	ret = riocp_pe_add_peer(pe, h, port, peer_port);
	if (ret) {
		RIOCP_ERROR("Could not add peer(h) to pe\n");
		goto err;
	}
	RIOCP_DEBUG("Add dev_id 0x%08x ct: 0x%08x, to pe(hc: %u, addr: %s) 0x%08x pe port %u, peer port %u\n",
		h->cap.dev_id, h->comptag, pe->hopcount, riocp_pe_handle_addr_ntoa(pe->address, pe->hopcount),
		pe->comptag, port, peer_port);

	*handle = h;

	RIOCP_DEBUG("Created new pe handle (hc: %d, dev_id: %08x, ct: 0x%08x, destid: %u (0x%08x), (addr: %s) sysfs %s\n",
		h->hopcount, h->cap.dev_id, h->comptag, h->destid, h,
		riocp_pe_handle_addr_ntoa(h->address, h->hopcount),
		&h->sysfs_name[0]);

	return 0;

err:
	riocp_pe_handle_destroy(&h);
	return -EIO;
}

/**
 * Create mport handle with minfo field and initialized maintainance access
 * @param mport   Mport device number
 * @param is_host Create host or agent mport handle
 * @param handle  Handle created for mport
 * @retval -ENOMEM Cannot allocate memory
 * @retval -EIO Unable to initialize or read rapidio maintenance
 */
int riocp_pe_handle_create_mport(uint8_t mport, bool is_host,
	struct riocp_pe **handle, struct riocp_reg_rw_driver *drv,
	ct_t *comptag, char *name)
{
	int ret = 0;
	struct riocp_pe *h = NULL;

	RIOCP_TRACE("Creating mport %d handle\n", mport);

	/* Create new empty handle */
	h = (struct riocp_pe *)calloc(1, sizeof(struct riocp_pe));
	if (h == NULL)
		return -ENOMEM;

	/* Create port field */
	h->port = (struct riocp_pe_port *)calloc(1, sizeof(struct riocp_pe_port));
	if (h->port == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	/* Create mport information */
	h->minfo = (struct riocp_pe_mport *)calloc(1, sizeof(*h->minfo));
	if (h->minfo == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	/* Initialize handle attributes */
	h->version        = RIOCP_PE_HANDLE_REV;
	h->hopcount       = HC_MP;
	h->mport          = h;
	h->minfo->ref     = 1; /* Initialize reference count */
	h->minfo->id      = mport;
	h->minfo->is_host = is_host;
	h->minfo->reg_acc = *drv;
	if (NULL != comptag)
		h->comptag = *comptag;

	/* Add new handle to mport handles list BEFORE any maintenace access
		(which depends on checking for valid handle in list) */
	ret = riocp_pe_llist_add(&riocp_pe_mport_handles, h);
	if (ret) {
		RIOCP_ERROR("Could not add new handle to list\n");
		goto err;
	}

	ret = riocp_drv_init_pe(h, comptag, NULL, name);
	if (ret) {
		RIOCP_ERROR("Could not intialize pe\n");
		goto err;
	}

	ret = riocp_pe_read_capabilities(h);
	if (ret) {
		RIOCP_ERROR("Could not read capabilities\n");
		goto err;
	}

	ret = riocp_pe_read_features(h);
	if (ret) {
		RIOCP_ERROR("Features could not be read\n");
		goto err;
	}

	ret = riocp_pe_comptag_read(h, &h->comptag);
	if (ret) {
		RIOCP_ERROR("Could not read comptag\n");
		ret = -EIO;
		goto err;
	}

	ret = riocp_pe_get_destid(h, &h->destid);
	if (ret) {
		RIOCP_ERROR("Could not read destid\n");
		ret = -EIO;
		goto err;
	}

	h->peers = (struct riocp_pe_peer *)calloc(RIOCP_PE_PORT_COUNT(h->cap),
						sizeof(struct riocp_pe_peer));
	if (h->peers == NULL) {
		RIOCP_ERROR("Unable to allocate memory for h->peers\n");
		ret = -ENOMEM;
		goto err;
	}

	*handle = h;

	RIOCP_TRACE("Created mport(id: %d, destid: %u (0x%08x))\n",
		mport, h->destid, h->destid);

	return ret;

err:
	riocp_pe_handle_destroy(&h);

	return ret;
}

/**
 * Increment mport PE reference count
 * @param mport Mport PE
 */
void riocp_pe_handle_mport_get(struct riocp_pe *mport)
{
	if (mport == NULL)
		return;

	mport->minfo->ref++;
}

/**
 * Decrement PE reference count
 * Automaticly free all resources when ref == 0
 * When all resources are freed *pe is set to NULL by riocp_pe_handle_destroy
 * @param[in,out] pe Target PE
 */
void riocp_pe_handle_mport_put(struct riocp_pe **mport)
{
	if (mport == NULL)
		return;
	if ((*mport)->minfo->ref == 0)
		return;

	(*mport)->minfo->ref--;
	if ((*mport)->minfo->ref == 0)
		riocp_pe_handle_destroy(mport);
}

/**
 * Search for existing processing element handle based on read RapidIO component tag
 * It first searches first in global mport handles list and then in mport argument
 * handles list
 * @param mport Mport to check on for existing PE handle
 * @param comptag  Peer comptag (Set when ret != -EIO)
 * @param peer     Found peer (NULL when not found)
 * @retval 1 Handle found
 * @retval 0 Handle not found
 * @retval -EIO Error in reading remote
 */
int riocp_pe_handle_pe_exists(struct riocp_pe *mport, ct_t comptag,
				struct riocp_pe **peer)
{
	int ret;
	struct riocp_pe_llist_item *item = NULL;
	struct riocp_pe *ptr = NULL;

	RIOCP_TRACE("Check of PE with comptag 0x%08x exists\n", comptag);

	/* Bail out directly when comptag is zero */
	if (comptag == RIOCP_PE_COMPTAG_UNSET)
		goto notfound;

	/* Loop through all mport handles */
	item = riocp_pe_mport_handles.next;
	while (item != NULL) {
		ptr = (struct riocp_pe *)item->data;
		if (NULL != ptr) {
			if ((ptr->comptag == comptag) &&
			(ptr->mport->minfo->is_host == mport->minfo->is_host))
				goto found;
		};
		item = item->next;
	}

	ret = riocp_pe_comptag_get_slot(mport, RIOCP_PE_COMPTAG_GET_NR(comptag), &ptr);
	if (ret)
		goto notfound;

found:
	RIOCP_TRACE("[%p] Handle found for PE with ct 0x%08x (host: %s)\n",
		ptr, ptr->comptag, mport->minfo->is_host ? "true" : "false");
	*peer = ptr;
	return 1;

notfound:
	RIOCP_DEBUG("No handle found for comptag 0x%08x\n", comptag);
	return 0;
}

/**
 * Search for existing host/agent master port handle
 * @param mport   Mport number
 * @param is_host Check for host or agent handle
 * @param[out] pe PE handle associated with mport number (NULL when not found)
 * @retval 0 Handle found
 * @retval -ENOENT Handle not found
 */
int riocp_pe_handle_mport_exists(uint8_t mport, bool is_host, struct riocp_pe **pe)
{
	struct riocp_pe_llist_item *item = NULL;
	struct riocp_pe *p = NULL;

	riocp_pe_llist_foreach(item, &riocp_pe_mport_handles) {
		p = (struct riocp_pe *)item->data;
		if (p) {
			if (is_host == p->mport->minfo->is_host &&
				mport == p->mport->minfo->id) {
				riocp_pe_handle_mport_get(p->mport); /* Increment reference count */
				*pe = p->mport;
				return 0;
			}
		}
	}

	*pe = NULL;
	return -ENOENT;
}

/**
 * Get list of handles from current internal administration
 * @param      mport Master port PE handle
 * @param[out] pe_list  List of PE handles including mport as first entry
 * @param[out] pe_list_size The amount of PE handles in list
 */
int RIOCP_SO_ATTR riocp_pe_handle_get_list(riocp_pe_handle mport,
        riocp_pe_handle **pe_list, size_t *pe_list_size)
{
        int ret = 0;
        size_t handle_counter = 0;
        riocp_pe_handle *_pe_list = NULL;
        size_t _pe_list_size;
        struct riocp_pe *p = NULL;
        struct riocp_pe_llist_item *item = NULL;

        RIOCP_TRACE("Get list of handles behind mport %u\n",
                mport->minfo->id);

        ret = riocp_pe_handle_check(mport);
        if (ret) {
                RIOCP_ERROR("Invalid mport handle\n");
                return -EINVAL;        }

        if (*pe_list != NULL) {
                RIOCP_ERROR("Pointer to be alloced is not NULL\n");
                return -EINVAL;
        }

        /* Include mport handle as first PE in list */
        handle_counter = 1;

        /* Count amount of handles in mport handles list */
        riocp_pe_llist_foreach(item, &mport->minfo->handles) {
                p = (struct riocp_pe *)item->data;
                if (!p)
                        continue;
                handle_counter++;
        }

        _pe_list = (struct riocp_pe **)calloc(handle_counter, sizeof(riocp_pe_handle));
        if (_pe_list == NULL) {
                RIOCP_TRACE("Could not allocate handle list\n");
                return -ENOMEM;
        }

        /* Mport handle is first handle in the list */
        _pe_list[0] = mport;

        /* Copy mport handles list pointers to newlist */
        _pe_list_size = 1;
        riocp_pe_llist_foreach(item, &mport->minfo->handles) {
                p = (struct riocp_pe *)item->data;
                if (!p)
                        continue;
                _pe_list[_pe_list_size] = p;
                _pe_list_size++;
                if (_pe_list_size >= handle_counter)
                        break;
        }

        *pe_list = _pe_list;
        *pe_list_size = _pe_list_size;

        return ret;
};

/**
 * Get peer on port of PE from internal handle administration. This will
 *  not perform any RapidIO maintenance transactions.
 * @param pe   Target PE
 * @param port Port on PE to peek
 * @param peer Peer PE handle connected to port on PE. When NULL then no peer
 *  is connected
 * @retval -EINVAL Invalid argument
 * @retval -EADDRNOTAVAIL Invalid port
 */
int RIOCP_SO_ATTR riocp_pe_handle_peek(riocp_pe_handle pe,
	uint8_t port,
	riocp_pe_handle *peer)
{
	if (peer == NULL) {
		RIOCP_ERROR("Invalid return pointer\n");
		return -EINVAL;
	}

	if (riocp_pe_handle_check(pe)) {
		RIOCP_ERROR("Invalid handle\n");
		return -EINVAL;
	}

	if (port >= RIOCP_PE_PORT_COUNT(pe->cap)) {
		RIOCP_ERROR("Invalid port\n");
		return -EADDRNOTAVAIL;
	}

	*peer = pe->peers[port].peer;

	return 0;
}

/**
 * Get RapidIO device name string based on device id (did)
 * @param pe Target PE
 */
const char RIOCP_SO_ATTR *riocp_pe_handle_get_device_str(riocp_pe_handle pe)
{
	unsigned int i;
	uint16_t vid = pe->cap.dev_id & 0xffff;
	uint16_t did = (pe->cap.dev_id >> 16) & 0xffff;

	for (i = 0; i < (sizeof(riocp_pe_device_ids)/sizeof(riocp_pe_device_ids[0])); i++)
		if (riocp_pe_device_ids[i].vid == vid && riocp_pe_device_ids[i].did == did)
			return riocp_pe_device_ids[i].name;

	return "unknown device";
}

/**
 * Get rapidio vendor name string based on vendor id (vid)
 * @param pe Target PE
 */
const char RIOCP_SO_ATTR *riocp_pe_handle_get_vendor_str(riocp_pe_handle pe)
{
	unsigned int i;
	uint16_t vid = pe->cap.dev_id & 0xffff;

	for (i = 0; i < (sizeof(riocp_pe_vendors)/sizeof(struct riocp_pe_vendor)); i++)
		if (riocp_pe_vendors[i].vid == vid)
			return riocp_pe_vendors[i].vendor;

	return "unknown vendor";
}


/**
 * Get PE connected peers list
 * @param      pe Target PE
 * @param[out] peer_list  List of handles including mport as first entry
 * @param[out] peer_list_size Amount of handles in list
 */
int RIOCP_SO_ATTR riocp_pe_handle_get_peer_list(riocp_pe_handle pe,
	riocp_pe_handle **peer_list, size_t *peer_list_size)
{
	int ret = 0;
	unsigned int i;
	riocp_pe_handle *_peer_list = NULL;

	ret = riocp_pe_handle_check(pe);
	if (ret) {
		RIOCP_TRACE("Invalid pe handle\n");
		return -EINVAL;
	}

	_peer_list = (struct riocp_pe **)
		calloc(RIOCP_PE_PORT_COUNT(pe->cap), sizeof(riocp_pe_handle));
	if (_peer_list == NULL) {
		RIOCP_TRACE("Could not allocate peer handle list\n");
		return -ENOMEM;
	}

	for (i = 0; i < RIOCP_PE_PORT_COUNT(pe->cap); i++)
		_peer_list[i] = pe->peers[i].peer;

	*peer_list = _peer_list;
	*peer_list_size = RIOCP_PE_PORT_COUNT(pe->cap);

	return ret;
}

/**
 * Free handle list aquired with riocp_pe_handle_get_list or riocp_pe_handle_get_peer_list
 * @param list List to free
 */
int RIOCP_SO_ATTR riocp_pe_handle_free_list(riocp_pe_handle **list)
{
	if (*list == NULL) {
		RIOCP_TRACE("Pointer to be freed is NULL\n");
		return -EINVAL;
	}

	free(*list);

	*list = NULL;

	return 0;
}

int RIOCP_WU riocp_pe_find_comptag(riocp_pe_handle mport, ct_t comptag,
							riocp_pe_handle *pe)
{
	size_t count;
	riocp_pe_handle *pes = NULL;
	size_t i;

	if ((NULL == mport) || (NULL == pe)) {
		errno = -EINVAL;
		goto fail;
	};
	*pe = NULL;

	if (COMPTAG_UNSET == comptag) {
		goto found;
	};

	// If the component tag is not in use, it will not be found.
	if (!ct_not_inuse(comptag, dev08_sz)) {
		goto found;
	};

	if (mport->comptag == comptag) {
		*pe = mport;
		goto found;
	};

	if (riocp_mport_get_pe_list(mport,&count, &pes)) {
		goto fail;
	};

	for (i = 0; i < count; i++) {
		if (pes[i]->comptag == comptag) {
			*pe = pes[i];
			break;
		};
	};

	if (riocp_mport_free_pe_list(&pes)) {
		*pe = NULL;
		goto fail;
	};
found:
	return NULL == *pe;
fail:
	return -1;
};

#ifdef __cplusplus
}
#endif

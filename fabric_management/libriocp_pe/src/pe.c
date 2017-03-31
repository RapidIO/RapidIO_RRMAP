/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file pe.c
 * Processing element helper functions
 */
#define _XOPEN_SOURCE 500

#include <stdint.h>
#include <string.h>

#include "rio_ecosystem.h"
#include "rio_standard.h"
#include "did.h"
#include "ct.h"
#include "pe.h"

#include "maint.h"
#include "handle.h"
#include "comptag.h"
#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read the capabilities into the handle
 * @param pe Target PE
 * @returns
 *    - 0 on success
 *    - >0 on failure
 */
int riocp_pe_read_capabilities(struct riocp_pe *pe)
{
	int ret = 0;

	RIOCP_TRACE("Read capabilities\n");

	ret = riocp_pe_maint_read(pe, RIO_DEV_IDENT, &pe->cap.dev_id);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_DEV_INF, &pe->cap.dev_info);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_ASSY_ID, &pe->cap.asbly_id);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_ASSY_INF, &pe->cap.asbly_info);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_PE_FEAT, &pe->cap.pe_feat);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_SW_PORT_INF, &pe->cap.sw_port);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_SRC_OPS, &pe->cap.src_op);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_DST_OPS, &pe->cap.dst_op);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_SW_RT_TBL_LIM, &pe->cap.lut_size);
	if (ret)
		return ret;

	RIOCP_TRACE("Read capabilities ok\n");

	return 0;
}

#define RIO_EFB_PTR_MASK	0xffff0000
#define RIO_GET_BLOCK_PTR(x)	((x & RIO_EFB_PTR_MASK) >> 16)

/**
 * Get pointer to next extended features block
 * @param pe    Target PE
 * @param from  Offset of current Extended Feature block header
 *  (if 0 starts from ExtFeaturePtr)
 * @param value Value read at offset
 */
static int RIOCP_WU riocp_pe_get_efb(struct riocp_pe *pe, uint32_t from, uint32_t *value)
{
	int ret;
	uint32_t val;

	if (from == 0) {
		ret = riocp_pe_maint_read(pe, RIO_ASSY_INF, &val);
		if (ret)
			return ret;
		val = val & RIO_ASSY_INF_EFB_PTR;
	} else {
		ret = riocp_pe_maint_read(pe, from, &val);
		if (ret)
			return ret;
		//val = RIO_EFB_ID(val);
                val = RIO_GET_BLOCK_PTR(val);
	}

	*value = val;

	return ret;
}

/**
 * Get RapidIO Physical extended feature pointer
 * @param pe Target PE
 * @param[out] efptr Extended feature pointer
 */
static int riocp_pe_get_efptr_phys(struct riocp_pe *pe, uint32_t *efptr, uint32_t *efptr_type, uint32_t *value)
{
	int ret;
	uint32_t _efptr;
	uint32_t _efptr_hdr;

	pe->efptr = pe->cap.asbly_info & RIO_ASSY_INF_EFB_PTR;

	ret = riocp_pe_get_efb(pe, 0, &_efptr);
	if (ret)
		return ret;

	while (_efptr) {
		ret = riocp_pe_maint_read(pe, _efptr, &_efptr_hdr);
		if (ret)
			return ret;

		_efptr_hdr = RIO_EFB_ID(_efptr_hdr);
		switch (_efptr_hdr) {
		case RIO_EFB_T_SP_EP:
		case RIO_EFB_T_SP_EP_SAER:
		case RIO_EFB_T_SP_NOEP:
		case RIO_EFB_T_SP_NOEP_SAER:
		case RIO_EFB_T_SP_EP3:
		case RIO_EFB_T_SP_EP3_SAER:
		case RIO_EFB_T_SP_NOEP3:
		case RIO_EFB_T_SP_NOEP3_SAER:
			*efptr = _efptr;
			*efptr_type = _efptr_hdr;
			return 0;
		// The next 3 are obsolete identifiers.
		// Just in case, translate them to standard, supported
		// identifier values.
		case RIO_EFB_SER_EP_ID:
			*efptr = _efptr;
			*efptr_type = RIO_EFB_T_SP_EP;
			return 0;
		case RIO_EFB_SER_EP_REC_ID:
			*efptr = _efptr;
			*efptr_type = RIO_EFB_T_SP_EP_SAER;
			return 0;
		case RIO_EFB_SER_EP_FREE_ID:
			*efptr = _efptr;
			*efptr_type = RIO_EFB_T_SP_NOEP;
			return 0;
                case RIO_EFB_T_EMHS:
                        RIOCP_DEBUG("Feature[0x%08x] found with value 0x%08x\n",
				_efptr_hdr, *value);
			*value = _efptr;
			return 0;
		default:
			break;
		}
		
		ret = riocp_pe_get_efb(pe, _efptr, &_efptr);
		if (ret)
			return ret;
	}

	*efptr = 0;
        return 0;
}

/**
 * Get PE extended feature
 * @note The PE should already have efptr attribute initialized
 * @param pe      Target PE
 * @param feature Extended feature code
 * @param value   Value read for requested feature
 * @retval -ENOENT Could not find feature
 * @retval -EIO Error in maintenance access
 */
/*static int riocp_pe_get_ef(struct riocp_pe *pe, uint32_t feature, uint32_t *value)
{
	int ret;
	uint32_t efptr = pe->efptr;
	uint32_t efptr_hdr;

	while (efptr) {
		ret = riocp_pe_maint_read(pe, efptr, &efptr_hdr);
		if (ret) {
			RIOCP_ERROR("Error reading efptr_hdr\n");
			return ret;
		}

		if (feature == RIO_EFB_ID(efptr_hdr)) {
			RIOCP_DEBUG("Feature[0x%08x] found with value 0x%08x\n",
				feature, *value);
			*value = efptr;
			return 0;
		}

		efptr = RIO_EFB_GET_NEXT(efptr_hdr);
		if (!efptr)
			break;
	}

	RIOCP_DEBUG("Feature[0x%08x] found with value 0x%08x\n",
		feature, *value);

	return -ENOENT;
}*/

/**
 * Read and initialize handle extended feature pointers when available
 * @note The pe should already have the cap attribute read
 * @param pe Target PE
 * @retval < 0 Error
 */
int riocp_pe_read_features(struct riocp_pe *pe)
{
	int ret = 0;

	/* Get extended feature pointers when available */
	if (pe->cap.pe_feat & RIO_PEF_EXT_FEATURES) {
		
		ret = riocp_pe_get_efptr_phys(pe, &pe->efptr_phys, &pe->efptr_phys_type, &pe->efptr_em);
		if (ret)
			return ret;

                /*ret = riocp_pe_get_ef(pe, RIO_EFB_T_EMHS, &pe->efptr_em);
		if (ret)
			return ret;*/

		RIOCP_TRACE("PE has extended features\n");
		RIOCP_TRACE(" - p->efptr      = 0x%04x\n", pe->efptr);
		RIOCP_TRACE(" - p->efptr_phys = 0x%08x\n", pe->efptr_phys);
		RIOCP_TRACE(" - p->efptr_type = 0x%08x\n", pe->efptr_phys_type);
		RIOCP_TRACE(" - p->efptr_em   = 0x%08x\n", pe->efptr_em);
	}

	return ret;
}

/**
 * Check if source port of PE is active using extended feature pointer
 * @retval 0 When PORT_OK is not set
 * @retval 1 When PORT_OK is set
 * @retval < 0 Error in reading if port is active
 */
int riocp_pe_is_port_active(struct riocp_pe *pe, uint32_t port)
{
	int ret;
	uint32_t val = 0;
	uint32_t efptr = pe->efptr_phys;
	uint32_t etype = pe->efptr_phys_type;
	uint32_t offset;

	RIOCP_TRACE("[pe 0x%08x] Check if port %u is active (efptr: 0x%08x)\n",
		pe->comptag, port, efptr);

	if (efptr) {
		offset = RIO_SPX_ERR_STAT(efptr, etype, port);
		ret = riocp_pe_maint_read(pe, offset, &val);
		if (ret) {
			RIOCP_ERROR("Unable to read RIO_SPX_ERR_STAT@0x%08x) for port %u\n",
				offset, port);
			return ret;
		}
	}

	if (val & RIO_SPX_ERR_STAT_OK)
		return 1;

	return 0;
}

/**
 * Read the Port gen control CSR
 * @param pe Target PE
 * @retval < 0 Error from maintenance read
 * @retval 0 When PE is not set as discovered
 * @retval 1 When PE is set as discovered
 */
int riocp_pe_is_discovered(struct riocp_pe *pe)
{
	int ret;
	uint32_t val = 0;

	ret = riocp_pe_maint_read(pe, RIO_SP_GEN_CTL(pe->efptr_phys), &val);
	if (ret) {
		return ret;
	}

	return (val & RIO_SP_GEN_CTL_DISC) ? 1 : 0;
}

/**
 * Set the Port gen control CSR to MASTER and DISCOVERED bit using the extended
 *  feature pointer address of the PE.
 * @param pe Target PE
 * @retval 0 When read/write was successfull
 * @retval >0 When read/write was unsuccessfull
 */
int riocp_pe_set_discovered(struct riocp_pe *pe)
{
	uint32_t val;
	int ret;

	ret = riocp_pe_maint_read(pe, RIO_SP_GEN_CTL(pe->efptr_phys), &val);
	val |= RIO_SP_GEN_CTL_DISC | RIO_SP_GEN_CTL_MAST_EN;
	ret += riocp_pe_maint_write(pe, RIO_SP_GEN_CTL(pe->efptr_phys), val);

	return ret;
}

/**
 * Add peer (from peer port) to pe (pe port)
 * @param pe        Target PE
 * @param peer      Peer PE
 * @param pe_port   Port on PE
 * @param peer_port Port on peer PE
 * @retval 0 Peer successfull added to PE
 * @retval -EINVAL Invalid parameter(s)
 */
int riocp_pe_add_peer(struct riocp_pe *pe, struct riocp_pe *peer,
		uint8_t pe_port, uint8_t peer_port)
{
	if (pe == NULL || peer == NULL)
		return -EINVAL;
	if (pe_port >= RIOCP_PE_PORT_COUNT(pe->cap))
		return -EINVAL;
	if (peer_port >= RIOCP_PE_PORT_COUNT(peer->cap))
		return -EINVAL;

	/* Connect peer to pe */
	pe->peers[pe_port].peer = peer;
	pe->peers[pe_port].remote_port = peer_port;

	/* Connect pe to peer */
	peer->peers[peer_port].peer = pe;
	peer->peers[peer_port].remote_port = pe_port;

	return 0;
}

/**
 * Remove peer from pe port
 * @param pe        Target PE
 * @param port   Port on PE
 * @retval 0 Peer successfull added to PE
 * @retval -EINVAL Invalid parameter(s)
 */
int riocp_pe_remove_peer(struct riocp_pe *pe, uint8_t port)
{
	struct riocp_pe *peer;
	uint8_t peer_port;

	if (pe == NULL)
		return -EINVAL;
	if (port >= RIOCP_PE_PORT_COUNT(pe->cap))
		return -EINVAL;

	peer = pe->peers[port].peer;
	peer_port = pe->peers[port].remote_port;

	/* Remove peer from pe */
	pe->peers[port].peer = NULL;
	pe->peers[port].remote_port = 0;

	if (peer == NULL)
		return 0;

	/* Remove pe from peer */
	peer->peers[peer_port].peer = NULL;
	peer->peers[peer_port].remote_port = 0;

	return 0;
}

/**
 * Prepare probe behind PEs port (when pe is switch)
 * - Check if port is active
 * - Program anyid route to PE handle
 * - Set ANY_ID route to port on PE
 * @param pe Target switch PE
 * @param port Port to prepare for probe
 * @retval -EIO Error in maintenance access
 * @retval -ENODEV Supplied port is inactive
 */
int riocp_pe_probe_prepare(struct riocp_pe *pe, uint8_t port)
{
	int ret = 0;

	RIOCP_TRACE("Prepare probe until pe 0x%08x\n", pe->comptag);

	/* Set DID_ANY_DEV8_ID route to access PE */
	if (RIOCP_PE_IS_SWITCH(pe->cap)) {
		struct riocp_pe_port_state_t state;

		ret = riocp_pe_maint_set_anyid_route(pe);
		if (ret) {
			RIOCP_ERROR(
					"Could not program DID_ANY_DEV8_ID route\n");
			return -EIO;
		}

		ret = riocp_drv_get_port_state(pe, port, &state);
		if (ret) {
			RIOCP_ERROR("Unable to read port state\n");
			return -EIO;
		}

		if (!state.port_ok) {
			RIOCP_ERROR("Try to probe inactive port\n");
			return -ENODEV;
		}

		ret = riocp_drv_set_route_entry(pe, RIOCP_PE_ANY_PORT,
				DID_ANY_DEV8_ID, port);
		if (ret) {
			RIOCP_ERROR("Could not program route\n");
			return -EIO;
		}
	}

	RIOCP_TRACE("Prepare probe until pe 0x%08x successfull\n", pe->comptag);

	return ret;
}

/**
 * Verify found handle by writing current PE comptag and verify peer comptag
 * @param pe Target PE (for comptag_alt)
 * @retval 0 New device, no redundant path
 * @retval 1 Redundant path
 */
int riocp_pe_probe_verify_found(struct riocp_pe *pe, uint8_t port, struct riocp_pe *peer)
{
	int ret;
	ct_t comptag_peer;
	ct_t comptag_alt;
	hc_t hopcount_alt;

	// initialize the hopcount
	HC_INCR(hopcount_alt, pe->hopcount);

	RIOCP_TRACE("Probe verify pe: hc: %u, comptag: 0x%08x, port %u\n",
			pe->hopcount, pe->comptag, port);
	RIOCP_TRACE("Probe verify pe_alt: hc: %u, d: %u\n", hopcount_alt,
			did_get_value(DID_ANY_DEV8_ID));
	RIOCP_TRACE("Probe verify peer: hc: %u, comptag: 0x%08x\n",
			peer->hopcount, peer->comptag, port);

	/* Reset the component tag for alternative route */
	ret = riocp_drv_raw_reg_wr(pe, DID_ANY_DEV8_ID, hopcount_alt,
			RIO_COMPTAG, 0);
	if (ret) {
		RIOCP_ERROR("Error reading comptag from d: %u, hc: %u\n",
				did_get_value(DID_ANY_DEV8_ID), hopcount_alt);
		return -EIO;
	}

	/* read same comptag again to make sure write has been performed
	 (we read pe comptag from potentially (shorter) different path) */
	ret = riocp_drv_raw_reg_rd(pe, DID_ANY_DEV8_ID, hopcount_alt,
			RIO_COMPTAG, &comptag_alt);
	if (ret) {
		RIOCP_ERROR("Error reading comptag from d: %u, hc: %u\n",
				did_get_value(DID_ANY_DEV8_ID), hopcount_alt);
		return -EIO;
	}

	/* Read peer, this programs ANY_ID route to the device */
	ret = riocp_pe_comptag_read(peer, &comptag_peer);
	if (ret) {
		RIOCP_ERROR("Error reading comptag from peer\n");
		return -EIO;
	}

	RIOCP_DEBUG("comptag_peer 0x%08x, comptag_alt 0x%08x\n",
		comptag_peer, comptag_alt);

	if (comptag_peer == comptag_alt) {
		/* Restore route to peer, and restore component tag */
		ret = riocp_pe_restore(peer);
		if (ret)
			return -EIO;
		RIOCP_DEBUG("Same device different route\n");
		ret = riocp_pe_probe_prepare(pe, port);
		if (ret)
			return ret;
		return 1;
	}

	RIOCP_DEBUG("Found different PE\n");

	/* Restore route to new peer */
	ret = riocp_pe_probe_prepare(pe, port);
	if (ret)
		return ret;

	return 0;
}

/**
 * Initialize new peer after probe
 * - Peer is switch
 *   - Set route in global LUT to host destid (mport->did_reg_val)
 *   - Set switch port enumerated
 * - Set peer discovered bit
 * - Initialize event handling
 * @param peer Target peer
 */
int riocp_pe_probe_initialize_peer(struct riocp_pe *peer)
{
	did_t did;
	int ret;

	/* Set route on new switch to port routing to the host destid */
	if (RIOCP_PE_IS_SWITCH(peer->cap)) {
		ret = did_get(&did, peer->mport->did_reg_val);
		if (ret) {
			RIOCP_ERROR("Unable to get did 0x%08x\n",
					peer->mport->did_reg_val);
			return ret;
		}

		ret = riocp_drv_set_route_entry(peer, RIOCP_PE_ANY_PORT, did,
				RIOCP_PE_SW_PORT(peer->cap));
		if (ret) {
			RIOCP_ERROR("Unable to set route on peer\n");
			return ret;
		}
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

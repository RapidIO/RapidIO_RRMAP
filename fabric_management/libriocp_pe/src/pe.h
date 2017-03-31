/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __RIOCP_PE_PE_H__
#define __RIOCP_PE_PE_H__

/**
 * @file pe.h
 * RapidIO processing element helper functions
 */

#include <stdint.h>

#include "did.h"
#include "ct.h"
#include "riocp_pe.h"
#include "riocp_pe_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIOCP_PE_PE_DEVICE(dev, ven) \
	.did = (dev), .vid = (ven), \
	.asm_did = ANY_ID, .asm_vid = ANY_ID

/** Identification of PE by DID/VID and Assembly DID/VID */
struct riocp_pe_device_id {
	uint16_t did;
	uint16_t vid;
	uint16_t asm_did;
	uint16_t asm_vid;
};

int RIOCP_WU riocp_pe_read_capabilities(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_read_features(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_is_port_active(struct riocp_pe *pe, uint32_t port);
int RIOCP_WU riocp_pe_is_discovered(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_set_discovered(struct riocp_pe *pe);
int RIOCP_WU riocp_pe_add_peer(struct riocp_pe *pe, struct riocp_pe *peer,
		uint8_t pe_port, uint8_t peer_port);
int RIOCP_WU riocp_pe_remove_peer(struct riocp_pe *pe, uint8_t port);

int RIOCP_WU riocp_pe_probe_prepare(struct riocp_pe *pe, uint8_t port);
int RIOCP_WU riocp_pe_probe_verify_found(struct riocp_pe *pe, uint8_t port, struct riocp_pe *peer);
int RIOCP_WU riocp_pe_probe_initialize_peer(struct riocp_pe *peer);

#ifdef __cplusplus
}
#endif

#endif /* __RIOCP_PE_PE_H__ */

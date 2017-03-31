/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __RIOCP_PE_H__
#define __RIOCP_PE_H__

#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rio_ecosystem.h"
#include "did.h"
#include "ct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIOCP_PE_LIB_REV 0
#define RIOCP_WU __attribute__((warn_unused_result))

/* Processing element features */
#define RIOCP_PE_PEF_STD_RT            (1<<8)      /* Standard routing */
#define RIOCP_PE_PEF_EXT_RT            (1<<9)      /* Extended routing */
#define RIOCP_PE_PEF_SWITCH            (1<<28)
#define RIOCP_PE_PEF_PROCESSOR         (1<<29)
#define RIOCP_PE_PEF_MEMORY            (1<<30)
#define RIOCP_PE_PEF_BRIDGE            (1<<31)
#define RIOCP_PE_IS_SWITCH(cap)        ((cap).pe_feat & RIOCP_PE_PEF_SWITCH)
#define RIOCP_PE_IS_PROCESSOR(cap)     ((cap).pe_feat & RIOCP_PE_PEF_PROCESSOR)
#define RIOCP_PE_IS_MEMORY(cap)        ((cap).pe_feat & RIOCP_PE_PEF_MEMORY)
#define RIOCP_PE_IS_BRIDGE(cap)        ((cap).pe_feat & RIOCP_PE_PEF_BRIDGE)
#define RIOCP_PE_PORT_COUNT(cap) \
	(RIOCP_PE_IS_SWITCH(cap) ? (cap).sw_port >> 8 & 0xff : 1)
#define RIOCP_PE_VID(cap)              ((cap).dev_id & 0xffff)
#define RIOCP_PE_DID(cap)              (((cap).dev_id >> 16) & 0xffff)
#define RIOCP_PE_SW_PORT(cap)          ((cap).sw_port & 0xff)

/* Flags */
#define RIOCP_PE_FLAG_FORCE  (1<<0)  /* Force operation */

/* Structure describing standard RapidIO capabilities registers (CARs) */
struct riocp_pe_capabilities {
	uint32_t dev_id;         /* 0x00 Device identity */
	uint32_t dev_info;       /* 0x04 Device information */
	uint32_t asbly_id;       /* 0x08 Assembly identity */
	uint32_t asbly_info;     /* 0x0c Assembly information */
	uint32_t pe_feat;        /* 0x10 Processing element features */
	uint32_t sw_port;        /* 0x14 Switch port information */
	uint32_t src_op;         /* 0x18 Source operation */
	uint32_t dst_op;         /* 0x1c Destination operation */
	uint32_t lut_size;       /* 0x34 Route LUT size */
};

/* Opaque handle for PE objects */
typedef struct riocp_pe *riocp_pe_handle;

/* RapidIO port and status */

struct riocp_pe_port_state_t
{
	// 0 - port not initialized, 1 - port initialized
	int port_ok;

	// Maximum number of lanes for the port
	int port_max_width;

	// Current operating width of the port
	int port_cur_width;

	// Lane speed in Mbaud
	// Values: 1250, 2500, 3125, 5000, 6250
	int port_lane_speed;
};

/* Structure describing a RapidIO port and its status */
struct riocp_pe_port {
	// Owner of this port
	riocp_pe_handle pe;

	// Peer port of this port (NULL=no peer)
	struct riocp_pe_port *peer;

	// Physical port number
	uint8_t id;

	// Port state
	struct riocp_pe_port_state_t state;
};


/*
 * Device Driver Functions
 */

#define RIOCP_PE_ALL_PE_PORTS ((uint8_t)(0xFF))

typedef uint32_t pe_rt_val;
#define RT_VAL_FIRST_PORT	((pe_rt_val)(0x00000000))
#define RT_VAL_LAST_PORT	((pe_rt_val)(0x000000FF))
#define RT_VAL_FIRST_MC		((pe_rt_val)(0x00000100))
#define RT_VAL_LAST_MC		((pe_rt_val)(0x000001FF))
#define RT_VAL_FIRST_NEXT_LVL 	((pe_rt_val)(0x00000200))
#define RT_VAL_LAST_NEXT_LVL 	((pe_rt_val)(0x000002FF))
#define RT_VAL_DROP		((pe_rt_val)(0x00000300))
#define RT_VAL_DEFAULT_ROUTE	((pe_rt_val)(0x00000301))
#define RT_VAL_BAD		((pe_rt_val)(0x0FFFFFFF))

/* Routing table definitions */
typedef uint8_t pe_port_t;
#define RIOCP_PE_ALL_PORTS (pe_port_t)0xff /* Use the global LUT */

/* Routing table entry values */
#define RIOCP_PE_EGRESS_PORT(n)	((pe_rt_val)(RT_VAL_FIRST_PORT + ((n) & 0xff)))
#define RIOCP_PE_MULTICAST_MASK(n) ((pe_rt_val)(RT_VAL_FIRST_MC + ((n) & 0xff)))
#define RIOCP_PE_NEXT_LEVEL_GROUP(n) ((pe_rt_val)(RT_VAL_FIRST_NEXT_LVL + ((n) & 0xff)))
#define RIOCP_PE_NO_ROUTE           ((pe_rt_val)(0x300))
#define RIOCP_PE_DEFAULT_ROUTE      ((pe_rt_val)(0x301))

#define RIOCP_PE_IS_EGRESS_PORT(n)       ((n) <= 0xff)
#define RIOCP_PE_IS_MULTICAST_MASK(n)    ((n) >= 0x100 && (n) <= 0x1ff)
#define RIOCP_PE_IS_NEXT_LEVEL_GROUP(n)  ((n) >= 0x200 && (n) <= 0x2ff)

#define RIOCP_PE_GET_EGRESS_PORT(n)      (RIOCP_PE_IS_EGRESS_PORT(n)?(((n) & 0xff)):RT_VAL_BAD)
#define RIOCP_PE_GET_MULTICAST_MASK(n)   (RIOCP_PE_IS_MULTICAST_MASK(n)?(((n) - 0x100) & 0xff):RT_VAL_BAD)
#define RIOCP_PE_GET_NEXT_LEVEL_GROUP(n) (RIOCP_PE_IS_NEXT_LEVEL_GROUP(n)?(((n) - 0x200) & 0xff):RT_VAL_BAD)

struct mport_regs {
	uint32_t memaddr_sz; // RIO_PE_LL_CTL
	did_reg_t host_did_reg_val; //
	did_reg_t my_did_reg_val; // RIO_DEVID
	ct_t comptag; // RIO_COMP_TAG
	uint32_t disc; // RIO_SP_GEN_CTL
	uint32_t p_err_stat; // RIO_SPX_ERR_STAT
	uint32_t p_ctl1; // RIO_SPX_CTL
	uint32_t scratch_cm_sock; // Endpoint implementation specific.
				// Tsi721: TSI721_RIO_WHITEBOARD
};

int RIOCP_WU riocp_pe_handle_set_private(riocp_pe_handle pe, void *data);
int RIOCP_WU riocp_pe_handle_get_private(riocp_pe_handle pe, void **data);

/*
 * API functions
 */

/* Discovery and enumeration */
int RIOCP_WU riocp_mport_get_port_list(size_t *count, uint8_t **ports);
int RIOCP_WU riocp_mport_free_port_list(uint8_t **ports);
int RIOCP_WU riocp_mport_get_pe_list(riocp_pe_handle mport, size_t *count,
		riocp_pe_handle *pes[]);
int RIOCP_WU riocp_mport_free_pe_list(riocp_pe_handle *pes[]);

int RIOCP_WU riocp_pe_create_host_handle(riocp_pe_handle *handle, uint8_t mport,
		unsigned int rev, uint32_t *ct, char *name);
int RIOCP_WU riocp_pe_create_agent_handle(riocp_pe_handle *handle,
		uint8_t mport, unsigned int rev, uint32_t *ct, char *name);

int RIOCP_WU riocp_pe_discover(riocp_pe_handle pe, uint8_t port,
		riocp_pe_handle *peer, char *name);
int RIOCP_WU riocp_pe_probe(riocp_pe_handle pe, uint8_t port,
		riocp_pe_handle *peer, ct_t *comptag_in, char *name,
		bool force_ct);
int RIOCP_WU riocp_pe_verify(riocp_pe_handle pe);
riocp_pe_handle riocp_pe_peek(riocp_pe_handle pe, uint8_t port);
int RIOCP_WU riocp_pe_restore(riocp_pe_handle pe);
int riocp_pe_destroy_handle(riocp_pe_handle *pe);
int RIOCP_WU riocp_pe_get_capabilities(riocp_pe_handle pe,
		struct riocp_pe_capabilities *capabilities);
int RIOCP_WU riocp_pe_get_ports(riocp_pe_handle pe,
		struct riocp_pe_port ports[]);
int RIOCP_WU riocp_pe_port_enable(riocp_pe_handle pe, uint8_t port);
int RIOCP_WU riocp_pe_port_disable(riocp_pe_handle pe, uint8_t port);
int RIOCP_WU riocp_pe_lock(riocp_pe_handle pe, int flags);
int RIOCP_WU riocp_pe_unlock(riocp_pe_handle pe);
int RIOCP_WU riocp_pe_get_destid(riocp_pe_handle pe, did_t *did);
int RIOCP_WU riocp_pe_set_destid(riocp_pe_handle pe, did_t did);
int RIOCP_WU riocp_pe_get_comptag(riocp_pe_handle pe, ct_t *comptag);
int RIOCP_WU riocp_pe_update_comptag(riocp_pe_handle pe, uint32_t wr_did);
int RIOCP_WU riocp_pe_find_comptag(riocp_pe_handle mport, ct_t comptag,
		riocp_pe_handle *pe);

int RIOCP_WU riocp_pe_clear_enumerated(struct riocp_pe *pe);

int RIOCP_WU riocp_pe_reset_port(riocp_pe_handle sw, pe_port_t port,
bool reset_lp);

/* Routing */
int RIOCP_WU riocp_sw_get_route_entry(riocp_pe_handle sw, pe_port_t port,
		did_t did, pe_rt_val *rt_val);
int RIOCP_WU riocp_sw_set_route_entry(riocp_pe_handle sw, pe_port_t port,
		did_t did, pe_rt_val rt_val);
int RIOCP_WU riocp_pe_maint_set_route(struct riocp_pe *pe, did_t did,
		pe_port_t pnum);
int RIOCP_WU riocp_sw_alloc_mcast_mask(riocp_pe_handle sw, pe_port_t port,
		pe_rt_val *rt_val, uint32_t port_mask);
int RIOCP_WU riocp_sw_free_mcast_mask(riocp_pe_handle sw, pe_port_t port,
		pe_rt_val rt_val);
int RIOCP_WU riocp_sw_change_mcast_mask(riocp_pe_handle sw, pe_port_t port,
		pe_rt_val rt_val, uint32_t port_mask);

int RIOCP_WU riocp_get_mport_regs(int mp_num, struct mport_regs *regs);
int RIOCP_WU riocp_enable_pe(struct riocp_pe *pe, pe_port_t port);

/* Debug functions */
int riocp_pe_maint_read(riocp_pe_handle pe, uint32_t offset, uint32_t *val);
int riocp_pe_maint_write(riocp_pe_handle pe, uint32_t offset, uint32_t val);
const char *riocp_pe_get_sysfs_name(riocp_pe_handle pe);
const char *riocp_pe_get_device_name(riocp_pe_handle pe);
const char *riocp_pe_get_vendor_name(riocp_pe_handle pe);

#ifdef __cplusplus
}
#endif

#endif /* __RIOCP_PE_H__ */

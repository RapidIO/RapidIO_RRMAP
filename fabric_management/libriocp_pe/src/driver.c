/*
 * Copyright (c) 2014, Prodrive Technologies
 * Copyright (c) 2015, Integrated Device Technology
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file device.c
 * Switch driver wrapper functions
 */
#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdbool.h>

#include "inc/riocp_pe.h"
#include "inc/riocp_pe_internal.h"

#include "maint.h"
#include "driver.h"
#include "pe_mpdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Clear enumerate boundary bit in switch port
 * @param sw   Target switch PE
 * @param port Port to be cleared
 */
int riocp_pe_port_clear_enumerated(struct riocp_pe *pe, uint8_t port)
{
	uint32_t offset;
	uint32_t val;
	int ret = 0;

	offset = RIO_SPX_CTL(pe->efptr_phys, pe->efptr_phys_type, port);
	ret = riocp_pe_maint_read(pe, offset, &val);
	if (ret)
		return ret;

	val &= ~RIO_SPX_CTL_ENUM_B;
	ret = riocp_pe_maint_write(pe, offset, val);

	return ret;
}

/**
 * Clear the enumerated bits on all the switch ports
 * @param sw Target switch PE
 */
int RIOCP_WU riocp_pe_clear_enumerated(struct riocp_pe *pe)
{
	unsigned int i = 0;
	int ret = 0;

	for (i = 0; i < RIOCP_PE_PORT_COUNT(pe->cap); i++) {
		ret = riocp_pe_port_clear_enumerated(pe, i);
		if (ret) {
			RIOCP_ERROR(
					"Could not clear enumerate state of pe 0x%08x\n",
					pe->comptag);
			return ret;
		}
	}

	return 0;
}

/**
 * Check if switch port has the enumerate boundary bit set
 * @param sw   Target switch PE
 * @param port Port to be set
 * @retval 0 When port is not enumerated
 * @retval 1 When port is enumerated
 */
int riocp_pe_switch_port_is_enumerated(struct riocp_pe *sw, uint8_t port)
{
	uint32_t offset;
	uint32_t val;
	int ret = 0;

	offset = RIO_SPX_CTL(sw->efptr_phys, sw->efptr_phys_type, port);
	ret = riocp_pe_maint_read(sw, offset, &val);
	if (ret) {
		return ret;
	}

	return (val & RIO_SPX_CTL_ENUM_B) ? 1 : 0;
}

/**
 * Set enumerate boundary bit in switch port
 * @param sw   Target switch PE
 * @param port Port to be set
 */
int riocp_pe_switch_port_set_enumerated(struct riocp_pe *sw, uint8_t port)
{
	uint32_t offset;
	uint32_t val;
	int ret = 0;

	offset = RIO_SPX_CTL(sw->efptr_phys, sw->efptr_phys_type, port);
	ret = riocp_pe_maint_read(sw, offset, &val);
	if (ret) {
		return ret;
	}

	val |= RIO_SPX_CTL_ENUM_B;
	return riocp_pe_maint_write(sw, offset, val);
}

/**
 * @brief Called whenever a new device is enumerated or discovered.
 *       Initializes device, disables all event reporting.
 *       Does nothing if the device has already been initialized.
 * @param[in] pe Processing element to be initialized.
 *       on the device to this value.  If previously initialized, return the
 *       component tag value.
 */
int RIOCP_WU riocp_drv_init_pe(struct riocp_pe *pe, struct riocp_pe *peer_pe,
		char *name)
{
	return mpsw_drv_init_pe(pe, peer_pe, name);
}

/**
 * @brief Called whenever a device is being removed from the DB.
 *       Deallocates all store, if necessary.
 *       Does nothing if the device has already been destroyed.
 * @param[in] pe Processing element to be initialized.
 */
int RIOCP_WU riocp_drv_destroy_pe(struct riocp_pe *pe)
{
	return mpsw_drv_destroy_pe(pe);
}

/**
 * @brief Recover port to allow packet exchange
 *
 * @param[in] pe Processing element to be initialized.
 * @param[in] port Port number on the pe to be initialized.
 * @param[in] lp_port Port number on the link partner to be recovered
 *
 * It should be possible to exchange packets with the link partner
 * after this routine has been called.
 * If the port is error free, no action is required.
 * If errors are present, the routine may invoke local and remote resets and
 * other severe measures to recover the port.
 */
int RIOCP_WU riocp_drv_recover_port(struct riocp_pe *pe, pe_port_t port,
		pe_port_t lp_port)
{
	return mpsw_drv_recover_port(pe, port, lp_port);
}

/**
 * @brief Get the current state of a port on a PE.
 *
 * @param[in] pe Processing element to be queried
 * @param[in] port Port number on the pe to be queried.
 *       Note that RIOCP_PE_ALL_PE_PORTS is a valid parameter.
 * @param[in] did Destination ID whose routing behavior will be returned.
 * @param[in] rt_val Current routing table control value for the Destination ID;
 *
 */
int RIOCP_WU riocp_drv_get_port_state(struct riocp_pe *pe, pe_port_t port,
		struct riocp_pe_port_state_t *state)
{
	return mpsw_drv_get_port_state(pe, port, state);
}

/**
 * @brief Halt all activity on this port and power it down if possible
 *
 * @param[in] pe Processing element whose port should be stopped
 * @param[in] port Stop this port number
 *
 */
int RIOCP_WU riocp_drv_reset_port(struct riocp_pe *pe, pe_port_t port,
bool reset_lp)
{
	return mpsw_drv_reset_port(pe, port, reset_lp);
}

/**
 * @brief Set the routing behavior for a device ID on a PE+port
 *
 * @param[in] pe Processing element to be programmed
 * @param[in] port Port number on the pe to be programmed.
 *       Note that RIOCP_PE_ALL_PE_PORTS is a valid parameter.
 * @param[in] did Destination ID whose routing behavior will be changed.
 * @param[in] rt_val New routing table control value for the Destination ID;
 *
 */
int RIOCP_WU riocp_drv_set_route_entry(struct riocp_pe *pe, pe_port_t port,
		did_t did, pe_rt_val rt_val)
{
	RIOCP_DEBUG("set_route_entry %s port 0x%x did 0x%x rt_val 0x%x\n",
			pe->sysfs_name, port, did_get_value(did), rt_val);
	return mpsw_drv_set_route_entry(pe, port, did, rt_val);
}

/**
 * @brief Get the routing behavior for a device ID on a PE+port
 *
 * @param[in] pe Processing element to be queried
 * @param[in] port Port number on the pe to be queried.
 *       Note that RIOCP_PE_ALL_PE_PORTS is a valid parameter.
 * @param[in] did Destination ID whose routing behavior will be returned.
 * @param[in] rt_val Current routing table control value for the Destination ID;
 *
 */
int RIOCP_WU riocp_drv_get_route_entry(struct riocp_pe *pe, pe_port_t port,
		did_t did, pe_rt_val *rt_val)
{
	return mpsw_drv_get_route_entry(pe, port, did, rt_val);
}

/**
 * @brief Get registers from and MPORT required to open a host/agent handle
 *
 * @param[in] mp_num Master port number to be openned.
 * @param[inout] regs Registers structure containing return value.
 *
 */
int RIOCP_WU riocp_get_mport_regs(int mp_num, struct mport_regs *regs)
{
	return mpsw_get_mport_regs(mp_num, regs);
}

int RIOCP_WU riocp_enable_pe(struct riocp_pe *pe, pe_port_t port)
{
	return mpsw_enable_pe(pe, port);
}

/**
 * @brief Read a register on a PE
 *
 * @param[in] pe Processing element
 * @param[in] offset Register address to be read
 * @param[out] value Value of register
 * @param[in] rt_val Current routing table control value for the Destination ID;
 *
 */
int RIOCP_WU riocp_drv_reg_rd(struct riocp_pe *pe, uint32_t offset,
		uint32_t *val)
{
	if (RIOCP_PE_IS_HOST(pe) || RIOCP_PE_IS_MPORT(pe)) {
		return mpsw_drv_reg_rd(pe, offset, val);
	}
	return -ENOSYS;
}

/**
 * @brief Write a register on a PE
 *
 * @param[in] pe Processing element
 * @param[in] offset Register address to be written
 * @param[out] value Value to be written to the register
 * @param[in] rt_val Current routing table control value for the Destination ID;
 *
 */
int RIOCP_WU riocp_drv_reg_wr(struct riocp_pe *pe, uint32_t offset,
		uint32_t val)
{
	if (RIOCP_PE_IS_HOST(pe) || RIOCP_PE_IS_MPORT(pe)) {
		return mpsw_drv_reg_wr(pe, offset, val);
	}
	return -ENOSYS;
}

int RIOCP_WU riocp_drv_raw_reg_rd(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t offset, uint32_t *val)
{
	if (RIOCP_PE_IS_HOST(pe) || RIOCP_PE_IS_MPORT(pe)) {
		return mpsw_drv_raw_reg_rd(pe, did, hc, offset, val);
	}
	return -ENOSYS;
}

int RIOCP_WU riocp_drv_raw_reg_wr(struct riocp_pe *pe, did_t did, hc_t hc,
		uint32_t offset, uint32_t val)
{
	if (RIOCP_PE_IS_HOST(pe) || RIOCP_PE_IS_MPORT(pe)) {
		return mpsw_drv_raw_reg_wr(pe, did, hc, offset, val);
	}
	return -ENOSYS;
}

#ifdef __cplusplus
}
#endif

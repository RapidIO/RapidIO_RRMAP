/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <riocp_pe.h>
#include <riocp_pe_internal.h>
#include "tok_parse.h"
#include "driver.h"

#define MAX_LOOKUP_HOPS 16

static void usage(char *name)
{
	printf("libriocp_pe test tool\n");
	printf("Usage:\n");
	printf("    %s <host> <set destid> <local destid>\n", name);
	printf("    %s <host|agent> <discover>\n", name);
	printf("    %s <host|agent> <lookup> <route graph>\n", name);
	printf("    %s <host|agent> <connected> <route graph>\n", name);
}

static bool is_host = false;
static riocp_pe_handle mport; /* mport handle */
static riocp_pe_handle pe;    /* last pe found in lookup path */
static riocp_pe_handle path[MAX_LOOKUP_HOPS];
static riocp_pe_handle *pes = NULL;
static size_t pes_count;

static int riocp_pe_test_dump_pe(riocp_pe_handle pe)
{
	int ret;
	ct_t comptag = 0;
	uint32_t destid;
	struct riocp_pe_capabilities cap;

	ret = riocp_pe_get_capabilities(pe, &cap);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_COMPTAG, &comptag);
	if (ret)
		return ret;

	if (!RIOCP_PE_IS_SWITCH(cap)) {
		ret = riocp_pe_get_destid(pe, &destid);
		if (ret)
			return ret;

		printf("dev_id = 0x%08x, destid = %u (0x%08x), comptag 0x%08x (%s)\n",
			cap.dev_id, destid, destid, comptag, riocp_pe_get_device_name(pe));
	} else {
		printf("dev_id = 0x%08x, comptag 0x%08x (%s)\n",
			cap.dev_id, comptag, riocp_pe_get_device_name(pe));
	}

	return 0;
}

static int discover(riocp_pe_handle root, riocp_pe_handle node)
{
	int ret = 0;
	size_t j = 0;
	ct_t comptag;
	struct riocp_pe_port_state_t state;
	riocp_pe_handle next = node;

	struct riocp_pe_port *ports = NULL;
	struct riocp_pe_capabilities cap;

	ret = riocp_pe_get_capabilities(node, &cap);
	if (ret)
		return ret;

	ports = (struct riocp_pe_port *) calloc(RIOCP_PE_PORT_COUNT(cap), sizeof(*ports));
	if (ports == NULL)
		return -ENOMEM;

	ret = riocp_pe_get_ports(node, ports);
	if (ret)
		goto err;

	ret = riocp_pe_maint_read(node, RIO_COMPTAG, &comptag);
	if (ret)
		goto err;

	for (j = 0; j < RIOCP_PE_PORT_COUNT(cap); j++) {
		ret = riocp_drv_get_port_state(pe, ports[j].id, &state);
		if (ret) {
			printf("Unable to read port state id:%d, ct:%d\n",
					ports[j].id, ports[j].pe->comptag);
			continue;
		}

		if (state.port_ok) {
			next = riocp_pe_peek(node, j);
			if (next != NULL) {
				continue;
			}
			if (is_host) {
				ret = riocp_pe_probe(node, j, &next, &comptag,
						node->sysfs_name, false);
				if (ret)
					goto err;
			} else {
				ret = riocp_pe_discover(node, j, &next,
						node->sysfs_name);
				if (ret)
					goto err;
			}

			if (discover(root, next)) {
				fprintf(stderr, "could not discover port %zu: %s\n",
					j, strerror(-ret));
				ret = -1;
				goto err;
			}
		}
	}

err:
	free(ports);
	return ret;
}

static int riocp_pe_test_discover()
{
	int ret = 0;
	ct_t comptag;
	riocp_pe_handle sw;

	if (is_host) {
		ret = riocp_pe_maint_read(mport, RIO_COMPTAG, &comptag);
		if (ret) {
			goto err;
		}
		ret = riocp_pe_probe(mport, 0, &sw, &comptag,
			 mport->sysfs_name, false);
	} else {
		ret = riocp_pe_discover(mport, 0, &sw, mport->sysfs_name);
	}

	ret = discover(mport, sw);

err:
	return ret;
}

static int riocp_pe_test_mport_get_pe_list()
{
	int ret;
	size_t i;

	ret = riocp_mport_get_pe_list(mport, &pes_count, &pes);

	for (i = 0; i < pes_count; i++) {
		printf("pes[%2zu] ", i);
		riocp_pe_test_dump_pe(pes[i]);
	}

	ret = riocp_mport_free_pe_list(&pes);
	return ret;
}

static int riocp_pe_test_lookup(int argc, char **argv)
{
	int ret = 0;
	uint32_t tmp;
	uint8_t port = 0;
	hc_t hopcount = 0;
	ct_t comptag;
	char *token;
	char *pathstring = argv[3];

	if (argc < 4) {
		usage(argv[0]);
		return -EINVAL;
	}

	token = strtok(pathstring, ",");
	if (tok_parse_port_num(token, &tmp, 0)) {
		fprintf(stderr, TOK_ERR_PORT_NUM_MSG_FMT);
		return -1;
	}
	port = (uint8_t)(tmp & UINT8_MAX);

	if (is_host) {
		ret = riocp_pe_maint_read(mport, RIO_COMPTAG, &comptag);
		if (ret) {
			return -1;
		}
		ret = riocp_pe_probe(mport, port, &path[hopcount], &comptag,
				mport->sysfs_name, false);
	} else {
		ret = riocp_pe_discover(mport, port, &path[hopcount],
				mport->sysfs_name);
	}

	if (ret) {
		fprintf(stderr, "could not probe/disc: %s\n", strerror(-ret));
		return -1;
	}

	printf("[hop %2u] port: %2u, \"%s\"\n",
		hopcount, port,
		riocp_pe_get_device_name(path[hopcount]));

	while ((token = strtok(NULL, ",")) && (token != NULL)
			&& hopcount < MAX_LOOKUP_HOPS-1) {
		if (tok_parse_port_num(token, &tmp, 0)) {
			fprintf(stderr, TOK_ERR_PORT_NUM_MSG_FMT);
			return -1;
		}
		port = (uint8_t)(tmp & UINT8_MAX);

		if (is_host) {
			ret = riocp_pe_maint_read(path[hopcount], RIO_COMPTAG,
					&comptag);
			if (ret) {
				return -1;
			}
			ret = riocp_pe_probe(path[hopcount], port,
					&path[hopcount + 1],
					&comptag, path[hopcount]->sysfs_name,
					false);
		} else {
			ret = riocp_pe_discover(path[hopcount], port,
					&path[hopcount + 1],
					path[hopcount]->sysfs_name);
		}

		printf("[hop %2u] port: %2u, \"%s\"\n",
			hopcount + 1, port,
			riocp_pe_get_device_name(path[hopcount + 1]));

		if (ret) {
			fprintf(stderr, "could not probe port %u at hopcount %u: %s\n",
				port, hopcount, strerror(-ret));
			return -1;
		}
		HC_INCR(hopcount, hopcount);
	}

	pe = path[hopcount];

	return 0;
}

static int riocp_pe_test_connected()
{
	int ret;
	size_t j;
	riocp_pe_handle peer;
	ct_t comptag;
	struct riocp_pe_port_state_t state;
	struct riocp_pe_capabilities cap;
	struct riocp_pe_port ports[32]; /* STATIC ! */

	ret = riocp_pe_get_capabilities(pe, &cap);
	if (ret)
		return ret;

	if (!RIOCP_PE_IS_SWITCH(cap)) {
		fprintf(stderr, "last PE in lookup not a switch\n");
		return -1;
	}

	ret = riocp_pe_get_ports(pe, ports);
	if (ret)
		return ret;

	ret = riocp_pe_maint_read(pe, RIO_COMPTAG, &comptag);
	if (ret) {
		return ret;
	}


	for (j = 0; j < RIOCP_PE_PORT_COUNT(cap); j++) {
		ret = riocp_drv_get_port_state(pe, ports[j].id, &state);
		if (ret) {
			printf("Unable to read port state id:%d, ct:%d\n",
					ports[j].id, ports[j].pe->comptag);
			continue;
		}

		if (state.port_ok) {
			if (is_host)
				ret = riocp_pe_probe(pe, j, &peer, &comptag,
						pe->sysfs_name, false);
			else
				ret = riocp_pe_discover(pe, j, &peer,
						pe->sysfs_name);
			if (ret)
				continue;

			printf(" > port %2zu: ", j);

			ret = riocp_pe_test_dump_pe(peer);
			if (ret)
				fprintf(stderr, "Error in dumping PE information\n");
		}
	}

	return 0;
}

static int riocp_pe_test_set_destid(int argc, char **argv)
{
	int ret;
	uint32_t destid;

	if (tok_parse_did(argv[3], &destid, 0)) {
		fprintf(stderr, TOK_ERR_DID_MSG_FMT);
		return -EINVAL;
	}

	if (argc < 4) {
		usage(argv[0]);
		return -EINVAL;
	}

	ret = riocp_pe_set_destid(mport, destid);
	if (ret) {
		fprintf(stderr, "error setting destid for mport: %s\n", strerror(-ret));
		return ret;
	}
	printf("set destid to 0x%08x\n", destid);
	return 0;
}

static int riocp_pe_test_check_destid()
{
	int ret;
	uint32_t destid;

	ret = riocp_pe_get_destid(mport, &destid);
	if (ret) {
		fprintf(stderr, "error reading local destid: %s\n", strerror(-ret));
		return -EFAULT;
	}
	if (destid == 0xff) {
		fprintf(stderr, "error: destid 0x%08x not supported \n", destid);
		return -EFAULT;
	}
	printf("local destid: %u (0x%08x)\n", destid, destid);

	return 0;
}

int main(int argc, char **argv)
{
	int ret = 0;
	ct_t comptag;
	struct riocp_reg_rw_driver drv;

	if (argc < 3) {
		usage(argv[0]);
		return -1;
	}

	switch (argv[1][0]) {
	case 'h':
	case 'H':
		is_host = true;
		break;
	case 'a':
	case 'A':
		is_host = false;
		break;
	default:
		usage(argv[0]);
		return -1;
	}

	ret = riocp_pe_maint_read(mport, RIO_COMPTAG, &comptag);
	if (ret) {
		fprintf(stderr, "Cannot get component tag: %s\n", strerror(-ret));
		return -1;
	}

	if (is_host) {
		printf("Creating host handle\n");
		ret = riocp_pe_create_host_handle(&mport, 0, RIOCP_PE_LIB_REV,
				&drv ,&comptag, mport->sysfs_name);
	} else {
		printf("Creating agent handle\n");
		ret = riocp_pe_create_agent_handle(&mport, 0, RIOCP_PE_LIB_REV,
				&drv ,&comptag, mport->sysfs_name);
	}
	if (ret) {
		fprintf(stderr, "error initializing mport handle: %s\n", strerror(-ret));
		return -1;
	}

	switch (argv[2][0]) {
	case 'c':
	case 'C':
		ret = riocp_pe_test_check_destid(); if (ret) goto err;
		ret = riocp_pe_test_lookup(argc, argv); if (ret) goto err;
		ret = riocp_pe_test_connected(); if (ret) goto err;
		break;
	case 's':
	case 'S':
		ret = riocp_pe_test_set_destid(argc, argv); if (ret) goto err;
		break;
	case 'd':
	case 'D':
		ret = riocp_pe_test_check_destid(); if (ret) goto err;
		ret = riocp_pe_test_discover(); if (ret) goto err;
		ret = riocp_pe_test_mport_get_pe_list(); if (ret) goto err;
		break;
	case 'l':
	case 'L':
		ret = riocp_pe_test_check_destid(); if (ret) goto err;
		ret = riocp_pe_test_lookup(argc, argv); if (ret) goto err;
		break;
	default:
		usage(argv[0]);
		return -1;
	}

err:
	if (is_host)
		ret = riocp_pe_dot_dump("/root/riocp_pe_test_host.dot", mport);
	else
		ret = riocp_pe_dot_dump("/root/riocp_pe_test_agent.dot", mport);

	if (riocp_pe_destroy_handle(&mport)) {
		fprintf(stderr, "error destroying mport handle\n");
		ret = -1;
	}

	return ret;
}

/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * Simple double linked list implementation by steos
 */
#ifndef RIOCP_PE_LLIST__
#define RIOCP_PE_LLIST__

#ifdef __cplusplus
extern "C" {
#endif

extern int riocp_pe_llist_add(struct riocp_pe_llist_item *head, void *data);
extern int riocp_pe_llist_del(struct riocp_pe_llist_item *head, void *data);
extern void *riocp_pe_llist_find(struct riocp_pe_llist_item *head, void *data);
extern int riocp_pe_llist_free(struct riocp_pe_llist_item *list);

#ifdef __cplusplus
}
#endif

#endif /* RIOCP_PE_LLIST__ */

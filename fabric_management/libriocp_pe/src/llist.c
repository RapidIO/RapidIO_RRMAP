/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file llist.c
 * Processing element linked list
 */
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>

#include "inc/riocp_pe.h"
#include "inc/riocp_pe_internal.h"

#include "llist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Add entry to linked list
 * @param head List head
 * @param data Entry
 * @retval 0 Entry succesfull added
 * @retval -EINVAL Invalid head
 * @retval -ENOMEM No memory to allocate a new entry
 */
int RIOCP_SO_ATTR riocp_pe_llist_add(struct riocp_pe_llist_item *head, void *data)
{
	struct riocp_pe_llist_item *cur = NULL;

	if (head == NULL)
		return -EINVAL;

	cur = head;
	while (cur->next != NULL) {
		cur = cur->next;
	}

	cur->next = (struct riocp_pe_llist_item *)
		calloc(1, sizeof(struct riocp_pe_llist_item));
	if (NULL == cur->next) {
		return -ENOMEM;
	}

	cur->next->next = NULL;
	cur->next->data = data;

	return 0;
}

/**
 * Delete entry from linked list
 * @param head List head
 * @param data Entry
 * @retval 0 Entry succesfull added
 * @retval -EINVAL Invalid list
 * @retval -ENOENT Entry not in list
 */
int RIOCP_SO_ATTR riocp_pe_llist_del(struct riocp_pe_llist_item *head, void *data)
{
	struct riocp_pe_llist_item *cur, *prev = NULL, *next = NULL;

	if (head == NULL || data == NULL) {
		return -EINVAL;
	}

	cur = head;
	while (cur != NULL) {
		if (cur->data == data) {
			goto found;
		}
		prev = cur;
		cur  = cur->next;
		next = cur->next;
	}

	return -ENOENT;
found:
	if (prev != NULL) {
		prev->next = next;
	}
	free(cur);
	return 0;
}

/**
 * Search for entry in linked list
 * @param head List head
 * @param data Entry
 * @retval NULL Entry could not be found
 * @retval !NULL Entry is in list
 */
void RIOCP_SO_ATTR *riocp_pe_llist_find(struct riocp_pe_llist_item *head, void *data)
{
	struct riocp_pe_llist_item *cur = NULL;

	if (head == NULL || data == NULL) {
		return NULL;
	}

	cur = head;
	while (cur != NULL) {
		if (cur->data == data) {
			return data;
		}
		cur = cur->next;
	}

	return NULL;
}

/**
 * Free all entries in linked list
 * @param head List head
 * @retval 0 List freed succesfull
 * @retval -EINVAL Invalid head
 */
int RIOCP_SO_ATTR riocp_pe_llist_free(struct riocp_pe_llist_item *head)
{
	struct riocp_pe_llist_item *cur, *next;

	if (head == NULL) {
		return -EINVAL;
	}

	cur = head;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

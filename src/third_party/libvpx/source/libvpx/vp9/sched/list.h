/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef SCHED_LIST_H_
#define SCHED_LIST_H_

#include "vpx_config.h"

struct list_node {
  struct list_node *prev;
  struct list_node *next;
};

static INLINE void list_node_init(struct list_node *l) {
  l->prev = l->next = l;
}

static INLINE void list_insert_head(struct list_node *n, struct list_node *l) {
  n->next = l->next;
  n->prev = l;
  l->next->prev = n;
  l->next = n;
}

static INLINE void list_insert_tail(struct list_node *n, struct list_node *l) {
  n->next = l;
  n->prev = l->prev;
  l->prev->next = n;
  l->prev = n;
}

static INLINE void list_del(struct list_node *n) {
  n->prev->next = n->next;
  n->next->prev = n->prev;
  list_node_init(n);
}

#define offset_of(type, mbr) ((size_t)&(((type *)0)->mbr))

#define container_of(ptr, type, mbr)  \
          ((type *)((char *)(ptr) - offset_of(type, mbr)))

#define list_entry(ptr, type, mbr) container_of(ptr, type, mbr)

#define list_first(head) (head)->next

#define list_first_entry(head, type, mbr) container_of((head)->next, type, mbr)

#define list_last(head) (head)->prev

#define list_last_entry(head, type, mbr) container_of((head)->prev, type, mbr)

static INLINE int list_empty(struct list_node *head) {
  return head->next == head;
}

#define list_for_each(pos, list)  \
  for (pos = list->next; pos != list; pos = pos->next)

#define list_for_each_safe(pos, pos1, list)  \
  for (pos = (list)->next, pos1 = pos->next; pos != (list); pos = pos1,  \
       pos1 = pos->next)

#define list_for_each_entry(pos, type, list, mbr)  \
  for (pos = container_of((list)->next, type, mbr);  \
       (&pos->mbr) != (list);  \
       pos = container_of((pos)->mbr.next, type, mbr))

#endif  // SCHED_LIST_H_

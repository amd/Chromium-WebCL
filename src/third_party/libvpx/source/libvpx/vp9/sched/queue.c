/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <assert.h>
#include "vpx_mem/vpx_mem.h"
#include "vp9/sched/thread.h"
#include "vp9/sched/queue.h"

#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)>(y))?(y):(x))

struct queue *queue_create(int head_count, int max_tasks) {
  int i;
  struct queue *q;
  size_t size = sizeof(*q) + sizeof(struct list_node) * head_count;

  q = calloc(1, size);
  if (!q) {
    return NULL;
  }

  pthread_mutex_init(&q->qlock, NULL);
  pthread_cond_init(&q->qready, NULL);
  q->head_count = head_count;
  q->max_tasks = max_tasks;
  q->high_pri_pos = -1;
  q->low_pri_pos = head_count;
  for (i = 0; i < head_count; i++) {
    list_node_init(q->qhead + i);
  }

  return q;
}

void queue_delete(struct queue *q) {
  pthread_mutex_destroy(&q->qlock);
  pthread_cond_destroy(&q->qready);
  free(q);
}

int queue_push(struct queue *q, struct list_node *node, int prio) {
  struct list_node *h;
  assert(prio < q->head_count);

  pthread_mutex_lock(&q->qlock);
  h = q->qhead + prio;
  q->task_count++;
  list_insert_tail(node, h);
  q->high_pri_pos = MAX(q->high_pri_pos, prio);
  q->low_pri_pos = MIN(q->low_pri_pos, prio);
  pthread_mutex_unlock(&q->qlock);
  pthread_cond_signal(&q->qready);

  return 0;
}

struct list_node *queue_pop(struct queue *q) {
  int i;
  struct list_node *n = NULL;

  pthread_mutex_lock(&q->qlock);

  while (q->task_count <= 0 && !q->is_stop) {
    pthread_cond_wait(&q->qready, &q->qlock);
  }

  if (q->is_stop) {
    goto out;
  }

  for (i = q->high_pri_pos; i >= q->low_pri_pos; i--) {
    if (list_empty(q->qhead + i))
      continue;
    n = list_first(q->qhead + i);
    if (n) {
      list_del(n);
      q->task_count--;
      q->high_pri_pos = i;
      break;
    }
  }

out:
  pthread_mutex_unlock(&q->qlock);

  return n;
}

struct list_node *queue_try_pop(struct queue *q) {
  int i;
  struct list_node *n = NULL;

  pthread_mutex_lock(&q->qlock);

  if (q->task_count <= 0) {
    pthread_mutex_unlock(&q->qlock);
    return NULL;
  }

  for (i = q->high_pri_pos; i >= q->low_pri_pos; i--) {
    if (list_empty(q->qhead + i))
      continue;
    n = list_first(q->qhead + i);
    if (n) {
      list_del(n);
      q->task_count--;
      q->high_pri_pos = i;
      break;
    }
  }
  pthread_mutex_unlock(&q->qlock);

  return n;
}

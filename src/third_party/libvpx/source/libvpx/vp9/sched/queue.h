/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef SCHED_QUEUE_H_
#define SCHED_QUEUE_H_

#include "vp9/sched/list.h"

struct queue {
  pthread_mutex_t qlock;
  pthread_cond_t qready;
  int max_tasks;
  int head_count;
  int is_stop;
  int high_pri_pos;
  int low_pri_pos;
  int task_count;
  struct list_node qhead[];
};

struct queue *queue_create(int head_count, int max_tasks);

void queue_delete(struct queue *q);

int queue_push(struct queue *q, struct list_node *node, int prio);

struct list_node *queue_pop(struct queue *q);

struct list_node *queue_try_pop(struct queue *q);

static INLINE void queue_stop(struct queue *q) {
  pthread_mutex_lock(&q->qlock);
  q->is_stop = 1;
  pthread_mutex_unlock(&q->qlock);
  pthread_cond_signal(&q->qready);
}

#endif  // SCHED_QUEUE_H_

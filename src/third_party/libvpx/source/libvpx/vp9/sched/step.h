/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SCHED_STEP_H_
#define SCHED_STEP_H_

#include "vpx_config.h"

struct task;

enum StepType {
  STEP_KEEP  = (1 << 0),
  STEP_SPLIT = (1 << 1),
  STEP_SYNC  = (1 << 2),
  STEP_MERGE = (1 << 3),
};

struct task_steps_pool;

struct task_step {
  const char *name;
  enum StepType type;
  int dev_type;
  int step_nr;
  int next_steps_map;
  int next_count;
  int prev_steps_map;
  int prev_count;
  int (*process)(struct task *tsk, struct task_step *step, int dev_type);

  struct task_steps_pool *pool;
  void *priv;
};

struct task_steps_pool {
  int steps_count;
  struct task_step *steps;
};

static INLINE int step_is_last(struct task_step *step) {
  if (step->step_nr >= step->pool->steps_count - 1)
    return 1;
  return 0;
}

struct task_steps_pool *task_steps_pool_create(struct task_step *steps,
                                               int count);

void task_steps_pool_delete(struct task_steps_pool *pool);

void task_step_for_each_prev(struct task_step *step,
                             int (*fn)(struct task_step *step, void *args),
                             void *args);

void task_step_for_each_next(struct task_step *step,
                             int (*fn)(struct task_step *step, void *args),
                             void *args);

#endif  // SCHED_STEP_H_

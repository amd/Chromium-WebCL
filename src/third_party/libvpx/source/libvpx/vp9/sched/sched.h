/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SCHED_SCHED_H_
#define SCHED_SCHED_H_

#include "vp9/sched/task.h"
#include "vp9/sched/device.h"
#include "vp9/sched/atomic.h"

#define MAX_STEPS 8
#define MAX_PRI_LEVELS 8

/*
 * GPU & DSP should have better power efficiency than CPU
 */
enum SchedStrategy {
  SCHED_POWER_FIRST,
  SCHED_PERF_FIRST,
  SCHED_BALANCE,
};

struct scheduler {
  enum SchedStrategy strategy;
  struct list_node devs_list;
  int devs_count;

  int blk_cpu_count;
  int blk_gpu_count;

  atomic_t tasks_count;
};

#define sched_for_each_dev(pos, sched)  \
  list_for_each_entry(pos, struct device, &sched->devs_list, entry)

struct scheduler *scheduler_create(void);

void scheduler_delete(struct scheduler *sched);

static INLINE void scheduler_set_strategy(struct scheduler *sched,
                                          int strategy) {
  sched->strategy = (enum SchedStrategy)strategy;
}

static INLINE int scheduler_get_strategy(struct scheduler *sched) {
  return (int)sched->strategy;
}

void scheduler_add_devices(struct scheduler *sched,
                           struct device *devs,
                           int devs_count);

static INLINE struct device *scheduler_get_dev(struct scheduler *sched,
                                               int dev_type) {
  struct device *dev;

  sched_for_each_dev(dev, sched) {
    if (dev->type & dev_type)
      return dev;
  }

  return NULL;
}

void scheduler_sched_task(struct scheduler *sched, struct task *tsk);

void scheduler_finish_task(struct scheduler *sched, struct task *tsk);

void scheduler_stop(struct scheduler *sched);

#endif  // SCHED_SCHED_H_

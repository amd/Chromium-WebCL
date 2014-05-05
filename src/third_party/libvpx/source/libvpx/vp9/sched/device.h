/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SCHED_DEVICDE_H_
#define SCHED_DEVICDE_H_

#include "vp9/sched/task.h"
#include "vp9/sched/queue.h"
#include "vp9/sched/atomic.h"

struct task;

enum DevType {
  DEV_CPU = (1 << 0),
  DEV_GPU = (1 << 1),
  DEV_DSP = (1 << 2),
};

enum DevStatus {
  DEV_STAT_ENABLED = (1 << 0),
  DEV_STAT_DISABLED = (1 << 1),
};

struct thread {
  pthread_t thread_id;
  struct device *dev;
  int nr;
  int line;
};

struct device {
  enum DevType type;
  int threads_count;
  int max_queue_tasks;

  enum DevStatus status;
  struct thread *threads;
  struct list_node entry;
  struct queue *q;
  struct scheduler *sched;
  atomic_t tasks_count;

  int quit;
  int (*init)(struct device *dev);
  int (*fini)(struct device *dev);
};

#define DEV_TO_SCHED(dev) (dev->sched)

static INLINE void device_enable(struct device *dev) {
  dev->status = DEV_STAT_ENABLED;
}

static INLINE void device_disable(struct device *dev) {
  dev->status = DEV_STAT_DISABLED;
}

static INLINE int device_is_enabled(struct device *dev) {
  return (dev->status & DEV_STAT_ENABLED);
}

static INLINE int device_status(struct device *dev) {
  return dev->status;
}

static INLINE int device_qlen(struct device *dev) {
  return dev->q->task_count;
}

int device_init(struct device *dev);

int device_fini(struct device *dev);

int device_push_task(struct device *dev, struct task *tsk);

int cpu_device_exec(struct device *dev);

#endif // SCHED_DEVICDE_H_

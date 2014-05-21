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
#include "vp9/sched/sched.h"
#include "vp9/sched/sleep.h"
#include "vp9/sched/debug.h"

struct scheduler *scheduler_create(void) {
  struct scheduler *sched;

  sched = (struct scheduler *)vpx_calloc(1, sizeof(*sched));
  if (!sched) {
    return NULL;
  }

  list_node_init(&sched->devs_list);
  atomic_init(&sched->tasks_count, 0);
  return sched;
}

void scheduler_delete(struct scheduler *sched) {
  struct list_node *pos, *pos1;
  list_for_each_safe(pos, pos1, &sched->devs_list) {
    struct device *dev = list_entry(pos, struct device, entry);
    list_del(pos);
    while (device_fini(dev) < 0) {
      msleep(1);
    }
    vpx_free(dev);
  }

  atomic_fini(&sched->tasks_count);
  vpx_free(sched);
}

void scheduler_add_devices(struct scheduler *sched,
                           struct device *devs,
                           int devs_count) {
  int i;

  for (i = 0; i < devs_count; i++) {
    struct device *dev = (struct device *)vpx_malloc(sizeof(*dev));
    assert(dev != NULL);
    *dev = devs[i];
    list_insert_tail(&dev->entry, &sched->devs_list);
    sched->devs_count++;
    dev->sched = sched;
    device_init(dev);
  }
}


typedef void (*sched_fn)(struct scheduler *sched, struct task *tsk);

#define sched_for_each_dev(pos, sched)  \
  list_for_each_entry(pos, struct device, &sched->devs_list, entry)

#define MAX_QLEN 1024

static void sched_power_first(struct scheduler *sched, struct task *tsk) {
  struct device *dev, *pos;
  struct task_step *step;
  int qlen = MAX_QLEN;
  int gpu_or_dsp = 0;
  int seleced_gpu_or_dsp = 0;

  step = TASK_TO_STEP(tsk, tsk->curr_step);
  if (step->dev_type & (DEV_DSP | DEV_GPU)) {
    gpu_or_dsp = 1;
  }

  dev = NULL;
  if (tsk->dev_type) {
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos) && (tsk->dev_type & pos->type)) {
        dev = pos;
        break;
      }
    }
  } else {
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos)
          && gpu_or_dsp
          && (pos->type & (DEV_DSP | DEV_GPU))) {
        if (seleced_gpu_or_dsp) {
          if (device_qlen(pos) < qlen && step->dev_type & pos->type) {
            qlen = device_qlen(pos);
            dev = pos;
          }
        } else if (step->dev_type & pos->type) {
          dev = pos;
          qlen = device_qlen(pos);
          seleced_gpu_or_dsp = 1;
        }
      } else if (pos->type & DEV_CPU) {
        dev = pos;
        qlen = device_qlen(pos);
      }
    }
  }

  if (dev) {
    device_push_task(dev, tsk);
  }
}

static void sched_perf_first(struct scheduler *sched, struct task *tsk) {
  struct device *dev, *pos;
  struct task_step *step;
  int qlen = MAX_QLEN;
  step = TASK_TO_STEP(tsk, tsk->curr_step);

  dev = NULL;

  if (tsk->dev_type) {
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos) && (tsk->dev_type & pos->type)) {
        dev = pos;
        break;
      }
    }
  } else {
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos) && (step->dev_type & pos->type)) {
        if (device_qlen(pos) < qlen) {
           qlen = device_qlen(pos);
           dev = pos;
        }
      }
    }
  }

  if (dev) {
    // scheduler control counter
    if (dev->type == DEV_CPU)
      sched->blk_cpu_count++;
    else
      sched->blk_gpu_count++;
    device_push_task(dev, tsk);
  }
}

static void sched_balance(struct scheduler *sched, struct task *tsk) {
  struct device *dev, *pos;
  struct task_step *step;
  int qlen = MAX_QLEN;
  static int weight[] = {
    2, 1, 1,
  };

  dev = NULL;
  if (tsk->dev_type) {
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos) && (tsk->dev_type & pos->type)) {
        dev = pos;
        break;
      }
    }
  } else { 
    step = TASK_TO_STEP(tsk, tsk->curr_step);
    sched_for_each_dev(pos, sched) {
      if (device_is_enabled(pos) && (step->dev_type & pos->type)) {
        int len = device_qlen(pos) * weight[pos->type];
        if (len  < qlen) {
          qlen = len;
          dev = pos;
        }
      }
    }
  }

  if (dev)
    device_push_task(dev, tsk);
}

static sched_fn sched_fn_array[] = {
  sched_power_first,
  sched_perf_first,
  sched_balance,
};

void scheduler_sched_task(struct scheduler *sched, struct task *tsk) {
  atomic_inc(&sched->tasks_count);
  sched_fn_array[sched->strategy](sched, tsk);
}

void scheduler_finish_task(struct scheduler *sched, struct task *tsk) {
  atomic_dec(&sched->tasks_count);
}

void scheduler_stop(struct scheduler *sched) {
  while (atomic_get(&sched->tasks_count) > 0) {
    msleep(1);
  }
}

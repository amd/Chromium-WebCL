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
#include "vp9/sched/sched.h"
#include "vp9/sched/device.h"
#include "vp9/sched/task.h"
#include "vp9/sched/debug.h"

#if __ANDROID__ ||  _BSD_SOURCE || _SVID_SOURCE || _XOPEN_SOURCE
#include <unistd.h>
static INLINE void vp9_nice(int inc) {
  nice(inc);
}
#else
static INLINE void vp9_nice(int inc) {
}
#endif

struct step_fn_args {
  struct task *tsk;
  struct device *dev;
  int mirror;
};

static int step_fn(struct task_step *step, void *args) {
  struct step_fn_args *fn_args = (struct step_fn_args *)args;
  struct task *tsk = fn_args->tsk;
  struct scheduler *sched = fn_args->dev->sched;

  if (step->type & STEP_SYNC) {
    if (task_is_mirror(tsk)) {
      struct task *orig = task_get_orig(tsk);
      task_cache_put_task(tsk->cache, tsk);
      tsk = orig;
    }
  }

  if (step->type & STEP_MERGE) {
    if (task_has_subs(tsk->parent)) {
      return 0;
    }
  }

  if (fn_args->mirror) {
    tsk = task_clone(tsk);
  }

  if (task_need_wait(tsk, step->step_nr)) {
    return 0;
  }

  tsk->curr_step = step->step_nr;
  scheduler_sched_task(sched, tsk);

  return 0;
}

static void sched_task_next_step(struct task *tsk, struct device *dev) {
  struct task_step *step, *next_step;
  struct step_fn_args args;

  step = TASK_TO_STEP(tsk, tsk->curr_step);
  if (step_is_last(step)) {
    scheduler_finish_task(dev->sched, tsk);
    task_finish(tsk);
    return;
  }

  next_step = TASK_TO_STEP(tsk, tsk->curr_step + 1);
  if (next_step->type & STEP_MERGE) {
    int count = atomic_dec(&tsk->parent->sub_count);
    if (count == 0) {
      tsk = tsk->parent;
      tsk->curr_step = next_step->step_nr;
      scheduler_sched_task(dev->sched, tsk);
    }
  } else {
    args.tsk = tsk;
    args.dev = dev;
    if (step->next_count > 1) {
      args.mirror = 1;
    } else {
      args.mirror = 0;
    }
  
    task_step_for_each_next(step, step_fn, &args);
  }
}

static void sched_sub_tasks(struct task *tsk, struct device *dev, int count) {
  struct task *sub;
  task_for_each_sub(sub, tsk) {
    sched_task_next_step(sub, dev);

    // In order to reduce a lock, we need check count here
    if (--count == 0) return;
  }
}

static void process_task(struct task *tsk, struct device *dev) {
  int rv;
  struct task_step *step;

  step = TASK_TO_STEP(tsk, tsk->curr_step);
  rv = step->process(tsk, step, dev->type);
  assert(rv >= 0);

  task_finish_step(tsk, step->step_nr);
  if (rv > 0) {  // multiple sub-tasks
    sched_sub_tasks(tsk, dev, rv);
  } else  {
    sched_task_next_step(tsk, dev);
  }

  atomic_dec(&dev->tasks_count);
  scheduler_finish_task(dev->sched, tsk);
}

static THREADFN thread_fn(void *arg) {
  struct thread *thr = (struct thread *)arg;
  struct device *dev = thr->dev;
  struct list_node *n;

  vp9_nice(-10);

  for ( ; ; ) {
    n = queue_pop(dev->q);
    if (n) {
      struct task *tsk = list_entry(n, struct task, entry);
      process_task(tsk, dev);
    } else {
      return THREAD_RETURN(NULL);
    }
  }

  return THREAD_RETURN(NULL);
}

static int device_start_threads(struct device *dev) {
  int i;
  struct thread *t;

  for (i = 0; i < dev->threads_count; i++) {
    t = dev->threads + i;
    t->nr = i;
    pthread_create(&t->thread_id, NULL, thread_fn, (void *)t);
  }

  return 0;
}

static int device_common_init(struct device *dev) {
  int i;
  int rv = 0;

  atomic_init(&dev->tasks_count, 0);
  dev->q = queue_create(MAX_PRI_LEVELS, dev->max_queue_tasks);
  if (!dev->q) {
    goto out;
  }

  dev->threads = (struct thread*)vpx_calloc( 1,
                                    sizeof(struct thread) * dev->threads_count);
  if (!dev->threads) {
    rv = -1;
    goto release_q;
  }

  for (i = 0; i < dev->threads_count; i++) {
    dev->threads[i].dev = dev;
  }

  rv = device_start_threads(dev);
  if (rv == 0)
    goto out;

  vpx_free(dev->threads);
release_q:
  queue_delete(dev->q);
out:
  return rv;
}

static int device_common_fini(struct device *dev) {
  int i;

  if (atomic_get(&dev->tasks_count) > 0) {
    return -1;
  }

  atomic_fini(&dev->tasks_count);
  dev->quit = 1;
  for (i = 0; i < dev->threads_count; i++) {
    queue_stop(dev->q);
  }

  for (i = 0; i < dev->threads_count; i++) {
    queue_stop(dev->q);
    pthread_join(dev->threads[i].thread_id, NULL);
  }

  queue_delete(dev->q);
  vpx_free(dev->threads);

  return 0;
}

int device_init(struct device *dev) {
  int rv = 0;

  if (dev->init) {
    rv = dev->init(dev);
    if (rv < 0) {
      return rv;
    }
  }

  return device_common_init(dev);
}

int device_fini(struct device *dev) {
  int rv;

  rv = device_common_fini(dev);
  if (rv < 0) {
    return rv;
  }

  if (dev->fini)
    return dev->fini(dev);
  return 0;
}

int device_push_task(struct device *dev, struct task *tsk) {
  int rv;

  rv = queue_push(dev->q, &tsk->entry, tsk->prio);
  if (rv == 0) {
    atomic_inc(&dev->tasks_count);
  }

  return rv;
}

int cpu_device_exec(struct device *dev) {
  struct list_node *n;

  while ((n = queue_try_pop(dev->q))) {
    struct task *tsk = list_entry(n, struct task, entry);
    process_task(tsk, dev);
  }

  return 0;
}

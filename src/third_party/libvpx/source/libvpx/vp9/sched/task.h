/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef SCHED_TASK_H_
#define SCHED_TASK_H_

#include "vpx_mem/vpx_mem.h"
#include "vp9/sched/thread.h"
#include "vp9/sched/list.h"
#include "vp9/sched/step.h"
#include "vp9/sched/debug.h"
#include "vp9/sched/atomic.h"

struct task_cache;

struct task {
  const char *name;
  int prio;

  pthread_mutex_t finish_mutex;
  pthread_cond_t finish_cond;
  int finished;

  struct task *parent;
  struct task *root;
  atomic_t sub_count;
  struct list_node sub_list;
  struct list_node sub_entry;
  struct list_node entry;
  struct task_cache *cache;
  pthread_mutex_t lock;

  // the NR bit of curr_steps_map is NR step status
  int curr_steps_map;
  // the NR bit of finished_steps_map is NR step status
  int finished_steps_map;
  int curr_step;

  int is_mirror;
  struct task *orig;
  void *priv;
  void (*dtor)(struct task *tsk);
};

#define task_for_each_sub(pos, tsk)  \
  list_for_each_entry(pos, struct task, &tsk->sub_list, sub_entry)

struct task_cache {
  pthread_mutex_t lock;
  int max_task_count;
  int idle_task_count;
  struct task_steps_pool *pool;
  struct list_node task_list;
};

#define TASK_TO_STEPS_POOL(task) (task->root->cache->pool)
#define TASK_TO_STEP(task, nr) (TASK_TO_STEPS_POOL(task)->steps + nr)

struct task_cache *task_cache_create(int max_tasks,
                                     struct task_steps_pool *pool);

void task_cache_delete(struct task_cache *cache);

struct task *task_cache_get_task(struct task_cache *cache,
                                 const char *task_name, int prio);

void task_cache_put_task(struct task_cache *cache, struct task *tsk);

static INLINE const char *task_get_name(struct task *tsk) {
  return tsk->name;
}

static INLINE void task_set_name(struct task *tsk, const char *name) {
  tsk->name = name;
}

static INLINE int task_get_prio(struct task *tsk) {
  return tsk->prio;
}

static INLINE void task_set_prio(struct task *tsk, int prio) {
  tsk->prio = prio;
}

struct task *task_create_sub(struct task *tsk);

void task_delete_sub(struct task *sub_tsk);

static INLINE struct task *task_clone(struct task *tsk) {
  struct task *mirror;

  mirror = task_cache_get_task(tsk->cache, NULL, tsk->prio);
  if (!mirror)
    return NULL;
  *mirror = *tsk;
  mirror->is_mirror = 1;
  mirror->orig = tsk;

  return mirror;
}

static INLINE struct task *task_get_orig(struct task *tsk) {
  return tsk->orig;
}

static INLINE int task_is_mirror(struct task *tsk) {
  return tsk != tsk->orig;
}

static INLINE int task_is_last_sub(struct task *tsk, struct task *sub) {
  return (sub->sub_entry.prev == &tsk->sub_list);
}

static INLINE void task_finish_step(struct task *tsk, int step_nr) {
  struct task *orig = tsk->orig;
  struct task *sub;
  orig->finished_steps_map |= (1 << step_nr);

  task_for_each_sub(sub, tsk) {
    sub->orig->finished_steps_map |= (1 << step_nr);
  }
}

int task_need_wait(struct task *tsk, int step_nr);

static INLINE int task_has_subs(struct task *tsk) {
  int rv;
  pthread_mutex_lock(&tsk->lock);
  rv = !list_empty(&tsk->sub_list);
  pthread_mutex_unlock(&tsk->lock);
  return rv;
}

static INLINE int task_sync(struct task *tsk) {
  pthread_mutex_lock(&tsk->finish_mutex);
  while (!tsk->finished) {
    pthread_cond_wait(&tsk->finish_cond, &tsk->finish_mutex);
  }
  pthread_mutex_unlock(&tsk->finish_mutex);

  return 0;
}

static INLINE void task_param_free(struct task *tsk) {
  vpx_free(tsk->priv);
}

static INLINE int task_finish(struct task *tsk) {
  pthread_mutex_lock(&tsk->finish_mutex);
  tsk->finished = 1;
  pthread_mutex_unlock(&tsk->finish_mutex);
  pthread_cond_signal(&tsk->finish_cond);

  return 0;
}

#endif  // SCHED_TASK_H_

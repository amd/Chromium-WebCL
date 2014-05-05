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
#include "vp9/sched/task.h"

struct task_cache *task_cache_create(int max_tasks,
                                     struct task_steps_pool *pool) {
  struct task_cache *cache;

  cache = (struct task_cache *)vpx_calloc(1, sizeof(*cache));
  if (!cache) {
    return NULL;
  }

  pthread_mutex_init(&cache->lock, NULL);
  cache->max_task_count = max_tasks;
  list_node_init(&cache->task_list);
  cache->pool = pool;

  return cache;
}

void task_cache_delete(struct task_cache *cache) {
  struct list_node *pos, *pos1;
  pthread_mutex_destroy(&cache->lock);
  list_for_each_safe(pos, pos1, &cache->task_list) {
    struct task *tsk = list_entry(pos, struct task, entry);
    pthread_mutex_destroy(&tsk->lock);
    pthread_mutex_destroy(&tsk->finish_mutex);
    pthread_cond_destroy(&tsk->finish_cond);
    vpx_free(tsk);
  }
  vpx_free(cache);
}

static INLINE void task_init(struct task *tsk, const char *name,
                             int prio, struct task_cache *cache) {
  tsk->name = name;
  tsk->parent = tsk;
  tsk->root = tsk;
  tsk->prio = prio;
  list_node_init(&tsk->sub_list);
  list_node_init(&tsk->sub_entry);
  tsk->cache = cache;
  tsk->curr_steps_map = 0;
  tsk->finished_steps_map = 0;
  tsk->curr_step = 0;
  tsk->priv = NULL;
  tsk->is_mirror = 0;
  tsk->orig = tsk;
  tsk->finished = 0;
  tsk->dtor = NULL;
  atomic_init(&tsk->sub_count, 0);
}

struct task *task_cache_get_task(struct task_cache *cache,
                                 const char *task_name, int prio) {
  struct task *tsk;
  pthread_mutex_lock(&cache->lock);
  if (!list_empty(&cache->task_list)) {
    tsk = list_first_entry(&cache->task_list, struct task, entry);
    list_del(&tsk->entry);
    task_init(tsk, task_name, prio, cache);
    cache->idle_task_count--;
    pthread_mutex_unlock(&cache->lock);
    return tsk;
  }
  pthread_mutex_unlock(&cache->lock);

  tsk = (struct task *)vpx_calloc(1, sizeof(*tsk));
  if (!tsk) {
    return NULL;
  }

  pthread_mutex_init(&tsk->lock, NULL);
  pthread_mutex_init(&tsk->finish_mutex, NULL);
  pthread_cond_init(&tsk->finish_cond, NULL);

  task_init(tsk, task_name, prio, cache);
  return tsk;
}

static void __task_cache_put_task(struct task_cache *cache, struct task *tsk) {
  assert(cache == tsk->cache);

  if (tsk->dtor)
    tsk->dtor(tsk);

  list_insert_tail(&tsk->entry, &cache->task_list);
  cache->idle_task_count++;
}

void task_cache_put_task(struct task_cache *cache, struct task *tsk) {
  struct list_node *pos, *pos1;
  pthread_mutex_lock(&cache->lock);

  if (!tsk->is_mirror) {
    list_for_each_safe(pos, pos1, &tsk->sub_list) {
      struct task *sub = list_entry(pos, struct task, sub_entry);
      list_del(&sub->sub_entry);
      __task_cache_put_task(cache, sub);
    }
  }
  __task_cache_put_task(cache, tsk);
  pthread_mutex_unlock(&cache->lock);
}

struct task *task_create_sub(struct task *tsk) {
  struct task *sub;

  sub = task_cache_get_task(tsk->cache, NULL, tsk->prio);
  if (!sub) {
    return NULL;
  }

  pthread_mutex_lock(&tsk->lock);
  sub->parent = tsk;
  sub->root = tsk->root;
  sub->orig = sub;
  list_insert_tail(&sub->sub_entry, &tsk->sub_list);
  atomic_inc(&tsk->sub_count);
  pthread_mutex_unlock(&tsk->lock);
  return sub;
}

void task_delete_sub(struct task *sub_tsk) {
  struct task *parent = sub_tsk->parent;
  pthread_mutex_lock(&parent->lock);
  list_del(&sub_tsk->sub_entry);
  pthread_mutex_unlock(&parent->lock);

  task_cache_put_task(sub_tsk->cache, sub_tsk);
}

int task_need_wait(struct task *tsk, int step_nr) {
  struct task_step *step;

  step = TASK_TO_STEP(tsk, step_nr);
  return (step->prev_steps_map & tsk->orig->finished_steps_map)
             != step->prev_steps_map;
}



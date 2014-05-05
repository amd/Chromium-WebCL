/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vp9/sched/step.h"
#include "vp9/decoder/vp9_loopfilter_step.h"
#include "vp9/decoder/vp9_decodeframe_recon.h"
#include "vp9/decoder/vp9_loopfilter_recon.h"
#include "vpx_mem/vpx_mem.h"

static int vp9_lf_block_cpu(struct task *tsk,
                            struct task_step *step) {
  struct lf_blk_param *param = tsk->priv;
  int mi_row, mi_col;

  for (mi_row = param->start_mi_row;
       mi_row < param->end_mi_row;
       mi_row += param->step_length) {
    for (mi_col = 0; mi_col < param->cm->mi_cols; mi_col += MI_BLOCK_SIZE) {
      if (param->upper) {
        struct lf_blk_param *up = param->upper->priv;
        pthread_mutex_lock(&up->mutex);
        while (up->mi_row < mi_row && 
               up->mi_col < mi_col + MI_BLOCK_SIZE*2) {
          pthread_cond_wait(&up->cond, &up->mutex);
        }
        pthread_mutex_unlock(&up->mutex);
      }

      vp9_loop_filter_block(param->frame_buffer, param->cm,
                            param->xd, mi_row, mi_col,
                            param->y_only);

      pthread_mutex_lock(&param->mutex);
      param->mi_row = mi_row;
      param->mi_col = mi_col;
      pthread_mutex_unlock(&param->mutex);
      pthread_cond_signal(&param->cond);
    }
  }

  pthread_mutex_lock(&param->mutex);
  param->mi_row += MI_BLOCK_SIZE;
  param->mi_col += MI_BLOCK_SIZE;
  pthread_mutex_unlock(&param->mutex);
  pthread_cond_signal(&param->cond);

  return 0;
}

static int vp9_lf_block(struct task *tsk,
                        struct task_step *step,
                        int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);

  return vp9_lf_block_cpu(tsk, step);
}


/**
 * This is for loopfilter WPP, so there is only ONE step
 */
static struct task_step lf_steps[] = {
  {
    "vp9_lf_block",               // name
    STEP_KEEP,                    // type
    DEV_CPU,                      // dev_type
    0,                            // step_nr
    0,                            // next_steps_map
    0,                            // next_count
    0,                            // prev_steps_map
    0,                            // prev_count
    vp9_lf_block,                 // process
    NULL,                         // pool
    NULL                          // priv
  },
};

#define ARRAY_SZ(arr) (sizeof(arr) / sizeof(arr[0]))

struct task_steps_pool *lf_steps_pool_get(void) {
  return task_steps_pool_create(lf_steps, ARRAY_SZ(lf_steps));
}

struct lf_blk_param *lf_blk_param_get(struct task *tsk) {
  struct lf_blk_param *param;

  param = vpx_calloc(1, sizeof(*param));
  tsk->priv = param;

  if (param) {
    pthread_mutex_init(&param->mutex, NULL);
    pthread_cond_init(&param->cond, NULL);
  }
  return param;
}

void lf_blk_param_put(struct task *tsk, struct lf_blk_param *param) {
  pthread_mutex_destroy(&param->mutex);
  pthread_cond_destroy(&param->cond);
  vpx_free(param);
}

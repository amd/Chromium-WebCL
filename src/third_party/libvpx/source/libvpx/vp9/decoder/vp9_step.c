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
#include "vp9/decoder/vp9_step.h"
#include "vp9/decoder/vp9_entropy_step.h"
#include "vp9/decoder/vp9_decodeframe_recon.h"
#include "vp9/decoder/vp9_append.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_calcu.h"

static INLINE int sched_tiles_entropy_dec(struct task *tsk,
                                          struct task_step *step) {
  struct frame_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_COMMON *const cm = &pbi->common;
  const int tile_cols = 1 << cm->log2_tile_cols;
  const int tile_rows = 1 << cm->log2_tile_rows;
  int tile_row, tile_col;

  for (tile_row = 0; tile_row < tile_rows; tile_row++) {
    for (tile_col = 0; tile_col < tile_cols; tile_col++) {
      struct task *en_task;
      struct entropy_dec_param *en_param;
      en_task = task_create_sub(tsk);
      assert(en_task);
      en_param = entropy_dec_param_get(en_task);
      assert(en_param);
      en_param->pbi = pbi;
      en_param->tile_col = tile_col;
    }
  }
  return tile_cols;
}

static INLINE int vp9_frame_entropy_dec_cpu(struct task *tsk,
                                            struct task_step *step) {
  return 0;
}

static int vp9_frame_entropy_dec(struct task *tsk,
                                 struct task_step *step,
                                 int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);
  vp9_frame_entropy_dec_cpu(tsk, step);
  return sched_tiles_entropy_dec(tsk, step);
}

static int vp9_tile_entropy_dec(struct task *tsk,
                                struct task_step *step,
                                int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);

  return 0;
}

static INLINE int vp9_mt_task_allocating_ocl(struct task *tsk,
                                             struct task_step *step) {
  struct frame_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_COMMON *const cm = &pbi->common;
  const int tile_cols = 1 << cm->log2_tile_cols;
  const int tile_rows = 1 << cm->log2_tile_rows;
  int tile_row, tile_col;

  struct task *gpu_task;
  struct entropy_dec_param *gpu_param;

  for (tile_row = 0; tile_row < tile_rows; tile_row++) {
    for (tile_col = 0; tile_col < tile_cols; tile_col++) {
      struct task *en_task;
      struct entropy_dec_param *en_param;
      en_task = task_create_sub(tsk);
      task_force_dev(en_task, DEV_CPU);
      assert(en_task);
      en_param = entropy_dec_param_get(en_task);
      assert(en_param);
      en_param->pbi = pbi;
      en_param->tile_col = tile_col;
    }
  }

  gpu_task = task_create_sub(tsk);
  task_force_dev(gpu_task, DEV_GPU);
  assert(gpu_task);
  gpu_param = entropy_dec_param_get(gpu_task);
  assert(gpu_param);
  gpu_param->pbi = pbi;
  gpu_param->tile_col = tile_cols;

  return tile_cols + 1;
}

static int vp9_mt_task_allocating(struct task *tsk,
                                  struct task_step *step,
                                  int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);

  return vp9_mt_task_allocating_ocl(tsk, step);
}

static int vp9_inter_pred_calcu_ocl(struct task *tsk,
                                    struct task_step *step,
                                    int dev_gpu) {
  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_DECODER_RECON *decoder_recon = pbi->decoder_recon + param->tile_col;
  int tile_col = param->tile_col;
  TileInfo *tile = &decoder_recon->tile;

  decode_tile_recon_inter_calcu_ocl(pbi, tile, &decoder_recon->r, tile_col, dev_gpu);

  return 0;
}

static int vp9_inter_pred_calcu(struct task *tsk,
                                struct task_step *step,
                                int dev_type) {
  int dev_gpu = 0;
  if (dev_type == DEV_GPU)
    dev_gpu = 1;

  vp9_inter_pred_calcu_ocl(tsk, step, dev_gpu);

  return 0;
}

static int vp9_inter_pred_cpu(struct task *tsk,
                              struct task_step *step) {
  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_DECODER_RECON *decoder_recon = pbi->decoder_recon + param->tile_col;
  int tile_col = param->tile_col;
  TileInfo *tile = &decoder_recon->tile;

  decode_tile_recon_inter(pbi, tile, &decoder_recon->r, tile_col);

  decode_tile_recon_inter_transform(pbi, tile, &decoder_recon->r, tile_col);

  return 0;
}

static int vp9_inter_pred_prepare_cpu(struct task *tsk,
                                      struct task_step *step) {
  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_DECODER_RECON *decoder_recon = pbi->decoder_recon + param->tile_col;
  int tile_col = param->tile_col;
  TileInfo *tile = &decoder_recon->tile;

  decode_tile_recon_inter_prepare_ocl(pbi, tile, &decoder_recon->r, tile_col);

  return 0;
}

static int vp9_inter_pred(struct task *tsk,
                          struct task_step *step,
                          int dev_type) {
  /*FIXME now we only support CPU */
  if (dev_type == DEV_CPU) {
    assert(dev_type == DEV_CPU);
    vp9_inter_pred_cpu(tsk, step);
  }

  return 0;
}

static int vp9_inter_pred_prepare(struct task *tsk,
                                  struct task_step *step,
                                  int dev_type) {
  assert(dev_type == DEV_CPU);
  vp9_inter_pred_prepare_cpu(tsk, step);

  return 0;
}

static int vp9_inter_sync(struct task *tsk,
                          struct task_step *step,
                          int dev_type) {
  int tile_col;
  struct task *sub;

  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;

  VP9_COMMON *const cm = &pbi->common;
  const int tile_cols = 1 << cm->log2_tile_cols;

  assert(dev_type == DEV_GPU);

  for (tile_col = 0; tile_col < tile_cols; tile_col++) {
    struct task *en_task;
    struct entropy_dec_param *en_param;
    en_task = task_create_sub(tsk);
    assert(en_task);
    en_param = entropy_dec_param_get(en_task);
    assert(en_param);
    en_param->pbi = pbi;
    en_param->tile_col = tile_col;
  }

  task_for_each_sub(sub, tsk) {
    sub->curr_step = step->step_nr;
  }

  atomic_init(&tsk->sub_count, tile_cols);

  return tile_cols;
}

static int vp9_intra_pred_cpu(struct task *tsk,
                              struct task_step *step) {

  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_DECODER_RECON *decoder_recon = pbi->decoder_recon + param->tile_col;
  TileInfo *tile = &decoder_recon->tile;
  int tile_col = param->tile_col;

#if USE_INTER_PREDICT_OCL
  decode_tile_recon_inter_transform(pbi, tile, &decoder_recon->r, tile_col);
#endif
  decode_tile_recon_intra(pbi, tile, &decoder_recon->r, param->tile_col);

  return 0;
}

static int vp9_intra_pred(struct task *tsk,
                          struct task_step *step,
                          int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);
  vp9_intra_pred_cpu(tsk, step);

  return 0;
}

static int vp9_loopfilter_cpu(struct task *tsk,
                              struct task_step *step) {
  struct frame_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;

  // *param->p_data_end = vp9_reader_find_end(pbi->last_reader);
  vp9_decode_frame_tail(pbi);

  return 0;
}

static int vp9_loopfilter(struct task *tsk,
                          struct task_step *step,
                          int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);
  vp9_loopfilter_cpu(tsk, step);
  return 0;
}


/*
 * 0(frame_entropy_dec) -----|
 *                           |
 * |----  1(entropy_dec) <---|
 * |
 * |----> 2(inter_pred) --> 3(intra) --> 4(loopfilter)
 */
static struct task_step steps[] = {
#if USE_INTER_PREDICT_OCL
  {
    "vp9_mt_task_allocating",     // name
    STEP_KEEP,                    // type
    DEV_CPU,                      // dev_type
    0,                            // step_nr
    (1 << 1),                     // next_steps_map
    1,                            // next_count
    0,                            // prev_steps_map
    0,                            // prev_count
    vp9_mt_task_allocating,       // process
    NULL,                         // pool
    NULL                          // priv
  },
  {
    "vp9_inter_pred_calcu", // name
    STEP_SPLIT,             // type
    DEV_CPU | DEV_GPU,      // dev_type
    1,                      // step_nr
    (1 << 2),               // next_steps_map
    1,                      // next_count
    (1 << 0),               // prev_steps_map
    1,                      // prev_count
    vp9_inter_pred_calcu,   // process
    NULL,                   // pool
    NULL                    // priv
  },
  {
    "vp9_inter_sync",               // name
    STEP_MERGE,                     // type
    DEV_GPU,                        // dev_type
    2,                              // step_nr
    (1 << 3),                       // next_steps_map
    1,                              // next_count
    (1 << 1),                       // prev_steps_map
    1,                              // prev_count
    vp9_inter_sync,                 // process
    NULL,                           // pool
    NULL                            // priv
  },
  {
    "vp9_intra_pred",       // name
    STEP_SYNC,              // type
    DEV_CPU,                // dev_type
    3,                      // step_nr
    (1 << 4),               // next_steps_map
    1,                      // next_count
    (1 << 2),               // prev_steps_map
    1,                      // prev_count
    vp9_intra_pred,         // process
    NULL,                   // pool
    NULL                    // priv
  },
  {
    "vp9_loopfilter",       // name
    STEP_MERGE,             // type
    DEV_CPU,                // dev_type
    4,                      // step_nr
    0,                      // next_steps_map
    0,                      // next_count
    (1 << 3),               // prev_steps_map
    1,                      // prev_count
    vp9_loopfilter,         // process
    NULL,                   // pool
    NULL                    // priv
  }
#else
  {
    "vp9_frame_entropy_dec",      // name
    STEP_KEEP,                    // type
    DEV_CPU,                      // dev_type
    0,                            // step_nr
    (1 << 1),                     // next_steps_map
    1,                            // next_count
    0,                            // prev_steps_map
    0,                            // prev_count
    vp9_frame_entropy_dec,        // process
    NULL,                         // pool
    NULL                          // priv
  },
  {
    "vp9_tile_entropy_dec", // name
    STEP_SPLIT,             // type
    DEV_CPU,                // dev_type
    1,                      // step_nr
    (1 << 2),               // next_steps_map
    1,                      // next_count
    (1 << 0),               // prev_steps_map
    1,                      // prev_count
    vp9_tile_entropy_dec,   // process
    NULL,                   // pool
    NULL                    // priv
  },
  {
    "vp9_inter_pred",       // name
    STEP_KEEP,              // type
    DEV_CPU,                // dev_type
    2,                      // step_nr
    (1 << 3),               // next_steps_map
    1,                      // next_count
    (1 << 1),               // prev_steps_map
    1,                      // prev_count
    vp9_inter_pred,         // process
    NULL,                   // pool
    NULL                    // priv
  },
  {
    "vp9_intra_pred",       // name
    STEP_SYNC,              // type
    DEV_CPU,                // dev_type
    3,                      // step_nr
    (1 << 4),               // next_steps_map
    1,                      // next_count
    (1 << 2),               // prev_steps_map
    1,                      // prev_count
    vp9_intra_pred,         // process
    NULL,                   // pool
    NULL                    // priv
  },
  {
    "vp9_loopfilter",       // name
    STEP_MERGE,             // type
    DEV_CPU,                // dev_type
    4,                      // step_nr
    0,                      // next_steps_map
    0,                      // next_count
    (1 << 3),               // prev_steps_map
    1,                      // prev_count
    vp9_loopfilter,         // process
    NULL,                   // pool
    NULL                    // priv
  }
#endif
};

#define ARRAY_SZ(arr) (sizeof(arr) / sizeof(arr[0]))

struct task_steps_pool *steps_pool_get(void) {
  return task_steps_pool_create(steps, ARRAY_SZ(steps));
}

static void task_dtor(struct task *tsk) {
  if (tsk->priv) {
    //vpx_free(tsk->priv);
  }
}

struct frame_dec_param *frame_dec_param_get(struct task *tsk) {
  struct frame_dec_param *param;

  param = vpx_malloc(sizeof(*param));
  if (!param) {
    return NULL;
  }

  tsk->priv = param;
  tsk->dtor = task_dtor;

  return param;
}

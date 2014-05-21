/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vp9/sched/step.h"
#include "vp9/decoder/vp9_entropy_step.h"
#include "vp9/decoder/vp9_decodeframe_recon.h"
#include "vp9/decoder/vp9_append.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"

static INLINE int sched_tiles_entropy(struct task *tsk,
                                      struct task_step *step) {
  struct frame_entropy_dec_param *param = tsk->priv;
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

static int vp9_entropy_head(struct task *tsk,
                            struct task_step *step,
                            int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);
  return sched_tiles_entropy(tsk, step);
}

static int vp9_tile_entropy_dec_cpu(struct task *tsk,
                                    struct task_step *step) {
  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_COMMON *const cm = &pbi->common;
  //VP9_DECODER_RECON *decoder_recon = &pbi->decoder_recon[param->tile_col];
  VP9_DECODER_RECON *decoder_recon = &pbi->decoder_for_entropy[param->tile_col];
  TileInfo *tile = &decoder_recon->tile;

  if (cm->frame_parallel_decoding_mode) {
    decode_tile_recon_entropy_for_entropy(pbi, tile, &decoder_recon->r, param->tile_col);
  }

#if USE_INTER_PREDICT_OCL
  decode_tile_recon_inter_prepare_ocl(
      pbi, tile, &decoder_recon->r, param->tile_col);
#endif // USE_INTER_PREDICT_OCL

  return 0;
}

static int vp9_inter_pred_index_gpu(struct task *tsk,
                                    struct task_step *step) {
  struct entropy_dec_param *param = tsk->priv;
  VP9D_COMP *pbi = param->pbi;
  VP9_DECODER_RECON *decoder_recon = &pbi->decoder_for_entropy[param->tile_col];
  int tile_col = param->tile_col;
  TileInfo *tile = &decoder_recon->tile;

  decode_tile_recon_inter_index_ocl(pbi, tile, &decoder_recon->r, tile_col);

  return 0;
}

static int vp9_entropy_tile(struct task *tsk,
                            struct task_step *step,
                            int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);
  vp9_tile_entropy_dec_cpu(tsk, step);
  return 0;
}


static int vp9_entropy_tail(struct task *tsk,
                            struct task_step *step,
                            int dev_type) {
  /*FIXME now we only support CPU */
  assert(dev_type == DEV_CPU);

#if USE_INTER_PREDICT_OCL
  vp9_inter_pred_index_gpu(tsk, step);
#endif // USE_INTER_PREDICT_OCL

  return 0;
}

static struct task_step steps[] = {
  {
    "vp9_entropy_head",           // name
    STEP_KEEP,                    // type
    DEV_CPU,                      // dev_type
    0,                            // step_nr
    (1 << 1),                     // next_steps_map
    1,                            // next_count
    0,                            // prev_steps_map
    0,                            // prev_count
    vp9_entropy_head,             // process
    NULL,                         // pool
    NULL                          // priv
  },
  {
    "vp9_entropy_tile",           // name
    STEP_KEEP,                    // type
    DEV_CPU,                      // dev_type
    1,                            // step_nr
    (1 << 2),                     // next_steps_map
    1,                            // next_count
    (1 << 0),                     // prev_steps_map
    1,                            // prev_count
    vp9_entropy_tile,             // process
    NULL,                         // pool
    NULL                          // priv
  },
  {
    "vp9_etropy_tail",            // name
    STEP_MERGE,                   // type
    DEV_CPU,                      // dev_type
    2,                            // step_nr
    0,                            // next_steps_map
    0,                            // next_count
    (1 << 1),                     // prev_steps_map
    1,                            // prev_count
    vp9_entropy_tail,             // process
    NULL,                         // pool
    NULL                          // priv
  },
};

#define ARRAY_SZ(arr) (sizeof(arr) / sizeof(arr[0]))

struct task_steps_pool *entropy_steps_pool_get(void) {
  return task_steps_pool_create(steps, ARRAY_SZ(steps));
}

static void task_dtor(struct task *tsk) {
  if (tsk->priv) {
    //vpx_free(tsk->priv);
  }
}

struct frame_entropy_dec_param *frame_entropy_dec_param_get(struct task *tsk) {
  struct frame_entropy_dec_param *param;

  param = vpx_malloc(sizeof(*param));
  if (!param) {
    return NULL;
  }

  tsk->priv = param;
  tsk->dtor = task_dtor;

  return param;
}

struct entropy_dec_param *entropy_dec_param_get(struct task *tsk) {
  struct entropy_dec_param *param;

  param = vpx_malloc(sizeof(*param));
  if (!param) {
    return NULL;
  }

  tsk->priv = param;
  tsk->dtor = task_dtor;

  return param;
}

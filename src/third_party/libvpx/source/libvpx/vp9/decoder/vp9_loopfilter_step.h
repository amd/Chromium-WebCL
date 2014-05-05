/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_DECODER_VP9_LOOPFILTER_STEP_H_
#define VP9_DECODER_VP9_LOOPFILTER_STEP_H_

#include "vp9/sched/sched.h"
#include "vp9/decoder/vp9_onyxd_int.h"
#include "vp9/decoder/vp9_reader.h"

struct task_steps_pool *lf_steps_pool_get(void);

struct lf_blk_param {
  VP9D_COMP *pbi;
  int step_length;
  const YV12_BUFFER_CONFIG *frame_buffer;
  VP9_COMMON *cm;
  MACROBLOCKD *xd;
  int start_mi_row;
  int end_mi_row;
  int mi_row;
  int mi_col;
  int y_only;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  int nr;
  struct task *upper;
};

struct lf_blk_param *lf_blk_param_get(struct task *tsk);

void lf_blk_param_put(struct task *tsk, struct lf_blk_param *param);

#endif  // VP9_DECODER_VP9_LOOPFILTER_STEP_H_

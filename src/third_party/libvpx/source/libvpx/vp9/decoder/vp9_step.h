/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_DECODER_VP9_STEP_H_
#define VP9_DECODER_VP9_STEP_H_

#include "vp9/sched/sched.h"
#include "vp9/decoder/vp9_onyxd_int.h"

struct task_steps_pool *steps_pool_get(void);

struct frame_entropy_dec_param {
  VP9D_COMP *pbi;
  const uint8_t **p_data_end;
  vp9_reader reader;
  int tid;
};

struct frame_entropy_dec_param *frame_entropy_dec_param_get(struct task *tsk);

struct entropy_dec_param {
  VP9D_COMP *pbi;
  int tile_col;
  const uint8_t **p_data_end;
  int tid;
};

struct entropy_dec_param *entropy_dec_param_get(struct task *tsk);

#endif  // VP9_DECODER_VP9_STEP_H_

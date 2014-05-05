/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./vpx_config.h"
#include "vp9/common/inter_ocl/vp9_convolve_ocl_c.h"

#define INTER_FILER_COUNT_OCL 512

static const int16_t vp9_inter_pred_filters_8_ocl_c[INTER_FILER_COUNT_OCL] = {
   0,  0,   0, 128,   0,   0,   0,  0,   0,   1,  -5, 126,   8,  -3,   1,  0,
  -1,  3, -10, 122,  18,  -6,   2,  0,  -1,   4, -13, 118,  27,  -9,   3, -1,
  -1,  4, -16, 112,  37, -11,   4, -1,  -1,   5, -18, 105,  48, -14,   4, -1,
  -1,  5, -19,  97,  58, -16,   5, -1,  -1,   6, -19,  88,  68, -18,   5, -1,
  -1,  6, -19,  78,  78, -19,   6, -1,  -1,   5, -18,  68,  88, -19,   6, -1,
  -1,  5, -16,  58,  97, -19,   5, -1,  -1,   4, -14,  48, 105, -18,   5, -1,
  -1,  4, -11,  37, 112, -16,   4, -1,  -1,   3,  -9,  27, 118, -13,   4, -1,
   0,  2,  -6,  18, 122, -10,   3, -1,   0,   1,  -3,   8, 126,  -5,   1,  0,
   0,  0,   0, 128,   0,   0,   0,  0,  -3,  -1,  32,  64,  38,   1,  -3,  0,
  -2, -2,  29,  63,  41,   2,  -3,  0,  -2,  -2,  26,  63,  43,   4,  -4,  0,
  -2, -3,  24,  62,  46,   5,  -4,  0,  -2,  -3,  21,  60,  49,   7,  -4,  0,
  -1, -4,  18,  59,  51,   9,  -4,  0,  -1,  -4,  16,  57,  53,  12,  -4, -1,
  -1, -4,  14,  55,  55,  14,  -4, -1,  -1,  -4,  12,  53,  57,  16,  -4, -1,
   0, -4,   9,  51,  59,  18,  -4, -1,   0,  -4,   7,  49,  60,  21,  -3, -2,
   0, -4,   5,  46,  62,  24,  -3, -2,   0,  -4,   4,  43,  63,  26,  -2, -2,
   0, -3,   2,  41,  63,  29,  -2, -2,   0,  -3,   1,  38,  64,  32,  -1, -3,
   0,  0,   0, 128,   0,   0,   0,  0,  -1,   3,  -7, 127,   8,  -3,   1,  0,
  -2,  5, -13, 125,  17,  -6,   3, -1,  -3,   7, -17, 121,  27, -10,   5, -2,
  -4,  9, -20, 115,  37, -13,   6, -2,  -4,  10, -23, 108,  48, -16,   8, -3,
  -4, 10, -24, 100,  59, -19,   9, -3,  -4,  11, -24,  90,  70, -21,  10, -4,
  -4, 11, -23,  80,  80, -23,  11, -4,  -4,  10, -21,  70,  90, -24,  11, -4,
  -3,  9, -19,  59, 100, -24,  10, -4,  -3,   8, -16,  48, 108, -23,  10, -4,
  -2,  6, -13,  37, 115, -20,   9, -4,  -2,   5, -10,  27, 121, -17,   7, -3,
  -1,  3,  -6,  17, 125, -13,   5, -2,   0,   1,  -3,   8, 127,  -7,   3, -1,
   0,  0,   0, 128,   0,   0,   0,  0,   0,   0,   0, 120,   8,   0,   0,  0,
   0,  0,   0, 112,  16,   0,   0,  0,   0,   0,   0, 104,  24,   0,   0,  0,
   0,  0,   0,  96,  32,   0,   0,  0,   0,   0,   0,  88,  40,   0,   0,  0,
   0,  0,   0,  80,  48,   0,   0,  0,   0,   0,   0,  72,  56,   0,   0,  0,
   0,  0,   0,  64,  64,   0,   0,  0,   0,   0,   0,  56,  72,   0,   0,  0,
   0,  0,   0,  48,  80,   0,   0,  0,   0,   0,   0,  40,  88,   0,   0,  0,
   0,  0,   0,  32,  96,   0,   0,  0,   0,   0,   0,  24, 104,   0,   0,  0,
   0,  0,   0,  16, 112,   0,   0,  0,   0,   0,   0,   8, 120,   0,   0,  0
};

static const int vp9_convolve_mode_ocl_c[2][2] = {{24, 16}, {8, 0}};

void *build_inter_pred_calcu_ocl_mt(void *pred_param) {
  int i;
  int mode_num;
  uint8_t *src;
  uint8_t *dst;
  const int16_t *filter_x;
  const int16_t *filter_y;

  INTER_MT_ATTR *inter_mt_attr = (INTER_MT_ATTR *)pred_param;

  int tile_num = inter_mt_attr->tile_num;
  uint8_t *new_buffer = inter_mt_attr->new_buffer;

  const int fri_block_count = inter_ocl_obj.cpu_fri_count[tile_num];
  const int sec_block_count = inter_ocl_obj.cpu_sec_count[tile_num];
  const INTER_PRED_PARAM_CPU *pred_param_fri =
            inter_ocl_obj.pred_param_cpu_fri[tile_num];
  const INTER_PRED_PARAM_CPU *pred_param_sec =
            inter_ocl_obj.pred_param_cpu_sec[tile_num];
  convolve_fn_t *switch_convolve_t = inter_ocl_obj.switch_convolve_t;

  for (i = 0; i < fri_block_count; ++i) {
    mode_num = vp9_convolve_mode_ocl_c[(pred_param_fri[i].x_step_q4 == 16)]
                                      [(pred_param_fri[i].y_step_q4 == 16)];

    src = pred_param_fri[i].psrc;
    dst = new_buffer + pred_param_fri[i].dst_mv;

    filter_x = pred_param_fri[i].filter_x;
    filter_y = pred_param_fri[i].filter_y;

    switch_convolve_t[pred_param_fri[i].pred_mode + mode_num](
                      src, pred_param_fri[i].src_stride,
                      dst, pred_param_fri[i].dst_stride,
                      filter_x, pred_param_fri[i].x_step_q4,
                      filter_y, pred_param_fri[i].y_step_q4,
                      pred_param_fri[i].w, pred_param_fri[i].h);
  }

  for (i = 0; i < sec_block_count; ++i) {
    mode_num = vp9_convolve_mode_ocl_c[(pred_param_sec[i].x_step_q4 == 16)]
                                      [(pred_param_sec[i].y_step_q4 == 16)];

    src = pred_param_sec[i].psrc;
    dst = new_buffer + pred_param_sec[i].dst_mv;

    filter_x = pred_param_sec[i].filter_x;
    filter_y = pred_param_sec[i].filter_y;

    switch_convolve_t[pred_param_sec[i].pred_mode + mode_num + 1](
                      src, pred_param_sec[i].src_stride,
                      dst, pred_param_sec[i].dst_stride,
                      filter_x, pred_param_sec[i].x_step_q4,
                      filter_y, pred_param_sec[i].y_step_q4,
                      pred_param_sec[i].w, pred_param_sec[i].h);
  }
}

void build_inter_pred_calcu_ocl_c(int tile_num, uint8_t *new_buffer) {
  int i;
  int mode_num;
  uint8_t *src;
  uint8_t *dst;
  const int16_t *filter_x;
  const int16_t *filter_y;

  const int fri_block_count = inter_ocl_obj.cpu_fri_count[tile_num];
  const int sec_block_count = inter_ocl_obj.cpu_sec_count[tile_num];
  const INTER_PRED_PARAM_CPU *pred_param_fri =
            inter_ocl_obj.pred_param_cpu_fri[tile_num];
  const INTER_PRED_PARAM_CPU *pred_param_sec =
            inter_ocl_obj.pred_param_cpu_sec[tile_num];
  convolve_fn_t *switch_convolve_t = inter_ocl_obj.switch_convolve_t;

  for (i = 0; i < fri_block_count; ++i) {
    mode_num = vp9_convolve_mode_ocl_c[(pred_param_fri[i].x_step_q4 == 16)]
                                      [(pred_param_fri[i].y_step_q4 == 16)];

    src = pred_param_fri[i].psrc;
    dst = new_buffer + pred_param_fri[i].dst_mv;

    filter_x = pred_param_fri[i].filter_x;
    filter_y = pred_param_fri[i].filter_y;

    switch_convolve_t[pred_param_fri[i].pred_mode + mode_num](
                      src, pred_param_fri[i].src_stride,
                      dst, pred_param_fri[i].dst_stride,
                      filter_x, pred_param_fri[i].x_step_q4,
                      filter_y, pred_param_fri[i].y_step_q4,
                      pred_param_fri[i].w, pred_param_fri[i].h);
  }

  for (i = 0; i < sec_block_count; ++i) {
    mode_num = vp9_convolve_mode_ocl_c[(pred_param_sec[i].x_step_q4 == 16)]
                                      [(pred_param_sec[i].y_step_q4 == 16)];

    src = pred_param_sec[i].psrc;
    dst = new_buffer + pred_param_sec[i].dst_mv;

    filter_x = pred_param_sec[i].filter_x;
    filter_y = pred_param_sec[i].filter_y;

    switch_convolve_t[pred_param_sec[i].pred_mode + mode_num + 1](
                      src, pred_param_sec[i].src_stride,
                      dst, pred_param_sec[i].dst_stride,
                      filter_x, pred_param_sec[i].x_step_q4,
                      filter_y, pred_param_sec[i].y_step_q4,
                      pred_param_sec[i].w, pred_param_sec[i].h);
  }
}

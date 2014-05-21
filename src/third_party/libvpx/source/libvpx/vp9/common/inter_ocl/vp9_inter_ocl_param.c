/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <assert.h>

#include "./vpx_scale_rtcd.h"
#include "./vpx_config.h"

#include "vpx/vpx_integer.h"

#include "vp9/common/vp9_filter.h"
#include "vp9/common/vp9_reconinter.h"
#include "vp9/common/vp9_reconintra.h"
#include "vp9/common/vp9_scale.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"

int CpuFlag;
OCL_CONTEXT ocl_context = {0};
INTER_OCL_OBJ inter_ocl_obj = {0};

static const int16_t *inter_filter[4] = {vp9_sub_pel_filters_8[0],
                                         vp9_sub_pel_filters_8lp[0],
                                         vp9_sub_pel_filters_8s[0],
                                         vp9_bilinear_filters[0]};

static INLINE int round_mv_comp_q4(int value) {
  return (value < 0 ? value - 2 : value + 2) / 4;
}

static MV mi_mv_pred_q4(const MODE_INFO *mi, int idx) {
  MV res = { round_mv_comp_q4(mi->bmi[0].as_mv[idx].as_mv.row +
      mi->bmi[1].as_mv[idx].as_mv.row +
      mi->bmi[2].as_mv[idx].as_mv.row +
      mi->bmi[3].as_mv[idx].as_mv.row),
  round_mv_comp_q4(mi->bmi[0].as_mv[idx].as_mv.col +
      mi->bmi[1].as_mv[idx].as_mv.col +
      mi->bmi[2].as_mv[idx].as_mv.col +
      mi->bmi[3].as_mv[idx].as_mv.col) };
  return res;
}

static MV clamp_mv_to_umv_border_sb(const MACROBLOCKD *xd, const MV *src_mv,
    int bw, int bh, int ss_x, int ss_y) {
  const int spel_left = (VP9_INTERP_EXTEND + bw) << SUBPEL_BITS;
  const int spel_top = (VP9_INTERP_EXTEND + bh) << SUBPEL_BITS;
  int factorX = 1 << (1 - ss_x);
  int factorY = 1 << (1 - ss_y);
  MV clamped_mv = {
    src_mv->row * factorY,
    src_mv->col * factorX
  };
  assert(ss_x <= 1);
  assert(ss_y <= 1);

  clamped_mv.col = clamp(clamped_mv.col,
      xd->mb_to_left_edge * factorX - spel_left,
      xd->mb_to_right_edge * factorX + spel_left - SUBPEL_SHIFTS);
  clamped_mv.row = clamp(clamped_mv.row,
      xd->mb_to_top_edge * factorY - spel_top,
      xd->mb_to_bottom_edge * factorY + spel_top - SUBPEL_SHIFTS);

  return clamped_mv;
}

int vp9_setup_interp_filters_ocl(MACROBLOCKD *xd,
                                 INTERPOLATION_TYPE mcomp_filter_type,
                                 VP9_COMMON *cm) {
  int ret_filter_num = 0;

  switch (mcomp_filter_type) {
    case EIGHTTAP:
    case SWITCHABLE:
      xd->subpix.filter_x = xd->subpix.filter_y = vp9_sub_pel_filters_8;
      break;
    case EIGHTTAP_SMOOTH:
      ret_filter_num = 1;
      xd->subpix.filter_x = xd->subpix.filter_y = vp9_sub_pel_filters_8lp;
      break;
    case EIGHTTAP_SHARP:
      ret_filter_num = 2;
      xd->subpix.filter_x = xd->subpix.filter_y = vp9_sub_pel_filters_8s;
      break;
    case BILINEAR:
      ret_filter_num = 3;
      xd->subpix.filter_x = xd->subpix.filter_y = vp9_bilinear_filters;
      break;
  }
  assert(((intptr_t)xd->subpix.filter_x & 0xff) == 0);

  return ret_filter_num;
}

void build_mc_border_ocl(const uint8_t *src, int src_stride,
                         uint8_t *dst, int dst_stride,
                         int x, int y, int b_w, int b_h, int w, int h) {
  // Get a pointer to the start of the real data for this row.
  const uint8_t *ref_row = src - x - y * src_stride;
  if (y >= h)
    ref_row += (h - 1) * src_stride;
  else if (y > 0)
    ref_row += y * src_stride;

  do {
    int right = 0, copy;
    int left = x < 0 ? -x : 0;

    if (left > b_w)
      left = b_w;

    if (x + b_w > w)
      right = x + b_w - w;

    if (right > b_w)
      right = b_w;

    copy = b_w - left - right;

    if (left)
      memset(dst, ref_row[0], left);

    if (copy)
      memcpy(dst + left, ref_row + x + left, copy);

    if (right)
      memset(dst + left + copy, ref_row[w - 1], right);

    dst += dst_stride;
    ++y;

    if (y > 0 && y < h)
      ref_row += src_stride;
  } while (--b_h);
}

void build_inter_pred_param_sec_ref_ocl(const int plane,
                                        const int block,
                                        BLOCK_SIZE bsize, void *argv,
                                        VP9_COMMON *const cm,
                                        const int src_num,
                                        const int filter_num,
                                        const int tile_num) {
  int xs, ys, w, h;
  int subpel_x, subpel_y;

  const uint8_t *dst_fri; // *src_fri
  const uint8_t *dst; // *pre

  MV32 scaled_mv;
  MV mv, mv_q4;
  struct scale_factors *sf;
  struct subpix_fn_table *subpix;
  struct buf_2d *pre_buf, *dst_buf;
  //const YV12_BUFFER_CONFIG *cfg_src;
  const YV12_BUFFER_CONFIG *cfg_dst;

  int reset_src_buffer = 0;

  const INTER_PRED_ARGS_OCL * const arg = argv;
  MACROBLOCKD *const xd = arg->xd;
  int mi_x = arg->x;
  int mi_y = arg->y;
  struct macroblockd_plane *const pd = &xd->plane[plane];

  const BLOCK_SIZE plane_bsize = get_plane_block_size(bsize, pd);
  const int bwl = b_width_log2(plane_bsize);
  const int bw = 4 << bwl;
  const int bh = 4 * num_4x4_blocks_high_lookup[plane_bsize];
  const int x = 4 * (block & ((1 << bwl) - 1));
  const int y = 4 * (block >> bwl);
  const MODE_INFO *mi = xd->mi_8x8[0];

  int pred_w = b_width_log2(bsize) - xd->plane[plane].subsampling_x;
  int pred_h = b_height_log2(bsize) - xd->plane[plane].subsampling_y;
  int x0, y0, x0_16, y0_16, x1, y1, frame_width, frame_height, buf_stride;
  uint8_t *ref_frame, *buf_ptr;
  const YV12_BUFFER_CONFIG *ref_buf;
  if (mi->mbmi.sb_type < BLOCK_8X8 && !plane) {
    pred_w = 0;
    pred_h = 0;
  }
  sf = &xd->block_refs[1]->sf;

  pre_buf = &pd->pre[1];
  dst_buf = &pd->dst;

  dst = dst_buf->buf + dst_buf->stride * y + x;

  mv = mi->mbmi.sb_type < BLOCK_8X8
       ? (plane == 0 ? mi->bmi[block].as_mv[1].as_mv
                       : mi_mv_pred_q4(mi, 1))
       : mi->mbmi.mv[1].as_mv;

  mv_q4 = clamp_mv_to_umv_border_sb(xd, &mv, bw, bh,
                                    pd->subsampling_x,
                                    pd->subsampling_y);

  ref_buf = xd->block_refs[1]->buf;
  w = 4 << pred_w;
  h = 4 << pred_h;
  // Get reference frame pointer, width and height.
  if (plane == 0) {
    frame_width = ref_buf->y_crop_width;
    frame_height = ref_buf->y_crop_height;
    ref_frame = ref_buf->y_buffer;
  } else {
    frame_width = ref_buf->uv_crop_width;
    frame_height = ref_buf->uv_crop_height;
    ref_frame = plane == 1 ? ref_buf->u_buffer : ref_buf->v_buffer;
  }
  x0 = (-xd->mb_to_left_edge >> (3 + pd->subsampling_x)) + x;
  y0 = (-xd->mb_to_top_edge >> (3 + pd->subsampling_y)) + y;
  x0_16 = x0 << SUBPEL_BITS;
  y0_16 = y0 << SUBPEL_BITS;

  if (vp9_is_scaled(sf)) {
    scaled_mv = vp9_scale_mv(&mv_q4, mi_x + x, mi_y + y, sf);
    xs = sf->x_step_q4;
    ys = sf->y_step_q4;
    x0 = sf->scale_value_x(x0, sf);
    y0 = sf->scale_value_y(y0, sf);
    x0_16 = sf->scale_value_x(x0_16, sf);
    y0_16 = sf->scale_value_y(y0_16, sf);
  } else {
    scaled_mv.row = mv_q4.row;
    scaled_mv.col = mv_q4.col;
    xs = ys = 16;
  }

  subpix = &xd->subpix;
  subpel_x = scaled_mv.col & SUBPEL_MASK;
  subpel_y = scaled_mv.row & SUBPEL_MASK;

  x0 += scaled_mv.col >> SUBPEL_BITS;
  y0 += scaled_mv.row >> SUBPEL_BITS;
  x0_16 += scaled_mv.col;
  y0_16 += scaled_mv.row;

  x1 = ((x0_16 + (w - 1) * xs) >> SUBPEL_BITS) + 1;
  y1 = ((y0_16 + (h - 1) * ys) >> SUBPEL_BITS) + 1;
  buf_ptr = ref_frame + y0 * pre_buf->stride + x0;
  buf_stride = pre_buf->stride;
  if (scaled_mv.col || scaled_mv.row ||
      (frame_width & 0x7) || (frame_height & 0x7)) {
    int x_pad = 0, y_pad = 0;

    if (subpel_x || (sf->x_step_q4 & SUBPEL_MASK)) {
      x0 -= VP9_INTERP_EXTEND - 1;
      x1 += VP9_INTERP_EXTEND;
      x_pad = 1;
    }

    if (subpel_y || (sf->y_step_q4 & SUBPEL_MASK)) {
      y0 -= VP9_INTERP_EXTEND - 1;
      y1 += VP9_INTERP_EXTEND;
      y_pad = 1;
    }
    if (x0 < 0 || x0 > frame_width - 1 || x1 < 0 || x1 > frame_width ||
          y0 < 0 || y0 > frame_height - 1 || y1 < 0 || y1 > frame_height - 1) {
      uint8_t *buf_ptr1 = ref_frame + y0 * pre_buf->stride + x0;

      reset_src_buffer = 1;

      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->pref =
          inter_ocl_obj.pref[tile_num];
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->buf_ptr1 = buf_ptr1;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->pre_stride = pre_buf->stride;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->x0 = x0;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->x1 = x1;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->y0 = y0;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->y1 = y1;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->frame_width = frame_width;
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->frame_height = frame_height;

      buf_stride = x1 - x0;
      buf_ptr = inter_ocl_obj.pref[tile_num] + y_pad * 3 * buf_stride + x_pad * 3;
      inter_ocl_obj.pref[tile_num] += (x1 - x0) * (y1 - y0);
    }
  }

  cfg_dst = &cm->yv12_fb[cm->new_fb_idx];
  dst_fri = cfg_dst->buffer_alloc;

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->pred_mode =
    ((subpel_x != 0) << 2) + ((subpel_y != 0) << 1);

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->psrc = buf_ptr;
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->src_stride = buf_stride;

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->dst_mv = dst - dst_fri;
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->dst_stride = dst_buf->stride;

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->filter_x =
    subpix->filter_x[subpel_x];
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->filter_y =
    subpix->filter_y[subpel_y];

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->x_step_q4 = xs;
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->y_step_q4 = ys;

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->w = 4 << pred_w;
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->h = 4 << pred_h;

  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]->reset_src_buffer = reset_src_buffer;

  inter_ocl_obj.cpu_sec_count_pre[tile_num]++;
  inter_ocl_obj.pred_param_cpu_sec_pre[tile_num]++;
}

void build_inter_pred_param_fri_ref_ocl(const int plane,
                                        const int block,
                                        BLOCK_SIZE bsize, void *argv,
                                        VP9_COMMON *const cm,
                                        const int ref_idx,
                                        const int src_num,
                                        const int filter_num,
                                        const int tile_num) {
  int xs, ys, w, h;
  int buf_offset;
  int filter_radix;
  int pred_mode;
  int subpel_x, subpel_y;
  int sub_x, sub_y;

  const int16_t *filter;
  const uint8_t *src_fri, *dst_fri;
  const uint8_t *dst; // *pre

  MV32 scaled_mv;
  MV mv, mv_q4;
  struct scale_factors *sf;
  struct subpix_fn_table *subpix;
  struct buf_2d *pre_buf, *dst_buf;
  const YV12_BUFFER_CONFIG *cfg_src;
  const YV12_BUFFER_CONFIG *cfg_dst;

  int reset_src_buffer = 0;

  const INTER_PRED_ARGS_OCL * const arg = argv;
  MACROBLOCKD *const xd = arg->xd;
  int mi_x = arg->x;
  int mi_y = arg->y;
  struct macroblockd_plane *const pd = &xd->plane[plane];

  const BLOCK_SIZE plane_bsize = get_plane_block_size(bsize, pd);
  const int bwl = b_width_log2(plane_bsize);
  const int bw = 4 << bwl;
  const int bh = 4 * num_4x4_blocks_high_lookup[plane_bsize];
  const int x = 4 * (block & ((1 << bwl) - 1));
  const int y = 4 * (block >> bwl);
  const MODE_INFO *mi = xd->mi_8x8[0];

  int pred_w = b_width_log2(bsize) - xd->plane[plane].subsampling_x;
  int pred_h = b_height_log2(bsize) - xd->plane[plane].subsampling_y;
  int x0, y0, x0_16, y0_16, x1, y1, frame_width, frame_height, buf_stride;
  uint8_t *ref_frame, *buf_ptr;
  const YV12_BUFFER_CONFIG *ref_buf;

  if (mi->mbmi.sb_type < BLOCK_8X8 && !plane) {
    pred_w = 0;
    pred_h = 0;
  }

  sf = &xd->block_refs[0]->sf;
  pre_buf = &pd->pre[0];
  dst_buf = &pd->dst;

  dst = dst_buf->buf + dst_buf->stride * y + x;

  mv = mi->mbmi.sb_type < BLOCK_8X8
       ? (plane == 0 ? mi->bmi[block].as_mv[0].as_mv
                       : mi_mv_pred_q4(mi, 0))
       : mi->mbmi.mv[0].as_mv;

  mv_q4 = clamp_mv_to_umv_border_sb(xd, &mv, bw, bh,
                                    pd->subsampling_x,
                                    pd->subsampling_y);
  w = 4 << pred_w;
  h = 4 << pred_h;

  ref_buf = xd->block_refs[0]->buf;
  // Get reference frame pointer, width and height.
  if (plane == 0) {
    frame_width = ref_buf->y_crop_width;
    frame_height = ref_buf->y_crop_height;
    ref_frame = ref_buf->y_buffer;
  } else {
    frame_width = ref_buf->uv_crop_width;
    frame_height = ref_buf->uv_crop_height;
    ref_frame = plane == 1 ? ref_buf->u_buffer : ref_buf->v_buffer;
  }

  x0 = (-xd->mb_to_left_edge >> (3 + pd->subsampling_x)) + x;
  y0 = (-xd->mb_to_top_edge >> (3 + pd->subsampling_y)) + y;
  x0_16 = x0 << SUBPEL_BITS;
  y0_16 = y0 << SUBPEL_BITS;

  if (vp9_is_scaled(sf)) {
    // pre = pre_buf->buf + scaled_buffer_offset(x, y, pre_buf->stride, sf);
    scaled_mv = vp9_scale_mv(&mv_q4, mi_x + x, mi_y + y, sf);
    xs = sf->x_step_q4;
    ys = sf->y_step_q4;
    x0 = sf->scale_value_x(x0, sf);
    y0 = sf->scale_value_y(y0, sf);
    x0_16 = sf->scale_value_x(x0_16, sf);
    y0_16 = sf->scale_value_y(y0_16, sf);
  } else {
    // pre = pre_buf->buf + (y * pre_buf->stride + x);
    scaled_mv.row = mv_q4.row;
    scaled_mv.col = mv_q4.col;
    xs = ys = 16;
  }

  subpix = &xd->subpix;
  subpel_x = scaled_mv.col & SUBPEL_MASK;
  subpel_y = scaled_mv.row & SUBPEL_MASK;

  x0 += scaled_mv.col >> SUBPEL_BITS;
  y0 += scaled_mv.row >> SUBPEL_BITS;
  x0_16 += scaled_mv.col;
  y0_16 += scaled_mv.row;

  x1 = ((x0_16 + (w - 1) * xs) >> SUBPEL_BITS) + 1;
  y1 = ((y0_16 + (h - 1) * ys) >> SUBPEL_BITS) + 1;
  buf_ptr = ref_frame + y0 * pre_buf->stride + x0;
  buf_stride = pre_buf->stride;
  if (scaled_mv.col || scaled_mv.row ||
      (frame_width & 0x7) || (frame_height & 0x7)) {
    int x_pad = 0, y_pad = 0;

    if (subpel_x || (sf->x_step_q4 & SUBPEL_MASK)) {
      x0 -= VP9_INTERP_EXTEND - 1;
      x1 += VP9_INTERP_EXTEND;
      x_pad = 1;
    }

    if (subpel_y || (sf->y_step_q4 & SUBPEL_MASK)) {
      y0 -= VP9_INTERP_EXTEND - 1;
      y1 += VP9_INTERP_EXTEND;
      y_pad = 1;
    }

    if (x0 < 0 || x0 > frame_width - 1 || x1 < 0 || x1 > frame_width ||
          y0 < 0 || y0 > frame_height - 1 || y1 < 0 || y1 > frame_height - 1) {
      uint8_t *buf_ptr1 = ref_frame + y0 * pre_buf->stride + x0;
      // Extend the border.

      reset_src_buffer = 1;

      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->pref =
          inter_ocl_obj.pref[tile_num];
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->buf_ptr1 = buf_ptr1;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->pre_stride = pre_buf->stride;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->x0 = x0;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->x1 = x1;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->y0 = y0;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->y1 = y1;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->frame_width = frame_width;
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->frame_height = frame_height;

      buf_stride = x1 - x0;
      buf_ptr = inter_ocl_obj.pref[tile_num] + y_pad * 3 * buf_stride + x_pad * 3;
      inter_ocl_obj.pref[tile_num] += (x1 - x0) * (y1 - y0);
    }
  }

  cfg_src = &cm->yv12_fb[src_num];
  cfg_dst = &cm->yv12_fb[cm->new_fb_idx];
  src_fri = cfg_src->buffer_alloc;
  dst_fri = cfg_dst->buffer_alloc;

  filter = inter_filter[filter_num];
  filter_radix = filter_num << 7;

  pred_mode = ((subpel_x != 0) << 2) + ((subpel_y != 0) << 1);
  sub_x = (subpel_x != 0);
  sub_y = (subpel_y != 0);
  buf_offset = buf_ptr - src_fri;

  if (!ref_idx && xs == 16 && ys == 16 && buf_offset > 0 &&
      buf_offset < inter_ocl_obj.buffer_size && !CpuFlag &&
      cm->show_frame) {
    inter_ocl_obj.pred_param_gpu_pre[tile_num]->src_stride = pre_buf->stride;
    inter_ocl_obj.pred_param_gpu_pre[tile_num]->filter_x_mv =
        filter_radix + subpix->filter_x[subpel_x] - filter;
    inter_ocl_obj.pred_param_gpu_pre[tile_num]->filter_y_mv =
        filter_radix + subpix->filter_y[subpel_y] - filter;

    h = h >> 2;
    w = w >> 2;

    inter_ocl_obj.index_param_gpu_pre[tile_num]->pred_mode = pred_mode;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->buf_offset = buf_offset;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->dst_offset = dst - dst_fri;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->dst_stride = dst_buf->stride;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->pre_stride = pre_buf->stride;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->src_num = src_num;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->w = w;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->h = h;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->sub_x = sub_x;
    inter_ocl_obj.index_param_gpu_pre[tile_num]->sub_y = sub_y;

    inter_ocl_obj.index_count_pre[tile_num] += w * h;

    inter_ocl_obj.gpu_block_count[tile_num]++;
    inter_ocl_obj.index_param_gpu_pre[tile_num]++;
    inter_ocl_obj.pred_param_gpu_pre[tile_num]++;
  } else {
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->pred_mode = pred_mode;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->psrc = buf_ptr;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->src_stride = buf_stride;

    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->dst_mv = dst - dst_fri;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->dst_stride = dst_buf->stride;

    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->filter_x =
      subpix->filter_x[subpel_x];
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->filter_y =
      subpix->filter_y[subpel_y];

    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->x_step_q4 = xs;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->y_step_q4 = ys;

    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->w = w;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->h = h;

    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]->reset_src_buffer = reset_src_buffer;

    inter_ocl_obj.cpu_fri_count_pre[tile_num]++;
    inter_ocl_obj.pred_param_cpu_fri_pre[tile_num]++;
  }
}

/*
  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "vp9/decoder/vp9_append.h"
#include "vp9/decoder/vp9_decodeframe_recon.h"
#include "vpx_mem/vpx_mem.h"
#include "vpx_ports/mem.h"
#include "vp9/common/vp9_blockd.h"

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_seg_common.h"
#include "vp9/decoder/vp9_onyxd_int.h"
#include "vp9/decoder/vp9_copy_mip_ocl.h"
#include "vp9/ppa.h"

void ret_pbi_queue(VP9D_COMP *pbi, VP9D_COMP *pbi_new) {
  pbi->decoder_for_entropy = pbi_new->decoder_recon;
}

void common_queue_recon(VP9_COMMON *cm, VP9_COMMON* cm_new) {
  int i;

  cm_new->last_width = cm->last_width;
  cm_new->last_height = cm->last_height;
  cm_new->frame_to_show = cm->frame_to_show;
  for (i =  0; i < cm->fb_count; i ++) {
    cm_new->fb_idx_ref_cnt[i] = cm->fb_idx_ref_cnt[i];
  }
  vpx_memcpy(cm_new->ref_frame_map, cm->ref_frame_map, REF_FRAMES * sizeof(int));
  vpx_memcpy(cm_new->frame_contexts, cm->frame_contexts, FRAME_CONTEXTS * sizeof(FRAME_CONTEXT));

}

void init_pre_pbi(VP9D_COMP *pbi, VP9D_COMP *pbi_new) {
  int i, j;
  for (i = 0; i < MAX_TILES; i ++) {
    for (j = 0; j < pbi->decoder_recon[i].dequant_count; j ++) {
      memset(pbi_new->decoder_recon[i].dequant_recon[j].qcoeff[0],
          0, sizeof(int16_t) * 64*64);
      memset(pbi_new->decoder_recon[i].dequant_recon[j].qcoeff[1],
          0, sizeof(int16_t) * 64*64);
      memset(pbi_new->decoder_recon[i].dequant_recon[j].qcoeff[2],
          0, sizeof(int16_t) * 64*64);
    }
  }
}
void pbi_queue_recon(VP9D_COMP *pbi, VP9D_COMP *pbi_new) {
  common_queue_recon(&pbi->common, &pbi_new->common);
  pbi_new->decoded_key_frame = pbi->decoded_key_frame;
  pbi_new->do_loopfilter_inline = pbi->do_loopfilter_inline;
}

void pbi_queue(VP9D_COMP *pbi, VP9D_COMP *pbi_new) {
  int i, j, tile_row, tile_col, tmp_offset, aligned_mi_cols;
  VP9_COMMON * cm_new;
  int tile_rows = 1 << (pbi->common.log2_tile_cols);
  int tile_cols = 1 << pbi->common.log2_tile_rows;
  pbi_new->mb = pbi->mb; // copy mb
  // ------------------------copy common------------------
  common_queue(&pbi->common, &pbi_new->common,((pbi->l_bufpool_flag_output + 1)& 1));
  // ------------------------copy decoder_recon------------
  for (i = 0; i < MAX_TILES; i ++) {
    pbi_new->decoder_recon[i].cm = &pbi_new->common;
    /*
    for (j = 0; j < pbi_new->decoder_recon[i].inter_blocks_count; j ++) {
      MODE_INFO *tmp_mi8x8 = NULL;
      tmp_mi8x8 = pbi_new->decoder_recon[i].inter_pre_recon[j].mi_8x8[0];
      tmp_offset =pbi_new->decoder_recon[i].inter_pre_recon[j].mi_8x8
        - pbi->common.mi_grid_visible;
      pbi_new->decoder_recon[i].inter_pre_recon[j].mi_8x8 = tmp_offset
        + pbi_new->common.mi_grid_visible;
      tmp_offset = tmp_mi8x8 - pbi->common.mi;
      pbi_new->decoder_recon[i].inter_pre_recon[j].mi_8x8[0] = tmp_offset
        + pbi_new->common.mi;
    }

    for (j = 0; j < pbi_new->decoder_recon[i].intra_blocks_count; j ++) {   
      MODE_INFO *tmp_mi8x8 = NULL;
      tmp_mi8x8 = pbi_new->decoder_recon[i].intra_pre_recon[j].mi_8x8[0];
      tmp_offset = pbi_new->decoder_recon[i].intra_pre_recon[j].mi_8x8
          - pbi->common.mi_grid_visible;
      pbi_new->decoder_recon[i].intra_pre_recon[j].mi_8x8 = tmp_offset
          + pbi_new->common.mi_grid_visible;
      tmp_offset = tmp_mi8x8 - pbi->common.mi;
      pbi_new->decoder_recon[i].intra_pre_recon[j].mi_8x8[0] = tmp_offset
          + pbi_new->common.mi;
    }*/
  }

  pbi_new->oxcf = pbi->oxcf;
  pbi_new->source = pbi->source;
  pbi_new->source_sz = pbi->source_sz;
  pbi_new->last_time_stamp = pbi->last_time_stamp;
  pbi_new->ready_for_new_data = pbi->ready_for_new_data;
  pbi_new->refresh_frame_flags = pbi->refresh_frame_flags;
  pbi_new->decoded_key_frame = pbi->decoded_key_frame;
  pbi_new->initial_width = pbi->initial_width;
  pbi_new->initial_height = pbi->initial_height;
  pbi_new->do_loopfilter_inline = pbi->do_loopfilter_inline;
  memcpy(&pbi_new->lf_worker, &pbi->lf_worker, sizeof(VP9Worker));
  pbi_new->num_tile_workers = pbi->num_tile_workers;
  pbi_new->mi_streams = vpx_realloc(pbi_new->mi_streams, tile_rows * tile_cols *
                                    sizeof(*pbi_new->mi_streams));
  for (tile_row = 0; tile_row < tile_rows; ++tile_row) {
    for (tile_col = 0; tile_col < tile_cols; ++tile_col) {
      TileInfo tile;
      vp9_tile_init(&tile, &pbi_new->common, tile_row, tile_col);
      pbi_new->mi_streams[tile_row * tile_cols + tile_col] =
          &pbi_new->common.mi[tile.mi_row_start * pbi_new->common.mode_info_stride
                  + tile.mi_col_start];
    }
  }
  aligned_mi_cols = mi_cols_aligned_to_sb(pbi_new->common.mi_cols);
  cm_new = &pbi_new->common;
  CHECK_MEM_ERROR(cm_new, pbi_new->above_context[0], vpx_realloc(pbi_new->above_context[0],
                  sizeof(*pbi_new->above_context[0]) * MAX_MB_PLANE * 2 * aligned_mi_cols));
  for (i = 1; i < MAX_MB_PLANE; ++i) {
    pbi_new->above_context[i] = pbi_new->above_context[0] +
                            i * sizeof(*pbi_new->above_context[0]) * 2 * aligned_mi_cols;
  }

  pbi_new->tile_workers = pbi->tile_workers;
  CHECK_MEM_ERROR(cm_new, pbi_new->above_seg_context,
                  vpx_realloc(pbi->above_seg_context,
                  sizeof(*pbi_new->above_seg_context) * aligned_mi_cols));
  memcpy(pbi_new->above_seg_context, pbi->above_seg_context,
      sizeof(*pbi_new->above_seg_context) * aligned_mi_cols);

  memcpy(pbi_new->token_cache, pbi->token_cache, 1024 * sizeof(uint8_t));
  pbi_new->sched = pbi->sched;
  pbi_new->steps_pool = pbi->steps_pool;
  pbi_new->lf_steps_pool = pbi->lf_steps_pool;
  pbi_new->tsk_cache = pbi->tsk_cache;
  pbi_new->lf_tsk_cache = pbi->lf_tsk_cache;
  pbi_new->last_reader = pbi->last_reader;
  pbi_new->l_bufpool_flag_output = pbi->l_bufpool_flag_output;
  pbi_new->res = pbi->res;
  for (i = 0; i < 4; i ++) {
    memcpy(&pbi_new->tile_buffers[i], &pbi->tile_buffers[i], 64);
  }
}


void common_queue(VP9_COMMON *cm, VP9_COMMON* cm_new,int copy_flag) {
  int i, width, height, ss_x, ss_y, border, aligned_width, aligned_height,
    y_stride, yplane_size, uv_height, uv_stride, uv_border_w, uv_border_h,
    uvplane_size;
  //int mi_size;
  cm_new->fb_count = cm->fb_count;

  cm_new->error = cm->error;
  for (i =  0; i < QINDEX_RANGE; i ++) {
    vpx_memcpy(cm_new->y_dequant[i], cm->y_dequant[i], 8 * sizeof(int16_t));
    vpx_memcpy(cm_new->uv_dequant[i], cm->uv_dequant[i], 8 *sizeof(int16_t));
  }

#if CONFIG_ALPHA
  vpx_memcpy(cm_new->a_dequant, cm->a_dequant, 128*8*sizeof(int16_t));
#endif

  cm_new->color_space = cm->color_space;
  cm_new->width = cm->width;
  cm_new->height = cm->height;
  cm_new->display_width = cm->display_width;
  cm_new->display_height = cm->display_height;
  cm_new->last_width = cm->last_width;
  cm_new->last_height = cm->last_height;

  // TODO(jkoleszar): this implies chroma ss right now, but could vary per
  // plane. Revisit as part of the future change to YV12_BUFFER_CONFIG to
  // support additional planes.
  cm_new->subsampling_x = cm->subsampling_x;
  cm_new->subsampling_y = cm->subsampling_y;

  cm_new->frame_to_show = cm->frame_to_show;

  cm_new->yv12_fb = cm->yv12_fb;
  for (i =  0; i < cm->fb_count; i ++) {
    cm_new->fb_idx_ref_cnt[i] = cm->fb_idx_ref_cnt[i];
  }
  vpx_memcpy(cm_new->ref_frame_map, cm->ref_frame_map, REF_FRAMES * sizeof(int));

  // TODO(jkoleszar): could expand active_ref_idx to 4, with 0 as intra, and
  // roll new_fb_idx into it.

  // Each frame can reference ALLOWED_REFS_PER_FRAME buffers
  vpx_memcpy(cm_new->frame_refs, cm->frame_refs, REFS_PER_FRAME * sizeof(RefBuffer));
  cm_new->new_fb_idx = cm->new_fb_idx;

  cm_new->post_proc_buffer.border = cm->post_proc_buffer.border;
  cm_new->post_proc_buffer.y_width = cm->post_proc_buffer.y_width;
  cm_new->post_proc_buffer.y_height = cm->post_proc_buffer.y_height;
  cm_new->post_proc_buffer.y_crop_width = cm->post_proc_buffer.y_crop_width;
  cm_new->post_proc_buffer.y_crop_height = cm->post_proc_buffer.y_crop_height;
  cm_new->post_proc_buffer.y_stride = cm->post_proc_buffer.y_stride;
  cm_new->post_proc_buffer.uv_width = cm->post_proc_buffer.uv_width;
  cm_new->post_proc_buffer.uv_height = cm->post_proc_buffer.uv_height;
  cm_new->post_proc_buffer.uv_crop_width = cm->post_proc_buffer.uv_crop_width;
  cm_new->post_proc_buffer.uv_crop_height = cm->post_proc_buffer.uv_crop_height;
  cm_new->post_proc_buffer.uv_stride = cm->post_proc_buffer.uv_stride;
  //memcpy(cm_new->post_proc_buffer.y_buffer, cm->post_proc_buffer.y_buffer, 0);
  cm_new->post_proc_buffer.frame_size = cm->post_proc_buffer.frame_size;
  width = cm_new->post_proc_buffer.y_width;
  height = cm_new->post_proc_buffer.y_height;
  ss_x = cm->subsampling_x;
  ss_y = cm->subsampling_y;
  border = VP9BORDERINPIXELS;
  aligned_width = (width + 7) & ~7;
  aligned_height = (height + 7) & ~7;
  y_stride = ((aligned_width + 2 * border) + 31) & ~31;
  yplane_size = (aligned_height + 2 * border) * y_stride;
  uv_height = aligned_height >> ss_y;
  uv_stride = y_stride >> ss_x;
  uv_border_w = border >> ss_x;
  uv_border_h = border >> ss_y;
  uvplane_size = (uv_height + 2 * uv_border_h) * uv_stride;

  //memcpy(cm_new->post_proc_buffer.buffer_alloc, cm->post_proc_buffer.buffer_alloc, frame_size);
  cm_new->post_proc_buffer.y_buffer = cm_new->post_proc_buffer.buffer_alloc + (border * y_stride) + border;
  cm_new->post_proc_buffer.u_buffer = cm_new->post_proc_buffer.buffer_alloc + yplane_size +
                 (uv_border_h * uv_stride) + uv_border_w;
  cm_new->post_proc_buffer.v_buffer = cm_new->post_proc_buffer.buffer_alloc + yplane_size + uvplane_size +
                 (uv_border_h * uv_stride) + uv_border_w;
#if CONFIG_ALPHA
  cm_new->post_proc_buffer.alpha_width = alpha_width;
  cm_new->post_proc_buffer.alpha_height = alpha_height;
  cm_new->post_proc_buffer.alpha_stride = alpha_stride;
  cm_new->post_proc_buffer.alpha_buffer = cm_new->post_proc_buffer.buffer_alloc + yplane_size + 2 * uvplane_size +
                     (alpha_border_h * alpha_stride) + alpha_border_w;
#endif
  cm_new->post_proc_buffer.buffer_alloc_sz = cm->post_proc_buffer.buffer_alloc_sz;
  cm_new->post_proc_buffer.corrupted = cm->post_proc_buffer.corrupted;
  cm_new->post_proc_buffer.flags = cm->post_proc_buffer.flags;

  cm_new->last_frame_type = cm->last_frame_type;//   last frame's frame type for motion search.
  cm_new->frame_type = cm->frame_type;
  cm_new->show_frame = cm->show_frame;

  cm_new->last_show_frame = cm->last_show_frame;
  cm_new->show_existing_frame = cm->show_existing_frame;

  // Flag signaling that the frame is encoded using only INTRA modes.
  cm_new->intra_only = cm->intra_only;
  cm_new->allow_high_precision_mv = cm->allow_high_precision_mv;

  // Flag signaling that the frame context should be reset to default values.
  // 0 or 1 implies don't reset, 2 reset just the context specified in the
  // frame header, 3 reset all contexts.
  cm_new->reset_frame_context = cm->reset_frame_context;
  cm_new->frame_flags = cm->frame_flags;
  // MBs, mb_rows/cols is in 16-pixel units; mi_rows/cols is in
  // MODE_INFO (8-pixel) units.
  cm_new->MBs = cm->MBs;
  cm_new->mb_rows = cm->mb_rows;
  cm_new->mi_rows = cm->mi_rows;
  cm_new->mb_cols = cm->mb_cols;
  cm_new->mi_cols = cm->mi_cols;
  cm_new->mode_info_stride = cm->mode_info_stride;

  //profile settings
  cm_new->tx_mode = cm->tx_mode;

  cm_new->base_qindex = cm->base_qindex;
  cm_new->y_dc_delta_q = cm->y_dc_delta_q;
  cm_new->uv_dc_delta_q = cm->uv_dc_delta_q;
  cm_new->uv_ac_delta_q = cm->uv_ac_delta_q;
#if CONFIG_ALPHA
  cm_new->a_dc_delta_q = cm->a_dc_delta_q;
  cm_new->a_ac_delta_q = cm->a_ac_delta_q;
#endif

  // We allocate a MODE_INFO struct for each macroblock, together with
  //   an extra row on top and column on the left to simplify prediction.


  //mi_size = cm->mode_info_stride * (cm->mi_rows + MI_BLOCK_SIZE);
  //vpx_memcpy(cm_new->mip, cm->mip, mi_size * sizeof(MODE_INFO));

  //cpy_mip_ocl(&ocl_cpy_mip_obj, copy_flag ,cm, cm_new); 

  //msg("mi_size * sizeof(MODE_INFO) = %d", mi_size * sizeof(MODE_INFO));
 // vpx_memcpy(cm_new->prev_mip, cm->prev_mip, mi_size * sizeof(MODE_INFO));
  //msg("mi_size = %d, sizeof(MODE_INFO) = %d", mi_size, sizeof(MODE_INFO));
 //vpx_memcpy(cm_new->last_frame_seg_map, cm->last_frame_seg_map, cm->mi_rows * cm->mi_cols *1);
 cm_new->mip = cm->mip;
 cm_new->prev_mip = cm->prev_mip;
 cm_new->mi = cm->mi;
 cm_new->prev_mi = cm->mi;
 cm_new->mi_grid_base = cm->mi_grid_base;
 cm_new->prev_mi_grid_base = cm->prev_mi_grid_base;
 cm_new->mi_grid_visible = cm->mi_grid_visible;
 cm_new->prev_mi_grid_visible = cm->prev_mi_grid_visible;

  //cm_new->mi_grid_visible = cm_new->mi_grid_base + cm_new->mode_info_stride + 1;
  //cm_new->prev_mi = cm_new->prev_mip + cm_new->mode_info_stride + 1;
  //cm_new->prev_mi_grid_visible = cm_new->prev_mi_grid_base + cm_new->mode_info_stride + 1;
  //cm_new->mi = cm_new->mip + cm_new->mode_info_stride + 1;
  cm_new->mcomp_filter_type = cm->mcomp_filter_type;

  cm_new->lf_info = cm->lf_info;

  cm_new->refresh_frame_context = cm->refresh_frame_context;     //Two state 0 = NO, 1 = YES

  //vpx_memcpy(cm_new->ref_frame_sign_bias, cm->ref_frame_sign_bias, MAX_REF_FRAMES * sizeof(int));
  cm_new->lf = cm->lf;
  cm_new->seg = cm->seg;

  // Context probabilities for reference frame prediction
  cm_new->allow_comp_inter_inter = cm->allow_comp_inter_inter;
  cm_new->comp_fixed_ref = cm->comp_fixed_ref;
  vpx_memcpy(cm_new->comp_var_ref, cm->comp_var_ref, 2 * sizeof(MV_REFERENCE_FRAME));
  cm_new->reference_mode = cm->reference_mode;

  cm_new->fc = cm->fc;
  vpx_memcpy(cm_new->frame_contexts, cm->frame_contexts, FRAME_CONTEXTS * sizeof(FRAME_CONTEXT));
  cm_new->frame_context_idx = cm->frame_context_idx;
  cm_new->counts = cm->counts;
  cm_new->current_video_frame = cm->current_video_frame;
  cm_new->version = cm->version;

#if CONFIG_VP9_POSTPROC
  cm_new->postproc_state = cm->postproc_state;
#endif

  cm_new->error_resilient_mode = cm->error_resilient_mode;
  cm_new->frame_parallel_decoding_mode = cm->frame_parallel_decoding_mode;
  cm_new->log2_tile_cols = cm->log2_tile_cols;

  cm_new->fb_list = cm->fb_list;
  //this should be move to the head of this function
  //cm_new->fb_count = cm->fb_count;
  cm_new->realloc_fb_cb = cm->realloc_fb_cb;
  cm_new->user_priv = cm->user_priv;

  cm_new->fb_lru = cm->fb_lru;
  if (cm_new->fb_lru) {
    for (i =  0; i < cm->fb_count; i ++) {
      cm_new->fb_idx_ref_lru[i] = cm->fb_idx_ref_lru[i];
    }
  }
  cm_new->fb_idx_ref_lru_count = cm->fb_idx_ref_lru_count;
 
}

/* If any buffer updating is signaled it should be done here. */
void swap_frame_buffers_recon(VP9D_COMP *pbi) {
  int ref_index = 0, mask;
  VP9_COMMON *const cm = &pbi->common;

  for (mask = pbi->refresh_frame_flags; mask; mask >>= 1) {
    if (mask & 1)
      ref_cnt_fb(cm->fb_idx_ref_cnt, &cm->ref_frame_map[ref_index],
                 cm->new_fb_idx);
    ++ref_index;
  }

  cm->frame_to_show = get_frame_new_buffer(cm);
  cm->fb_idx_ref_cnt[cm->new_fb_idx]--;

  // Invalidate these references until the next frame starts.
  for (ref_index = 0; ref_index < 3; ref_index++)
    cm->frame_refs[ref_index].idx = INT_MAX;
}

void pbi_copy(VP9D_COMP *pbi, VP9D_COMP *pbi_new) {

  VP9_COMMON *cm = &pbi->common;

#if USE_PPA
  PPAStartCpuEventFunc(vp9_cpy_mi_time);
#endif
  pbi_queue(pbi, pbi_new);
  vp9_decode_frame_tail(pbi_new);
  pbi_queue_recon(pbi_new, pbi);

  swap_frame_buffers_recon(pbi);
  cm = &pbi->common;

  cm->last_show_frame = cm->show_frame;
  if (1) { //cm->show_frame) {
    if (!cm->show_existing_frame) {
      // current mip will be the prev_mip for the next frame
      MODE_INFO *temp = cm->prev_mip;
      MODE_INFO **temp2 = cm->prev_mi_grid_base;

      cm->prev_mip = cm->mip;
      cm->mip = cm->trip_mip;
      cm->trip_mip = temp;

      cm->prev_mi_grid_base = cm->mi_grid_base;
      cm->mi_grid_base = cm->trip_mi_grid_base;
      cm->trip_mi_grid_base = temp2;

      // update the upper left visible macroblock ptrs
      cm->mi = cm->mip + cm->mode_info_stride + 1;
      cm->prev_mi = cm->prev_mip + cm->mode_info_stride + 1;
      cm->mi_grid_visible = cm->mi_grid_base + cm->mode_info_stride + 1;
      cm->prev_mi_grid_visible = cm->prev_mi_grid_base +
                                 cm->mode_info_stride + 1;

      pbi->mb.mi_8x8 = cm->mi_grid_visible;
      pbi->mb.mi_8x8[0] = cm->mi;
    }
    cm->current_video_frame++;
  }
#if USE_PPA
  PPAStopCpuEventFunc(vp9_cpy_mi_time);
#endif
}

/*store_intra_info_recon, this function store the necessary
    parameter used by the intra predicition,intra dequantization,
    intra inv-transformation of intra block*/
void store_frame_size(VP9D_COMP *pbi, int *width, int *height) {
  VP9_COMMON *cm = &pbi->common;
  *width = cm->width;
  *height = cm->height;
}

void store_intra_info_recon(MACROBLOCKD *xd, int offset,int mi_col, int mi_row,
                         BLOCK_SIZE bsize, VP9_DECODER_RECON *decoder_recon) {
  int i = 0;
  INTRA_PRE_RECON *intra =
      &decoder_recon->intra_pre_recon[decoder_recon->intra_blocks_count];

  intra->bsize = bsize;
  intra->mi_col = mi_col;
  intra->mi_row = mi_row;
  intra->up_available = xd->up_available;
  intra->left_available = xd->left_available;
  intra->lossless = xd->lossless;
  intra->mb_to_top_edge = xd->mb_to_top_edge;
  intra->mb_to_bottom_edge = xd->mb_to_bottom_edge;
  intra->mb_to_left_edge = xd->mb_to_left_edge;
  intra->mb_to_right_edge = xd->mb_to_right_edge;
  intra->mi_8x8 = xd->mi_8x8;

  if (!xd->mi_8x8[0]->mbmi.skip_coeff) {
    intra->offset = offset;
    intra->skip_coeff = xd->mi_8x8[0]->mbmi.skip_coeff;
    intra->qcoeff_flag = decoder_recon->dequant_count;
  } else {
    intra->skip_coeff = xd->mi_8x8[0]->mbmi.skip_coeff;
  }

  for (i = 0; i<MAX_MB_PLANE; i++) {
    intra->dst[i] = xd->plane[i].dst.buf;
  }
}

/*store_inter_info_recon, this function store the necessary
    parameter used by the intra predicition,intra dequantization,
    intra inv-transformation of intra block*/
void store_inter_info_recon(MACROBLOCKD *xd, int offset,int mi_col, int mi_row,
                         BLOCK_SIZE bsize, VP9_DECODER_RECON *decoder_recon) {
  int i = 0;
  INTER_PRE_RECON *inter =
      &decoder_recon->inter_pre_recon[decoder_recon->inter_blocks_count];

  inter->mi_col = mi_col;
  inter->mi_row = mi_row;
  inter->bsize = bsize;
  inter->up_available = xd->up_available;
  inter->left_available = xd->left_available;
  inter->lossless = xd->lossless;
  inter->mi_8x8 = xd->mi_8x8;

  if (!xd->mi_8x8[0]->mbmi.skip_coeff) {
    inter->offset = offset;
    inter->skip_coeff = xd->mi_8x8[0]->mbmi.skip_coeff;
    inter->qcoeff_flag = decoder_recon->dequant_count;
  } else {
    inter->skip_coeff = xd->mi_8x8[0]->mbmi.skip_coeff;
  }

  inter->mb_to_top_edge = xd->mb_to_top_edge;
  inter->mb_to_bottom_edge = xd->mb_to_bottom_edge;
  inter->mb_to_left_edge = xd->mb_to_left_edge;
  inter->mb_to_right_edge = xd->mb_to_right_edge;

  for (i = 0; i < MAX_MB_PLANE; i++) {
    inter->dst[i] = xd->plane[i].dst.buf;
  }
}

/*store_eobtotal_less8x8_recon, this function store the necessary
    parameter used by the inv-transformation of intra block*/
void store_eobtotal_less8x8_recon(int less8x8, int16_t eobtotal,
                                          unsigned char skip_coeff_org,
                                          VP9_DECODER_RECON *decoder_recon) {
  INTER_PRE_RECON *inter =
      &decoder_recon->inter_pre_recon[decoder_recon->inter_blocks_count];

  inter->eobtotal = eobtotal;
  inter->less8x8 = less8x8;
  inter->skip_coeff_org = skip_coeff_org;

}

void free_buffers_recon(VP9_DECODER_RECON *decoder_recon) {

  vpx_free(decoder_recon->inter_pre_recon);
  vpx_free(decoder_recon->intra_pre_recon);
  vpx_free(decoder_recon->dequant_recon);

  decoder_recon->inter_pre_recon = NULL;
  decoder_recon->intra_pre_recon = NULL;
  decoder_recon->dequant_recon = NULL;

  decoder_recon->inter_blocks_count = 0;
  decoder_recon->intra_blocks_count = 0;
  decoder_recon->dequant_count = 0;

}

int alloc_buffers_recon(VP9_COMMON *cm, VP9_DECODER_RECON *decoder_recon) {
  int mi8x8_size;
  int mi64x64_size;
  int col_offset;
  const int tile_rows = 1 << cm->log2_tile_rows;
  const int tile_cols = cm->log2_tile_cols;

  free_buffers_recon(decoder_recon);
  
  assert(tile_rows <= 1);
  mi8x8_size = (cm->mi_rows) * cm->mi_cols;
  mi64x64_size = MAX_64X64_COLS * MAX_64X64_ROWS;

  if(tile_cols > 0) {
    col_offset = DEV_THREE -tile_cols;
    mi8x8_size = mi8x8_size * col_offset /DEV_THREE;
    mi64x64_size = mi64x64_size * col_offset /DEV_THREE;
  }
   
  decoder_recon->dequant_recon =
      vpx_memalign(16, mi64x64_size * sizeof(DEQUANT_RECON));
  if (!decoder_recon->dequant_recon)
    goto fail;
  
  decoder_recon->inter_pre_recon =
      vpx_calloc(mi8x8_size, sizeof(INTER_PRE_RECON));
  if (!decoder_recon->inter_pre_recon)
    goto fail;

  decoder_recon->intra_pre_recon =
      vpx_calloc(mi8x8_size, sizeof(INTRA_PRE_RECON));
  if(!decoder_recon->intra_pre_recon)
    goto fail;
 
  return 0;

 fail:
  free_buffers_recon(decoder_recon);
  return 1;
}

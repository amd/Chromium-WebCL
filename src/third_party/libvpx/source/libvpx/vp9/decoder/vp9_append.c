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

#include "vpx_mem/vpx_mem.h"
#include "vpx_ports/mem.h"
#include "vp9/common/vp9_blockd.h"

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_seg_common.h"
#include "vp9/decoder/vp9_onyxd_int.h"


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



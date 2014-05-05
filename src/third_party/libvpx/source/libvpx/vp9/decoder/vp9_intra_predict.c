/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "vp9/common/vp9_enums.h"
#include "vp9/common/vp9_blockd.h"
#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/common/vp9_reconintra.h"
#include "vp9/common/vp9_idct.h"
#include "vp9/decoder/vp9_onyxd_int.h"

struct intra_predict_args {
  MACROBLOCKD *xd;
  uint8_t *token_cache;
};

static void inverse_transform_block(MACROBLOCKD* xd, int plane, int block,
                                    TX_SIZE tx_size, uint8_t *dst, int stride,
                                    uint8_t *token_cache) {
  struct macroblockd_plane *const pd = &xd->plane[plane];
  const int eob = pd->eobs[block];
  if (eob > 0) {
    TX_TYPE tx_type = 0;
    const int plane_type = pd->plane_type;
    int16_t *const dqcoeff = BLOCK_OFFSET(pd->dqcoeff, block);
    switch (tx_size) {
      case TX_4X4:
        tx_type = get_tx_type_4x4(plane_type, xd, block);
        if (tx_type == DCT_DCT)
          xd->itxm_add(dqcoeff, dst, stride, eob);
        else
          vp9_iht4x4_16_add(dqcoeff, dst, stride, tx_type);
        break;
      case TX_8X8:
        tx_type = get_tx_type_8x8(plane_type, xd);
        vp9_iht8x8_add(tx_type, dqcoeff, dst, stride, eob);
        break;
      case TX_16X16:
        tx_type = get_tx_type_16x16(plane_type, xd);
        vp9_iht16x16_add(tx_type, dqcoeff, dst, stride, eob);
        break;
      case TX_32X32:
        tx_type = DCT_DCT;
        vp9_idct32x32_add(dqcoeff, dst, stride, eob);
        break;
      default:
        assert(!"Invalid transform size");
    }

    if (eob == 1) {
      vpx_memset(dqcoeff, 0, 2 * sizeof(dqcoeff[0]));
      vpx_memset(token_cache, 0, 2 * sizeof(token_cache[0]));
    } else {
      if (tx_type == DCT_DCT && tx_size <= TX_16X16 && eob <= 10) {
        vpx_memset(dqcoeff, 0, 4 * (4 << tx_size) * sizeof(dqcoeff[0]));
        vpx_memset(token_cache, 0,
                   4 * (4 << tx_size) * sizeof(token_cache[0]));
      } else if (tx_size == TX_32X32 && eob <= 34) {
        vpx_memset(dqcoeff, 0, 256 * sizeof(dqcoeff[0]));
        vpx_memset(token_cache, 0, 256 * sizeof(token_cache[0]));
      } else {
        vpx_memset(dqcoeff, 0, (16 << (tx_size << 1)) * sizeof(dqcoeff[0]));
        vpx_memset(token_cache, 0,
                   (16 << (tx_size << 1)) * sizeof(token_cache[0]));
      }
    }
  }
}

static void decode_block_intra_recon(int plane, int block,
                                     BLOCK_SIZE plane_bsize,
                                     TX_SIZE tx_size, void *args) {
  struct intra_predict_args *const arg = args;
  MACROBLOCKD *const xd = arg->xd;
  struct macroblockd_plane *const pd = &xd->plane[plane];
  MODE_INFO *const mi = xd->mi_8x8[0];
  const MB_PREDICTION_MODE mode = (plane == 0)
      ? ((mi->mbmi.sb_type < BLOCK_8X8) ? mi->bmi[block].as_mode
      : mi->mbmi.mode) : mi->mbmi.uv_mode;
  int x, y;
  uint8_t *dst;
  txfrm_block_to_raster_xy(plane_bsize, tx_size, block, &x, &y);
  dst = &pd->dst.buf[4 * y * pd->dst.stride + 4 * x];

  if (xd->mb_to_right_edge < 0 || xd->mb_to_bottom_edge < 0)
    extend_for_intra(xd, plane_bsize, plane, x, y);

  vp9_predict_intra_block(xd, block >> (tx_size << 1),
                          b_width_log2(plane_bsize), tx_size, mode,
                          dst, pd->dst.stride, dst, pd->dst.stride,
                          x, y, plane);

  if (!mi->mbmi.skip_coeff)
    inverse_transform_block(xd, plane, block, tx_size, dst, pd->dst.stride,
                            arg->token_cache);
}

int vp9_intra_predict_recon(void *func, MACROBLOCKD *xd,
    VP9_DECODER_RECON *decoder_recon, int i_intra_blocks_count) {
  int ret = 0, i = 0, bsize = 0;

  INTRA_PRE_RECON *intra =
      &decoder_recon->intra_pre_recon[i_intra_blocks_count];

  struct intra_predict_args args = {
      xd, decoder_recon->token_cache
  };

  xd->mb_to_left_edge = intra->mb_to_left_edge;
  xd->mb_to_right_edge = intra->mb_to_right_edge;
  xd->mb_to_top_edge = intra->mb_to_top_edge;
  xd->mb_to_bottom_edge = intra->mb_to_bottom_edge;

  xd->up_available = intra->up_available;
  xd->left_available = intra->left_available;

  for (i = 0; i < MAX_MB_PLANE; i++) {
    xd->plane[i].dst.buf = intra->dst[i];

    xd->plane[i].dqcoeff
        = decoder_recon->dequant_recon[intra->qcoeff_flag].qcoeff[i]
        + intra->offset;

    xd->plane[i].eobs = decoder_recon->dequant_recon[intra->qcoeff_flag].eobs[i]
        + intra->offset /16;
  }

  xd->mi_8x8 = intra->mi_8x8;

  xd->itxm_add = func;

  xd->lossless = intra->lossless;

  bsize = intra->bsize;

  foreach_transformed_block(xd, bsize, decode_block_intra_recon, &args);

  return ret;
}


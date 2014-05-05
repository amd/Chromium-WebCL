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
#include "vp9/decoder/vp9_onyxd_int.h"
#include "vp9/decoder/vp9_idct_blk.h"

void decode_block_recon(int plane, int block, BLOCK_SIZE plane_bsize,
                        TX_SIZE tx_size, void *arg) {
  MACROBLOCKD *const xd = (MACROBLOCKD *)arg;
  struct macroblockd_plane *const pd = &xd->plane[plane];
  int16_t *const qcoeff = BLOCK_OFFSET(pd->q_coeff, block);
  const int stride = pd->dst.stride;
  const int eob = pd->peob[block];
  const int raster_block = txfrm_block_to_raster_block(plane_bsize, tx_size,
                                                       block);
  uint8_t *const dst = raster_block_offset_uint8(plane_bsize, raster_block,
                                                 pd->dst.buf, stride);
  switch (tx_size) {
    case TX_4X4: {
      const TX_TYPE tx_type = get_tx_type_4x4(pd->plane_type, xd, raster_block);
      if (tx_type == DCT_DCT)
        xd->itxm_add(qcoeff, dst, stride, eob);
      else
        vp9_iht_add_c(tx_type, qcoeff, dst, stride, eob);
      break;
    }
    case TX_8X8:
      vp9_iht_add_8x8_c(get_tx_type_8x8(pd->plane_type, xd), qcoeff, dst,
                        stride, eob);
      break;
    case TX_16X16:
      vp9_iht_add_16x16_c(get_tx_type_16x16(pd->plane_type, xd), qcoeff, dst,
                          stride, eob);
      break;
    case TX_32X32:
      vp9_idct_add_32x32(qcoeff, dst, stride, eob);
      break;
    default:
      assert(!"Invalid transform size");
  }

}

static void decode_transform_recon(int plane, int block, BLOCK_SIZE plane_bsize,
                                   TX_SIZE tx_size, void *arg) {
  MACROBLOCKD *const xd = (MACROBLOCKD *)arg;
  MODE_INFO *const mi = xd->this_mi;

  if (!mi->mbmi.skip_coeff)
    decode_block_recon(plane, block, plane_bsize, tx_size, arg);
}

int vp9_decode_transform(void *func, VP9_DECODER_RECON *decoder_recon,
                         int i_inter_blocks_count, MACROBLOCKD *xd) {
  int ret = 0, i = 0, bsize = 0;

  INTER_PRE_RECON *inter =
    &decoder_recon->inter_pre_recon[i_inter_blocks_count];

  xd->up_available = inter->up_available;
  xd->left_available = inter->left_available;

  for (i = 0; i < MAX_MB_PLANE; i++) {
   
    if(!(xd->this_mi->mbmi.skip_coeff)) {
      xd->plane[i].q_coeff =
        decoder_recon->dequant_recon[inter->qcoeff_flag].qcoeff[i] + 
        inter->offset;
    }

    xd->plane[i].peob = inter->eobs[i];
  }

  xd->itxm_add = func;
  xd->lossless = inter->lossless;
  bsize = inter->bsize;
  foreach_transformed_block(xd, bsize, decode_transform_recon, xd);

  return ret;
}


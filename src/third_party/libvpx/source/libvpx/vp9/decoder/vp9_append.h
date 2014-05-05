/*
  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_APPEND_H_
#define VP9_APPEND_H_

enum { MAX_64X64_ROWS = 60};
enum { MAX_64X64_COLS = 34};
enum { DEV_THREE = 3};

#include "vp9/common/vp9_enums.h"
#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/decoder/vp9_onyxd_int.h"

void store_frame_size(VP9D_COMP *pbi, int *width, int *height);

void store_inter_info_recon(MACROBLOCKD *xd, int offset,int mi_col,
    int mi_row, BLOCK_SIZE bsize, VP9_DECODER_RECON *decoder_recon);

void store_intra_info_recon(MACROBLOCKD *xd, int offset,int mi_col,
    int mi_row, BLOCK_SIZE bsize, VP9_DECODER_RECON *decoder_recon);

void store_eobtotal_less8x8_recon(int less8x8,int16_t eobtotal,
                                          unsigned char skip_coeff_org,
                                          VP9_DECODER_RECON *decoder_recon);

int alloc_buffers_recon(VP9_COMMON *cm, VP9_DECODER_RECON *decoder_recon);

void free_buffers_recon(VP9_DECODER_RECON *decoder_recon);

#endif


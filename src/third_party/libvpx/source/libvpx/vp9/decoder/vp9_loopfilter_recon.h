/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_DECODER_VP9_LOOPFILTER_RECON_H_
#define VP9_DECODER_VP9_LOOPFILTER_RECON_H_

#include "vpx_ports/mem.h"
#include "vpx_config.h"

#include "vp9/common/vp9_blockd.h"
#include "vp9/common/vp9_seg_common.h"
#include "vp9/decoder/vp9_onyxd_int.h"

void vp9_loop_filter_init_wpp(VP9_COMMON *cm);

void vp9_loop_filter_frame_wpp(VP9D_COMP *pbi, VP9_COMMON *cm,
                               MACROBLOCKD *xd, int frame_filter_level,
                               int y_only, int partial);


void vp9_loop_filter_block(const YV12_BUFFER_CONFIG *frame_buffer,
                           VP9_COMMON *cm, MACROBLOCKD *xd,
                           int mi_row, int mi_col, int y_only);

#endif

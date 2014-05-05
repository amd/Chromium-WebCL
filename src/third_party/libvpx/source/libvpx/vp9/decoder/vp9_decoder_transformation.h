/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_DECODER_TRANSFORM_H_
#define VP9_DECODER_TRANSFORM_H_

void decode_block_recon(int plane, int block, 
                        BLOCK_SIZE plane_bsize, 
                        TX_SIZE tx_size, void *arg);

int vp9_decode_transform(void *func, VP9_DECODER_RECON *decoder_recon,
                         int i_inter_blocks_count, MACROBLOCKD *xd);

#endif  // VP9_DECODER_TRANSFORM_H_


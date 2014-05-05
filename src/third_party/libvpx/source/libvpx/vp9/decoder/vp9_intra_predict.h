/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#ifndef VP9_INTRA_PREDICT_H_
#define VP9_INTRA_PREDICT_H_

int vp9_intra_predict_recon(void *func, MACROBLOCKD *xd,
                 VP9_DECODER_RECON *decoder_recon, int i_intra_blocks_count);

#endif  // VP9_INTRA_PREDICT_H_


/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_INTER_OCL_CALCU_H_
#define VP9_INTER_OCL_CALCU_H_

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_onyxc_int.h"

int get_second_ref_count_ocl();

int inter_pred_index_ocl_whole_frame(VP9_COMMON *const cm);

int inter_pred_calcu_ocl(VP9_COMMON *const cm, int tile_num, int dev_gpu);

#endif // VP9_INTER_OCL_CALCU_H_

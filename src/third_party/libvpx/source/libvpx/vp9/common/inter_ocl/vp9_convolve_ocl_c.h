/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_CONVOLVE_OCL_C_H_
#define VP9_CONVOLVE_OCL_C_H_

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"

void *build_inter_pred_calcu_ocl_mt(void *pred_param);

void build_inter_pred_calcu_ocl_c(int tile_num, uint8_t *new_buffer);

#endif  // VP9_ONVOLVE_OCL_C_H_

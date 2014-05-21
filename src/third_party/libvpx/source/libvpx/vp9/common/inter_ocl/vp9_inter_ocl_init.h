/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_INTER_OCL_INIT_H_
#define VP9_INTER_OCL_INIT_H_

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_onyxc_int.h"

#define USE_INTER_PREDICT_OCL 1

#define USE_INTER_PARAM_ZERO_COPY 1

#define USE_KERNEL_UPDATE_BUFFER_POOL 0

#define DO_PROFILING 0

int vp9_init_ocl();

int vp9_init_ocl_ex(void *id3d9_devices);

int vp9_release_ocl();

int vp9_init_inter_ocl(VP9_COMMON *const cm, int tile_count);

int reset_inter_ocl_param_buffer(int tile_num);

int vp9_inter_write_param_to_gpu(int tile_num);

int vp9_update_gpu_buffer_pool(VP9_COMMON *const cm);

#endif // VP9_INTER_OCL_INIT_H_

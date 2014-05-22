/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"

#define COPY_MIP_OCL 0
#define COPY_MIP_GPU (USE_INTER_PREDICT_OCL & COPY_MIP_OCL)

typedef struct cpy_mip_ocl_obj {
  cl_mem mip_src;
  cl_mem prev_mip;
  cl_mem mip_dst;

  cl_mem mip_dst_1;
  cl_kernel cpy_mip_kernel;
  cl_uchar *src_mip_map;
  cl_uchar *dst_mip_map;

  cl_uchar *dst_mip_map_1;

  cl_uchar *prev_mip_map;
  cl_program program;
  char *source;
  size_t source_len;
  size_t globalThreads[3];
  size_t localThreads[3];

  cl_double cpy_mip_kernel_time;
  cl_uint cpy_mip_run_times;

  int offset[3];

  int cpy_size;

}CPY_MIP_OCL_OBJ;

int create_cpy_mip_kernel();

int init_cpy_mip_ocl(VP9_COMMON *cm, CPY_MIP_OCL_OBJ *obj);

int release_cpy_mip_ocl(CPY_MIP_OCL_OBJ *obj);

int cpy_mip_ocl(CPY_MIP_OCL_OBJ *obj,
                int copy_flag,
                VP9_COMMON *cm,
                VP9_COMMON *cm_new);

extern CPY_MIP_OCL_OBJ ocl_cpy_mip_obj;

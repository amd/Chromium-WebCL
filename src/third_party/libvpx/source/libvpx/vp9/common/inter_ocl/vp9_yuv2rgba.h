/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef VP9_YUV2RGBA_H_
#define VP9_YUV2RGBA_H_

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_blockd.h"
#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"

extern FILE *pLog;

#define IMAGE_CACHE 50
typedef struct vp9_yuv2rgba_ocl {
  int sub_sampling_x;
  int sub_sampling_y;
  int output_format;
  int real_width;
  int real_height;
  int real_uv_width;
  int real_uv_height;
  int y_width;
  int uv_width;
  int uv_height;
  int y_height;
  int Y_stride;
  int UV_stride;
  int y_plane_offset;
  int u_plane_offset;
  int v_plane_offset;
  int use_ex_flag;
  cl_mem rgb_buffer;
  uint8_t *rgb_map;
  int show_id;

  int prev_fb_idx;
 
  
  cl_mem clImages[50];
  void *d3d9_surfaces[50];
  int surface_index;
  char *source;
  size_t source_len;
  size_t globalThreads[2];
  
  //cl_mem clImag;
  cl_program program;
  cl_kernel yuv_rgba_kernel;
  cl_kernel only_color_space_transform_kernel; //for the last frame
} VP9_YUV2RGBA_OCL;

struct IDirect3DSurface9;

int init_yuv2rgba_ocl_obj();

int release_yuv2rgba_ocl_obj();

int vp9_yuv2rgba( VP9_YUV2RGBA_OCL *yuv2rgba_ocl_obj, void *texture) ;


extern VP9_YUV2RGBA_OCL yuv2rgba_ocl_obj;


#endif

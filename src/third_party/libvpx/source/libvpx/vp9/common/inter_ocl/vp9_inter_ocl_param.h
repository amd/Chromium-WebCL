/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VP9_INTER_OCL_PARAM_H_
#define VP9_INTER_OCL_PARAM_H_

#include "vp9/common/vp9_common.h"
#include "vp9/common/vp9_blockd.h"
#include "vp9/common/vp9_onyxc_int.h"
#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"

#include "vp9/sched/atomic.h"
#include "vp9/sched/thread.h"

#define MAX_TILE_COUNT_OCL 4

typedef struct inter_pred_param_gpu {
  int src_stride;
  int filter_x_mv;
  int filter_y_mv;
}INTER_PRED_PARAM_GPU;

typedef struct inter_pred_param_cpu {
  int pred_mode;

  uint8_t * psrc;
  int src_stride;

  int dst_mv;
  int dst_stride;

  const short *filter_y;
  const short *filter_x;

  int x_step_q4;
  int y_step_q4;

  int w;
  int h;

  int reset_src_buffer;

  uint8_t *pref;
  uint8_t *buf_ptr1;

  int pre_stride;

  int x0;
  int x1;

  int y0;
  int y1;

  int frame_width;
  int frame_height;
}INTER_PRED_PARAM_CPU;

typedef struct inter_pred_args {
  MACROBLOCKD *xd;
  int x, y;
}INTER_PRED_ARGS_OCL;

typedef struct inter_index_param {
    int pred_mode;
    int buf_offset;
    int dst_offset;
    int dst_stride;
    int pre_stride;
    int src_num;
    int w;
    int h;
    int sub_x;
    int sub_y;
}INTER_INDEX_PARAM_GPU;

typedef struct inter_mt_attr {
  int tile_num;
  uint8_t *new_buffer;
}INTER_MT_ATTR;

typedef struct inter_ocl_obj {
  int inter_ocl_init;
  int previous_f;
  int before_previous_f;
  int previous_f_show;
  int tile_count;
  int release_max;
  int buffer_size;
  int pred_param_size;
  int pred_param_size_all;
  int buffer_pool_size;
  int buffer_pool_flag;

  int index_size_param_num;
  int index_size_param_num_all;
  int index_size_xmv;
  int index_size_xmv_all;
  uint8_t *new_buffer[MAX_TILE_COUNT_OCL];

  int base_w;
  int base_h;

  char *source;
  size_t source_len;
  size_t globalThreads[3];
  size_t localThreads[3];

  convolve_fn_t switch_convolve_t[32];

  cl_program program;
  cl_kernel kernel;
  cl_kernel update_buffer_pool_kernel;

  cl_mem buffer_pool_kernel;
  cl_mem buffer_pool_read_only_kernel;
  uint8_t  *buffer_pool_map_ptr;

  cl_mem new_buffer_kernel[MAX_TILE_COUNT_OCL];

  cl_mem pred_param_kernel_td0;
  cl_mem pred_param_kernel_td1;
  cl_mem *pred_param_kernel_pre;
  cl_mem *pred_param_kernel;

  cl_mem index_param_num_kernel_td0;
  cl_mem index_param_num_kernel_td1;
  cl_mem *index_param_num_kernel_pre;
  cl_mem *index_param_num_kernel;

  cl_mem index_xmv_kernel_td0;
  cl_mem index_xmv_kernel_td1;
  cl_mem *index_xmv_kernel_pre;
  cl_mem *index_xmv_kernel;

  cl_mem dst_index_xmv_kernel_td0;
  cl_mem dst_index_xmv_kernel_td1;
  cl_mem *dst_index_xmv_kernel_pre;
  cl_mem *dst_index_xmv_kernel;

  cl_mem case_count_kernel_td0;
  cl_mem case_count_kernel_td1;
  cl_mem *case_count_kernel_pre;
  cl_mem *case_count_kernel;

  int *case_count_gpu;
  int *case_count_gpu_td0;
  int *case_count_gpu_td1;

  cl_mem one_case_interval_count_kernel;

  int switch_td_param;
  int switch_td_calcu_gpu;
  int switch_td_calcu_cpu[MAX_TILE_COUNT_OCL];

  int index_count_td0[MAX_TILE_COUNT_OCL];
  int index_count_td1[MAX_TILE_COUNT_OCL];
  int cpu_fri_count_td0[MAX_TILE_COUNT_OCL];
  int cpu_fri_count_td1[MAX_TILE_COUNT_OCL];
  int cpu_sec_count_td0[MAX_TILE_COUNT_OCL];
  int cpu_sec_count_td1[MAX_TILE_COUNT_OCL];

  int *all_b_count_gpu;
  int *gpu_block_count;

  int *index_count;
  int *index_count_pre;

  int *cpu_fri_count_pre;
  int *cpu_sec_count_pre;
  int *cpu_fri_count[MAX_TILE_COUNT_OCL];
  int *cpu_sec_count[MAX_TILE_COUNT_OCL];

  int all_of_block_count_gpu;
  int param_count_gpu_all;
  int tile_param_count_gpu;

  int *index_case_gpu;
  int *one_case_interval_count_gpu;

  int tile_param_count_gpu_offset[MAX_TILE_COUNT_OCL];
  int index_case_mode_offset[4];

  INTER_PRED_PARAM_GPU *pred_param_gpu[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_GPU *pred_param_gpu_td0[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_GPU *pred_param_gpu_td1[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_GPU *pred_param_gpu_pre[MAX_TILE_COUNT_OCL];

  INTER_PRED_PARAM_CPU *pred_param_cpu_fri_td0[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_fri_td1[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_sec_td0[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_sec_td1[MAX_TILE_COUNT_OCL];

  INTER_PRED_PARAM_CPU *pred_param_cpu_fri[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_sec[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_fri_pre[MAX_TILE_COUNT_OCL];
  INTER_PRED_PARAM_CPU *pred_param_cpu_sec_pre[MAX_TILE_COUNT_OCL];

  uint8_t *ref_buffer[MAX_TILE_COUNT_OCL];
  uint8_t *pref[MAX_TILE_COUNT_OCL];

  // This for inter parameter index
  cl_kernel kernel_index;

  int index_param_size;

  cl_mem index_param_kernel;

  cl_mem new_fb_idx_kernel;
  cl_mem buffer_size_kernel;
  cl_mem gpu_block_count_kernel;
  cl_mem index_case_mode_offset_kernel;
  cl_mem tile_param_count_gpu_offset_kernel;

  cl_mem all_b_count_kernel;
  cl_mem tile_count_kernel;

  int *new_fb_idx_gpu;
  int *buffer_size_gpu;
  int *index_case_mode_offset_gpu;
  int *tile_param_count_gpu_offset_gpu;

  INTER_INDEX_PARAM_GPU *index_param_gpu[MAX_TILE_COUNT_OCL];
  INTER_INDEX_PARAM_GPU *index_param_gpu_pre[MAX_TILE_COUNT_OCL];
}INTER_OCL_OBJ;

int vp9_setup_interp_filters_ocl(MACROBLOCKD *xd,
                                 INTERPOLATION_TYPE mcomp_filter_type,
                                 VP9_COMMON *cm);

void build_inter_pred_param_sec_ref_ocl(const int plane,
                                        const int block,
                                        BLOCK_SIZE bsize, void *argv,
                                        VP9_COMMON *const cm,
                                        const int src_num,
                                        const int filter_num,
                                        const int tile_num);

void build_inter_pred_param_fri_ref_ocl(const int plane,
                                        const int block,
                                        BLOCK_SIZE bsize, void *argv,
                                        VP9_COMMON *const cm,
                                        const int ref_idx,
                                        const int src_num,
                                        const int filter_num,
                                        const int tile_num);


void build_mc_border_ocl(const uint8_t *src,
                         int src_stride,
                         uint8_t *dst,
                         int dst_stride,
                         int x, int y,
                         int b_w, int b_h,
                         int w, int h);

extern int CpuFlag;
extern OCL_CONTEXT ocl_context;
extern INTER_OCL_OBJ inter_ocl_obj;

#endif // VP9_INTER_OCL_PARAM_H_

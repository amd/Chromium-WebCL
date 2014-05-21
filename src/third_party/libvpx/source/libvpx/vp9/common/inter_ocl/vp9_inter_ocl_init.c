/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <assert.h>
#include <stdio.h>

#include "vpx_mem/vpx_mem.h"
#include "vp9/sched/thread.h"

#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"
#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/vp9_yuv2rgba.h"
#include <d3d9.h>

#define LOGI(...) fprintf(stdout, __VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)

#define STABLE_BUFFER_SIZE_OCL   15475200

#define FREE_INTER(alloc_p)      vpx_free(alloc_p)
#define MALLOC_INTER(type, size) (type *)vpx_memalign(32, size)

static void inter_convolve_init_ocl() {
  inter_ocl_obj.switch_convolve_t[0] = vp9_convolve_copy;
  inter_ocl_obj.switch_convolve_t[1] = vp9_convolve_avg;
  inter_ocl_obj.switch_convolve_t[2] = vp9_convolve8_vert;
  inter_ocl_obj.switch_convolve_t[3] = vp9_convolve8_avg_vert;
  inter_ocl_obj.switch_convolve_t[4] = vp9_convolve8_horiz;
  inter_ocl_obj.switch_convolve_t[5] = vp9_convolve8_avg_horiz;
  inter_ocl_obj.switch_convolve_t[6] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[7] = vp9_convolve8_avg;

  inter_ocl_obj.switch_convolve_t[8] = vp9_convolve8_vert;
  inter_ocl_obj.switch_convolve_t[9] = vp9_convolve8_avg_vert;
  inter_ocl_obj.switch_convolve_t[10] = vp9_convolve8_vert;
  inter_ocl_obj.switch_convolve_t[11] = vp9_convolve8_avg_vert;
  inter_ocl_obj.switch_convolve_t[12] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[13] = vp9_convolve8_avg;
  inter_ocl_obj.switch_convolve_t[14] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[15] = vp9_convolve8_avg;

  inter_ocl_obj.switch_convolve_t[16] = vp9_convolve8_horiz;
  inter_ocl_obj.switch_convolve_t[17] = vp9_convolve8_avg_horiz;
  inter_ocl_obj.switch_convolve_t[18] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[19] = vp9_convolve8_avg;
  inter_ocl_obj.switch_convolve_t[20] = vp9_convolve8_horiz;
  inter_ocl_obj.switch_convolve_t[21] = vp9_convolve8_avg_horiz;
  inter_ocl_obj.switch_convolve_t[22] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[23] = vp9_convolve8_avg;

  inter_ocl_obj.switch_convolve_t[24] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[25] = vp9_convolve8_avg;
  inter_ocl_obj.switch_convolve_t[26] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[27] = vp9_convolve8_avg;
  inter_ocl_obj.switch_convolve_t[28] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[29] = vp9_convolve8_avg;
  inter_ocl_obj.switch_convolve_t[30] = vp9_convolve8;
  inter_ocl_obj.switch_convolve_t[31] = vp9_convolve8_avg;
}

static int create_inter_ocl_buffer(const int buffer_size,
                                   const int tile_count) {
  int i;
  int status;

  int param_count_cpu = ((buffer_size >> 4) / tile_count) << 1;
  int param_count_cpu_all = (buffer_size >> 4) << 1;
  int param_count_gpu = param_count_cpu;
  int param_count_gpu_all = param_count_cpu_all;

  int index_size_param = param_count_gpu * sizeof(INTER_INDEX_PARAM_GPU);
  int index_size_param_all =
          param_count_gpu_all * sizeof(INTER_INDEX_PARAM_GPU);
  inter_ocl_obj.index_param_size = index_size_param;

  inter_ocl_obj.tile_count = tile_count;
  inter_ocl_obj.buffer_size = sizeof(uint8_t) * buffer_size;
  inter_ocl_obj.buffer_pool_size =
  inter_ocl_obj.buffer_size * FRAME_BUFFERS;

  inter_ocl_obj.param_count_gpu_all = param_count_gpu_all;
  inter_ocl_obj.tile_param_count_gpu = param_count_gpu;

  inter_ocl_obj.pred_param_size =
    param_count_gpu * sizeof(INTER_PRED_PARAM_GPU);
  inter_ocl_obj.pred_param_size_all =
    param_count_gpu_all * sizeof(INTER_PRED_PARAM_GPU);
  inter_ocl_obj.index_size_param_num = param_count_gpu * sizeof(int);
  inter_ocl_obj.index_size_param_num_all = param_count_gpu_all * sizeof(int);
  inter_ocl_obj.index_size_xmv = inter_ocl_obj.index_size_param_num;
  inter_ocl_obj.index_size_xmv_all = inter_ocl_obj.index_size_param_num_all;

  // Alloc prameters buffers: the 0th gpu buffer size is the whole frame size
  inter_ocl_obj.pred_param_kernel_td0 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     inter_ocl_obj.pred_param_size_all,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe pred_param_kernel_td0, error: %d \n", status);
    return -1;
  }
  inter_ocl_obj.pred_param_kernel_td1 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     inter_ocl_obj.pred_param_size_all,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe pred_param_kernel_td1, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.index_param_num_kernel_td0 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                     param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_param_num_kernel_td0, error: %d \n",
         status);
    return -1;
  }
  inter_ocl_obj.index_param_num_kernel_td1 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                     param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_param_num_kernel_td1, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.index_xmv_kernel_td0 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                    param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_xmv_kernel_td0, error: %d \n", status);
    return -1;
  }
  inter_ocl_obj.index_xmv_kernel_td1 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                    param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_xmv_kernel_td1, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.dst_index_xmv_kernel_td0 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                     param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe dst_index_xmv_kernel_td0, error: %d \n", status);
    return -1;
  }
  inter_ocl_obj.dst_index_xmv_kernel_td1 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_READ_WRITE,
                     param_count_gpu_all * sizeof(int) * 4,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe dst_index_xmv_kernel_td1, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.case_count_kernel_td0 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_WRITE,
                     4 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe case_count_kernel_td0, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.case_count_kernel_td1 =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_WRITE,
                     4 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe case_count_kernel_td1, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.one_case_interval_count_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     1 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe param_count_kernel, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.all_b_count_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     1 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe all_b_count_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.new_fb_idx_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     1 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe new_fb_idx_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.buffer_size_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     1 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe buffer_size_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.index_case_mode_offset_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     4 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_case_mode_offset_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.tile_param_count_gpu_offset_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     tile_count * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe tile_param_count_gpu_offset_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.gpu_block_count_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     tile_count * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe gpu_block_count_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.index_param_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     index_size_param_all,
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe index_param_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.tile_count_kernel =
      clCreateBuffer(ocl_context.context,
                     CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_READ_ONLY,
                     1 * sizeof(int),
                     NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateBuffe tile_count_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.case_count_gpu_td0 =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.case_count_kernel_td0,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 4 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer case_count_gpu_td0, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.case_count_gpu_td1 =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.case_count_kernel_td1,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 4 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer case_count_gpu_td1, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.all_b_count_gpu =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.all_b_count_kernel,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 1 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer all_b_count_gpu, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.new_fb_idx_gpu =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.new_fb_idx_kernel,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 1 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer new_fb_idx_gpu, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.buffer_size_gpu =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.buffer_size_kernel,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 1 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer buffer_size_gpu, error: %d \n", status);
    return -1;
  }

  inter_ocl_obj.index_case_mode_offset_gpu =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.index_case_mode_offset_kernel,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 4 * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer index_case_mode_offset_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.tile_param_count_gpu_offset_gpu =
      (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.tile_param_count_gpu_offset_kernel,
                                 CL_TRUE, CL_MAP_WRITE, 0,
                                 tile_count * sizeof(int),
                                 0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer tile_param_count_gpu_offset_kernel, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.one_case_interval_count_gpu =
  (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                             inter_ocl_obj.one_case_interval_count_kernel,
                             CL_TRUE, CL_MAP_WRITE, 0,
                             1 * sizeof(int),
                             0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer param_count_gpu, error: %d \n", status);
    return -1;
  }

  // This for inter index (Init some buffer)
  status = clEnqueueWriteBuffer(
               ocl_context.command_queue,
               inter_ocl_obj.tile_count_kernel,
               CL_TRUE,
               0,
               sizeof(int),
               &tile_count,
               0, NULL, NULL);
  if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueWriteBuffer(tile_count_kernel), error: %d \n",
           status);
      return -1;
  }

  inter_ocl_obj.buffer_size_gpu[0] = inter_ocl_obj.buffer_size;
  inter_ocl_obj.one_case_interval_count_gpu[0] = param_count_gpu_all;

#if 0//USE_INTER_PARAM_ZERO_COP
  inter_ocl_obj.gpu_block_count =
  (int *) clEnqueueMapBuffer(ocl_context.command_queue,
                             inter_ocl_obj.gpu_block_count_kernel,
                             CL_TRUE, CL_MAP_WRITE, 0,
                             tile_count * sizeof(int),
                             0, NULL, NULL, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueMapBuffer gpu_block_count, error: %d \n", status);
    return -1;
  }
#else
  inter_ocl_obj.gpu_block_count =
      MALLOC_INTER(int, sizeof(int) * tile_count);
#endif // USE_INTER_PARAM_ZERO_COPY

  for (i = 0; i < 4; ++i){
    inter_ocl_obj.index_case_mode_offset[i] =
        i * inter_ocl_obj.param_count_gpu_all;
    inter_ocl_obj.index_case_mode_offset_gpu[i] =
        inter_ocl_obj.index_case_mode_offset[i];
  }

  for (i = 0; i < tile_count; ++i) {
    inter_ocl_obj.tile_param_count_gpu_offset[i] =
        i * inter_ocl_obj.tile_param_count_gpu;
    inter_ocl_obj.tile_param_count_gpu_offset_gpu[i] =
        inter_ocl_obj.tile_param_count_gpu_offset[i];

    inter_ocl_obj.ref_buffer[i] =
        MALLOC_INTER(uint8_t, inter_ocl_obj.buffer_size);

    inter_ocl_obj.pred_param_cpu_fri_td0[i] =
        MALLOC_INTER(INTER_PRED_PARAM_CPU,
                     param_count_cpu * sizeof(INTER_PRED_PARAM_CPU));
    inter_ocl_obj.pred_param_cpu_fri_td1[i] =
        MALLOC_INTER(INTER_PRED_PARAM_CPU,
                     param_count_cpu * sizeof(INTER_PRED_PARAM_CPU));
    inter_ocl_obj.pred_param_cpu_sec_td0[i] =
        MALLOC_INTER(INTER_PRED_PARAM_CPU,
                     param_count_cpu * sizeof(INTER_PRED_PARAM_CPU));
    inter_ocl_obj.pred_param_cpu_sec_td1[i] =
        MALLOC_INTER(INTER_PRED_PARAM_CPU,
                     param_count_cpu * sizeof(INTER_PRED_PARAM_CPU));

#if USE_INTER_PARAM_ZERO_COPY
    inter_ocl_obj.pred_param_gpu_td0[i] =
    (INTER_PRED_PARAM_GPU *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.pred_param_kernel_td0,
                                 CL_TRUE, CL_MAP_WRITE,
                                 i * inter_ocl_obj.pred_param_size,
                                 inter_ocl_obj.pred_param_size,
                                 0, NULL, NULL, &status);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueMapBuffer pred_param_gpu_td0 %d, error: %d \n",
           i, status);
      return -1;
    }

    inter_ocl_obj.pred_param_gpu_td1[i] =
    (INTER_PRED_PARAM_GPU *) clEnqueueMapBuffer(ocl_context.command_queue,
                                 inter_ocl_obj.pred_param_kernel_td1,
                                 CL_TRUE, CL_MAP_WRITE,
                                 i * inter_ocl_obj.pred_param_size,
                                 inter_ocl_obj.pred_param_size,
                                 0, NULL, NULL, &status);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueMapBuffer pred_param_gpu_td0 %d, error: %d \n",
           i, status);
      return -1;
    }

    inter_ocl_obj.index_param_gpu[i] =
    (INTER_INDEX_PARAM_GPU *) clEnqueueMapBuffer(ocl_context.command_queue,
                                  inter_ocl_obj.index_param_kernel,
                                  CL_TRUE, CL_MAP_WRITE,
                                  i * index_size_param,
                                  index_size_param,
                                  0, NULL, NULL, &status);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueMapBuffer index_param_gpu %d, error: %d \n",
           i, status);
      return -1;
    }

    inter_ocl_obj.pred_param_gpu_pre[i] =
        inter_ocl_obj.pred_param_gpu_td0[i];
#else
    inter_ocl_obj.pred_param_gpu[i] =
        MALLOC_INTER(INTER_PRED_PARAM_GPU, inter_ocl_obj.pred_param_size);
    assert(inter_ocl_obj.pred_param_gpu[i] != NULL);

    inter_ocl_obj.index_param_gpu[i] =
        MALLOC_INTER(INTER_INDEX_PARAM_GPU, index_size_param);
#endif // USE_INTER_PARAM_ZERO_COPY

    assert(inter_ocl_obj.pred_param_cpu_fri_td0[i] != NULL);
    assert(inter_ocl_obj.pred_param_cpu_fri_td1[i] != NULL);
    assert(inter_ocl_obj.pred_param_cpu_sec_td0[i] != NULL);
    assert(inter_ocl_obj.pred_param_cpu_sec_td1[i] != NULL);
    assert(inter_ocl_obj.index_param_gpu[i] != NULL);

    inter_ocl_obj.pred_param_cpu_fri_pre[i] =
        inter_ocl_obj.pred_param_cpu_fri_td0[i];
    inter_ocl_obj.pred_param_cpu_sec_pre[i] =
        inter_ocl_obj.pred_param_cpu_sec_td0[i];

    inter_ocl_obj.switch_td_calcu_cpu[i] = 0;
  }

  inter_ocl_obj.pred_param_kernel_pre =
      &inter_ocl_obj.pred_param_kernel_td0;
  inter_ocl_obj.index_param_num_kernel_pre =
      &inter_ocl_obj.index_param_num_kernel_td0;
  inter_ocl_obj.index_xmv_kernel_pre =
      &inter_ocl_obj.index_xmv_kernel_td0;
  inter_ocl_obj.dst_index_xmv_kernel_pre =
      &inter_ocl_obj.dst_index_xmv_kernel_td0;
  inter_ocl_obj.case_count_kernel_pre =
      &inter_ocl_obj.case_count_kernel_td0;

  inter_ocl_obj.case_count_gpu = inter_ocl_obj.case_count_gpu_td0;
  inter_ocl_obj.index_count_pre = inter_ocl_obj.index_count_td0;
  inter_ocl_obj.cpu_fri_count_pre = inter_ocl_obj.cpu_fri_count_td0;
  inter_ocl_obj.cpu_sec_count_pre = inter_ocl_obj.cpu_sec_count_td0;

  inter_ocl_obj.switch_td_param = 1;
  inter_ocl_obj.switch_td_calcu_gpu = 0;

  return 0;
}

static int release_inter_ocl_buffer(const int tile_count) {
  int i;
  int status = 0;

  for (i = 0; i < tile_count; ++i) {
    if (inter_ocl_obj.ref_buffer[i]) {
      FREE_INTER(inter_ocl_obj.ref_buffer[i]);
      inter_ocl_obj.ref_buffer[i] = NULL;
    }
    if (inter_ocl_obj.pred_param_cpu_fri_td0[i] != NULL) {
      FREE_INTER(inter_ocl_obj.pred_param_cpu_fri_td0[i]);
      inter_ocl_obj.pred_param_cpu_fri_td0[i] = NULL;
      inter_ocl_obj.pred_param_cpu_fri[i] = NULL;
      inter_ocl_obj.pred_param_cpu_fri_pre[i] = NULL;
    }
    if (inter_ocl_obj.pred_param_cpu_fri_td1[i] != NULL) {
      FREE_INTER(inter_ocl_obj.pred_param_cpu_fri_td1[i]);
      inter_ocl_obj.pred_param_cpu_fri_td1[i] = NULL;
    }
    if (inter_ocl_obj.pred_param_cpu_sec_td0[i] != NULL) {
      FREE_INTER(inter_ocl_obj.pred_param_cpu_sec_td0[i]);
      inter_ocl_obj.pred_param_cpu_sec_td0[i] = NULL;
      inter_ocl_obj.pred_param_cpu_sec[i] = NULL;
      inter_ocl_obj.pred_param_cpu_sec_pre[i] = NULL;
    }
    if (inter_ocl_obj.pred_param_cpu_sec_td1[i] != NULL) {
      FREE_INTER(inter_ocl_obj.pred_param_cpu_sec_td1[i]);
      inter_ocl_obj.pred_param_cpu_sec_td1[i] = NULL;
    }


#if USE_INTER_PARAM_ZERO_COPY
    status = clEnqueueUnmapMemObject(
                 ocl_context.command_queue,
                 inter_ocl_obj.pred_param_kernel_td0,
                 inter_ocl_obj.pred_param_gpu_td0[i], 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueUnMapBuffer pred_param_gpu_td0 %d, error: %d \n",
           i, status);
      return -1;
    }

    status = clEnqueueUnmapMemObject(
                 ocl_context.command_queue,
                 inter_ocl_obj.pred_param_kernel_td1,
                 inter_ocl_obj.pred_param_gpu_td1[i], 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueUnMapBuffer pred_param_gpu_td1 %d, error: %d \n",
           i, status);
      return -1;
    }

    status = clEnqueueUnmapMemObject(
                 ocl_context.command_queue,
                 inter_ocl_obj.index_param_kernel,
                 inter_ocl_obj.index_param_gpu[i], 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueUnMapBuffer param_count_gpu %d, error: %d \n",
           i, status);
      return -1;
    }

    inter_ocl_obj.pred_param_gpu_td0[i] = NULL;
    inter_ocl_obj.pred_param_gpu_td1[i] = NULL;
    inter_ocl_obj.pred_param_gpu_pre[i] = NULL;
#else
    if (inter_ocl_obj.pred_param_gpu[i] != NULL) {
      FREE_INTER(inter_ocl_obj.pred_param_gpu[i]);
      inter_ocl_obj.pred_param_gpu[i] = NULL;
      inter_ocl_obj.pred_param_gpu_pre[i] = NULL;
    }

    if (inter_ocl_obj.index_param_gpu[i] != NULL)
      FREE_INTER(inter_ocl_obj.index_param_gpu[i]);
#endif // USE_INTER_PARAM_ZERO_COPY

    inter_ocl_obj.index_param_gpu[i] = NULL;
    inter_ocl_obj.index_param_gpu_pre[i] = NULL;
  }

#if 0//USE_INTER_PARAM_ZERO_COPY
    status = clEnqueueUnmapMemObject(
                 ocl_context.command_queue,
                 inter_ocl_obj.gpu_block_count_kernel,
                 inter_ocl_obj.gpu_block_count, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueUnMapBuffer pred_param_gpu_td0 %d, error: %d \n",
           i, status);
      return -1;
    }

    inter_ocl_obj.gpu_block_count = NULL;
#else
    if (inter_ocl_obj.gpu_block_count != NULL) {
      FREE_INTER(inter_ocl_obj.gpu_block_count);
      inter_ocl_obj.gpu_block_count = NULL;
    }
#endif // USE_INTER_PARAM_ZERO_COPY

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.one_case_interval_count_kernel,
               inter_ocl_obj.one_case_interval_count_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer param_count_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.case_count_kernel_td0,
               inter_ocl_obj.case_count_gpu_td0, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer all_b_count_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.case_count_kernel_td1,
               inter_ocl_obj.case_count_gpu_td1, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer all_b_count_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.all_b_count_kernel,
               inter_ocl_obj.all_b_count_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer all_b_count_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.new_fb_idx_kernel,
               inter_ocl_obj.new_fb_idx_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer new_fb_idx_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.buffer_size_kernel,
               inter_ocl_obj.buffer_size_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer buffer_size_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.index_case_mode_offset_kernel,
               inter_ocl_obj.index_case_mode_offset_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer index_case_mode_offset_gpu, error: %d \n",
         status);
    return -1;
  }

  status = clEnqueueUnmapMemObject(
               ocl_context.command_queue,
               inter_ocl_obj.index_case_mode_offset_kernel,
               inter_ocl_obj.tile_param_count_gpu_offset_gpu, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueUnMapBuffer tile_param_count_gpu_offset_gpu, error: %d \n",
         status);
    return -1;
  }

  inter_ocl_obj.case_count_gpu = NULL;
  inter_ocl_obj.case_count_gpu_td0 = NULL;
  inter_ocl_obj.case_count_gpu_td1 = NULL;

  inter_ocl_obj.new_fb_idx_gpu = NULL;
  inter_ocl_obj.all_b_count_gpu = NULL;
  inter_ocl_obj.buffer_size_gpu = NULL;
  inter_ocl_obj.index_case_mode_offset_gpu = NULL;
  inter_ocl_obj.one_case_interval_count_gpu= NULL;
  inter_ocl_obj.tile_param_count_gpu_offset_gpu = NULL;

  if (inter_ocl_obj.pred_param_kernel_td0)
    status |= clReleaseMemObject(inter_ocl_obj.pred_param_kernel_td0);
  if (inter_ocl_obj.pred_param_kernel_td1)
    status |= clReleaseMemObject(inter_ocl_obj.pred_param_kernel_td1);
  if (inter_ocl_obj.index_param_num_kernel_td0)
    status |= clReleaseMemObject(inter_ocl_obj.index_param_num_kernel_td0);
  if (inter_ocl_obj.index_param_num_kernel_td1)
    status |= clReleaseMemObject(inter_ocl_obj.index_param_num_kernel_td1);
  if (inter_ocl_obj.index_xmv_kernel_td0)
    status |= clReleaseMemObject(inter_ocl_obj.index_xmv_kernel_td0);
  if (inter_ocl_obj.index_xmv_kernel_td1)
    status |= clReleaseMemObject(inter_ocl_obj.index_xmv_kernel_td1);
  if (inter_ocl_obj.dst_index_xmv_kernel_td0)
    status |= clReleaseMemObject(inter_ocl_obj.dst_index_xmv_kernel_td0);
  if (inter_ocl_obj.dst_index_xmv_kernel_td1)
    status |= clReleaseMemObject(inter_ocl_obj.dst_index_xmv_kernel_td1);

  if (inter_ocl_obj.case_count_kernel_td0)
    status |= clReleaseMemObject(inter_ocl_obj.case_count_kernel_td0);
  if (inter_ocl_obj.case_count_kernel_td1)
    status |= clReleaseMemObject(inter_ocl_obj.case_count_kernel_td1);
  if (inter_ocl_obj.one_case_interval_count_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.one_case_interval_count_kernel);

  if (inter_ocl_obj.all_b_count_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.all_b_count_kernel);
  if (inter_ocl_obj.tile_count_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.tile_count_kernel);
  if (inter_ocl_obj.new_fb_idx_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.new_fb_idx_kernel);
  if (inter_ocl_obj.buffer_size_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.buffer_size_kernel);
  if (inter_ocl_obj.index_param_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.index_param_kernel);
  if (inter_ocl_obj.gpu_block_count_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.gpu_block_count_kernel);
  if (inter_ocl_obj.index_case_mode_offset_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.index_case_mode_offset_kernel);
  if (inter_ocl_obj.tile_param_count_gpu_offset_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.tile_param_count_gpu_offset_kernel);

  return 0;
}

int vp9_init_ocl() {
  int status = 0;
  const char *psource = NULL;
  inter_ocl_obj.buffer_pool_flag = 0;
  inter_ocl_obj.inter_ocl_init = 1;

  status = ocl_wrapper_init();
  if (status < 0) {
    LOGE("Failed to init ocl wrapper, error: %d\n", status);
    exit(1);
  }

  status = ocl_context_init(&ocl_context, 1);
  if (status < 0) {
    LOGE("Failed to init ocl context, error: %d\n", status);
    exit(1);
  }
  status = load_source_from_file(
               "vp9_inter_pred_4x4.cl",
               &inter_ocl_obj.source,
               &inter_ocl_obj.source_len);
  if (status < 0) {
    LOGE("Failed to load kernel, error: %d\n", status);
    exit(1);
  }

  psource = inter_ocl_obj.source;
  inter_ocl_obj.program = create_and_build_program(
                              &ocl_context, 1,
                              (const char **)&psource,
                              &inter_ocl_obj.source_len, &status);
  if (status < 0) {
    LOGE("There is some error in create&build program, error: %d\n", status);
    exit(1);
  }

  inter_ocl_obj.kernel = clCreateKernel(
                                inter_ocl_obj.program,
                                "inter_pred_calcu", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel inter_pred_calcu, error: %d\n", status);
    exit(1);
  }
  // This for inter index
  inter_ocl_obj.kernel_index = clCreateKernel(
                                inter_ocl_obj.program,
                                "inter_pred_index", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel inter_pred_index, error: %d\n", status);
    exit(1);
  }

  inter_ocl_obj.update_buffer_pool_kernel = clCreateKernel(
                                                inter_ocl_obj.program,
                                                "update_buffer_pool", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel update_buffer_pool, error: %d\n", status);
    exit(1);
  }

  status = create_inter_ocl_buffer(STABLE_BUFFER_SIZE_OCL, MAX_TILE_COUNT_OCL);
  if (status < 0) {
    LOGE("Failed to create inter opencl buffer \n");
    exit(1);
  }

  return 0;
}

int vp9_init_ocl_ex(void *id3d9_devices) {
 Interop_Context *P_context;
  int status = 0;
  const char *psource = NULL;
  inter_ocl_obj.buffer_pool_flag = 0;
  inter_ocl_obj.inter_ocl_init = 1;

  P_context = (Interop_Context*)id3d9_devices;
  status = ocl_wrapper_init();
  if (status < 0) {
    LOGE("Failed to init ocl wrapper, error: %d\n", status);
    exit(1);
  }

  status = ocl_context_init_for_d3d9_interOp(&ocl_context,P_context, 1);
  if (status < 0) {
    LOGE("Failed to init ocl context, error: %d\n", status);
    exit(1);
  }
  status = load_source_from_file(
               "vp9_inter_pred_4x4.cl",
               &inter_ocl_obj.source,
               &inter_ocl_obj.source_len);
  if (status < 0) {
    LOGE("Failed to load kernel, error: %d\n", status);
    exit(1);
  }

  psource = inter_ocl_obj.source;
  inter_ocl_obj.program = create_and_build_program(
                              &ocl_context, 1,
                              (const char **)&psource,
                              &inter_ocl_obj.source_len, &status);
  if (status < 0) {
    LOGE("There is some error in create&build program, error: %d\n", status);
    exit(1);
  }

  inter_ocl_obj.kernel = clCreateKernel(
                                inter_ocl_obj.program,
                                "inter_pred_calcu", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel inter_pred_calcu, error: %d\n", status);
    exit(1);
  }
  // This for inter index
  inter_ocl_obj.kernel_index = clCreateKernel(
                                inter_ocl_obj.program,
                                "inter_pred_index", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel inter_pred_index, error: %d\n", status);
    exit(1);
  }

   inter_ocl_obj.update_buffer_pool_kernel = clCreateKernel(
                                                inter_ocl_obj.program,
                                                "update_buffer_pool", &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateKernel update_buffer_pool, error: %d\n", status);
    exit(1);
  }

  status = create_inter_ocl_buffer(STABLE_BUFFER_SIZE_OCL, MAX_TILE_COUNT_OCL);
  if (status < 0) {
    LOGE("Failed to create inter opencl buffer \n");
    exit(1);
  }
  return 0;
}

int vp9_init_inter_ocl(VP9_COMMON *const cm, int tile_count) {
  int status = 0;
  const YV12_BUFFER_CONFIG *cfg_source = &cm->yv12_fb[cm->new_fb_idx];
  int param_count_gpu = ((cfg_source->buffer_alloc_sz >> 4) / tile_count) << 1;

  inter_ocl_obj.tile_count = tile_count;
  inter_ocl_obj.globalThreads[0] = param_count_gpu * tile_count;
  inter_ocl_obj.globalThreads[1] = 1;
  inter_ocl_obj.globalThreads[2] = 1;
  inter_ocl_obj.localThreads[0] = 64;
  inter_ocl_obj.localThreads[1] = 1;
  inter_ocl_obj.localThreads[2] = 1;

  if (cfg_source->buffer_alloc_sz != STABLE_BUFFER_SIZE_OCL
      || tile_count != MAX_TILE_COUNT_OCL) {
    status = release_inter_ocl_buffer(MAX_TILE_COUNT_OCL);
    if (status < 0) {
      LOGE("Failed to release inter opencl buffer \n");
      return -1;
    }

    status = create_inter_ocl_buffer(cfg_source->buffer_alloc_sz, tile_count);
    if (status < 0) {
      LOGE("Failed to create inter opencl buffer \n");
      return -1;
    }

    inter_ocl_obj.release_max = 0;
  } else {
    inter_ocl_obj.release_max = 1;
  }

  status = clSetKernelArg(
             inter_ocl_obj.update_buffer_pool_kernel,
             0, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 0, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.update_buffer_pool_kernel,
             1, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_read_only_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 1, error: %d\n", status);
    return -1;
  }

  // Args' setting for kernel
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             0, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 0, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
               inter_ocl_obj.kernel,
               6, sizeof(cl_mem),
               (void*) &inter_ocl_obj.one_case_interval_count_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 6, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             8, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_read_only_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 8, error: %d\n", status);
    return -1;
  }

  // Args' setting for kernel_index
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             0, sizeof(cl_mem),
             (void*) &inter_ocl_obj.index_param_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 0, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             1, sizeof(cl_mem),
             (void*) &inter_ocl_obj.new_fb_idx_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 1, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             2, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_size_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 2, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             3, sizeof(cl_mem),
             (void*) &inter_ocl_obj.gpu_block_count_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 3, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             4, sizeof(cl_mem),
             (void*) &inter_ocl_obj.index_case_mode_offset_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 4, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             5, sizeof(cl_mem),
             (void*) &inter_ocl_obj.tile_param_count_gpu_offset_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 5, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             6, sizeof(cl_mem),
             (void*) &inter_ocl_obj.all_b_count_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 6, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel_index,
             7, sizeof(cl_mem),
             (void*) &inter_ocl_obj.tile_count_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set kernel_index arguments 7, error: %d\n", status);
    return -1;
  }

  inter_ocl_obj.previous_f = cm->new_fb_idx;
  inter_ocl_obj.before_previous_f = cm->new_fb_idx;

  inter_ocl_obj.previous_f_show = 1;

  inter_convolve_init_ocl();

  return 0;
}

int vp9_release_ocl() {
  int status = 0;

  if (inter_ocl_obj.release_max) {
    status = release_inter_ocl_buffer(MAX_TILE_COUNT_OCL);
  } else {
    status = release_inter_ocl_buffer(inter_ocl_obj.tile_count);
  }

  if (inter_ocl_obj.buffer_pool_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.buffer_pool_kernel);
  if (inter_ocl_obj.buffer_pool_read_only_kernel)
    status |= clReleaseMemObject(inter_ocl_obj.buffer_pool_read_only_kernel);

  if (inter_ocl_obj.kernel)
    status |= clReleaseKernel(inter_ocl_obj.kernel);
  if (inter_ocl_obj.program)
    status |= clReleaseProgram(inter_ocl_obj.program);
  if (inter_ocl_obj.source != NULL) {
    free(inter_ocl_obj.source);
    inter_ocl_obj.source = NULL;
  }

  // This for inter index
  if (inter_ocl_obj.kernel_index)
    status |= clReleaseKernel(inter_ocl_obj.kernel_index);

  if (inter_ocl_obj.update_buffer_pool_kernel)
    status |= clReleaseKernel(inter_ocl_obj.update_buffer_pool_kernel);

  ocl_context_fini(&ocl_context);
  ocl_wrapper_finalize();

  if (status != CL_SUCCESS) {
    LOGE("Failed to Release ocl! \n");
    exit(1);
  }

  return 0;
}

int reset_inter_ocl_param_buffer(int tile_num) {
 inter_ocl_obj.gpu_block_count[tile_num] = 0;
 inter_ocl_obj.index_count_pre[tile_num] = 0;
 inter_ocl_obj.cpu_fri_count_pre[tile_num] = 0;
 inter_ocl_obj.cpu_sec_count_pre[tile_num] = 0;

#if !USE_INTER_PARAM_ZERO_COPY
  inter_ocl_obj.pred_param_gpu_pre[tile_num] =
      inter_ocl_obj.pred_param_gpu[tile_num];
#endif
  inter_ocl_obj.pref[tile_num] =
      inter_ocl_obj.ref_buffer[tile_num];
  inter_ocl_obj.index_param_gpu_pre[tile_num] =
      inter_ocl_obj.index_param_gpu[tile_num];

  return 0;
}

int vp9_inter_write_param_to_gpu(int tile_num) {
  int status;

#if !USE_INTER_PARAM_ZERO_COPY
  if (inter_ocl_obj.gpu_block_count[tile_num] > 0) {
    status = clEnqueueWriteBuffer(
                 ocl_context.command_queue,
                 *inter_ocl_obj.pred_param_kernel_pre,
                 CL_FALSE,
                 inter_ocl_obj.pred_param_size * tile_num,
                 inter_ocl_obj.gpu_block_count[tile_num] *
                 sizeof(INTER_PRED_PARAM_GPU),
                 inter_ocl_obj.pred_param_gpu[tile_num],
                 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        LOGE("Failed to clEnqueueWriteBuffer(pred_param) %d, error: %d \n",
             tile_num, status);
        return -1;
    }

    // This for inter index
    status = clEnqueueWriteBuffer(
                 ocl_context.command_queue,
                 inter_ocl_obj.index_param_kernel,
                 CL_FALSE,
                 inter_ocl_obj.index_param_size * tile_num,
                 inter_ocl_obj.gpu_block_count[tile_num] *
                 sizeof(INTER_INDEX_PARAM_GPU),
                 inter_ocl_obj.index_param_gpu[tile_num],
                 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        LOGE("Failed to clEnqueueWriteBuffer(index_param) %d, error: %d \n",
             tile_num, status);
        return -1;
    }
  }
#endif // USE_INTER_PARAM_ZERO_COPY

  status = clEnqueueWriteBuffer(
               ocl_context.command_queue,
               inter_ocl_obj.gpu_block_count_kernel,
               CL_FALSE, sizeof(int) * tile_num, sizeof(int),
               &inter_ocl_obj.gpu_block_count[tile_num],
               0, NULL, NULL);
  if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueWriteBuffer(gpu_block_count) %d, error: %d \n",
           tile_num, status);
      return -1;
  }

  status = clFlush(ocl_context.command_queue);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clFlush Writebuffer %d, error: %d\n", tile_num, status);
    return -1;
  }

  return 0;
}

int vp9_update_gpu_buffer_pool(VP9_COMMON *const cm) {
  int status;
  int buffer_pool_offset;
  const YV12_BUFFER_CONFIG *cfg_source;

  if (cm->frame_type != KEY_FRAME && cm->show_frame) {
    if (!inter_ocl_obj.previous_f_show) {
      cfg_source = &cm->yv12_fb[inter_ocl_obj.before_previous_f];
      buffer_pool_offset =
          cfg_source->buffer_alloc - inter_ocl_obj.buffer_pool_map_ptr;
#if USE_KERNEL_UPDATE_BUFFER_POOL
      const size_t global_threads = cfg_source->buffer_alloc_sz >> 2;

      status = clSetKernelArg(
                 inter_ocl_obj.update_buffer_pool_kernel,
                 2, sizeof(int),
                 (void*) &buffer_pool_offset);
      if (status != CL_SUCCESS) {
        LOGE("Failed to set arguments 2, error: %d\n", status);
        return -1;
      }

      status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                      inter_ocl_obj.update_buffer_pool_kernel,
                                      1, 0, &global_threads,
                                      NULL, 0, NULL, NULL);
      if (status != CL_SUCCESS) {
        LOGE("Failed to clEnqueueNDRangeKernel update buffer pool, error: %d\n",
             status);
        return -1;
      }
#else
      status = clEnqueueCopyBuffer(
                   ocl_context.command_queue,
                   inter_ocl_obj.buffer_pool_kernel,
                   inter_ocl_obj.buffer_pool_read_only_kernel,
                   buffer_pool_offset, buffer_pool_offset,
                   cfg_source->buffer_alloc_sz,
                   0, NULL, NULL);
      if (status != CL_SUCCESS) {
          LOGE("Failed to clEnqueueCopyBuffer(gpu_buffer_pool), error: %d \n",
               status);
          return -1;
      }
#endif // USE_KERNEL_UPDATE_BUFFER_POOL
    }

    cfg_source = &cm->yv12_fb[inter_ocl_obj.previous_f];
    buffer_pool_offset =
        cfg_source->buffer_alloc - inter_ocl_obj.buffer_pool_map_ptr;
#if USE_KERNEL_UPDATE_BUFFER_POOL
    const size_t global_threads = cfg_source->buffer_alloc_sz >> 2;

    status = clSetKernelArg(
               inter_ocl_obj.update_buffer_pool_kernel,
               2, sizeof(int),
               (void*) &buffer_pool_offset);
    if (status != CL_SUCCESS) {
      LOGE("Failed to set arguments 2, error: %d\n", status);
      return -1;
    }

    status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                    inter_ocl_obj.update_buffer_pool_kernel,
                                    1, 0, &global_threads,
                                    NULL, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueNDRangeKernel update buffer pool, error: %d\n",
           status);
      return -1;
    }
#else
    status = clEnqueueCopyBuffer(
                 ocl_context.command_queue,
                 inter_ocl_obj.buffer_pool_kernel,
                 inter_ocl_obj.buffer_pool_read_only_kernel,
                 buffer_pool_offset, buffer_pool_offset,
                 cfg_source->buffer_alloc_sz,
                 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        LOGE("Failed to clEnqueueCopyBuffer(gpu_buffer_pool), error: %d \n",
             status);
        return -1;
    }
#endif // USE_KERNEL_UPDATE_BUFFER_POOL

    status = clFlush(ocl_context.command_queue);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clFlush update gpu buffer pool, error: %d\n", status);
      return -1;
    }
  }


  inter_ocl_obj.before_previous_f = inter_ocl_obj.previous_f;
  inter_ocl_obj.previous_f_show = cm->show_frame;
  inter_ocl_obj.previous_f = cm->new_fb_idx;

  return 0;
}

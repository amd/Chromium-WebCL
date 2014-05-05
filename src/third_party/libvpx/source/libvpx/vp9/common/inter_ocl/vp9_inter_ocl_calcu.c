/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdio.h>

#include "./vpx_config.h"

#include "vp9/common/inter_ocl/vp9_inter_ocl_calcu.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_init.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"
#include "vp9/common/inter_ocl/vp9_convolve_ocl_c.h"
#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/ppa.h"

#define LOGI(...) fprintf(stdout, __VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)

static int build_inter_pred_calcu_cpu_whole_frame(VP9_COMMON *const cm) {
  int tile_num;
  const YV12_BUFFER_CONFIG *cfg_source = &cm->yv12_fb[cm->new_fb_idx];

  for (tile_num = 0; tile_num < inter_ocl_obj.tile_count; ++tile_num)
    build_inter_pred_calcu_ocl_c(tile_num, cfg_source->buffer_alloc);

  return 0;
}

static int build_inter_pred_calcu_cpu_whole_frame_mt(VP9_COMMON *const cm) {
  pthread_t pid[8];

  int tile_num;
  INTER_MT_ATTR inter_mt_attr[MAX_TILE_COUNT_OCL];

  const YV12_BUFFER_CONFIG *cfg_source = &cm->yv12_fb[cm->new_fb_idx];

  for (tile_num = 1; tile_num < inter_ocl_obj.tile_count; ++tile_num) {
    inter_mt_attr[tile_num].new_buffer = cfg_source->buffer_alloc;
    inter_mt_attr[tile_num].tile_num = tile_num;

  //  pthread_create(&pid[tile_num], NULL,
                 //  build_inter_pred_calcu_ocl_mt,
                  // (void *)&inter_mt_attr[tile_num]);
  }

  build_inter_pred_calcu_ocl_c(0, cfg_source->buffer_alloc);

  for (tile_num = 1; tile_num < inter_ocl_obj.tile_count; ++tile_num) {
    pthread_join(pid[tile_num], NULL);
  }

  return 0;
}

static int build_inter_pred_calcu_gpu_whole_frame(VP9_COMMON *const cm) {
  int i, status;

#if USE_PPA
  PPAStartCpuEventFunc(para_copy_set_time);
#endif

  inter_ocl_obj.new_fb_idx_gpu[0] = cm->new_fb_idx;

  // Executive inter prediction index part
  status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                  inter_ocl_obj.kernel_index, 1,
                                  0, &inter_ocl_obj.globalThreads,
                                  NULL, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueNDRangeKernel inter index, error: %d\n", status);
    return -1;
  }
  status = clEnqueueReadBuffer(
               ocl_context.command_queue,
               inter_ocl_obj.case_count_kernel,
               CL_FALSE, 0, sizeof(int) * 4,
               inter_ocl_obj.case_count_gpu,
               0, NULL, NULL);
  if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueReadBuffer(case count), error: %d \n",
           status);
      return -1;
  }
  status = clFinish(ocl_context.command_queue);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clFinish, error: %d\n", status);
    return -1;
  }

  for (i = 0; i < 4; ++i) {
    inter_ocl_obj.all_of_block_count_gpu[0] +=
        inter_ocl_obj.case_count_gpu[i];
  }
 #if USE_PPA
  PPAStopCpuEventFunc(para_copy_set_time);
#endif

  // Executive inter prediction GPU part
  inter_ocl_obj.globalThreads[0] = (inter_ocl_obj.all_of_block_count_gpu[0] + 64) - 
                                                    (inter_ocl_obj.all_of_block_count_gpu[0] % 64);
  status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                  inter_ocl_obj.kernel, 3,
                                  0, inter_ocl_obj.globalThreads,
                                  inter_ocl_obj.localThreads, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueNDRangeKernel inter pred, error: %d\n", status);
    return -1;
  }
  status = clFlush(ocl_context.command_queue);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clFlush, error: %d\n", status);
    return -1;
  }

#if USE_PPA
  PPAStartCpuEventFunc(INTER_CPU_PART);
#endif

  // Executive inter prediction CPU part
#if USE_INTER_CALCU_CPU_MT
  build_inter_pred_calcu_cpu_whole_frame_mt(cm);
#else
  build_inter_pred_calcu_cpu_whole_frame(cm);
#endif // USE_INTER_CALCU_CPU_MT

#if USE_PPA
  PPAStopCpuEventFunc(INTER_CPU_PART);
#endif

  status = clFinish(ocl_context.command_queue);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clFinish, error: %d\n", status);
    return -1;
  }

  return 0;
}

int inter_pred_calcu_ocl_whole_frame(VP9_COMMON *const cm) {
  int i, status;
  int sum_counts = 0;

#if USE_PPA
  PPAStartCpuEventFunc(inter_pred_all_gpu);
#endif
  for (i = 0; i < inter_ocl_obj.tile_count; i++)
    sum_counts += inter_ocl_obj.gpu_block_count[i];

  if (sum_counts > 0) {
    status = clEnqueueWriteBuffer(
                 ocl_context.command_queue,
                 inter_ocl_obj.all_b_count_kernel,
                 CL_FALSE,
                 0,
                 sizeof(int),
                 &sum_counts,
                 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        LOGE("Failed to clEnqueueWriteBuffer(all_b_count), error: %d \n",
             status);
        return -1;
    }

    build_inter_pred_calcu_gpu_whole_frame(cm);
  } else {
    build_inter_pred_calcu_cpu_whole_frame(cm);
  }

  reset_inter_ocl_buffer();
#if USE_PPA
  PPAStopCpuEventFunc(inter_pred_all_gpu);
#endif

  return 0;
}

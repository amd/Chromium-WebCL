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

static void inter_switch_param_td() {
  int tile_num;

  if (inter_ocl_obj.switch_td_param) {
    for (tile_num = 0; tile_num < inter_ocl_obj.tile_count; ++tile_num) {
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num] =
          inter_ocl_obj.pred_param_cpu_fri_td1[tile_num];
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num] =
          inter_ocl_obj.pred_param_cpu_sec_td1[tile_num];

#if USE_INTER_PARAM_ZERO_COPY
      inter_ocl_obj.pred_param_gpu_pre[tile_num] =
          inter_ocl_obj.pred_param_gpu_td1[tile_num];
#endif
    }

    inter_ocl_obj.pred_param_kernel_pre =
        &inter_ocl_obj.pred_param_kernel_td1;
    inter_ocl_obj.index_param_num_kernel_pre =
        &inter_ocl_obj.index_param_num_kernel_td1;
    inter_ocl_obj.index_xmv_kernel_pre =
        &inter_ocl_obj.index_xmv_kernel_td1;
    inter_ocl_obj.dst_index_xmv_kernel_pre =
        &inter_ocl_obj.dst_index_xmv_kernel_td1;
    inter_ocl_obj.case_count_kernel_pre =
        &inter_ocl_obj.case_count_kernel_td1;

    inter_ocl_obj.index_count_pre = inter_ocl_obj.index_count_td1;
    inter_ocl_obj.case_count_gpu = inter_ocl_obj.case_count_gpu_td1;
    inter_ocl_obj.cpu_fri_count_pre = inter_ocl_obj.cpu_fri_count_td1;
    inter_ocl_obj.cpu_sec_count_pre = inter_ocl_obj.cpu_sec_count_td1;

    inter_ocl_obj.switch_td_param = 0;
  } else {
    for (tile_num = 0; tile_num < inter_ocl_obj.tile_count; ++tile_num) {
      inter_ocl_obj.pred_param_cpu_fri_pre[tile_num] =
         inter_ocl_obj.pred_param_cpu_fri_td0[tile_num];
      inter_ocl_obj.pred_param_cpu_sec_pre[tile_num] =
         inter_ocl_obj.pred_param_cpu_sec_td0[tile_num];

#if USE_INTER_PARAM_ZERO_COPY
      inter_ocl_obj.pred_param_gpu_pre[tile_num] =
          inter_ocl_obj.pred_param_gpu_td0[tile_num];
#endif
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

    inter_ocl_obj.index_count_pre = inter_ocl_obj.index_count_td0;
    inter_ocl_obj.case_count_gpu = inter_ocl_obj.case_count_gpu_td0;
    inter_ocl_obj.cpu_fri_count_pre = inter_ocl_obj.cpu_fri_count_td0;
    inter_ocl_obj.cpu_sec_count_pre = inter_ocl_obj.cpu_sec_count_td0;

    inter_ocl_obj.switch_td_param = 1;
  }
}

static void inter_switch_calcu_td_gpu() {
  if (inter_ocl_obj.switch_td_calcu_gpu) {
    inter_ocl_obj.pred_param_kernel =
        &inter_ocl_obj.pred_param_kernel_td1;
    inter_ocl_obj.index_param_num_kernel =
        &inter_ocl_obj.index_param_num_kernel_td1;
    inter_ocl_obj.index_xmv_kernel =
        &inter_ocl_obj.index_xmv_kernel_td1;
    inter_ocl_obj.dst_index_xmv_kernel =
        &inter_ocl_obj.dst_index_xmv_kernel_td1;
    inter_ocl_obj.case_count_kernel =
        &inter_ocl_obj.case_count_kernel_td1;

    inter_ocl_obj.index_count = inter_ocl_obj.index_count_td1;
    inter_ocl_obj.switch_td_calcu_gpu = 0;
  } else {
    inter_ocl_obj.pred_param_kernel =
        &inter_ocl_obj.pred_param_kernel_td0;
    inter_ocl_obj.index_param_num_kernel =
        &inter_ocl_obj.index_param_num_kernel_td0;
    inter_ocl_obj.index_xmv_kernel =
        &inter_ocl_obj.index_xmv_kernel_td0;
    inter_ocl_obj.dst_index_xmv_kernel =
        &inter_ocl_obj.dst_index_xmv_kernel_td0;
    inter_ocl_obj.case_count_kernel =
        &inter_ocl_obj.case_count_kernel_td0;

    inter_ocl_obj.index_count = inter_ocl_obj.index_count_td0;
    inter_ocl_obj.switch_td_calcu_gpu = 1;
  }
}

static void inter_switch_calcu_td_cpu(int tile_num) {
  if (inter_ocl_obj.switch_td_calcu_cpu[tile_num]) {
    inter_ocl_obj.pred_param_cpu_fri[tile_num] =
        inter_ocl_obj.pred_param_cpu_fri_td1[tile_num];
    inter_ocl_obj.pred_param_cpu_sec[tile_num] =
        inter_ocl_obj.pred_param_cpu_sec_td1[tile_num];

    inter_ocl_obj.cpu_fri_count[tile_num] =
        inter_ocl_obj.cpu_fri_count_td1 + tile_num;
    inter_ocl_obj.cpu_sec_count[tile_num] =
        inter_ocl_obj.cpu_sec_count_td1 + tile_num;

    inter_ocl_obj.switch_td_calcu_cpu[tile_num] = 0;
  } else {
    inter_ocl_obj.pred_param_cpu_fri[tile_num] =
        inter_ocl_obj.pred_param_cpu_fri_td0[tile_num];
    inter_ocl_obj.pred_param_cpu_sec[tile_num] =
        inter_ocl_obj.pred_param_cpu_sec_td0[tile_num];

    inter_ocl_obj.cpu_fri_count[tile_num] =
        inter_ocl_obj.cpu_fri_count_td0 + tile_num;
    inter_ocl_obj.cpu_sec_count[tile_num] =
        inter_ocl_obj.cpu_sec_count_td0 + tile_num;

    inter_ocl_obj.switch_td_calcu_cpu[tile_num] = 1;
  }
}

static int build_inter_pred_index_whole_frame(VP9_COMMON *const cm) {
  int tile_num, status;
  cl_event index_event;

  inter_ocl_obj.all_b_count_gpu[0] = 0;
  for (tile_num = 0; tile_num < inter_ocl_obj.tile_count; ++tile_num)
    inter_ocl_obj.all_b_count_gpu[0] += inter_ocl_obj.gpu_block_count[tile_num];

  if (inter_ocl_obj.all_b_count_gpu[0] > 0) {
    inter_ocl_obj.new_fb_idx_gpu[0] = cm->new_fb_idx;
    memset(inter_ocl_obj.case_count_gpu, 0, sizeof(int) * 4);

    status = clSetKernelArg(
                 inter_ocl_obj.kernel_index,
                 8, sizeof(cl_mem),
                 (void*) inter_ocl_obj.dst_index_xmv_kernel_pre);
    if (status != CL_SUCCESS) {
      LOGE("Failed to set arguments 8, error: %d\n", status);
      return -1;
    }
    status = clSetKernelArg(
                 inter_ocl_obj.kernel_index,
                 9, sizeof(cl_mem),
                 (void*) inter_ocl_obj.index_param_num_kernel_pre);
    if (status != CL_SUCCESS) {
      LOGE("Failed to set arguments 9, error: %d\n", status);
      return -1;
    }
    status = clSetKernelArg(
                 inter_ocl_obj.kernel_index,
                 10, sizeof(cl_mem),
                 (void*) inter_ocl_obj.index_xmv_kernel_pre);
    if (status != CL_SUCCESS) {
      LOGE("Failed to set arguments 10, error: %d\n", status);
      return -1;
    }
    status = clSetKernelArg(
                 inter_ocl_obj.kernel_index,
                 11, sizeof(cl_mem),
                 (void*) inter_ocl_obj.case_count_kernel_pre);
    if (status != CL_SUCCESS) {
      LOGE("Failed to set arguments 11, error: %d\n", status);
      return -1;
    }

    // Executive inter prediction index part
    status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                    inter_ocl_obj.kernel_index, 1,
                                    0, inter_ocl_obj.globalThreads,
                                    NULL, 0, NULL, &index_event);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clEnqueueNDRangeKernel inter index, error: %d\n", status);
      return -1;
    }
    status = clFlush(ocl_context.command_queue);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clFlush, error: %d\n", status);
      return -1;
    }

    status = clWaitForEvents(1, &index_event);
    if (status != CL_SUCCESS) {
      LOGE("Failed to clWaitForEvents index_event, error: %d\n", status);
      return -1;
    }
  }

  return 0;
}

static int build_inter_pred_calcu_cpu(VP9_COMMON *const cm, int tile_num) {
  const YV12_BUFFER_CONFIG *cfg_source = &cm->yv12_fb[cm->new_fb_idx];

#if USE_PPA
  PPAStartCpuEventFunc(inter_pred_calcu_cpu_tile);
#endif
  build_inter_pred_calcu_ocl_c(tile_num, cfg_source->buffer_alloc);
#if USE_PPA
  PPAStopCpuEventFunc(inter_pred_calcu_cpu_tile);
#endif

  return 0;
}

static int build_inter_pred_calcu_gpu(VP9_COMMON *const cm) {
  int status;
  cl_event calcu_gpu_event;

#if USE_PPA
  PPAStartCpuEventFunc(inter_pred_calcu_gpu_all_frame);
#endif
  inter_ocl_obj.globalThreads[0] =
    (inter_ocl_obj.all_of_block_count_gpu + 64) -
    (inter_ocl_obj.all_of_block_count_gpu % 64);

  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             1, sizeof(cl_mem),
             (void*) inter_ocl_obj.dst_index_xmv_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 1, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             2, sizeof(cl_mem),
             (void*) inter_ocl_obj.pred_param_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 2, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             3, sizeof(cl_mem),
             (void*) inter_ocl_obj.index_param_num_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 3, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             4, sizeof(cl_mem),
             (void*) inter_ocl_obj.index_xmv_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 4, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
             inter_ocl_obj.kernel,
             5, sizeof(cl_mem),
             (void*) inter_ocl_obj.case_count_kernel);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 5, error: %d\n", status);
    return -1;
  }
  status = clSetKernelArg(
               inter_ocl_obj.kernel,
               7, sizeof(int),
               (void*) &inter_ocl_obj.all_of_block_count_gpu);
  if (status != CL_SUCCESS) {
    LOGE("Failed to set arguments 7, error: %d\n", status);
    return -1;
  }

  // Executive inter prediction GPU part
  status = clEnqueueNDRangeKernel(ocl_context.command_queue,
                                  inter_ocl_obj.kernel, 3,
                                  0, inter_ocl_obj.globalThreads,
                                  inter_ocl_obj.localThreads, 0, NULL,
                                  &calcu_gpu_event);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clEnqueueNDRangeKernel inter pred, error: %d\n", status);
    return -1;
  }
  status = clFlush(ocl_context.command_queue);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clFlush, error: %d\n", status);
    return -1;
  }

  status = clWaitForEvents(1, &calcu_gpu_event);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clWaitForEvents calcu_gpu_event, error: %d\n", status);
    return -1;
  }
#if USE_PPA
  PPAStopCpuEventFunc(inter_pred_calcu_gpu_all_frame);
#endif

  return 0;
}

int inter_pred_index_ocl_whole_frame(VP9_COMMON *const cm) {
  int status;

#if USE_PPA
  PPAStartCpuEventFunc(inter_pred_index_time);
#endif
  status = build_inter_pred_index_whole_frame(cm);
  if (status < 0) {
    LOGE("Failed to build_inter_pred_index_whole_frame");
    return -1;
  }

  inter_switch_param_td();
#if USE_PPA
  PPAStopCpuEventFunc(inter_pred_index_time);
#endif

  return 0;
}

int inter_pred_calcu_ocl(VP9_COMMON *const cm, int tile_num, int dev_gpu) {
  int i;

  if (dev_gpu) {
    inter_switch_calcu_td_gpu();

    inter_ocl_obj.all_of_block_count_gpu = 0;
    for (i = 0; i < inter_ocl_obj.tile_count; ++i)
      inter_ocl_obj.all_of_block_count_gpu += inter_ocl_obj.index_count[i];

    if (inter_ocl_obj.all_of_block_count_gpu > 0)
      build_inter_pred_calcu_gpu(cm);
  } else {
    inter_switch_calcu_td_cpu(tile_num);

    build_inter_pred_calcu_cpu(cm, tile_num);
  }

  return 0;
}

int get_second_ref_count_ocl() {
  int tile_num;
  int sec_ref_count = 0;

  for (tile_num = 0; tile_num < inter_ocl_obj.tile_count; ++tile_num) {
    if (inter_ocl_obj.switch_td_calcu_cpu[tile_num]) {
      sec_ref_count += inter_ocl_obj.cpu_sec_count_td1[tile_num];
    } else {
      sec_ref_count += inter_ocl_obj.cpu_sec_count_td0[tile_num];
    }
  }

  return sec_ref_count;
}

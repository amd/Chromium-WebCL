/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "vp9/common/inter_ocl/vp9_yuv2rgba.h"
#include "vp9/common/inter_ocl/vp9_inter_ocl_param.h"
#include "vp9/common/inter_ocl/opencl/CL/cl_dx9_media_sharing.h"
#include <d3d9.h>

VP9_YUV2RGBA_OCL yuv2rgba_ocl_obj;

int init_yuv2rgba_ocl_obj() {
  int status;
  char *psource;
  status = load_source_from_file(
               "vp9_yuv2rgba.cl",
               &yuv2rgba_ocl_obj.source,
               &yuv2rgba_ocl_obj.source_len);
  if (status < 0) {
    printf("Failed to load kernel, error: %d\n", status);
    exit(1);
  }

  psource = yuv2rgba_ocl_obj.source;
  yuv2rgba_ocl_obj.program = create_and_build_program(
                              &ocl_context, 1,
                              (const char **)&psource,
                              &yuv2rgba_ocl_obj.source_len, &status);
  if (status < 0) {
    printf("There is some error in create&build program, error: %d\n", status);
    exit(1);
  }

 
  // This for yuv_rgba
  yuv2rgba_ocl_obj.yuv_rgba_kernel= clCreateKernel(
                                yuv2rgba_ocl_obj.program,
                                "yuv_rgba", &status);
  if (status != CL_SUCCESS) {
    printf("Failed to clCreateKernel yuv_rgba, error: %d\n", status);
    exit(1);
  }
  return 0;
}


int create_buffer_from_d3d9_surface(VP9_YUV2RGBA_OCL *yuv2rgba_ocl_obj,
                                                                         struct IDirect3DSurface9 *d3d_surface9) {
  cl_int status;
 
  typedef struct cl_dx9_surface_info_khr
  {
      IDirect3DSurface9 *resource;
      HANDLE shared_handle;
  }cl_dx9_surface_info_khr;

  cl_dx9_surface_info_khr surface = {NULL, NULL};
  surface.resource= (IDirect3DSurface9*)(d3d_surface9);
  
  yuv2rgba_ocl_obj->clImag = clCreateFromDX9MediaSurfaceKHR(ocl_context.context, 
                                                                                                   CL_MEM_WRITE_ONLY, 
                                                                                                   CL_ADAPTER_D3D9EX_KHR,
                                                                                                   &surface, 0, &status);
  if(CL_SUCCESS != status) {
    printf("fail to clCreateFromDX9MediaSurfaceKHR\n");
    return -1;
  }  
  return status;
}

int vp9_i420_to_rgba_ocl(VP9_COMMON *cm,VP9_YUV2RGBA_OCL *yuv2rgba_ocl_obj, void*texture) {
  int status, new_fb, arg = 0;
  IDirect3DSurface9 *d3d_surface9;
  YV12_BUFFER_CONFIG *current_frame;
  //create opencl buffer from d3d9 surface
  d3d_surface9 = (IDirect3DSurface9 *)texture;

  
  status = create_buffer_from_d3d9_surface(yuv2rgba_ocl_obj, d3d_surface9);
  if (status != 0) {
    printf("fail to create buffer from d3d9 surface\n");
    return -1;
  }
  
  
  new_fb = cm->new_fb_idx;
  current_frame = &cm->yv12_fb[new_fb];

  yuv2rgba_ocl_obj->y_plane_offset = (current_frame->y_buffer - current_frame->buffer_alloc) +
                                                       (cm->new_fb_idx * current_frame->buffer_alloc_sz);
  yuv2rgba_ocl_obj->u_plane_offset = (current_frame->u_buffer - current_frame->buffer_alloc) +
                                                       (cm->new_fb_idx * current_frame->buffer_alloc_sz);
  yuv2rgba_ocl_obj->v_plane_offset = (current_frame->v_buffer - current_frame->buffer_alloc) +
                                                       (cm->new_fb_idx * current_frame->buffer_alloc_sz);
  // set kernel args
  // (1) buffer_Pool
   status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_kernel);
  if (status != CL_SUCCESS) {
    printf("Failed to set arguments 0, error: %d\n", status);
    return -1;
  }
  // (2) buffer pool read only buffer
   status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_read_only_kernel);
  if (status != CL_SUCCESS) {
    printf("Failed to set arguments 1, error: %d\n", status);
    return -1;
  }
 // (3) y_plane_offset
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->y_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 2, error: %d\n", status);
    return -1;
  }
  // (4) u_plane_offset
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->u_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 3, error: %d\n", status);
    return -1;
  }
   // (5) v_plane_offset
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->v_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 4, error: %d\n", status);
    return -1;
  }
  // (6) y_stride
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(int),
             (void*) &current_frame->y_stride);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 5, error: %d\n", status);
    return -1;
  }
   // (7) uv_stride
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(int),
             (void*) &current_frame->uv_stride);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 6, error: %d\n", status);
    return -1;
  }
  
  // (8) dst buffer
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->yuv_rgba_kernel,
             arg++, sizeof(cl_mem),
             (void*) &yuv2rgba_ocl_obj->clImag);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 7, error: %d\n", status);
    return -1;
  }
  yuv2rgba_ocl_obj->globalThreads[0] = (cm->width >> cm->subsampling_x);
  yuv2rgba_ocl_obj->globalThreads[1] = (cm->height >> cm->subsampling_y);
  //NDRange kernel
   status = clEnqueueAcquireDX9MediaSurfacesKHR(ocl_context.command_queue, 1, 
                                                               &yuv2rgba_ocl_obj->clImag, 0, NULL, NULL);
  if(CL_SUCCESS != status) {
    printf("Fail to clEnqueueAcquireDX9MediaSurfacesKHR status = %d\n", status);
    return -1;
  }

  clEnqueueNDRangeKernel(ocl_context.command_queue, yuv2rgba_ocl_obj->yuv_rgba_kernel, 2,
                                      NULL, yuv2rgba_ocl_obj->globalThreads, NULL, 0, NULL, NULL );
  
  
  status = clEnqueueReleaseDX9MediaSurfacesKHR(ocl_context.command_queue, 1,
                                                                   &yuv2rgba_ocl_obj->clImag, 0, NULL, NULL);
  if(CL_SUCCESS != status) {
    printf("Fail to clEnqueueReleaseDX9MediaSurfacesKHR status = %d\n", status);
    return -1;
  }
  clFinish(ocl_context.command_queue);
  // release cl image
  status = clReleaseMemObject(yuv2rgba_ocl_obj->clImag);                                                      
  if(CL_SUCCESS != status) {
    printf("Fail to clReleaseMemObject clImag status = %d\n", status);
    return -1;
  }
  return 0;
}

int release_yuv2rgba_ocl_obj() {
  int status;
  if (yuv2rgba_ocl_obj.yuv_rgba_kernel)
    status |= clReleaseKernel(yuv2rgba_ocl_obj.yuv_rgba_kernel);
  if (yuv2rgba_ocl_obj.program)
    status |= clReleaseProgram(yuv2rgba_ocl_obj.program);
  if (yuv2rgba_ocl_obj.source != NULL) {
    free(yuv2rgba_ocl_obj.source);
    yuv2rgba_ocl_obj.source = NULL;
  }
  return status;
}




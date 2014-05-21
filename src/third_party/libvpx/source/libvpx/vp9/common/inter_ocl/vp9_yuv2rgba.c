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

#include "vpx_ports/vpx_timer.h"
#include <d3d9.h>
#ifdef __WIN32
#include <windows.h>
#else
typedef void * HANDLE;
#endif
FILE *pLog = NULL;
VP9_YUV2RGBA_OCL yuv2rgba_ocl_obj;

int init_yuv2rgba_ocl_obj() {
  int status;
  char *psource;
  memset(yuv2rgba_ocl_obj.clImages, 0, 50 * sizeof(cl_mem));
  memset(yuv2rgba_ocl_obj.d3d9_surfaces, 0, 50 * sizeof(void*));
  yuv2rgba_ocl_obj.surface_index = 0;
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
 

   yuv2rgba_ocl_obj.only_color_space_transform_kernel= clCreateKernel(
                                yuv2rgba_ocl_obj.program,
                                "yuv_rgba", &status);
  if (status != CL_SUCCESS) {
    printf("Failed to clCreateKernel yuv_rgba, error: %d\n", status);
    exit(1);
  }
  #if 0
  yuv2rgba_ocl_obj.rgb_buffer = clCreateBuffer(ocl_context.context,
  	    CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
  	                                                            1920 * 1080 * 3/2,
  	                                                             NULL,
  	                                                             &status);
  if (status != CL_SUCCESS) {
    printf("Failed to clCreateBuffer rgb, error: %d\n", status);
    exit(1);
  }
  printf("ocl_context.command_queue=%d\n", ocl_context.command_queue);
  yuv2rgba_ocl_obj.rgb_map = (uint8_t *)clEnqueueMapBuffer(ocl_context.command_queue,
                                  yuv2rgba_ocl_obj.rgb_buffer,
                                  CL_TRUE, CL_MAP_READ,
                                  0,
                                  1920*1080*3/2,
                                  0, NULL, NULL, &status);
   if (status != CL_SUCCESS) {
    printf("Failed to clEnqueueMapBuffer rgb, error: %d\n", status);
    exit(1);
  }
  #endif
  return 0;
}


static int create_buffer_from_d3d9_surface(VP9_YUV2RGBA_OCL *yuv2rgba_ocl_obj,
                                    struct IDirect3DSurface9 *d3d_surface9,
                                    void *pSharedHandle,
                                    cl_mem *image,
									int plane) {
  cl_int status;

  typedef struct cl_dx9_surface_info_khr
  {
      IDirect3DSurface9 *resource;
      HANDLE shared_handle;
  }cl_dx9_surface_info_khr;

 
  cl_dx9_surface_info_khr surface = {NULL, NULL};
  surface.resource= (IDirect3DSurface9*)(d3d_surface9);
  surface.shared_handle = (HANDLE)pSharedHandle;
  
  (*image) = clCreateFromDX9MediaSurfaceKHR(ocl_context.context,
	  CL_MEM_WRITE_ONLY, 
                                                                                                   CL_ADAPTER_D3D9EX_KHR,
                                                                                                   &surface, plane, &status);
  if(CL_SUCCESS != status) {
    printf("fail to clCreateFromDX9MediaSurfaceKHR\n");
    return -1;
  }  
  return status;
}

static cl_mem* get_cl_image(VP9_YUV2RGBA_OCL *obj, void *d3d_surface9, HANDLE pSharedHandle ) {
   int i, status;
   cl_mem image[3];
   int aa;
   for (i = 0; i < 50; i++) {
    if (obj->d3d9_surfaces[i] == d3d_surface9) {
      return obj->clImages + i * 3;
    }    
  }
  if(obj->d3d9_surfaces[obj->surface_index] == NULL) {
    obj->d3d9_surfaces[obj->surface_index] = d3d_surface9;
    status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9,
                                                         pSharedHandle, &image[0], 0);

    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
    obj->clImages[obj->surface_index * 3 + 0] = image[0];
	 status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9,
                                                         pSharedHandle, &image[1], 1);

    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
    obj->clImages[obj->surface_index * 3 + 1] = image[1];
	 status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9,
                                                         pSharedHandle, &image[2], 2);

    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
    obj->clImages[obj->surface_index * 3 + 2] = image[2];
	aa = obj->surface_index;
    obj->surface_index++;
    obj->surface_index = obj->surface_index % 50;
	return obj->clImages + aa * 3;
  } else {
    clReleaseMemObject(obj->clImages[obj->surface_index * 3 + 0]);
	clReleaseMemObject(obj->clImages[obj->surface_index * 3 + 1]);
	clReleaseMemObject(obj->clImages[obj->surface_index * 3 + 2]);
    status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9, 
                                                     pSharedHandle, &image[0], 0);
    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
	 obj->clImages[obj->surface_index * 3 + 0] = image[0];
	 status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9,
                                                         pSharedHandle, &image[1], 1);

    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
    obj->clImages[obj->surface_index * 3 + 1] = image[1];
	 status = create_buffer_from_d3d9_surface(obj, (IDirect3DSurface9 *)d3d_surface9,
                                                         pSharedHandle, &image[2], 2);

    if (status != 0) {
      printf("fail to create buffer from d3d9 surface\n");
      return NULL;
    }
    obj->clImages[obj->surface_index * 3 + 2] = image[2];

    obj->d3d9_surfaces[obj->surface_index] = d3d_surface9;
    aa = obj->surface_index;
    obj->surface_index++;
    obj->surface_index = obj->surface_index % 50;
	return obj->clImages + aa * 3;
  }  
}



int vp9_yuv2rgba( VP9_YUV2RGBA_OCL *yuv2rgba_ocl_obj, void *texture) {

  int status, arg = 0;
  Interop_Context *p_context;
  struct vpx_usec_timer timer;
  unsigned long get_cl_image_time = 0;
  unsigned long clEnqueueAcquireDX9MediaSurfacesKHR_time = 0;
  unsigned long clEnqueueNDRangeKernel_time = 0;
  unsigned long clEnqueueReleaseDX9MediaSurfacesKHR_time = 0;
  int ty;
  cl_image_format format;
  cl_mem *real_imag;
  //create opencl buffer from d3d9 surface
  p_context = (Interop_Context *)(texture);
  vpx_usec_timer_start(&timer);
  real_imag = get_cl_image(yuv2rgba_ocl_obj, p_context->pSurface, (HANDLE)(p_context->pSharedHandle));
   
 // clGetMemObjectInfo(real_imag, CL_MEM_TYPE, sizeof(int), &ty, NULL);
 
 // clGetImageInfo(real_imag, CL_IMAGE_FORMAT, sizeof(format), &format, NULL);
  //printf("ty = %xd\n", ty);
 // printf("order = %xd\n", format.image_channel_order);
 // printf("chael_type = %xd\n", format.image_channel_data_type);
  vpx_usec_timer_mark(&timer);
  get_cl_image_time = (unsigned int)vpx_usec_timer_elapsed(&timer);
  
  // set kernel args
  // (1) buffer_Pool
   status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(cl_mem),
             (void*) &inter_ocl_obj.buffer_pool_kernel);
  if (status != CL_SUCCESS) {
    printf("Failed to set arguments 0, error: %d\n", status);
    return -1;
  }
 
 // (3) y_plane_offset
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->y_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 1, error: %d\n", status);
    return -1;
  }
  // (4) u_plane_offset
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->u_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 2, error: %d\n", status);
    return -1;
  }
   // (5) v_plane_offset
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->v_plane_offset);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 3, error: %d\n", status);
    return -1;
  }
  // (6) y_stride
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->Y_stride);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 4, error: %d\n", status);
    return -1;
  }
   // (7) uv_stride
    status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(int),
             (void*) &yuv2rgba_ocl_obj->UV_stride);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 5, error: %d\n", status);
    return -1;
  }
  
  // (8) dst buffer
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(cl_mem),
             (void*) &real_imag[0]);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 6, error: %d\n", status);
    return -1;
  }

  // (8) dst buffer
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(cl_mem),
             (void*) &real_imag[1]);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 7, error: %d\n", status);
    return -1;
  }

  // (8) dst buffer
  status = clSetKernelArg(
             yuv2rgba_ocl_obj->only_color_space_transform_kernel,
             arg++, sizeof(cl_mem),
             (void*) &real_imag[2]);
  if (status != CL_SUCCESS) {
    printf("yuv_rgba_kernel:Failed to set arguments 8, error: %d\n", status);
    return -1;
  }
  
  //NDRange kernel
  vpx_usec_timer_start(&timer);
  status = clEnqueueAcquireDX9MediaSurfacesKHR(ocl_context.command_queue, 1, 
                                                              &real_imag[0], 0, NULL, NULL);
  status = clEnqueueAcquireDX9MediaSurfacesKHR(ocl_context.command_queue, 1, 
                                                              &real_imag[1], 0, NULL, NULL);
  status = clEnqueueAcquireDX9MediaSurfacesKHR(ocl_context.command_queue, 1, 
                                                              &real_imag[2], 0, NULL, NULL);
  if(CL_SUCCESS != status) {
    printf("Fail to clEnqueueAcquireDX9MediaSurfacesKHR status = %d\n", status);
    return -1;
  }
  vpx_usec_timer_mark(&timer);
  clEnqueueAcquireDX9MediaSurfacesKHR_time = (unsigned int)vpx_usec_timer_elapsed(&timer);

  vpx_usec_timer_start(&timer);
  status = clEnqueueNDRangeKernel(ocl_context.command_queue, yuv2rgba_ocl_obj->only_color_space_transform_kernel, 2,
                                      NULL, yuv2rgba_ocl_obj->globalThreads, NULL, 0, NULL, NULL );
  if(CL_SUCCESS != status) {
    printf("Fail to clEnqueueNDRangeKernel status = %d\n", status);
    return -1;
  }
  clFinish(ocl_context.command_queue);
  vpx_usec_timer_mark(&timer);
  clEnqueueNDRangeKernel_time = (unsigned int)vpx_usec_timer_elapsed(&timer);
  
  vpx_usec_timer_start(&timer);
  status = clEnqueueReleaseDX9MediaSurfacesKHR(ocl_context.command_queue, 1,
                                                                   &real_imag[0], 0, NULL, NULL);
  status = clEnqueueReleaseDX9MediaSurfacesKHR(ocl_context.command_queue, 1,
                                                                   &real_imag[1], 0, NULL, NULL);
  status = clEnqueueReleaseDX9MediaSurfacesKHR(ocl_context.command_queue, 1,
                                                                   &real_imag[2], 0, NULL, NULL);
  if(CL_SUCCESS != status) {
    printf("Fail to clEnqueueReleaseDX9MediaSurfacesKHR status = %d\n", status);
    return -1;
  }
  vpx_usec_timer_mark(&timer);
  clEnqueueReleaseDX9MediaSurfacesKHR_time = (unsigned int)vpx_usec_timer_elapsed(&timer);
 
  ///////////////////////////////////////////////////////////////////////////////////////////////
   
  fprintf(pLog, "create buffer time(from d3d9 surface): %lu us\n"
	            "clEnqueueAcquireDX9MediaSurfacesKHR API time: %lu us\n"
				"YUV to RGB kernel time: %lu us\n"
				"clEnqueueReleaseDX9MediaSurfacesKHR API time: %lu us\n",
				get_cl_image_time, clEnqueueAcquireDX9MediaSurfacesKHR_time, 
				clEnqueueNDRangeKernel_time, clEnqueueReleaseDX9MediaSurfacesKHR_time);
  
  return 0;
}



int release_yuv2rgba_ocl_obj() {
  int status;
  int i;
  status = 0;
  for(i = 0; i < 50; i++) {
    if(yuv2rgba_ocl_obj.clImages[i] != NULL)
      status |= clReleaseMemObject(yuv2rgba_ocl_obj.clImages[i]);
  }
   /*if(yuv2rgba_ocl_obj.clImag != NULL)
      status |= clReleaseMemObject(yuv2rgba_ocl_obj.clImag);*/
  
    if (yuv2rgba_ocl_obj.only_color_space_transform_kernel)
    status |= clReleaseKernel(yuv2rgba_ocl_obj.only_color_space_transform_kernel);
  if (yuv2rgba_ocl_obj.program)
    status |= clReleaseProgram(yuv2rgba_ocl_obj.program);
  if (yuv2rgba_ocl_obj.source != NULL) {
    free(yuv2rgba_ocl_obj.source);
    yuv2rgba_ocl_obj.source = NULL;
  }
  return status;
}




/*
 * Copyright (C) 2012, 2013 Samsung Electronics Corporation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
* CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
* ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
* NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ComputeTypes_h
#define ComputeTypes_h


//#if PLATFORM(MAC)
//   #include <OpenCL/opencl.h>
//#else
   #include <CL/opencl.h>
//#endif

typedef unsigned int CCenum;
typedef int CCerror;

typedef cl_int CCint;
typedef cl_short CCshort;
typedef cl_ushort CCushort;
typedef cl_char CCchar;
typedef cl_uchar CCuchar;
typedef cl_bool CCbool;
typedef cl_uint CCuint;
typedef cl_long CClong;
typedef cl_ulong CCulong;
typedef cl_float CCfloat;
typedef cl_half CChalf;
typedef cl_double CCdouble;
typedef cl_mem PlatformComputeObject;
typedef cl_platform_id CCPlatformID;
typedef cl_device_type CCDeviceType;
typedef cl_device_id CCDeviceID;
typedef cl_context_properties CCContextProperties;
typedef cl_image_format CCImageFormat;
#if defined(CL_VERSION_1_2) && CL_VERSION_1_2
typedef cl_image_desc CCImageDescriptor;
#endif
typedef cl_sampler CCSampler;
typedef cl_command_queue CCCommandQueue;
typedef cl_command_queue_properties CCCommandQueueProperties;
typedef cl_command_type CCCommandType;
typedef cl_program CCProgram;
typedef cl_kernel_work_group_info CCKernelWorkGroupInfo;
typedef cl_kernel CCKernel;
typedef cl_event CCEvent;
typedef cl_buffer_region CCBufferRegion;
typedef cl_mem_object_type CCMemoryObjectType;
typedef cl_addressing_mode CCAddressingMode;
typedef cl_filter_mode CCFilterMode;
typedef cl_buffer_create_type CCBufferCreateType;
typedef cl_mem_flags CCMemoryFlags;
typedef cl_device_mem_cache_type CCDeviceMemCachetype;
typedef cl_device_exec_capabilities CCDeviceExecCapabilities;
typedef cl_device_local_mem_type CCDeviceLocalMemType;
typedef cl_device_fp_config CCDeviceFPConfig;
typedef cl_build_status CCBuildStatus;
typedef cl_context CCContext;

// FIXME: It has to follow the CCxxx pattern here.
typedef void (*pfnNotify)(CCProgram, void*);
typedef void (*pfnEventNotify)(CCEvent, CCint eventCommandExecStatus, void* userData);

typedef cl_device_info CCDeviceInfoType;
typedef cl_platform_info CCPlatformInfoType;
typedef cl_program_info CCProgramInfoType;
typedef cl_program_build_info CCProgramBuildInfoType;
typedef cl_sampler_info CCSamplerInfoType;
typedef cl_mem_info CCMemInfoType;
typedef cl_command_queue_info CCCommandQueueInfoType;
typedef cl_kernel_info CCKernelInfoType;
typedef cl_kernel_work_group_info CCKernelWorkGroupInfoType;
typedef cl_image_info CCImageInfoType;

// FIXME: guard against disabled WebGL builds?
typedef cl_gl_texture_info CCImageTextureInfoType;
typedef cl_event_info CCEventInfoType;
typedef cl_profiling_info CCEventProfilingInfoType;

#define JUST_WEBCL_FUNC_DECLARATIONS
#include "WebCLInclude.h"
#undef JUST_WEBCL_FUNC_DECLARATIONS

#define clGetPlatformIDs webcl_clGetPlatformIDs
#define clGetPlatformInfo webcl_clGetPlatformInfo
#define clGetDeviceIDs webcl_clGetDeviceIDs
#define clGetDeviceInfo webcl_clGetDeviceInfo
#define clCreateSubDevices webcl_clCreateSubDevices
#define clRetainDevice webcl_clRetainDevice
#define clReleaseDevice webcl_clReleaseDevice
#define clCreateContext webcl_clCreateContext
#define clCreateContextFromType webcl_clCreateContextFromType
#define clRetainContext webcl_clRetainContext
#define clReleaseContext webcl_clReleaseContext
#define clGetContextInfo webcl_clGetContextInfo
#define clCreateCommandQueue webcl_clCreateCommandQueue
#define clRetainCommandQueue webcl_clRetainCommandQueue
#define clReleaseCommandQueue webcl_clReleaseCommandQueue
#define clGetCommandQueueInfo webcl_clGetCommandQueueInfo
#define clCreateBuffer webcl_clCreateBuffer
#define clCreateSubBuffer webcl_clCreateSubBuffer
#define clCreateImage webcl_clCreateImage
#define clRetainMemObject webcl_clRetainMemObject
#define clReleaseMemObject webcl_clReleaseMemObject
#define clGetSupportedImageFormats webcl_clGetSupportedImageFormats
#define clGetMemObjectInfo webcl_clGetMemObjectInfo
#define clGetImageInfo webcl_clGetImageInfo
#define clSetMemObjectDestructorCallback webcl_clSetMemObjectDestructorCallback
#define clCreateSampler webcl_clCreateSampler
#define clRetainSampler webcl_clRetainSampler
#define clReleaseSampler webcl_clReleaseSampler
#define clGetSamplerInfo webcl_clGetSamplerInfo
#define clCreateProgramWithSource webcl_clCreateProgramWithSource
#define clCreateProgramWithBinary webcl_clCreateProgramWithBinary
#define clCreateProgramWithBuiltInKernels webcl_clCreateProgramWithBuiltInKernels
#define clRetainProgram webcl_clRetainProgram
#define clReleaseProgram webcl_clReleaseProgram
#define clBuildProgram webcl_clBuildProgram
#define clCompileProgram webcl_clCompileProgram
#define clLinkProgram webcl_clLinkProgram
#define clUnloadPlatformCompiler webcl_clUnloadPlatformCompiler
#define clGetProgramInfo webcl_clGetProgramInfo
#define clGetProgramBuildInfo webcl_clGetProgramBuildInfo
#define clCreateKernel webcl_clCreateKernel
#define clCreateKernelsInProgram webcl_clCreateKernelsInProgram
#define clRetainKernel webcl_clRetainKernel
#define clReleaseKernel webcl_clReleaseKernel
#define clSetKernelArg webcl_clSetKernelArg
#define clSetKernelArg_vector webcl_clSetKernelArg_vector
#define clGetKernelInfo webcl_clGetKernelInfo
#define clGetKernelArgInfo webcl_clGetKernelArgInfo
#define clGetKernelWorkGroupInfo webcl_clGetKernelWorkGroupInfo
#define clWaitForEvents webcl_clWaitForEvents
#define clGetEventInfo webcl_clGetEventInfo
#define clCreateUserEvent webcl_clCreateUserEvent
#define clRetainEvent webcl_clRetainEvent
#define clReleaseEvent webcl_clReleaseEvent
#define clSetUserEventStatus webcl_clSetUserEventStatus
#define clSetEventCallback webcl_clSetEventCallback
#define clGetEventProfilingInfo webcl_clGetEventProfilingInfo
#define clFlush webcl_clFlush
#define clFinish webcl_clFinish
#define clEnqueueReadBuffer webcl_clEnqueueReadBuffer
#define clEnqueueReadBufferRect webcl_clEnqueueReadBufferRect
#define clEnqueueWriteBuffer webcl_clEnqueueWriteBuffer
#define clEnqueueWriteBufferRect webcl_clEnqueueWriteBufferRect
#define clEnqueueFillBuffer webcl_clEnqueueFillBuffer
#define clEnqueueCopyBuffer webcl_clEnqueueCopyBuffer
#define clEnqueueCopyBufferRect webcl_clEnqueueCopyBufferRect
#define clEnqueueReadImage webcl_clEnqueueReadImage
#define clEnqueueWriteImage webcl_clEnqueueWriteImage
#define clEnqueueFillImage webcl_clEnqueueFillImage
#define clEnqueueCopyImage webcl_clEnqueueCopyImage
#define clEnqueueCopyImageToBuffer webcl_clEnqueueCopyImageToBuffer
#define clEnqueueCopyBufferToImage webcl_clEnqueueCopyBufferToImage
#define clEnqueueMapBuffer webcl_clEnqueueMapBuffer
#define clEnqueueMapImage webcl_clEnqueueMapImage
#define clEnqueueUnmapMemObject webcl_clEnqueueUnmapMemObject
#define clEnqueueMigrateMemObjects webcl_clEnqueueMigrateMemObjects
#define clEnqueueNDRangeKernel webcl_clEnqueueNDRangeKernel
#define clEnqueueTask webcl_clEnqueueTask
#define clEnqueueNativeKernel webcl_clEnqueueNativeKernel
#define clEnqueueMarkerWithWaitList webcl_clEnqueueMarkerWithWaitList
#define clEnqueueBarrierWithWaitList webcl_clEnqueueBarrierWithWaitList
#define clSetPrintfCallback webcl_clSetPrintfCallback

#define clCreateFromGLBuffer webcl_clCreateFromGLBuffer
#define clCreateFromGLTexture webcl_clCreateFromGLTexture
#define clEnqueueAcquireGLObjects webcl_clEnqueueAcquireGLObjects
#define clEnqueueReleaseGLObjects webcl_clEnqueueReleaseGLObjects

#define clEnqueueMarker webcl_clEnqueueMarker
#define clEnqueueWaitForEvents webcl_clEnqueueWaitForEvents
#define clEnqueueBarrier webcl_clEnqueueBarrier



#endif

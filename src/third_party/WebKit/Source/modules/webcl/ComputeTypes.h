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
#endif

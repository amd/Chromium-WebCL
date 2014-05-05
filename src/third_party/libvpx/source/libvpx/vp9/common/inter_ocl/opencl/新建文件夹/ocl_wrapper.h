/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef OCL_WRAPPER_H_
#define OCL_WRAPPER_H_

#include "vp9/common/inter_ocl/opencl/CL/cl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_CL_FILE
#define CHECK_OPENCL(status,name)    \
if( status != CL_SUCCESS )    \
{    \
    printf ("OpenCL error code is %d for %s .\n", status,#name);    \
    return 0;    \
}
#define CHECK_CPU_FREE(name)    \
if ( name )    \
{    \
    free( name );    \
    name = NULL;    \
}

#define VENDOR_INFO_SIZE 100

typedef cl_int (*RetainBufferProto)        (cl_mem);
typedef cl_int (*ReleaseBufferProto)       (cl_mem);
typedef cl_int (*FlushProto)               (cl_command_queue);
typedef cl_int (*FinishProto)              (cl_command_queue);
typedef cl_int (*ReleaseContextProto)      (cl_context);
typedef cl_int (*ReleaseCommandQueueProto) (cl_command_queue);
typedef cl_int (*RetainEventProto)         (cl_event);
typedef cl_int (*ReleaseProgramProto)      (cl_program);
typedef cl_int (*ReleaseMemObjectProto)    (cl_mem);
typedef cl_int (*ReleaseKernelProto)       (cl_kernel);

typedef cl_int (*WaitForEventsProto) (
            cl_uint,
            const cl_event *);

typedef cl_int (*SetUserEventStatusProto) (
            cl_event,
            cl_int);

typedef cl_event (*CreateUserEventProto) (
            cl_context,
            cl_int *);

typedef cl_int (*GetBufferInfoProto) (
            cl_mem,
            cl_mem,
            size_t,
            void *,
            size_t);

typedef cl_context (*CreateContextProto) (
            const cl_context_properties *,
            cl_uint, const cl_device_id *,
            void(CL_CALLBACK *pfn_notify)
            (const char *, const void*, size_t, void *),
            void *,
            cl_int *);

typedef cl_command_queue (*CreateCommandQueueProto) (
            cl_context,
            cl_device_id,
            cl_command_queue_properties,
            cl_int *);

typedef cl_mem (*CreateBufferProto) (
            cl_context,
            cl_mem_flags,
            size_t,
            void *,
            cl_int*);

typedef cl_mem (*CreateSubBufferProto) (
            cl_mem,
            cl_mem_flags,
            cl_buffer_create_type,
            const void *,
            cl_int *);

typedef cl_int (*CopyBufferProto) (
            cl_command_queue,
            cl_mem,
            cl_mem,
            size_t,
            size_t,
            size_t,
            cl_uint,
            const cl_event *,
            cl_event *);

typedef void * (*MapBufferProto) (
            cl_command_queue,
            cl_mem,
            cl_bool,
            cl_map_flags,
            size_t offset,
            size_t size,
            cl_uint,
            const  cl_event *,
            cl_event*,
            cl_int*);

typedef cl_int (*UnmapBufferProto) (
            cl_command_queue,
            cl_mem,
            void *, cl_uint,
            const cl_event *,
            cl_event*);

typedef cl_int (*ReadBufferProto) (
            cl_command_queue,
            cl_mem,
            cl_bool,
            size_t,
            size_t,
            void *,
            cl_uint,
            const cl_event*,
            cl_event*);

typedef cl_int (*SetEventCallbackProto) (
            cl_event,
            cl_int,
            void (CL_CALLBACK *pfn_event_notify)
            (cl_event, cl_int, void *),
            void *);

typedef cl_int (*GetEventInfoProto) (
            cl_event,
            cl_event_info,
            size_t,
            void *,
            size_t *);

typedef cl_program (*CreateProgramProto) (
            cl_context, cl_uint,
            const cl_device_id *,
            const size_t *,
            const unsigned char **,
            cl_int*, cl_int*);

typedef cl_program (*CreateProgramWithSourceProto) (
            cl_context,
            cl_uint,
            const char **,
            const size_t *,
            cl_int*);

typedef cl_int (*BuildProgramProto) (
            cl_program,
            cl_uint,
            const cl_device_id *,
            const char *,
            void (CL_CALLBACK *pfn_notify)
            (cl_program program, void *user_data),
            void *);

typedef cl_kernel (*CreateKernelProto)(
            cl_program,
            const char*,
            cl_int*);

typedef cl_int (*SetKernelArgProto)(
            cl_kernel,
            cl_uint,
            size_t,
            const void *);

typedef cl_int (*EnqueueKernelProto) (
            cl_command_queue,
            cl_kernel,
            cl_uint,
            const size_t *,
            const size_t *,
            const size_t *,
            cl_uint,
            const cl_event *,
            cl_event *);

typedef cl_int (*GetBuildInfoProto) (
            cl_program,
            cl_device_id,
            cl_program_build_info,
            size_t,
            void *,
            size_t*);

typedef cl_int (*GetPlatformIDsProto) (
            cl_uint,
            cl_platform_id *,
            cl_uint *);

typedef cl_int (*GetPlatformInfoProto) (
            cl_platform_id platform,
            cl_platform_info param_name,
            size_t param_value_size,
            void *param_value,
            void *param_value_size_ret);

typedef cl_int (*GetDeviceIDsProto) (
            cl_platform_id,
            cl_device_type,
            cl_uint,
            cl_device_id*,
            cl_uint*);

typedef cl_int (*GetContextInfo) (
            cl_context         /* context */,
            cl_context_info    /* param_name */,
            size_t             /* param_value_size */,
            void *             /* param_value */,
            size_t *           /* param_value_size_ret */);

typedef cl_int (*GetDeviceInfo) (
            cl_device_id    /* device */,
            cl_device_info  /* param_name */,
            size_t          /* param_value_size */,
            void *          /* param_value */,
            size_t *        /* param_value_size_ret */);

typedef cl_int (*GetProgramInfo) (
            cl_program         /* program */,
            cl_program_info    /* param_name */,
            size_t             /* param_value_size */,
            void *             /* param_value */,
            size_t *           /* param_value_size_ret */);

typedef cl_context (*CreateContextFromType) (
            const cl_context_properties * /* properties */,
            cl_device_type                /* device_type */,
            void (CL_CALLBACK *           /* pfn_notify*/ )
            (const char *, const void *, size_t, void *),
            void *                        /* user_data */,
            cl_int *                      /* errcode_ret */);

typedef cl_int (*GetCommandQueueInfo) (
            cl_command_queue      /* command_queue */,
            cl_command_queue_info /* param_name */,
            size_t                /* param_value_size */,
            void *                /* param_value */,
            size_t *              /* param_value_size_ret */);

typedef cl_int (*GetProgramBuildInfo) (
            cl_program            /* program */,
            cl_device_id          /* device */,
            cl_program_build_info /* param_name */,
            size_t                /* param_value_size */,
            void *                /* param_value */,
            size_t *              /* param_value_size_ret */);

typedef cl_int (*EnqueueReadBuffer) (
            cl_command_queue    /* command_queue */,
            cl_mem              /* buffer */,
            cl_bool             /* blocking_read */,
            size_t              /* offset */,
            size_t              /* cb */,
            void *              /* ptr */,
            cl_uint             /* num_events_in_wait_list */,
            const cl_event *    /* event_wait_list */,
            cl_event *          /* event */);

typedef cl_int (*EnqueueWriteBuffer) (
            cl_command_queue    /* command_queue */,
            cl_mem              /* buffer */,
            cl_bool             /* blocking_read */,
            size_t              /* offset */,
            size_t              /* cb */,
            void *              /* ptr */,
            cl_uint             /* num_events_in_wait_list */,
            const cl_event *    /* event_wait_list */,
            cl_event *          /* event */);

typedef cl_int (*EnqueueUnmapMemObject) (
            cl_command_queue /* command_queue */,
            cl_mem           /* memobj */,
            void *           /* mapped_ptr */,
            cl_uint          /* num_events_in_wait_list */,
            const cl_event * /* event_wait_list */,
            cl_event *       /* event */);

typedef cl_int (*EnqueueCopyBuffer) (
            cl_command_queue    /* command_queue */,
            cl_mem              /* src_buffer */,
            cl_mem              /* dst_buffer */,
            size_t              /* src_offset */,
            size_t              /* dst_offset */,
            size_t              /* cb */,
            cl_uint             /* num_events_in_wait_list */,
            const cl_event *    /* event_wait_list */,
            cl_event *          /* event */);

typedef cl_int (*GetEventProfilingInfo) (
            cl_event            /* event */,
            cl_profiling_info   /* param_name */,
            size_t              /* param_value_size */,
            void *              /* param_value */,
            size_t *            /* param_value_size_ret */);



typedef struct ocl_wrapper {
  GetPlatformIDsProto clGetPlatformIDs;
  GetPlatformInfoProto clGetPlatformInfo;
  GetDeviceIDsProto clGetDeviceIDs;
  CreateContextProto clCreateContext;
  CreateCommandQueueProto clCreateCommandQueue;
  ReleaseContextProto clReleaseContext;
  ReleaseCommandQueueProto clReleaseCommandQueue;
  FlushProto clFlush;
  FinishProto clFinish;
  CreateBufferProto clCreateBuffer;
  CreateSubBufferProto clCreateSubBuffer;
  CopyBufferProto clCopyBuffer;
  MapBufferProto clEnqueueMapBuffer;
  UnmapBufferProto clUnmapBuffer;
  RetainBufferProto clRetainBuffer;
  ReleaseBufferProto clReleaseBuffer;
  GetBufferInfoProto clGetBufferInfo;
  ReadBufferProto clReadBuffer;
  CreateUserEventProto clCreateUserEvent;
  SetUserEventStatusProto clSetUserEventStatus;
  SetEventCallbackProto clSetEventCallback;
  WaitForEventsProto clWaitForEvents;
  RetainEventProto clRetainEvent;
  GetEventInfoProto clGetEventInfo;
  CreateProgramProto clCreateProgramWithBinary;
  CreateProgramWithSourceProto clCreateProgramWithSource;
  BuildProgramProto clBuildProgram;
  ReleaseProgramProto clReleaseProgram;
  CreateKernelProto clCreateKernel;
  ReleaseKernelProto clReleaseKernel;
  SetKernelArgProto clSetKernelArg;
  EnqueueKernelProto clEnqueueNDRangeKernel;
  GetBuildInfoProto clGetBuildInfo;
  ReleaseMemObjectProto clReleaseMemObject;
  GetContextInfo clGetContextInfo;
  GetDeviceInfo clGetDeviceInfo;
  GetProgramInfo clGetProgramInfo;
  CreateContextFromType clCreateContextFromType;
  GetCommandQueueInfo clGetCommandQueueInfo;
  GetProgramBuildInfo clGetProgramBuildInfo;
  EnqueueReadBuffer clEnqueueReadBuffer;
  EnqueueWriteBuffer clEnqueueWriteBuffer;
  EnqueueUnmapMemObject clEnqueueUnmapMemObject;
  EnqueueCopyBuffer clEnqueueCopyBuffer;
  GetEventProfilingInfo clGetEventProfilingInfo;
}OCL_WRAPPER;

typedef struct OCL_CONTEXT {
  cl_uint num_platforms;
  cl_platform_id *platforms;
  char vendor[VENDOR_INFO_SIZE];
  cl_context context;
  cl_device_id *devices;
  cl_command_queue command_queue;
}OCL_CONTEXT;

int ocl_wrapper_init(void);

int ocl_wrapper_finalize(void);

int ocl_context_init(OCL_CONTEXT *ctx, int use_gpu);

void ocl_context_fini(OCL_CONTEXT *context);

int load_source_from_file(const char* fileName,
                          char **source,
                          size_t *source_len);

cl_program create_and_build_program(const OCL_CONTEXT *ctx,
                                    int count,
                                    const char **strings,
                                    const size_t *lengths,
                                    int *err);
cl_program vp9_create_and_build_program(const OCL_CONTEXT *ctx,
                                        int count,
                                        const char **strings,
                                        const size_t *lengths,
                                        int *err);
extern OCL_WRAPPER ocl_wrapper;

#ifdef __cplusplus
}
#endif

#endif  // OCL_WRAPPER_H_

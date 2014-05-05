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
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/opencl/CL/cl_dx9_media_sharing.h"
// it's a switch to control run on Linux system.
#define RUN_PC 0

// Imagination OpenCL
#if RUN_PC
    #define OCL_LIB_FILE_IMG    "/usr/lib/libOpenCL.so"
#else
    #define OCL_LIB_FILE_IMG    "/vendor/lib/libPVROCL.so"
#endif

#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#define LOGI(...) fprintf(stdout, __VA_ARGS__)

#define CL_GET_PLATFORM_ID_SYM                        "clGetPlatformIDs"
#define CL_GET_PLATFORM_INFO_SYM                      "clGetPlatformInfo"
#define CL_CREATE_CONTEXT_SYM                         "clCreateContext"
#define CL_CREATE_CONTEXT_FROM_TYPE_SYM               "clCreateContextFromType"
#define CL_GET_CONTEXT_INFO_SYM                       "clGetContextInfo"
#define CL_GET_DEVICE_ID_SYM                          "clGetDeviceIDs"
#define CL_GET_DEVICE_INFO_SYM                        "clGetDeviceInfo"
#define CL_CREATE_COMMAND_QUEUE_SYM                   "clCreateCommandQueue"
#define CL_GET_COMMAND_QUEUE_INFO_SYM                 "clGetCommandQueueInfo"
#define CL_CREATE_PROGRAM_SYM                         "clCreateProgramWithBinary"
#define CL_CREATE_PROGRAM_SOURCE                      "clCreateProgramWithSource"
#define CL_BUILD_PROGRAM_SYM                          "clBuildProgram"
#define CL_GET_PROGRAM_INFO_SYM                       "clGetProgramInfo"
#define CL_GET_PROGRAM_BUILD_INFO_SYM                 "clGetProgramBuildInfo"
#define CL_CREATE_BUFFER_SYM                          "clCreateBuffer"
#define CL_CREATE_SUB_BUFFER_SYM                      "clCreateSubBuffer"
#define CL_COPY_BUFFER_SYM                            "clEnqueueCopyBuffer"
#define CL_READ_BUFFER_SYM                            "clEnqueueReadBuffer"
#define CL_WRITE_BUFFER_SYM                           "clEnqueueWriteBuffer"
#define CL_CREATE_ALL_KERNELS_SYM                     "clCreateKernelsInProgram"
#define CL_CREATE_KERNEL_SYM                          "clCreateKernel"
#define CL_GET_KERNEL_INFO_SYM                        "clGetKernelInfo"
#define CL_SET_KERNEL_ARG_SYM                         "clSetKernelArg"
#define CL_ENQUEUE_KERNEL_SYM                         "clEnqueueNDRangeKernel"
#define CL_CREATE_EVENT_SYM                           "clCreateUserEvent"
#define CL_SET_USER_EVENT_STATUS_SYM                  "clSetUserEventStatus"
#define CL_GET_EVENT_INFO_SYM                         "clGetEventInfo"
#define CL_SET_EVENT_CALLBACK_SYM                     "clSetEventCallback"
#define CL_WAIT_FOR_EVENTS_SYM                        "clWaitForEvents"
#define CL_RETAIN_EVENT_SYM                           "clRetainEvent"
#define CL_GET_EVENT_PROFILING_INFO_EVENT_SYM         "clGetEventProfilingInfo"
#define CL_FINISH_SYM                                 "clFinish"
#define CL_FLUSH_SYM                                  "clFlush"
#define CL_GET_BUFFER_INFO_SYM                        "clGetMemObjectInfo"
#define CL_MAP_BUFFER_SYM                             "clEnqueueMapBuffer"
#define CL_UNMAP_BUFFER_SYM                           "clEnqueueUnmapMemObject"
#define CL_RETAIN_BUFFER_SYM                          "clRetainMemObject"
#define CL_RELEASE_BUFFER_SYM                         "clReleaseMemObject"
#define CL_RELEASE_COMMAND_QUEUE_SYM                  "clReleaseCommandQueue"
#define CL_RELEASE_CONTEXT_SYM                        "clReleaseContext"
#define CL_RELEASE_KERNEL_SYM                         "clReleaseKernel"
#define CL_RELEASE_PROGRAM_SYM                        "clReleaseProgram"

OCL_WRAPPER ocl_wrapper;

static void *lib_ocl = NULL;

static inline void * load_ocl_library() {
  void * lib_handle;

  const char *ocl_lib = OCL_LIB_FILE_IMG;

  lib_handle = dlopen(ocl_lib, RTLD_NOW);
  if (!lib_handle) {
    LOGE("Failed to load OpenCL driver !\n");
    LOGE("%s", dlerror());
  }

  return lib_handle;
}

int ocl_wrapper_init(void) {
  if (lib_ocl) {
    return 1;
  }

  lib_ocl = load_ocl_library();
  if (!lib_ocl) {
    LOGE("Failed to load OpenCL library \n");
    return -1;
  }
  if (!(ocl_wrapper.clGetPlatformIDs =
            dlsym(lib_ocl, CL_GET_PLATFORM_ID_SYM))) {
    return -1;
  }
  if (!(ocl_wrapper.clGetPlatformInfo =
            dlsym(lib_ocl, CL_GET_PLATFORM_INFO_SYM))) {
    return -1;
  }

  ocl_wrapper.clCreateContext = dlsym(lib_ocl, CL_CREATE_CONTEXT_SYM);
  ocl_wrapper.clCreateContextFromType =
      dlsym(lib_ocl, CL_CREATE_CONTEXT_FROM_TYPE_SYM);
  ocl_wrapper.clGetContextInfo = dlsym(lib_ocl, CL_GET_CONTEXT_INFO_SYM);
  ocl_wrapper.clGetDeviceIDs = dlsym(lib_ocl, CL_GET_DEVICE_ID_SYM);
  ocl_wrapper.clGetDeviceInfo = dlsym(lib_ocl, CL_GET_DEVICE_INFO_SYM);
  ocl_wrapper.clCreateCommandQueue =
      dlsym(lib_ocl, CL_CREATE_COMMAND_QUEUE_SYM);
  ocl_wrapper.clGetCommandQueueInfo =
      dlsym(lib_ocl, CL_GET_COMMAND_QUEUE_INFO_SYM);
  ocl_wrapper.clCreateProgramWithBinary =
      dlsym(lib_ocl, CL_CREATE_PROGRAM_SYM);
  ocl_wrapper.clCreateProgramWithSource =
      dlsym(lib_ocl, CL_CREATE_PROGRAM_SOURCE);
  ocl_wrapper.clBuildProgram = dlsym(lib_ocl, CL_BUILD_PROGRAM_SYM);
  ocl_wrapper.clGetProgramInfo = dlsym(lib_ocl, CL_GET_PROGRAM_INFO_SYM);
  ocl_wrapper.clGetProgramBuildInfo =
      dlsym(lib_ocl, CL_GET_PROGRAM_BUILD_INFO_SYM);
  ocl_wrapper.clCreateBuffer = dlsym(lib_ocl, CL_CREATE_BUFFER_SYM);
  ocl_wrapper.clCreateSubBuffer = dlsym(lib_ocl, CL_CREATE_SUB_BUFFER_SYM);
  ocl_wrapper.clCopyBuffer = dlsym(lib_ocl, CL_COPY_BUFFER_SYM);
  ocl_wrapper.clEnqueueCopyBuffer = dlsym(lib_ocl, CL_COPY_BUFFER_SYM);
  ocl_wrapper.clEnqueueMapBuffer = dlsym(lib_ocl, CL_MAP_BUFFER_SYM);
  ocl_wrapper.clUnmapBuffer = dlsym(lib_ocl, CL_UNMAP_BUFFER_SYM);
  ocl_wrapper.clEnqueueUnmapMemObject = dlsym(lib_ocl, CL_UNMAP_BUFFER_SYM);
  ocl_wrapper.clRetainBuffer = dlsym(lib_ocl, CL_RETAIN_BUFFER_SYM);
  ocl_wrapper.clReadBuffer = dlsym(lib_ocl, CL_READ_BUFFER_SYM);
  ocl_wrapper.clEnqueueReadBuffer = dlsym(lib_ocl, CL_READ_BUFFER_SYM);
  ocl_wrapper.clEnqueueWriteBuffer = dlsym(lib_ocl, CL_WRITE_BUFFER_SYM);
  ocl_wrapper.clCreateUserEvent = dlsym(lib_ocl, CL_CREATE_EVENT_SYM);
  ocl_wrapper.clSetUserEventStatus =
      dlsym(lib_ocl, CL_SET_USER_EVENT_STATUS_SYM);
  ocl_wrapper.clSetEventCallback = dlsym(lib_ocl, CL_SET_EVENT_CALLBACK_SYM);
  ocl_wrapper.clWaitForEvents = dlsym(lib_ocl, CL_WAIT_FOR_EVENTS_SYM);
  ocl_wrapper.clRetainEvent = dlsym(lib_ocl, CL_RETAIN_EVENT_SYM);
  ocl_wrapper.clGetEventInfo = dlsym(lib_ocl, CL_GET_EVENT_INFO_SYM);
  ocl_wrapper.clCreateKernel = dlsym(lib_ocl, CL_CREATE_KERNEL_SYM);
  ocl_wrapper.clSetKernelArg = dlsym(lib_ocl, CL_SET_KERNEL_ARG_SYM);
  ocl_wrapper.clEnqueueNDRangeKernel = dlsym(lib_ocl, CL_ENQUEUE_KERNEL_SYM);
  ocl_wrapper.clGetEventProfilingInfo =
      dlsym(lib_ocl, CL_GET_EVENT_PROFILING_INFO_EVENT_SYM);
  ocl_wrapper.clFlush = dlsym(lib_ocl, CL_FLUSH_SYM);
  ocl_wrapper.clFinish = dlsym(lib_ocl, CL_FINISH_SYM);
  ocl_wrapper.clReleaseContext = dlsym(lib_ocl, CL_RELEASE_CONTEXT_SYM);
  ocl_wrapper.clReleaseCommandQueue =
      dlsym(lib_ocl, CL_RELEASE_COMMAND_QUEUE_SYM);
  ocl_wrapper.clReleaseProgram = dlsym(lib_ocl, CL_RELEASE_PROGRAM_SYM);
  ocl_wrapper.clReleaseKernel = dlsym(lib_ocl, CL_RELEASE_KERNEL_SYM);
  ocl_wrapper.clReleaseBuffer = dlsym(lib_ocl, CL_RELEASE_BUFFER_SYM);
  ocl_wrapper.clReleaseMemObject = dlsym(lib_ocl, CL_RELEASE_BUFFER_SYM);

  return 0;
}

int ocl_wrapper_finalize(void) {
  if (lib_ocl) {
    dlclose(lib_ocl);
    lib_ocl = NULL;
    return 1;
  }

  return 0;
}

int ocl_context_init(OCL_CONTEXT *ctx, int use_gpu) {
  cl_int status = 0;
  size_t devices_byte_size;

  cl_context_properties cps[3];
  cl_context_properties *cprops;

  status = ocl_wrapper.clGetPlatformIDs(0, NULL, &ctx->num_platforms);
  if (CL_SUCCESS != status) {
    LOGE("Fail to getting platforms number, error : %d\n", status);
    return -1;
  }
  if (ctx->num_platforms <= 0) {
    LOGE("There is no OpenCL platform \n");
    return -1;
  }

  ctx->platforms =
      (cl_platform_id *) malloc( ctx->num_platforms * sizeof(cl_platform_id));

  status = ocl_wrapper.clGetPlatformIDs(
               ctx->num_platforms,
               ctx->platforms, NULL);
  if (CL_SUCCESS != status) {
    LOGE("Fail to getting platforms, error: %d\n", status);
    free(ctx->platforms);
    return -1;
  }

  status = ocl_wrapper.clGetPlatformInfo(
               ctx->platforms[0],
               CL_PLATFORM_VENDOR,
               VENDOR_INFO_SIZE,
               ctx->vendor, NULL);

  cps[0] = CL_CONTEXT_PLATFORM;
  cps[1] = (cl_context_properties) ctx->platforms[0],
  cps[2] = 0;
  cprops = cps;

  if (use_gpu) {
    ctx->context = ocl_wrapper.clCreateContextFromType(
                       cprops, CL_DEVICE_TYPE_GPU,
                       NULL, NULL, &status);
  } else {
    ctx->context = ocl_wrapper.clCreateContextFromType(
                       cprops, CL_DEVICE_TYPE_CPU,
                       NULL, NULL, &status);
  }
  if (status != CL_SUCCESS) {
    LOGE("Fail to get context: error %d\n", status);
    free(ctx->platforms);
    return -1;
  }

  status = ocl_wrapper.clGetContextInfo(
               ctx->context, CL_CONTEXT_DEVICES,
               0, NULL, &devices_byte_size);

  ctx->devices = (cl_device_id *) malloc(devices_byte_size);

  status = ocl_wrapper.clGetContextInfo(
               ctx->context,
               CL_CONTEXT_DEVICES,
               devices_byte_size,
               ctx->devices, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Fail to get devices, error: %d\n", status);
    free(ctx->devices);
    ocl_wrapper.clReleaseContext(ctx->context);
    free(ctx->platforms);
    return -1;
  }

  ctx->command_queue = ocl_wrapper.clCreateCommandQueue(
                           ctx->context,
                           ctx->devices[0],
                           0, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateCommandQueue, error: %d\n", status);
    free(ctx->devices);
    ocl_wrapper.clReleaseContext(ctx->context);
    free(ctx->platforms);
    return -1;
  }

  return 0;
}

void ocl_context_fini(OCL_CONTEXT *ctx) {
  cl_int status = CL_SUCCESS;

  if (ctx->command_queue) {
    status = ocl_wrapper.clReleaseCommandQueue(ctx->command_queue);
    if (status != CL_SUCCESS) {
      LOGE("Failed to release command queue, error: %d\n", status);
    }
  }
  if (ctx->devices)
    free(ctx->devices);
  if (ctx->context){
    status = ocl_wrapper.clReleaseContext(ctx->context);
    if (status != CL_SUCCESS) {
      LOGE("Failed to release cl_conext, error: %d\n", status);
    }
  }
  if (ctx->platforms)
    free(ctx->platforms);
}

int load_source_from_file(const char* fileName,
                          char **source,
                          size_t *source_len) {
  int err;
  int size;
  FILE *fp = NULL;
#if RUN_PC
  fp = fopen( "vp9kernel.bin", "rb" );
#else
  fp = fopen( "/sdcard/vp9kernel.bin", "rb" );
#endif
  if ( !fp ) {
    LOGE( "Can't open the bin file: /sdcard/vp9kernel.bin \n" );
  } else {
    fclose( fp );
    return 0;
  }

  fp = fopen(fileName, "r");
  if ( !fp ) {
    LOGE("Can't open file: %s\n", fileName);
    return -1;
  }
  err = fseek(fp, 0, SEEK_END);
  if (err) {
    LOGE("Failed to seek EOF of: %s\n", fileName);
    fclose(fp);
    return -1;
  }
  *source_len = ftell(fp);
  err = fseek(fp, 0, SEEK_SET);
  if (err) {
    LOGE("Failed to seek BEGIN of: %s\n", fileName);
    fclose(fp);
    return -1;
  }
  *source = (char*) malloc(*source_len + 1);
  if (*source == NULL) {
    LOGE("Out of memory!\n");
    fclose(fp);
    return -1;
  }
  size = fread(*source, 1, *source_len, fp);
  if (size != *source_len) {
    LOGE("Failed to read source code! source_len: %d, read_size: %d\n",
          *source_len, size);
    free(*source);
    *source = NULL;
    fclose(fp);
    return -1;
  }
  err = fclose(fp);
  if (err) {
    LOGE("Failed to close file: %s\n", fileName);
    free(*source);
    *source = NULL;
    return -1;
  }
  (*source)[size] = '\0';
  return 0;
}

int binary_generated(const OCL_CONTEXT *ctx, FILE **fhandle) {
  FILE *fd = NULL;
  cl_uint numDevices = 0;
  int clStatus = 0, i = 0;

  clStatus = ocl_wrapper.clGetDeviceIDs(ctx->platforms[0],
                              CL_DEVICE_TYPE_GPU,
                              0,
                              NULL,
                              &numDevices);

  CHECK_OPENCL(clStatus, "clGetDeviceIDs");
  for (i = 0; i < numDevices; i++) {
    char fileName[256] = {0};
    if (ctx->devices[i] != 0) {
      char deviceName[1024];
      clStatus = ocl_wrapper.clGetDeviceInfo(ctx->devices[i],
                                CL_DEVICE_NAME,
                                sizeof(deviceName),
                                deviceName,
                                NULL);
      CHECK_OPENCL(clStatus, "clGetDeviceInfo");
#if RUN_PC
      sprintf(fileName, "vp9kernel.bin");
#else
      sprintf(fileName, "/sdcard/vp9kernel.bin");
#endif
      fd = fopen(fileName, "rb");
      clStatus = (fd != NULL) ? 1 : 0;
    }
  }
  if (fd != NULL) {
    *fhandle = fd;
  }
  LOGE("In binary_generated, status is %d\n", clStatus);
  return clStatus;
}

int write_binary_to_file(const char *fileName,
                         const char *birary,
                         size_t numBytes) {
  FILE *output = NULL;
  output = fopen(fileName, "wb");
  if (output == NULL) {
    return 0;
  }

  fwrite(birary, sizeof(char), numBytes, output);
  fclose(output);

  return 1;
}

int generate_bin_from_kernel_source(cl_program program) {
  unsigned int i = 0;
  cl_int clStatus = 0;
  size_t *binarySizes = NULL, numDevices = 0;
  cl_device_id *clDevsID = NULL;
  char **binaries = NULL;

  clStatus = ocl_wrapper.clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES,
                 sizeof(numDevices), &numDevices, NULL);
  CHECK_OPENCL(clStatus, "clGetProgramInfo");

  clDevsID = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
  if (clDevsID == NULL) {
    return 0;
  }
  // Get the context information.
  clStatus = ocl_wrapper.clGetProgramInfo(program, CL_PROGRAM_DEVICES,
                 sizeof(cl_device_id) * numDevices, clDevsID, NULL);
  CHECK_OPENCL(clStatus, "clGetProgramInfo");

  // Figure out the sizes of each of the binaries.
  binarySizes = (size_t*)malloc(sizeof(size_t) * numDevices);

  clStatus = ocl_wrapper.clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES,
                 sizeof(size_t) * numDevices, binarySizes, NULL);
  CHECK_OPENCL( clStatus, "clGetProgramInfo" );

  // Copy over all of the generated binaries.
  binaries = (char**)malloc(sizeof(char *) * numDevices);
  if (binaries == NULL) {
    return 0;
  }

  for (i = 0; i < numDevices; i++) {
    if (binarySizes[i] != 0) {
      binaries[i] = (char*)malloc(sizeof(char) * binarySizes[i]);
      if (binaries[i] == NULL) {
        return 0;
      }
    } else {
      binaries[i] = NULL;
    }
  }

  clStatus = ocl_wrapper.clGetProgramInfo(program, CL_PROGRAM_BINARIES,
                 sizeof(char *) * numDevices, binaries, NULL);
  CHECK_OPENCL(clStatus,"clGetProgramInfo");

  /* dump out each binary into its own separate file. */
  for (i = 0; i < numDevices; i++)
  {
    char fileName[256] = {0};

    if (binarySizes[i] != 0) {
      char deviceName[1024];
      clStatus = ocl_wrapper.clGetDeviceInfo(clDevsID[i], CL_DEVICE_NAME,
                     sizeof(deviceName), deviceName, NULL);
      CHECK_OPENCL(clStatus, "clGetDeviceInfo");
#if RUN_PC
      sprintf( fileName, "vp9kernel.bin");
#else
      sprintf( fileName, "/sdcard/vp9kernel.bin");
#endif
      if (!write_binary_to_file(fileName, binaries[i], binarySizes[i])) {
        LOGE("vp9: write binary [%s] failds\n", fileName);
        return 0;
      }
      LOGI("vp9: write binary [%s] succesfully\n", fileName);
    }
  }

  // Release all resouces and memory
  for (i = 0; i < numDevices; i++) {
    CHECK_CPU_FREE(binaries[i]);
  }

  CHECK_CPU_FREE(binaries);
  CHECK_CPU_FREE(binarySizes);
  CHECK_CPU_FREE(clDevsID);

  return 1;
}

cl_program vp9_create_and_build_program(const OCL_CONTEXT *ctx, int count,
               const char **strings, const size_t *lengths, int *err) {
  char *binary = NULL;
  size_t numDevices = 0, length = 0, source_size = 0;
  int b_error = 0, binary_status, binaryExisted = 0;
  cl_device_id *clDevsID;
  cl_program program;
  FILE *fd;

#ifdef USE_CL_FILE // use cl file
  const char * cpTotalKernelFile = *strings;
  source_size = *lengths;
#else // use string
  uint nStrSize = strlen(kernel_src);
  char *cpTotalKernelFile = (char *)malloc(nStrSize * sizeof(char) + 1);
  memcpy(cpTotalKernelFile, kernel_src, nStrSize);
  cpTotalKernelFile[nStrSize]='\0';
  source_size = strlen(cpTotalKernelFile);
  count = 1;
#endif
  LOGI("compileKernelFile ... \n");
  if ((binaryExisted = binary_generated(ctx, &fd)) == 1) {
    *err = ocl_wrapper.clGetContextInfo(ctx->context,
                             CL_CONTEXT_NUM_DEVICES,
                             sizeof(numDevices),
                             &numDevices,
                             NULL);
    CHECK_OPENCL(*err, "clGetContextInfo");

    clDevsID = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
    if (clDevsID == NULL) {
      return 0;
    }

    b_error = 0;
    length = 0;
    b_error |= fseek( fd, 0, SEEK_END ) < 0;
    b_error |= ( length = ftell(fd) ) <= 0;
    b_error |= fseek( fd, 0, SEEK_SET ) < 0;
    if (b_error) {
      return 0;
    }

    binary = (char*)malloc(length + 2);
    if (!binary) {
      return 0;
    }

    memset(binary, 0, length + 2);
    b_error |= fread(binary, 1, length, fd) != length;

    fclose(fd);
    fd = NULL;
    // Get the context information
    *err = ocl_wrapper.clGetContextInfo(ctx->context,
                             CL_CONTEXT_DEVICES,
                             sizeof(cl_device_id) * numDevices,
                             clDevsID,
                             NULL);
    CHECK_OPENCL(*err, "clGetContextInfo");

    LOGE("Create kernel from binary\n");
    program = ocl_wrapper.clCreateProgramWithBinary(ctx->context,
                numDevices, clDevsID, &length,
                (const unsigned char**) &binary, &binary_status, err);
    CHECK_OPENCL(*err, "clCreateProgramWithBinary");

    CHECK_CPU_FREE(binary);
    CHECK_CPU_FREE(clDevsID);
  } else {
    // create a CL program using the kernel source
    LOGE( "Create kernel from source\n" );
    program = ocl_wrapper.clCreateProgramWithSource(ctx->context,
                                                    count,
                                                    &cpTotalKernelFile,
                                                    &source_size,
                                                    err);
    CHECK_OPENCL(*err, "clCreateProgramWithSource");
  }

  if (program == (cl_program)NULL) {
    return 0;
  }

  // create a cl program executable for all the devices specified
  LOGE( "BuildProgram...\n" );
  *err = ocl_wrapper.clBuildProgram(program,
                                    1,
                                    ctx->devices,
                                    "-cl-mad-enable",//buildOption
                                    NULL,
                                    NULL);
  if (*err != CL_SUCCESS) {
    LOGE("Fail to build program, error: %d \n",*err);
    if (*err == CL_BUILD_PROGRAM_FAILURE) {
      char * build_log = NULL;
      size_t build_log_size = 0;
      cl_int logState = ocl_wrapper.clGetProgramBuildInfo(program,
                                           ctx->devices[0],
                                           CL_PROGRAM_BUILD_LOG,
                                           build_log_size,
                                           NULL,
                                           &build_log_size);
      if (logState != CL_SUCCESS) {
        LOGE("Fail to get build log, error: %d\n", *err);
        *err = -1;
        return 0;
      }
      build_log = (char *)malloc(build_log_size);
      memset(build_log, '0', build_log_size);
      logState = ocl_wrapper.clGetProgramBuildInfo(program, ctx->devices[0],
                     CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
      LOGI(" \n\t\t\tBUILD LOG\n");
      LOGE(" ************************************************\n");
      LOGI("%s\n", build_log);
      LOGE(" ************************************************\n");
      free(build_log);
    }
    *err = -1;
    return 0;
  }

  if (binaryExisted == 0) {
      generate_bin_from_kernel_source(program);
  }

  return program;
}

cl_program create_and_build_program(const OCL_CONTEXT *ctx,
                                    int count,
                                    const char **strings,
                                    const size_t *lengths,
                                    int *err) {
  cl_int status = CL_SUCCESS;
  const char *opt = "-cl-mad-enable";//"-cl-fast-relaxed-math -cl-mad-enable";

  cl_program program = ocl_wrapper.clCreateProgramWithSource(
                           ctx->context, count,
                           strings, lengths, &status);
  if (status != CL_SUCCESS) {
    LOGE("There is some error in create program\n");
    *err = -1;
    return 0;
  }

  status = ocl_wrapper.clBuildProgram(
               program, 1, ctx->devices,
               opt, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Fail to build program, error: %d \n",status);

    if (status == CL_BUILD_PROGRAM_FAILURE) {
      char * build_log = NULL;
      size_t build_log_size = 0;

      cl_int logState = ocl_wrapper.clGetProgramBuildInfo(
                            program, ctx->devices[0],
                            CL_PROGRAM_BUILD_LOG,
                            build_log_size, NULL,
                            &build_log_size);
      if (logState != CL_SUCCESS) {
        LOGE("Fail to get build log, error: %d\n", status);
        *err = -1;
        return 0;
      }

      build_log = (char *) malloc(build_log_size);
      memset(build_log, '0', build_log_size);

      logState = ocl_wrapper.clGetProgramBuildInfo(
                     program, ctx->devices[0],
                     CL_PROGRAM_BUILD_LOG,
                     build_log_size,
                     build_log, NULL);

      LOGI(" \n\t\t\tBUILD LOG\n");
      LOGE(" ************************************************\n");
      LOGI("%s\n", build_log);
      LOGE(" ************************************************\n");

      free(build_log);
    }

    *err = -1;
    return 0;
  }

  *err = 0;

  return program;
}

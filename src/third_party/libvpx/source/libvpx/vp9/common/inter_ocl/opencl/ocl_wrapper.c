#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vp9/common/inter_ocl/opencl/ocl_wrapper.h"
#include "vp9/common/inter_ocl/opencl/CL/cl_dx9_media_sharing.h"
#include <d3d9.h>
#include "vpx/vpx_integer.h"

#define LOGE(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#define LOGI(...) fprintf(stdout, __VA_ARGS__);fflush(stdout)

#define CLEW_CHECK_FUNCTION(f)              \
    if ((f) == NULL)                        \
    {                                       \
        printf("can not get function address\n");\
        return CLEW_ERROR_IMPORT_FAILED;    \
    }                                       \

//add for D3D9 and OpenCl interOp
PFNCLGETDEVICEIDSFROMDX9MEDIAADAPTERKHR    __clewGetDeviceIDsFromDX9MediaAdapterKHR   = NULL;
PFNCLCREATEFROMDX9MEDIASURFACEKHR    __clewCreateFromDX9MediaSurfaceKHR   = NULL;
PFNCLENQUEUEACQUIREDX9MEDIASURFACESKHR   __clewEnqueueAcquireDX9MediaSurfacesKHR = NULL;
PFNCLENQUEUERELEASEDX9MEDIASURFACESKHR   __clewEnqueueReleaseDX9MediaSurfacesKHR =  NULL;
int ocl_wrapper_init(void) {
  return clewInit("OpenCL.dll");
}

int ocl_wrapper_finalize(void) {
  return 0;
}

int ocl_context_init(OCL_CONTEXT *ctx, int use_gpu) {
  cl_int status = 0;
  size_t devices_byte_size;

  cl_context_properties cps[3];
  cl_context_properties *cprops;

  status = clGetPlatformIDs(0, NULL, &ctx->num_platforms);
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

  status = clGetPlatformIDs(
               ctx->num_platforms,
               ctx->platforms, NULL);
  if (CL_SUCCESS != status) {
    LOGE("Fail to getting platforms, error: %d\n", status);
    free(ctx->platforms);
    return -1;
  }
 
  //end
 
  status = clGetPlatformInfo(
               ctx->platforms[0],
               CL_PLATFORM_VENDOR,
               VENDOR_INFO_SIZE,
               ctx->vendor, NULL);

  cps[0] = CL_CONTEXT_PLATFORM;
  cps[1] = (cl_context_properties) ctx->platforms[0],
  cps[2] = 0;
  cprops = cps;

  if (use_gpu) {
    ctx->context = clCreateContextFromType(
                       cprops, CL_DEVICE_TYPE_GPU,
                       NULL, NULL, &status);
  } else {
    ctx->context = clCreateContextFromType(
                       cprops, CL_DEVICE_TYPE_CPU,
                       NULL, NULL, &status);
  }
  if (status != CL_SUCCESS) {
    LOGE("Fail to get context: error %d\n", status);
    free(ctx->platforms);
    return -1;
  }

  status = clGetContextInfo(
               ctx->context, CL_CONTEXT_DEVICES,
               0, NULL, &devices_byte_size);

  ctx->devices = (cl_device_id *) malloc(devices_byte_size);

  status = clGetContextInfo(
               ctx->context,
               CL_CONTEXT_DEVICES,
               devices_byte_size,
               ctx->devices, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Fail to get devices, error: %d\n", status);
    free(ctx->devices);
    clReleaseContext(ctx->context);
    free(ctx->platforms);
    return -1;
  }

  ctx->command_queue = clCreateCommandQueue(
                           ctx->context,
                           ctx->devices[0],
                           0, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateCommandQueue, error: %d\n", status);
    free(ctx->devices);
    clReleaseContext(ctx->context);
    free(ctx->platforms);
    return -1;
  }

  return 0;
}

//add function for D3D9 and OpenCL interOp
int ocl_context_init_for_d3d9_interOp(OCL_CONTEXT *ctx, Interop_Context *interop_context, int use_gpu) {
  cl_int status = 0;
  cl_uint numDevices;
  LPDIRECT3DDEVICE9EX id3d9_device;

  cl_dx9_media_adapter_type_khr tpAdapter = CL_ADAPTER_D3D9EX_KHR;
  // Number of OpenCL devices corresponding to the Direct3D 9 object
  cl_context_properties cps[7];
  id3d9_device = (LPDIRECT3DDEVICE9EX)(interop_context->pDevice);

  status = clGetPlatformIDs(0, NULL, &ctx->num_platforms);
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

  status = clGetPlatformIDs(
               ctx->num_platforms,
               ctx->platforms, NULL);
  if (CL_SUCCESS != status) {
    LOGE("Fail to getting platforms, error: %d\n", status);
    free(ctx->platforms);
    return -1;
  }
  // get interop function address
  CLEW_CHECK_FUNCTION(__clewGetDeviceIDsFromDX9MediaAdapterKHR   = (PFNCLGETDEVICEIDSFROMDX9MEDIAADAPTERKHR )clGetExtensionFunctionAddressForPlatform(ctx->platforms[0], "clGetDeviceIDsFromDX9MediaAdapterKHR"));
  CLEW_CHECK_FUNCTION(__clewCreateFromDX9MediaSurfaceKHR   = (PFNCLCREATEFROMDX9MEDIASURFACEKHR )clGetExtensionFunctionAddressForPlatform(ctx->platforms[0], "clCreateFromDX9MediaSurfaceKHR"));
  CLEW_CHECK_FUNCTION(__clewEnqueueAcquireDX9MediaSurfacesKHR   = (PFNCLENQUEUEACQUIREDX9MEDIASURFACESKHR )clGetExtensionFunctionAddressForPlatform(ctx->platforms[0], "clEnqueueAcquireDX9MediaSurfacesKHR"));
  CLEW_CHECK_FUNCTION(__clewEnqueueReleaseDX9MediaSurfacesKHR   = (PFNCLENQUEUERELEASEDX9MEDIASURFACESKHR )clGetExtensionFunctionAddressForPlatform(ctx->platforms[0], "clEnqueueReleaseDX9MediaSurfacesKHR"));

  // get opencl devices from d3d9 device
  status = clGetDeviceIDsFromDX9MediaAdapterKHR(ctx->platforms[0], 1, 
                                                                                 &tpAdapter, (void *)id3d9_device, 
                                                                                 CL_PREFERRED_DEVICES_FOR_DX9_MEDIA_ADAPTER_KHR, 
                                                                                 0, NULL, &numDevices);
  if (status != CL_SUCCESS || numDevices == 0)
  {
    LOGE("Faild to Init D3D9 Device\n");
    free(ctx->platforms);
    return -1;
  }

  ctx->devices = (cl_device_id *) malloc(numDevices * sizeof(cl_device_id));
  //deviceID = ( deviceID > (numDevices-1)) ? (numDevices-1):deviceID;
  status = clGetDeviceIDsFromDX9MediaAdapterKHR(ctx->platforms[0], 1, &tpAdapter, 
              (void *)id3d9_device, CL_PREFERRED_DEVICES_FOR_DX9_MEDIA_ADAPTER_KHR, 
              numDevices,  ctx->devices, NULL);
  if(CL_SUCCESS != status) {
    LOGE("faile to get devices ids from D3D device\n");
    free(ctx->devices);
    free(ctx->platforms);
    return -1;
  }

  cps[0] = CL_CONTEXT_PLATFORM;
  cps[1] = (cl_context_properties)ctx->platforms[0];
  cps[2] = CL_CONTEXT_ADAPTER_D3D9EX_KHR;
  cps[3] = (int64_t)id3d9_device;
  cps[4] = CL_CONTEXT_INTEROP_USER_SYNC;
  cps[5] = CL_TRUE;
  cps[6] = 0;

 
  // create opencl context
  ctx->context = clCreateContext(cps, 1, &ctx->devices[0], 0, 0, &status);
  if (status != CL_SUCCESS)
  {
    LOGE("Failed to create context\n");
    free(ctx->devices);
    free(ctx->platforms);
    return -1;  
  }

  // create opencl command queue
  ctx->command_queue = clCreateCommandQueue(
                           ctx->context,
                           ctx->devices[0],
                           0, &status);
  if (status != CL_SUCCESS) {
    LOGE("Failed to clCreateCommandQueue, error: %d\n", status);
    free(ctx->devices);
    clReleaseContext(ctx->context);
    free(ctx->platforms);
    return -1;
  }
  return 0;
}

void ocl_context_fini(OCL_CONTEXT *ctx) {
  cl_int status = CL_SUCCESS;

  if (ctx->command_queue) {
    status = clReleaseCommandQueue(ctx->command_queue);
    if (status != CL_SUCCESS) {
      LOGE("Failed to release command queue, error: %d\n", status);
    }
  }
  if (ctx->devices)
    free(ctx->devices);
  if (ctx->context){
    status = clReleaseContext(ctx->context);
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
  FILE *fp = fopen(fileName, "rb");

  if (fp == NULL) {
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
#if 0
  //if (size != *source_len) {
  if (0) {
    LOGE("Failed to read source code! source_len: %d, read_size: %d\n",
          *source_len, size);
    free(*source);
    *source = NULL;
    fclose(fp);
    return -1;
  }
#endif
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

cl_program create_and_build_program(const OCL_CONTEXT *ctx,
                                    int count,
                                    const char **strings,
                                    const size_t *lengths,
                                    int *err) {
  cl_int status = CL_SUCCESS;
  const char *opt = "-cl-fast-relaxed-math -cl-mad-enable";

  cl_program program = clCreateProgramWithSource(
                           ctx->context, count,
                           strings, lengths, &status);
  if (status != CL_SUCCESS) {
    LOGE("There is some error in create program\n");
    *err = -1;
    return 0;
  }
  status = clBuildProgram(
               program, 1, ctx->devices,
               opt, NULL, NULL);
  if (status != CL_SUCCESS) {
    LOGE("Fail to build program, error: %d \n",status);

    if (status == CL_BUILD_PROGRAM_FAILURE) {
      char * build_log = NULL;
      size_t build_log_size = 0;

      cl_int logState = clGetProgramBuildInfo(
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

      logState = clGetProgramBuildInfo(
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

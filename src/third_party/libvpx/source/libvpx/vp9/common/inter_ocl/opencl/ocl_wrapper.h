#ifndef OCL_WRAPPER_H_
#define OCL_WRAPPER_H_

#include "vp9/common/inter_ocl/opencl/clew.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VENDOR_INFO_SIZE 100
#define D3D9_INTEROP 1

typedef struct OCL_CONTEXT {
  cl_uint num_platforms;
  cl_platform_id *platforms;
  char vendor[VENDOR_INFO_SIZE];
  cl_context context;
  cl_device_id *devices;
  cl_command_queue command_queue;
}OCL_CONTEXT;

typedef struct Interop_Context {
  void* pSharedHandle; //HANDLE
  void* pDevice;       //LPDIRECT3DDEVICE9EX
  void* pSurface;      //LPDIRECT3DSURFACE9
}Interop_Context;

 struct IDirect3DDevice9Ex;

int ocl_wrapper_init(void);

int ocl_wrapper_finalize(void);

int ocl_context_init(OCL_CONTEXT *ctx, int use_gpu);

int ocl_context_init_for_d3d9_interOp(OCL_CONTEXT *ctx, Interop_Context *interop_context, int use_gpu);

void ocl_context_fini(OCL_CONTEXT *context);

int load_source_from_file(const char* fileName,
                          char **source,
                          size_t *source_len);

cl_program create_and_build_program(const OCL_CONTEXT *ctx,
                                    int count,
                                    const char **strings,
                                    const size_t *lengths,
                                    int *err);


#ifdef __cplusplus
}
#endif

#endif  // OCL_WRAPPER_H_

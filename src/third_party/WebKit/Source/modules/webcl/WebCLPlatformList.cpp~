/*
* Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
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

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLPlatformList.h"
#define  __WEBCL_CL_INIT_MAIN__
#define WEBCLEXPORT
#include "WebCL.h"
#define WEBCLEXPORT
#undef   __WEBCL_CL_INIT_MAIN__

content::GpuChannelHost* webcl_channel_ = NULL;
void setWebCLChannelHost(content::GpuChannelHost* channel_webcl)
{
  webcl_channel_ = channel_webcl;
}

//void setWebCLclGetPlatformIDs(h_clGetPlatformIDs input){webcl_clGetPlatformIDs = input;}

#define WEBCL_LOAD_FUNCTION(func)                                            \
  void setWebCL##func(h_##func input) {                                      \
    webcl_##func = input;                                                    \
  }

WEBCL_LOAD_FUNCTION(clGetPlatformIDs                 )
WEBCL_LOAD_FUNCTION(clGetPlatformInfo                )
WEBCL_LOAD_FUNCTION(clGetDeviceIDs                   )
WEBCL_LOAD_FUNCTION(clGetDeviceInfo                  )
WEBCL_LOAD_FUNCTION(clCreateSubDevices               )
WEBCL_LOAD_FUNCTION(clRetainDevice                   )
WEBCL_LOAD_FUNCTION(clReleaseDevice                  )
WEBCL_LOAD_FUNCTION(clCreateContext                  )
WEBCL_LOAD_FUNCTION(clCreateContextFromType          )
WEBCL_LOAD_FUNCTION(clRetainContext                  )
WEBCL_LOAD_FUNCTION(clReleaseContext                 )
WEBCL_LOAD_FUNCTION(clGetContextInfo                 )
WEBCL_LOAD_FUNCTION(clCreateCommandQueue             )
WEBCL_LOAD_FUNCTION(clRetainCommandQueue             )
WEBCL_LOAD_FUNCTION(clReleaseCommandQueue            )
WEBCL_LOAD_FUNCTION(clGetCommandQueueInfo            )
WEBCL_LOAD_FUNCTION(clCreateBuffer                   )
WEBCL_LOAD_FUNCTION(clCreateSubBuffer                )
WEBCL_LOAD_FUNCTION(clCreateImage                    )
WEBCL_LOAD_FUNCTION(clRetainMemObject                )
WEBCL_LOAD_FUNCTION(clReleaseMemObject               )
WEBCL_LOAD_FUNCTION(clGetSupportedImageFormats       )
WEBCL_LOAD_FUNCTION(clGetMemObjectInfo               )
WEBCL_LOAD_FUNCTION(clGetImageInfo                   )
WEBCL_LOAD_FUNCTION(clSetMemObjectDestructorCallback )
WEBCL_LOAD_FUNCTION(clCreateSampler                  )
WEBCL_LOAD_FUNCTION(clRetainSampler                  )
WEBCL_LOAD_FUNCTION(clReleaseSampler                 )
WEBCL_LOAD_FUNCTION(clGetSamplerInfo                 )
WEBCL_LOAD_FUNCTION(clCreateProgramWithSource        )
WEBCL_LOAD_FUNCTION(clCreateProgramWithBinary        )
WEBCL_LOAD_FUNCTION(clCreateProgramWithBuiltInKernels)
WEBCL_LOAD_FUNCTION(clRetainProgram                  )
WEBCL_LOAD_FUNCTION(clReleaseProgram                 )
WEBCL_LOAD_FUNCTION(clBuildProgram                   )
WEBCL_LOAD_FUNCTION(clCompileProgram                 )
WEBCL_LOAD_FUNCTION(clLinkProgram                    )
WEBCL_LOAD_FUNCTION(clUnloadPlatformCompiler         )
WEBCL_LOAD_FUNCTION(clGetProgramInfo                 )
WEBCL_LOAD_FUNCTION(clGetProgramBuildInfo            )
WEBCL_LOAD_FUNCTION(clCreateKernel                   )
WEBCL_LOAD_FUNCTION(clCreateKernelsInProgram         )
WEBCL_LOAD_FUNCTION(clRetainKernel                   )
WEBCL_LOAD_FUNCTION(clReleaseKernel                  )
WEBCL_LOAD_FUNCTION(clSetKernelArg                   )
WEBCL_LOAD_FUNCTION(clGetKernelInfo                  )
WEBCL_LOAD_FUNCTION(clGetKernelArgInfo               )
WEBCL_LOAD_FUNCTION(clGetKernelWorkGroupInfo         )
WEBCL_LOAD_FUNCTION(clWaitForEvents                  )
WEBCL_LOAD_FUNCTION(clGetEventInfo                   )
WEBCL_LOAD_FUNCTION(clCreateUserEvent                )
WEBCL_LOAD_FUNCTION(clRetainEvent                    )
WEBCL_LOAD_FUNCTION(clReleaseEvent                   )
WEBCL_LOAD_FUNCTION(clSetUserEventStatus             )
WEBCL_LOAD_FUNCTION(clSetEventCallback               )
WEBCL_LOAD_FUNCTION(clGetEventProfilingInfo          )
WEBCL_LOAD_FUNCTION(clFlush                          )
WEBCL_LOAD_FUNCTION(clFinish                         )
WEBCL_LOAD_FUNCTION(clEnqueueReadBuffer              )
WEBCL_LOAD_FUNCTION(clEnqueueReadBufferRect          )
WEBCL_LOAD_FUNCTION(clEnqueueWriteBuffer             )
WEBCL_LOAD_FUNCTION(clEnqueueWriteBufferRect         )
WEBCL_LOAD_FUNCTION(clEnqueueFillBuffer              )
WEBCL_LOAD_FUNCTION(clEnqueueCopyBuffer              )
WEBCL_LOAD_FUNCTION(clEnqueueCopyBufferRect          )
WEBCL_LOAD_FUNCTION(clEnqueueReadImage               )
WEBCL_LOAD_FUNCTION(clEnqueueWriteImage              )
WEBCL_LOAD_FUNCTION(clEnqueueFillImage               )
WEBCL_LOAD_FUNCTION(clEnqueueCopyImage               )
WEBCL_LOAD_FUNCTION(clEnqueueCopyImageToBuffer       )
WEBCL_LOAD_FUNCTION(clEnqueueCopyBufferToImage       )
WEBCL_LOAD_FUNCTION(clEnqueueMapBuffer               )
WEBCL_LOAD_FUNCTION(clEnqueueMapImage                )
WEBCL_LOAD_FUNCTION(clEnqueueUnmapMemObject          )
WEBCL_LOAD_FUNCTION(clEnqueueMigrateMemObjects       )
WEBCL_LOAD_FUNCTION(clEnqueueNDRangeKernel           )
WEBCL_LOAD_FUNCTION(clEnqueueTask                    )
WEBCL_LOAD_FUNCTION(clEnqueueNativeKernel            )
WEBCL_LOAD_FUNCTION(clEnqueueMarkerWithWaitList      )
WEBCL_LOAD_FUNCTION(clEnqueueBarrierWithWaitList     )
WEBCL_LOAD_FUNCTION(clSetPrintfCallback              )

namespace WebCore {


WebCLPlatformList::WebCLPlatformList()
{
}

WebCLPlatformList::~WebCLPlatformList()
{
}

PassRefPtr<WebCLPlatformList> WebCLPlatformList::create(WebCL* ctx)
{
	
	return adoptRef(new WebCLPlatformList(ctx));
}

WebCLPlatformList::WebCLPlatformList(WebCL* ctx) : m_context(ctx)
{
	
	cl_int err = 0;
	
	err = webcl_clGetPlatformIDs(webcl_channel_, 0, NULL, &m_num_platforms);
	if (err != CL_SUCCESS) {
		// TODO (siba samal) Error handling
	}
	
	m_cl_platforms = new cl_platform_id[m_num_platforms];
	err = webcl_clGetPlatformIDs(webcl_channel_, m_num_platforms, m_cl_platforms, NULL);
	if (err != CL_SUCCESS) {
		// TODO (siba samal) Error handling
	}
	
	for (unsigned int i = 0 ; i < m_num_platforms; i++) {
		RefPtr<WebCLPlatform> o = WebCLPlatform::create(m_context, m_cl_platforms[i]);
		if (o != NULL) {
			m_platform_id_list.append(o);
		} else {
			// TODO (siba samal) Error handling
		}
	}
	
}

cl_platform_id WebCLPlatformList::getCLPlatforms()
{
	return *m_cl_platforms;
}

unsigned WebCLPlatformList::length() const
{
	return m_num_platforms;
}

WebCLPlatform* WebCLPlatformList::item(unsigned index)
{
	if (index >= m_num_platforms) {
		return 0;
	}
	WebCLPlatform* ret = (m_platform_id_list[index]).get();
	return ret;
}


} // namespace WebCore

#endif // ENABLE(WEBCL)

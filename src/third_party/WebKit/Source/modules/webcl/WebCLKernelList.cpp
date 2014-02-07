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

#include "WebCLKernelList.h"
#include "WebCL.h"
#include "WebCLException.h"

namespace WebCore {

WebCLKernelList::WebCLKernelList()
{
}

WebCLKernelList::~WebCLKernelList()
{
}

PassRefPtr<WebCLKernelList> WebCLKernelList::create(WebCL* ctx , cl_kernel* kernellist, cl_uint num_kernels)
{
	
	return adoptRef(new WebCLKernelList(ctx ,kernellist, num_kernels));
}

WebCLKernelList::WebCLKernelList(WebCL* ctx,cl_kernel* kernellist, cl_uint num_kernels ) : 
					m_context(ctx),m_cl_kernels(kernellist),m_num_kernels(num_kernels)
{
	if (m_num_kernels == 0) {
		printf("Error: Number of kernels is 0");
	}
	//m_cl_kernels = new m_cl_kernels[m_num_kernels];
	
	for (unsigned int i = 0 ; i < m_num_kernels; i++) {
		RefPtr<WebCLKernel> o = WebCLKernel::create(m_context, m_cl_kernels[i]);
		if (o != NULL) {
			m_kernel_id_list.append(o);
		} else {
			// TODO (siba samal) Error handling
		}
	}
	
}

cl_kernel WebCLKernelList::getCLKernels()
{
	return *m_cl_kernels;
}

unsigned WebCLKernelList::length() const
{
	return m_num_kernels;
}

WebCLKernel* WebCLKernelList::item(unsigned index)
{
	if (index >= m_num_kernels) {
		printf("Error: Kernel Index Out of range");
		return 0;
	}
	WebCLKernel* ret = (m_kernel_id_list[index]).get();
	return ret;
}


} // namespace WebCore

#endif // ENABLE(WEBCL)

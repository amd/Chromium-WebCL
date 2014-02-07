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

#ifndef WebCLSampler_h
#define WebCLSampler_h

#ifndef THIRD_PARTY_WEBKIT_MODULES_WEBCL // ScalableVision to avoid conflict between TraceEvent.h and trace_event.h
#define THIRD_PARTY_WEBKIT_MODULES_WEBCL
#endif

#if OS(DARWIN)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
//#include <PlatformString.h>

#include "WebCLGetInfo.h"

namespace WebCore {

class WebCL;

class WebCLSampler : public RefCounted<WebCLSampler> {
public:
	virtual ~WebCLSampler();
	static PassRefPtr<WebCLSampler> create(WebCL*, cl_sampler);
	WebCLGetInfo getInfo(cl_sampler_info, ExceptionState&);
	void releaseCL( ExceptionState&);
private:
	WebCLSampler(WebCL*, cl_sampler);	
	WebCL* m_context;
	cl_sampler m_cl_sampler;
	long m_num_samplers;
	Vector<RefPtr<WebCLSampler> > m_sampler_list;
	cl_sampler getCLSampler();
};

} // namespace WebCore

#endif // WebCLSampler_h

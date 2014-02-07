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

#ifndef WebCLDeviceList_h
#define WebCLDeviceList_h

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
#include <wtf/Vector.h>
#include "../../bindings/v8/ExceptionState.h"

namespace WebCore {

class WebCL;
class WebCLDevice;

class WebCLDeviceList : public RefCounted<WebCLDeviceList> {
public:
	// TODO (siba samal) Remove if not needed 
	virtual ~WebCLDeviceList();
	static PassRefPtr<WebCLDeviceList> create(WebCL*, cl_platform_id, int);
	static PassRefPtr<WebCLDeviceList> create(WebCL* ,cl_device_id*, int);
	cl_device_id getCLDevices();
	unsigned length() const;
	WebCLDevice* item(unsigned index);

private:	
	WebCLDeviceList(WebCL*, cl_platform_id, int);
	WebCLDeviceList(WebCL*, cl_device_id*, int);
	WebCL* m_context;
	Vector<RefPtr<WebCLDevice> > m_device_id_list;
	cl_device_id* m_cl_devices;
	cl_uint m_num_devices;
};

} // namespace WebCore

#endif // WebCLDeviceList_h

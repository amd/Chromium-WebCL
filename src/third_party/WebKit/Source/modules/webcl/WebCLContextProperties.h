/*
* Copyright (C) 2012 Intel Corporation. All rights reserved.
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

#ifndef WebCLContextProperties_h
#define WebCLContextProperties_h

#ifndef THIRD_PARTY_WEBKIT_MODULES_WEBCL // ScalableVision to avoid conflict between TraceEvent.h and trace_event.h
#define THIRD_PARTY_WEBKIT_MODULES_WEBCL
#endif

#include "WebCLPlatform.h"
#include "WebCLDeviceList.h"
#include "..\..\core\html\canvas\WebGLRenderingContext.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class WebCLContextProperties : public RefCounted<WebCLContextProperties> {
public:
	static PassRefPtr<WebCLContextProperties> create();

	virtual ~WebCLContextProperties();

	// The platform to create a context for. If this is null, the implementation will decide which platform to use. 
	// Note that in typical systems, there is only one WebCLPlatform
	PassRefPtr<WebCLPlatform> platform() const;
	void setPlatform(PassRefPtr<WebCLPlatform> platform);

	// An array of devices to create the context for.The devices must be on the same platform.
	// If the platform field is non-null, the devices must be on that particular WebCLPlatform
	PassRefPtr<WebCLDeviceList> devices() const;
	void setDevices(PassRefPtr<WebCLDeviceList> devices);

	// This field is ignored if devices is non-null. If platform and devices are both null,
	// we will select any platform that has one or more devices of the given type, and creates a context
	// that spans all of those devices.
	unsigned int deviceType() const;
	void setDeviceType(unsigned int deviceType);

	// The webgl context to be shared with webcl.
	PassRefPtr<WebGLRenderingContext> sharedWebGLContext() const;
	void setSharedWebGLContext(PassRefPtr<WebGLRenderingContext> webglContext);

	unsigned int shareGroup() const { return m_shareGroup; }
	void setShareGroup(unsigned int i) { m_shareGroup = i; }
private:
	WebCLContextProperties();

	RefPtr<WebCLPlatform> m_webclPlatform;
	RefPtr<WebCLDeviceList> m_webclDeviceList;
	unsigned int m_deviceType;
	RefPtr<WebGLRenderingContext> m_webglContext;
	unsigned int m_shareGroup;
};

} // namespace WebCore

#endif // WebCLContextProperties_h
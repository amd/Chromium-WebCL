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

#include "config.h"
#if ENABLE(WEBCL)

#include "WebCLContextProperties.h"

#include "WebCL.h"

namespace WebCore {

PassRefPtr<WebCLContextProperties> WebCLContextProperties::create()
{
	return adoptRef(new WebCLContextProperties);
}

WebCLContextProperties::WebCLContextProperties()
	: m_deviceType(WebCL::DEVICE_TYPE_DEFAULT)
{
}

WebCLContextProperties::~WebCLContextProperties()
{
}

PassRefPtr<WebCLPlatform> WebCLContextProperties::platform() const
{
	return m_webclPlatform;
}

void WebCLContextProperties::setPlatform(PassRefPtr<WebCLPlatform> platform)
{
	m_webclPlatform = platform;
}

PassRefPtr<WebCLDeviceList> WebCLContextProperties::devices() const
{
	return m_webclDeviceList;
}

void WebCLContextProperties::setDevices(PassRefPtr<WebCLDeviceList> devices)
{
	m_webclDeviceList = devices;
}

unsigned int WebCLContextProperties::deviceType() const
{
	return m_deviceType;
}

void WebCLContextProperties::setDeviceType(unsigned int deviceType)
{
	m_deviceType = deviceType;
}

PassRefPtr<WebGLRenderingContext> WebCLContextProperties::sharedWebGLContext() const
{
	return m_webglContext;
}

void WebCLContextProperties::setSharedWebGLContext(PassRefPtr<WebGLRenderingContext> webglContext)
{
	m_webglContext = webglContext;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
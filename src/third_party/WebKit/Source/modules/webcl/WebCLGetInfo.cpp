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

#include "WebCLGetInfo.h"

namespace WebCore {

WebCLGetInfo::WebCLGetInfo(bool value)
    : m_type(kTypeBool)
    , m_bool(value)
{
}

WebCLGetInfo::WebCLGetInfo(CCint value)
    : m_type(kTypeInt)
    , m_int(value)
{
}

WebCLGetInfo::WebCLGetInfo()
    : m_type(kTypeNull)
{
}

WebCLGetInfo::WebCLGetInfo(const String& value)
    : m_type(kTypeString)
    , m_string(value)
{
}

WebCLGetInfo::WebCLGetInfo(CCuint value)
    : m_type(kTypeUnsignedInt)
    , m_unsignedInt(value)
{
}

WebCLGetInfo::WebCLGetInfo(CCulong value)
    : m_type(kTypeUnsignedLong)
    , m_unsignedLong(value)
{
}

WebCLGetInfo::WebCLGetInfo(void* value)
    : m_type(kTypeVoidPointer)
    , m_voidPointer(value)
{
}
WebCLGetInfo::WebCLGetInfo(const Vector<CCuint>& value)
    : m_type(kTypeWebCLIntArray)
    , m_webclIntArray(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLImageDescriptor* value)
    : m_type(kTypeWebCLImageDescriptor)
    , m_webclImageDescriptor(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLProgram* value)
    : m_type(kTypeWebCLProgram)
    , m_webclProgram(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLContext* value)
    : m_type(kTypeWebCLContext)
    , m_webclContext(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLCommandQueue* value)
    : m_type(kTypeWebCLCommandQueue)
    , m_webCLCommandQueue(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLDevice* value)
    : m_type(kTypeWebCLDevice)
    , m_webCLDevice(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLMemoryObject* value)
    : m_type(kTypeWebCLMemoryObject)
    , m_webCLMemoryObject(value)
{
}

WebCLGetInfo::WebCLGetInfo(const Vector<RefPtr<WebCLDevice> >& value)
    : m_type(kTypeWebCLDevices)
    , m_webCLDevices(value)
{
}

WebCLGetInfo::WebCLGetInfo(WebCLPlatform* value)
    : m_type(kTypeWebCLPlatform)
    , m_webCLPlatform(value)
{
}

WebCLGetInfo::~WebCLGetInfo()
{
}

WebCLGetInfo::Type WebCLGetInfo::getType() const
{
    return m_type;
}

bool WebCLGetInfo::getBool() const
{
    ASSERT(getType() == kTypeBool);
    return m_bool;
}

CCint WebCLGetInfo::getInt() const
{
    ASSERT(getType() == kTypeInt);
    return m_int;
}

const String& WebCLGetInfo::getString() const
{
    ASSERT(getType() == kTypeString);
    return m_string;
}

CCuint WebCLGetInfo::getUnsignedInt() const
{
    ASSERT(getType() == kTypeUnsignedInt);
    return m_unsignedInt;
}

CCulong WebCLGetInfo::getUnsignedLong() const
{
    ASSERT(getType() == kTypeUnsignedLong);
    return m_unsignedLong;
}

void* WebCLGetInfo::getVoidPointer() const
{
    ASSERT(getType() == kTypeVoidPointer);
    return m_voidPointer;
}

Vector<CCuint> WebCLGetInfo::getWebCLUintArray() const
{
    ASSERT(getType() == kTypeWebCLIntArray);
    return m_webclIntArray;
}

PassRefPtr<WebCLImageDescriptor> WebCLGetInfo::getWebCLImageDescriptor() const
{
    ASSERT(getType() == kTypeWebCLImageDescriptor);
    return m_webclImageDescriptor;
}

PassRefPtr<WebCLContext> WebCLGetInfo::getWebCLContext() const
{
    ASSERT(getType() == kTypeWebCLContext);
    return m_webclContext;
}
PassRefPtr<WebCLProgram> WebCLGetInfo::getWebCLProgram() const
{
    ASSERT(getType() == kTypeWebCLProgram);
    return m_webclProgram;
}
PassRefPtr<WebCLCommandQueue> WebCLGetInfo::getWebCLCommandQueue() const
{
    ASSERT(getType() == kTypeWebCLCommandQueue);
    return m_webCLCommandQueue;
}
PassRefPtr<WebCLDevice> WebCLGetInfo::getWebCLDevice() const
{
    ASSERT(getType() == kTypeWebCLDevice);
    return m_webCLDevice;
}
const Vector<RefPtr<WebCLDevice> >& WebCLGetInfo::getWebCLDevices() const
{
    ASSERT(getType() == kTypeWebCLDevices);
    return m_webCLDevices;
}
PassRefPtr<WebCLMemoryObject> WebCLGetInfo::getWebCLMemoryObject() const
{
    ASSERT(getType() == kTypeWebCLMemoryObject);
    return m_webCLMemoryObject;
}
PassRefPtr<WebCLPlatform> WebCLGetInfo::getWebCLPlatform() const
{
    ASSERT(getType() == kTypeWebCLPlatform);
    return m_webCLPlatform;
}
} // namespace WebCore

#endif // ENABLE(WEBCL)

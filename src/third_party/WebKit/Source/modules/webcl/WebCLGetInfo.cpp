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

#include "WebCLProgram.h"
#include "WebCLContext.h"
#include <wtf/Float32Array.h>
#include <wtf/Int32Array.h>
#include <wtf/Uint32Array.h>
#include <wtf/Uint8Array.h>

namespace WebCore {

WebCLGetInfo::WebCLGetInfo(bool value)
    : m_type(kTypeBool)
    , m_bool(value)
{
}

WebCLGetInfo::WebCLGetInfo(const bool* value, int size)
    : m_type(kTypeBoolArray)
{
    if (!value || size <=0)
        return;
    m_boolArray.resize(size);
    for (int ii = 0; ii < size; ++ii)
        m_boolArray[ii] = value[ii];
}

WebCLGetInfo::WebCLGetInfo(float value)
    : m_type(kTypeFloat)
    , m_float(value)
{
}

WebCLGetInfo::WebCLGetInfo(int value)
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

WebCLGetInfo::WebCLGetInfo(unsigned int value)
    : m_type(kTypeUnsignedInt)
    , m_unsignedInt(value)
{
}

WebCLGetInfo::WebCLGetInfo(unsigned long value)
    : m_type(kTypeUnsignedInt)
    , m_unsignedLong(value)
{
}
WebCLGetInfo::WebCLGetInfo(void* value)
    : m_type(kTypeVoidPointer)
    , m_voidPointer(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<Float32Array> value)
    : m_type(kTypeWebCLFloatArray)
    , m_webclFloatArray(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<Int32Array> value)
    : m_type(kTypeWebCLIntArray)
    , m_webclIntArray(value)
{

	printf("Int32 Construcor Called \n");
}


WebCLGetInfo::WebCLGetInfo(PassRefPtr<Int8Array> value)
    : m_type(kTypeWebCLInt8Array)
    , m_webclInt8Array(value)
{
}
WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLProgram> value)
    : m_type(kTypeWebCLProgram)
    , m_webclProgram(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLContext> value)
    : m_type(kTypeWebCLContext)
    , m_webclContext(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLCommandQueue> value)
    : m_type(kTypeWebCLCommandQueue)
    , m_webCLCommandQueue(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLDevice> value)
    : m_type(kTypeWebCLDevice)
    , m_webCLDevice(value)
{
}

WebCLGetInfo::WebCLGetInfo(PassRefPtr<WebCLDeviceList> value)
    : m_type(kTypeWebCLDeviceList)
    , m_webCLDeviceList(value)
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

const Vector<bool>& WebCLGetInfo::getBoolArray() const
{
    ASSERT(getType() == kTypeBoolArray);
    return m_boolArray;
}

float WebCLGetInfo::getFloat() const
{
    ASSERT(getType() == kTypeFloat);
    return m_float;
}

int WebCLGetInfo::getInt() const
{
    ASSERT(getType() == kTypeInt);
    return m_int;
}

const String& WebCLGetInfo::getString() const
{
    ASSERT(getType() == kTypeString);
    return m_string;
}

unsigned int WebCLGetInfo::getUnsignedInt() const
{
    ASSERT(getType() == kTypeUnsignedInt);
    return m_unsignedInt;
}

unsigned long WebCLGetInfo::getUnsignedLong() const
{
    ASSERT(getType() == kTypeUnsignedLong);
    return m_unsignedLong;
}
void* WebCLGetInfo::getVoidPointer() const
{
    ASSERT(getType() == kTypeVoidPointer);
    return m_voidPointer;
}

PassRefPtr<Float32Array> WebCLGetInfo::getWebCLFloatArray() const
{
    ASSERT(getType() == kTypeWebCLFloatArray);
    return m_webclFloatArray;
}

PassRefPtr<Int32Array> WebCLGetInfo::getWebCLIntArray() const
{
	printf("Int32 Construcor Called \n");
    ASSERT(getType() == kTypeWebCLIntArray);
    return m_webclIntArray;
}


PassRefPtr<Int8Array> WebCLGetInfo::getWebCLInt8Array() const
{
    ASSERT(getType() == kTypeWebCLInt8Array);
    return m_webclInt8Array;
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
PassRefPtr<WebCLDeviceList> WebCLGetInfo::getWebCLDeviceList() const
{
    ASSERT(getType() == kTypeWebCLDeviceList);
    return m_webCLDeviceList;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

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

#ifndef WebCLGetInfo_h
#define WebCLGetInfo_h

#if ENABLE(WEBCL)

#include "ComputeTypes.h"
#include "WebCLContext.h"
#include "WebCLDevice.h"
#include "WebCLImageDescriptor.h"
#include "WebCLMemoryObject.h"
#include "WebCLPlatform.h"
#include "WebCLProgram.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class WebCLCommandQueue;

// A tagged union representing the result of get queries like
// getParameter (encompassing getBooleanv, getIntegerv, getFloatv) and
// similar variants. For reference counted types, increments and
// decrements the reference count of the target object.

class WebCLGetInfo {
public:
    enum Type {
        kTypeBool,
        kTypeInt,
        kTypeNull,
        kTypeString,
        kTypeUnsignedInt,
        kTypeUnsignedLong,
        kTypeVoidPointer,
        kTypeWebCLImageDescriptor,
        kTypeWebCLIntArray,
        kTypeWebCLObjectArray,
        kTypeWebCLProgram,
        kTypeWebCLContext,
        kTypeWebCLCommandQueue,
        kTypeWebCLDevice,
        kTypeWebCLDevices,
        kTypeWebCLMemoryObject,
        kTypeWebCLPlatform,
    };

    WebCLGetInfo();
    explicit WebCLGetInfo(bool value);
    explicit WebCLGetInfo(CCint value);
    explicit WebCLGetInfo(const String& value);
    explicit WebCLGetInfo(CCuint value);
    explicit WebCLGetInfo(CCulong value);
    explicit WebCLGetInfo(void* value);
    explicit WebCLGetInfo(const Vector<RefPtr<WebCLDevice> >& value);
    explicit WebCLGetInfo(const Vector<CCuint>& value);
    explicit WebCLGetInfo(WebCLImageDescriptor* value);
    explicit WebCLGetInfo(WebCLProgram* value);
    explicit WebCLGetInfo(WebCLContext* value);
    explicit WebCLGetInfo(WebCLCommandQueue* value);
    explicit WebCLGetInfo(WebCLDevice* value);
    explicit WebCLGetInfo(WebCLMemoryObject* value);
    explicit WebCLGetInfo(WebCLPlatform* value);

    virtual ~WebCLGetInfo();
    Type getType() const;

    bool getBool() const;
    CCint getInt() const;
    const String& getString() const;
    CCuint getUnsignedInt() const;
    CCulong getUnsignedLong() const;
    CCenum getEnum() const;
    void* getVoidPointer() const;
    PassRefPtr<WebCLImageDescriptor> getWebCLImageDescriptor() const;
    Vector<CCuint> getWebCLUintArray() const;
    PassRefPtr<WebCLProgram> getWebCLProgram() const;
    PassRefPtr<WebCLContext> getWebCLContext() const;
    PassRefPtr<WebCLCommandQueue> getWebCLCommandQueue() const;
    PassRefPtr<WebCLDevice> getWebCLDevice() const;
    const Vector<RefPtr<WebCLDevice> >& getWebCLDevices() const;
    PassRefPtr<WebCLMemoryObject> getWebCLMemoryObject() const;
    PassRefPtr<WebCLPlatform> getWebCLPlatform() const;

private:
    Type m_type;

    bool m_bool;
    CCint m_int;
    String m_string;
    CCuint m_unsignedInt;
    CCulong m_unsignedLong;
    void* m_voidPointer;
    RefPtr<WebCLImageDescriptor> m_webclImageDescriptor;
    Vector<CCuint> m_webclIntArray;
    RefPtr<WebCLProgram> m_webclProgram;
    RefPtr<WebCLContext> m_webclContext;
    RefPtr<WebCLCommandQueue> m_webCLCommandQueue;
    RefPtr<WebCLDevice> m_webCLDevice;
    Vector<RefPtr<WebCLDevice> > m_webCLDevices;
    RefPtr<WebCLMemoryObject> m_webCLMemoryObject;
    RefPtr<WebCLPlatform> m_webCLPlatform;
};

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // WebCLGetInfo_h

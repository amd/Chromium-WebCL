/*
 * Copyright (C) 2011, 2013 Samsung Electronics Corporation. All rights reserved.
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

#ifndef WebCLMemoryObject_h
#define WebCLMemoryObject_h

#if ENABLE(WEBCL)

#include "WebCLObject.h"
#include "WebCLGLObjectInfo.h"

namespace WebCore {

class ComputeMemoryObject;
class WebCLContext;
class WebCLGetInfo;

typedef ComputeMemoryObject* ComputeMemoryObjectPtr;
class WebCLMemoryObject : public WebCLObjectImpl<ComputeMemoryObjectPtr> {
public:
    virtual ~WebCLMemoryObject();
    static PassRefPtr<WebCLMemoryObject> create(WebCLContext*, ComputeMemoryObject*, CCuint sizeInBytes);

    WebCLGetInfo getInfo(CCenum, ExceptionObject&);
#if ENABLE(WEBGL)
    WebCLGLObjectInfo* getGLObjectInfo(ExceptionObject&);
#endif
    bool sharesGLResources() const;
    WebCLContext* context()
    {
        return m_context.get();
    };
    size_t sizeInBytes() const
    {
        return m_sizeInBytes;
    }
    bool isExtensionEnabled(WebCLContext*, const String& name) const;

protected:
    WebCLMemoryObject(WebCLContext*, ComputeMemoryObject*, CCuint sizeInBytes, WebCLMemoryObject* = 0);

    RefPtr<WebCLContext> m_context;
    //FIXME: We need to decide what to do with parent mem objects
    WebCLMemoryObject* m_parentMemObject;
    size_t m_sizeInBytes;

#if ENABLE(WEBGL)
    RefPtr<WebCLGLObjectInfo> m_objectInfo;
#endif
};

} // namespace WebCore

#endif
#endif // WebCLMemoryObject_h

/*
 * Copyright (C) 2011, 2012, 2013 Samsung Electronics Corporation. All rights reserved.
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

#include "WebCLMemoryObject.h"

#include "ComputeMemoryObject.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLImageDescriptor.h"
#include "WebCLProgram.h"

namespace WebCore {

WebCLMemoryObject::~WebCLMemoryObject()
{
    releasePlatformObject();
}

PassRefPtr<WebCLMemoryObject> WebCLMemoryObject::create(WebCLContext* context, ComputeMemoryObject* memoryObject, CCuint sizeInBytes)
{
    return adoptRef(new WebCLMemoryObject(context, memoryObject, sizeInBytes));
}

WebCLMemoryObject::WebCLMemoryObject(WebCLContext* context, ComputeMemoryObject* memoryObject, CCuint sizeInBytes, WebCLMemoryObject* parentBuffer)
    : WebCLObjectImpl(memoryObject)
    , m_context(context)
    , m_parentMemObject(parentBuffer)
    , m_sizeInBytes(sizeInBytes)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

WebCLGetInfo WebCLMemoryObject::getInfo(CCenum paramName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;
    switch (paramName) {
    case ComputeContext::MEM_TYPE: {
        CCMemoryObjectType memoryType = 0;
        err = platformObject()->getMemoryObjectInfo(paramName, &memoryType);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(memoryType));
        break;
        }
    case ComputeContext::MEM_FLAGS: {
        CCMemoryFlags memoryFlags = 0;
        err = platformObject()->getMemoryObjectInfo(paramName, &memoryFlags);
        // Masking out CL_MEM_COPY_HOST_PTR value obtained from OpenCL and expose only MEM_FLAG value used to create memory object, to JS.
        CCenum memCopyHostPtrMask = 0x07;
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(memoryFlags & memCopyHostPtrMask));
        break;
        }
    case ComputeContext::MEM_SIZE:
        return WebCLGetInfo(static_cast<CCuint>(m_sizeInBytes));
    case ComputeContext::MEM_OFFSET: {
        size_t memorySizeValue = 0;
        err = platformObject()->getMemoryObjectInfo(paramName, &memorySizeValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCuint>(memorySizeValue));
        break;
        }
    case ComputeContext::MEM_CONTEXT:
        return WebCLGetInfo(m_context.get());
    case ComputeContext::MEM_ASSOCIATED_MEMOBJECT:
        return WebCLGetInfo(m_parentMemObject);
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

bool WebCLMemoryObject::isExtensionEnabled(WebCLContext* context, const String& name) const
{
    return context->isExtensionEnabled(name);
};

bool WebCLMemoryObject::sharesGLResources() const
{
#if ENABLE(WEBGL)
    return m_objectInfo.get();
#else
    return false;
#endif
}

#if ENABLE(WEBGL)
WebCLGLObjectInfo* WebCLMemoryObject::getGLObjectInfo(ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "KHR_gl_sharing")) {
        setExtensionsNotEnabledException(exception);
        return 0;
    }

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return 0;
    }

    return m_objectInfo.get();
}
#endif

} // namespace WebCore

#endif // ENABLE(WEBCL)

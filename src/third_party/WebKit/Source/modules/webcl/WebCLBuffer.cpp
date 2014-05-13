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

#include "config.h"

#if ENABLE (WEBCL)

#include "WebCLBuffer.h"

#include "ComputeMemoryObject.h"
#include "WebCLContext.h"
#include "WebCLGLObjectInfo.h"
#include "WebCLInputChecker.h"
#include "WebGLBuffer.h"

namespace WebCore {

WebCLBuffer::~WebCLBuffer()
{
}

PassRefPtr<WebCLBuffer> WebCLBuffer::create(WebCLContext* context, CCenum memoryFlags, CCuint sizeInBytes, void* data, ExceptionObject& exception)
{
    CCerror error = ComputeContext::SUCCESS;
    ComputeMemoryObject* buffer = context->computeContext()->createBuffer(memoryFlags, sizeInBytes, data, error);
    if (error != ComputeContext::SUCCESS) {
        delete buffer;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }

    return adoptRef(new WebCLBuffer(context, buffer, sizeInBytes));
}

#if ENABLE(WEBGL)
PassRefPtr<WebCLBuffer> WebCLBuffer::create(WebCLContext* context, CCenum memoryFlags, WebGLBuffer* webGLBuffer, ExceptionObject& exception)
{
    Platform3DObject platform3DObject = webGLBuffer->object();
    ASSERT(platform3DObject);
    CCerror error = ComputeContext::SUCCESS;
    ComputeMemoryObject* buffer = context->computeContext()->createFromGLBuffer(memoryFlags, platform3DObject, error);
    if (error != ComputeContext::SUCCESS) {
        delete buffer;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }
    RefPtr<WebCLBuffer> clglBuffer = adoptRef(new WebCLBuffer(context, buffer, /*webGLBuffer->byteLength()*/ 0)); // ScalableVision: the GL buffer's length has to be fetched from the GPU Process, therefore not as easily obtainable as in webkit-webcl.
    clglBuffer->cacheGLObjectInfo(webGLBuffer);
    return clglBuffer.release();
}
#endif

WebCLBuffer::WebCLBuffer(WebCLContext* context, ComputeMemoryObject* buffer, CCuint sizeInBytes, WebCLBuffer* parentBuffer)
    : WebCLMemoryObject(context, buffer, sizeInBytes, parentBuffer)
    , m_weakFactoryForLazyInitialization(this)
{
}

PassRefPtr<WebCLBuffer> WebCLBuffer::createSubBuffer(CCenum memoryFlags, CCuint origin, CCuint sizeInBytes, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return 0;
    }

    if (m_parentMemObject) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return 0;
    }

    if (!WebCLInputChecker::isValidMemoryObjectFlag(memoryFlags)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return 0;
    }

    CCBufferRegion bufferCreateInfo = {origin, sizeInBytes};
    CCerror error = 0;
    ComputeMemoryObject* computeSubBuffer = platformObject()->createSubBuffer(memoryFlags, ComputeContext::BUFFER_CREATE_TYPE_REGION, &bufferCreateInfo, error);

    if (error != ComputeContext::SUCCESS) {
        delete computeSubBuffer;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }
    RefPtr<WebCLBuffer> subBuffer = adoptRef(new WebCLBuffer(m_context.get(), computeSubBuffer, sizeInBytes, this));
    return subBuffer.release();
}

#if ENABLE(WEBGL)
void WebCLBuffer::cacheGLObjectInfo(WebGLBuffer* webGLBuffer)
{
    m_objectInfo = WebCLGLObjectInfo::create(ComputeContext::GL_OBJECT_BUFFER, webGLBuffer);
}
#endif

} // namespace WebCore

#endif // ENABLE(WEBCL)

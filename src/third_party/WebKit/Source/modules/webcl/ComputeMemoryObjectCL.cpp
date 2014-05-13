/*
 * Copyright (C) 2014 Samsung Electronics Corporation. All rights reserved.
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
#include "ComputeMemoryObject.h"

#include "ComputeContext.h"

namespace WebCore {

// FIXME: Remove it when the CL<->GL interoperability turns to be an extension.
static cl_mem_flags computeMemoryTypeToCL(int memoryType)
{
    CCint clMemoryType = CL_INVALID_VALUE;
    switch (memoryType) {
    case ComputeContext::MEM_READ_ONLY:
        clMemoryType = CL_MEM_READ_ONLY;
        break;
    case ComputeContext::MEM_WRITE_ONLY:
        clMemoryType = CL_MEM_WRITE_ONLY;
        break;
    case ComputeContext::MEM_READ_WRITE:
        clMemoryType = CL_MEM_READ_WRITE;
        break;
    case ComputeContext::MEM_USE_HOST_PTR:
        clMemoryType = CL_MEM_USE_HOST_PTR;
        break;
    case ComputeContext::MEM_ALLOC_HOST_PTR:
        clMemoryType = CL_MEM_ALLOC_HOST_PTR;
        break;
    case ComputeContext::MEM_COPY_HOST_PTR:
        clMemoryType = CL_MEM_COPY_HOST_PTR;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    return clMemoryType;
}

ComputeMemoryObject::~ComputeMemoryObject()
{
    clReleaseMemObject(m_memoryObject);
}

ComputeMemoryObject::ComputeMemoryObject(ComputeContext* context, CCMemoryFlags flags, size_t size, void* data, CCerror& error)
{
    m_memoryObject = clCreateBuffer(context->context(), flags, size, data, &error);
}

ComputeMemoryObject::ComputeMemoryObject(ComputeContext* context, CCMemoryFlags flags, size_t width, size_t height, CCuint rowPitch, const CCImageFormat& imageFormat, void* data, CCerror& error)
{
#if defined(CL_VERSION_1_2) && CL_VERSION_1_2
    CCImageDescriptor clImageDescriptor = {CL_MEM_OBJECT_IMAGE2D, width, height, 0 /*imageDepth*/, 0 /*arraySize*/,
        static_cast<size_t>(rowPitch), 0 /*slicePitch*/, 0 /*numMipLevels*/, 0 /*numSamples*/, 0 /*buffer*/};
    m_memoryObject = clCreateImage(context->context(), flags, &imageFormat, &clImageDescriptor, data, &error);
#else
    m_memoryObject = clCreateImage2D(context->context(), flags, &imageFormat, width, height, rowPitch, data, &error);
#endif
}

ComputeMemoryObject::ComputeMemoryObject(ComputeContext* context, CCMemoryFlags flags, GC3Duint bufferId, GLBufferSourceType type, CCerror& error)
{
    CCint memoryType = computeMemoryTypeToCL(flags);
    if (type == GLBuffer)
        m_memoryObject = clCreateFromGLBuffer(context->context(), memoryType, bufferId, &error);
    else if (type == GLRenderbuffer)
        m_memoryObject = clCreateFromGLRenderbuffer(context->context(), memoryType, bufferId, &error);
}

ComputeMemoryObject::ComputeMemoryObject(ComputeContext* context, CCMemoryFlags flags, GC3Denum textureTarget, GC3Dint mipLevel, GC3Duint texture, CCerror& error)
{
    CCMemoryFlags memoryType = computeMemoryTypeToCL(flags);
#if defined(CL_VERSION_1_2) && CL_VERSION_1_2
    m_memoryObject = clCreateFromGLTexture(context->context(), memoryType, textureTarget, mipLevel, texture, &error);
#else
    m_memoryObject = clCreateFromGLTexture2D(context->context(), memoryType, textureTarget, mipLevel, texture, &error);
#endif
}

ComputeMemoryObject::ComputeMemoryObject(PlatformComputeObject memoryObject)
    : m_memoryObject(memoryObject)
{
}

ComputeMemoryObject* ComputeMemoryObject::createSubBuffer(CCMemoryFlags flags, CCBufferCreateType bufferCreateType, CCBufferRegion* bufferRegion, CCerror& error)
{
    PlatformComputeObject memoryObject = clCreateSubBuffer(m_memoryObject, flags, bufferCreateType, bufferRegion, &error);
    return new ComputeMemoryObject(memoryObject);
}

CCerror ComputeMemoryObject::getImageInfoBase(PlatformComputeObject image, CCImageInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetImageInfo(image, infoType, sizeOfData, data, retSize);
}

CCerror ComputeMemoryObject::getGLTextureInfoBase(PlatformComputeObject image, CCImageTextureInfoType textureInfoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetGLTextureInfo(image, textureInfoType, sizeOfData, data, retSize);
}

CCerror ComputeMemoryObject::getMemoryObjectInfoBase(PlatformComputeObject memObject, CCMemInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetMemObjectInfo(memObject, infoType, sizeOfData, data, retSize);
}

CCerror ComputeMemoryObject::release()
{
    return clReleaseMemObject(m_memoryObject);
}

}

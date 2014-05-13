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

#ifndef ComputeMemoryObject_h
#define ComputeMemoryObject_h

#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"
//#include "GraphicsTypes3D.h"
#include "../../core/platform/graphics/GraphicsContext3D.h"

namespace WebCore {

class ComputeContext;

typedef enum { GLBuffer, GLRenderbuffer } GLBufferSourceType;

class ComputeMemoryObject {
public:
    ComputeMemoryObject(ComputeContext*, CCMemoryFlags, size_t sizeInBytes, void* data, CCerror&);
    ComputeMemoryObject(ComputeContext*, CCMemoryFlags, size_t width, size_t height, CCuint rowPitch, const CCImageFormat&, void* data, CCerror&);
    ComputeMemoryObject(ComputeContext*, CCMemoryFlags, GC3Duint bufferId, GLBufferSourceType, CCerror&);
    ComputeMemoryObject(ComputeContext*, CCMemoryFlags, GC3Denum textureTarget, GC3Dint mipLevel, GC3Duint texture, CCerror&);

    ~ComputeMemoryObject();

    ComputeMemoryObject* createSubBuffer(CCMemoryFlags, CCBufferCreateType, CCBufferRegion*, CCerror&);

    template <typename T>
    CCerror getImageInfo(CCImageInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeMemoryObject::getImageInfoBase, m_memoryObject, infoType, data);
    }
    template <typename T>
    CCerror getGLTextureInfo(CCImageTextureInfoType textureInfoType, T* data)
    {
        return getInfoHelper(ComputeMemoryObject::getGLTextureInfoBase, m_memoryObject, textureInfoType, data);
    }
    template <typename T>
    CCerror getMemoryObjectInfo(CCMemInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeMemoryObject::getMemoryObjectInfoBase, m_memoryObject, infoType, data);
    }

    PlatformComputeObject memoryObject() const
    {
        return m_memoryObject;
    }

    CCerror release();

private:
    ComputeMemoryObject(PlatformComputeObject); // Used for creating a subbuffer.

    static CCerror getImageInfoBase(PlatformComputeObject, CCImageInfoType, size_t, void *data, size_t* actualSize);
    static CCerror getGLTextureInfoBase(PlatformComputeObject, CCImageTextureInfoType, size_t, void *data, size_t* actualSize);
    static CCerror getMemoryObjectInfoBase(PlatformComputeObject, CCMemInfoType, size_t, void *data, size_t* actualSize);

private:
    PlatformComputeObject m_memoryObject;
};

}

#endif

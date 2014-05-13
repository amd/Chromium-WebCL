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

#ifndef ComputeKernel_h
#define ComputeKernel_h

#include "ComputeDevice.h"
#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"

#include <wtf/text/WTFString.h>

namespace WebCore {

class ComputeMemoryObject;
class ComputeProgram;
class ComputeSampler;

class ComputeKernel {
public:
    ComputeKernel(ComputeProgram*, const String& kernelName, CCerror&);
    ComputeKernel(CCKernel);
    ~ComputeKernel();

    CCerror setKernelArg(CCuint argIndex, ComputeMemoryObject*);
    CCerror setKernelArg(CCuint argIndex, ComputeSampler*);
    CCerror setKernelArg(CCuint argIndex, size_t argSize, const void* argValue);

    template <typename T>
    CCerror getKernelInfo(CCKernelInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeKernel::getKernelInfoBase, m_kernel, infoType, data);
    }
    template <typename T>
    CCerror getWorkGroupInfo(ComputeDevice* device, CCKernelWorkGroupInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeKernel::getWorkGroupInfoBase, m_kernel, device->device(), infoType, data);
    }

    CCKernel kernel() const
    {
        return m_kernel;
    }

    CCerror release();

private:

    static CCerror getKernelInfoBase(CCKernel, CCKernelInfoType, size_t, void *data, size_t* actualSize);
    static CCerror getWorkGroupInfoBase(CCKernel, CCDeviceID, CCKernelWorkGroupInfoType, size_t, void *data, size_t* actualSize);
private:

    CCKernel m_kernel;
};

}

#endif

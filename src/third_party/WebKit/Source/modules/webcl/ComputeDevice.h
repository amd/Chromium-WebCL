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

#ifndef ComputeDevice_h
#define ComputeDevice_h

#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"

#include <wtf/RefCounted.h>
#include <wtf/HashMap.h>

namespace WebCore {

class ComputeDevice : public RefCounted<ComputeDevice> {
public:
    static ComputeDevice* create(CCDeviceID);

    CCDeviceID device() const
    {
        return m_device;
    }

    template <typename T>
    CCerror getDeviceInfo(CCDeviceInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeDevice::getDeviceInfoBase, m_device, infoType, data);
    }

    // FIXME: Differently from other getXXXInfo methods, this one is static because of the way
    // it talks to ComputeExtensionsTraits.
    template <typename T>
    static CCerror getDeviceInfo2(CCDeviceID device, CCDeviceInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeDevice::getDeviceInfoBase, device, infoType, data);
    }

private:
    ComputeDevice(CCDeviceID);

    static CCerror getDeviceInfoBase(CCDeviceID, CCDeviceInfoType, size_t, void *data, size_t* actualSize);

    CCDeviceID m_device;
};

}

#endif

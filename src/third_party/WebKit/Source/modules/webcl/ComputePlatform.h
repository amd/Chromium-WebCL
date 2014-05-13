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

#ifndef ComputePlatform_h
#define ComputePlatform_h

#include "ComputeDevice.h"
#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"

#include <wtf/RefCounted.h>

namespace WebCore {

class ComputePlatform : public RefCounted<ComputePlatform> {
public:
    static ComputePlatform* create(CCPlatformID);

    static CCerror getPlatformIDs(Vector<RefPtr<ComputePlatform> >&);
    CCerror getDeviceIDs(CCDeviceType, Vector<RefPtr<ComputeDevice> >&);

    CCPlatformID platform() const
    {
        return m_platform;
    }

    // FIXME: Differently from other getXXXInfo methods, this one is static because of the way
    // it talks to ComputeExtensionsTraits.
    template <typename T>
    static CCerror getPlatformInfo(CCPlatformID platformID, CCPlatformInfoType infoType, T* data)
    {
        return getInfoHelper(ComputePlatform::getPlatformInfoBase, platformID, infoType, data);
    }

private:
    ComputePlatform(CCPlatformID);

    static CCerror getPlatformInfoBase(CCPlatformID, CCPlatformInfoType, size_t, void *data, size_t* actualSize);

    CCPlatformID m_platform;
};

}

#endif

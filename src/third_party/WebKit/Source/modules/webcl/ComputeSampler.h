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

#ifndef ComputeSampler_h
#define ComputeSampler_h

#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"

namespace WebCore {

class ComputeContext;

class ComputeSampler {
public:
    ComputeSampler(ComputeContext*, CCbool normalizedCoords, CCAddressingMode, CCFilterMode, CCerror&);
    ~ComputeSampler();

    CCSampler sampler() const
    {
        return m_sampler;
    }

    template <typename T>
    CCerror getSamplerInfo(CCSamplerInfoType infoType, T* data)
    {
        return getInfoHelper(ComputeSampler::getSamplerInfoBase, m_sampler, infoType, data);
    }

    CCerror release();

private:
    static CCerror getSamplerInfoBase(CCSampler, CCSamplerInfoType, size_t, void *data, size_t* actualSize);

    CCSampler m_sampler;
};

}

#endif

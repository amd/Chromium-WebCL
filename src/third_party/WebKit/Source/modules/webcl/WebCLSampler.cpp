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

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLSampler.h"

#include "ComputeSampler.h"
#include "WebCLContext.h"

namespace WebCore {

WebCLSampler::~WebCLSampler()
{
    releasePlatformObject();
}

PassRefPtr<WebCLSampler> WebCLSampler::create(WebCLContext* context, CCbool normCoords, CCenum addressingMode, CCenum filterMode, ExceptionObject& exception)
{
    CCerror error;
    ComputeSampler* computeSampler = context->computeContext()->createSampler(normCoords, addressingMode, filterMode, error);
    if (error != ComputeContext::SUCCESS) {
        delete computeSampler;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }

    return adoptRef(new WebCLSampler(context, computeSampler, normCoords, addressingMode, filterMode));
}

WebCLSampler::WebCLSampler(WebCLContext* context, ComputeSampler* sampler, CCbool normCoords, CCenum addressingMode, CCenum filterMode)
    : WebCLObjectImpl(sampler)
    , m_normCoords(normCoords)
    , m_addressingMode(addressingMode)
    , m_filterMode(filterMode)
    , m_context(context)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

WebCLGetInfo WebCLSampler::getInfo(CCenum infoType, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_SAMPLER, exception);
        return WebCLGetInfo();
    }

    switch (infoType) {
    case ComputeContext::SAMPLER_NORMALIZED_COORDS:
        return WebCLGetInfo(static_cast<bool>(m_normCoords));
    case ComputeContext::SAMPLER_CONTEXT:
        return WebCLGetInfo(m_context.get());
    case ComputeContext::SAMPLER_ADDRESSING_MODE:
        return WebCLGetInfo(m_addressingMode);
    case ComputeContext::SAMPLER_FILTER_MODE:
        return WebCLGetInfo(m_filterMode);
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

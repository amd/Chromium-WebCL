/*
 * Copyright (C) 2013 Samsung Electronics Corporation. All rights reserved.
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

#include "WebCLUserEvent.h"

#include "ComputeEvent.h"
#include "WebCLContext.h"
#include "WebCLCommandQueue.h"

namespace WebCore {

WebCLUserEvent::~WebCLUserEvent()
{
    releasePlatformObject();
}

PassRefPtr<WebCLUserEvent> WebCLUserEvent::create(WebCLContext* context, ExceptionObject& exception)
{
    CCerror userEventError = 0;
    ComputeEvent* userEvent = context->computeContext()->createUserEvent(userEventError);
    if (!userEvent) {
        ASSERT(userEventError != ComputeContext::SUCCESS);
        setExceptionFromComputeErrorCode(userEventError, exception);
        return 0;
    }
    return adoptRef(new WebCLUserEvent(context, userEvent));
}

WebCLUserEvent::WebCLUserEvent(WebCLContext* context, ComputeEvent* event)
    : WebCLEvent(event)
    , m_context(context)
    , m_eventStatusSituation(StatusUnset)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

void WebCLUserEvent::setStatus(CCint executionStatus, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return;
    }

    if (!(executionStatus < 0 || executionStatus == ComputeContext::COMPLETE)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (m_eventStatusSituation == StatusSet) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_OPERATION, exception);
        return;
    }
    m_eventStatusSituation = StatusSet;

    CCerror userEventError = platformObject()->setUserEventStatus(executionStatus);
    setExceptionFromComputeErrorCode(userEventError, exception);
}

WebCLGetInfo WebCLUserEvent::getInfo(CCenum name, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return WebCLGetInfo();
    }

    switch (name) {
    case ComputeContext::EVENT_CONTEXT:
        ASSERT(m_context);
        return WebCLGetInfo(m_context.get());
    case ComputeContext::EVENT_COMMAND_QUEUE:
        return WebCLGetInfo();
    }

    return WebCLEvent::getInfo(name, exception);
}

WebCLContext* WebCLUserEvent::context() const
{
    return m_context.get();
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

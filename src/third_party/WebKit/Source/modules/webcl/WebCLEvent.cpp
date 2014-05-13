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

#include "WebCLEvent.h"

#include "ComputeEvent.h"
#include "WebCLCallback.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLGetInfo.h"

#include <wtf/MainThread.h>

namespace WebCore {
WebCLEvent::~WebCLEvent()
{
    releasePlatformObject();
}

PassRefPtr<WebCLEvent> WebCLEvent::create()
{
    return adoptRef(new WebCLEvent(new ComputeEvent));
}

WebCLEvent::WebCLEvent(ComputeEvent* event)
    : WebCLObjectImpl(event)
    , m_commandQueue(0)
{
}

WebCLGetInfo WebCLEvent::getInfo(CCenum paramName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;
    switch (paramName) {
    case ComputeContext::EVENT_COMMAND_EXECUTION_STATUS: {
        if (!holdsValidCLObject())
            return WebCLGetInfo(-1);

        CCint ccExecStatus = 0;
        err = platformObject()->getEventInfo(paramName, &ccExecStatus);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCint>(ccExecStatus));
        break;
    }
    case ComputeContext::EVENT_COMMAND_TYPE: {
        if (!holdsValidCLObject())
            return WebCLGetInfo();

        CCCommandType ccCommandType = 0;
        err= platformObject()->getEventInfo(paramName, &ccCommandType);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(ccCommandType));
        break;
    }
    case ComputeContext::EVENT_CONTEXT: {
        if (!holdsValidCLObject())
            return WebCLGetInfo();

        ASSERT(m_commandQueue);
        ASSERT(m_commandQueue->context());
        ASSERT(!isUserEvent());
        return WebCLGetInfo(m_commandQueue->context());
    }
    case ComputeContext::EVENT_COMMAND_QUEUE: {
        if (!holdsValidCLObject())
            return WebCLGetInfo();

        ASSERT(m_commandQueue);
        ASSERT(!isUserEvent());
        return WebCLGetInfo(m_commandQueue.get());
    }
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

WebCLGetInfo WebCLEvent::getProfilingInfo(CCenum paramName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return WebCLGetInfo();
    }

    if (isUserEvent()) {
        setExceptionFromComputeErrorCode(ComputeContext::PROFILING_INFO_NOT_AVAILABLE, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;
    switch (paramName) {
    case ComputeContext::PROFILING_COMMAND_QUEUED:
    case ComputeContext::PROFILING_COMMAND_SUBMIT:
    case ComputeContext::PROFILING_COMMAND_START:
    case ComputeContext::PROFILING_COMMAND_END: {
        CCulong eventProfilingInfo = 0;
        err = platformObject()->getEventProfilingInfo(paramName, &eventProfilingInfo);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<unsigned>(eventProfilingInfo));
        }
        break;
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

void WebCLEvent::setAssociatedCommandQueue(WebCLCommandQueue* commandQueue)
{
    m_commandQueue = commandQueue;
}

WebCLContext* WebCLEvent::context() const
{
    return m_commandQueue ? m_commandQueue->context() : 0;
}

void WebCLEvent::callbackProxyOnMainThread(void* userData)
{
    WebCLEvent* callee = static_cast<WebCLEvent*>(userData);

    // spec says "If a callback function is associated with a WebCL
    // object that is subsequently released, the callback will no longer be
    // invoked."
    // FIXME: Should we check the command queue, buffers, etc?
    if (callee->isPlatformObjectNeutralized()) {
        callbackRegisterQueue().remove(callee);
        return;
    }

    CallbackDataVector* vector = callbackRegisterQueue().get(callee);
    ASSERT(vector);

    for (size_t i = 0; i < vector->size(); ++i) {
        std::pair<CCint, RefPtr<WebCLCallback> > current = vector->at(i);
        current.second->handleEvent();
    }
    callbackRegisterQueue().remove(callee);
}

void WebCLEvent::callbackProxy(CCEvent, CCint, void* userData)
{
    // Callbacks might get called from non-mainthread. When it happens,
    // dispatch it to the mainthread, so that it can call JS back safely.
    if (!isMainThread()) {
        callOnMainThread(callbackProxyOnMainThread, userData);
        return;
    }
    callbackProxyOnMainThread(userData);
}

void WebCLEvent::setCallback(CCenum commandExecCallbackType, PassRefPtr<WebCLCallback> callback, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized() || !holdsValidCLObject()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return;
    }

   if (commandExecCallbackType != ComputeContext::COMPLETE) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
   }

    ASSERT(callback);

    if (CallbackDataVector* vector = callbackRegisterQueue().get(this)) {
        vector->append(std::make_pair(commandExecCallbackType, callback));
        return;
    }

    OwnPtr<CallbackDataVector> vector = adoptPtr(new CallbackDataVector);
    vector->append(std::make_pair(commandExecCallbackType, callback));
    callbackRegisterQueue().set(this, vector.release());

    CCerror error = ComputeContext::SUCCESS;
    pfnEventNotify callbackProxyPtr = &callbackProxy;
    error = platformObject()->setEventCallback(commandExecCallbackType, callbackProxyPtr, this);
    setExceptionFromComputeErrorCode(error, exception);
}

bool WebCLEvent::holdsValidCLObject() const
{
    return platformObject() && platformObject()->event();
}

bool WebCLEvent::isUserEvent() const
{
    return platformObject() && platformObject()->isUserEvent();
}

bool WebCLEvent::isPlatformObjectNeutralized() const
{
    return isReleased();
}

} // namespace WebCore

#endif // ENABLE(WEBCL)


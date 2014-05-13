/*
 * Copyright (C) 2011, 2012, 2013 Samsung Electronics Corporation. All rights reserved.
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

#include "WebCL.h"

#include "ComputeEvent.h"
#include "WebCLContext.h"
#include "WebCLDevice.h"
#include "WebCLEvent.h"

//using namespace JSC;

namespace WebCore {

PassRefPtr<WebCL> WebCL::create()
{
    return adoptRef(new WebCL);
}

WebCL::WebCL()
    : WebCLExtensionsAccessor(0)
{
}

Vector<RefPtr<WebCLPlatform> > WebCL::getPlatforms(ExceptionObject& exception)
{
    if (m_platforms.size())
        return m_platforms;

    CCerror error = WebCore::getPlatforms(m_platforms);
    if (error != ComputeContext::SUCCESS) {
        setExceptionFromComputeErrorCode(error, exception);
        m_platforms.clear();
        return m_platforms;
    }
    return m_platforms;
}

// FIXME: We currently do not support the asynchronous variant of this method.
void WebCL::waitForEvents(const Vector<RefPtr<WebCLEvent> >& events, ExceptionObject& exception)
{
    if (!events.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    // So if the event being waited on has not been initialized (1) or is an user
    // event (2), we would hand the browser.
    if (events[0]->isPlatformObjectNeutralized()
        || !events[0]->holdsValidCLObject()
        || events[0]->isUserEvent()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
        return;
    }
    ASSERT(events[0]->context());
    WebCLContext* referenceContext  = events[0]->context();
    Vector<ComputeEvent*> computeEvents;
    computeEvents.append(events[0]->platformObject());

    for (size_t i = 1; i < events.size(); ++i) {
        if (events[i]->isPlatformObjectNeutralized()
            || !events[i]->holdsValidCLObject()
            || events[i]->isUserEvent()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
            return;
        }
        ASSERT(events[i]->context());
        if (!WebCLInputChecker::compareContext(events[i]->context(), referenceContext)) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
            return;
        }

        computeEvents.append(events[i]->platformObject());
    }

    CCerror error = ComputeContext::waitForEvents(computeEvents);
    setExceptionFromComputeErrorCode(error, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(ExceptionObject& exception)
{
    return createContext(ComputeContext::DEVICE_TYPE_DEFAULT, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(CCenum deviceType, ExceptionObject& exception)
{
    getPlatforms(exception);
    if (willThrowException(exception))
        return 0;

    return createContext(m_platforms[0].get(), deviceType, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebCLPlatform* platform, ExceptionObject& exception)
{
    return createContext(platform, ComputeContext::DEVICE_TYPE_DEFAULT, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebCLPlatform* platform, CCenum deviceType, ExceptionObject& exception)
{
    if (!platform) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PLATFORM, exception);
        return 0;
    }

    if (!WebCLInputChecker::isValidDeviceType(deviceType)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return 0;
    }

    Vector<RefPtr<WebCLDevice> > devices = platform->getDevices(deviceType, exception);
    if (willThrowException(exception))
        return 0;

    return WebCLContext::create(this, 0 /*glContext*/, platform, devices, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(const Vector<RefPtr<WebCLDevice> >& devices, ExceptionObject& exception)
{
    if (!devices.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_DEVICE, exception);
        return 0;
    }

    return WebCLContext::create(this, 0 /*glContext*/, devices[0]->platform(), devices, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebCLDevice* device, ExceptionObject& exception)
{
    Vector<RefPtr<WebCLDevice>> devices;
    devices.append(device);

    return createContext(devices, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, ExceptionObject& exception)
{
    // FIXME: Pick a device that best suits to cl-gl instead of the default one.
    return createContext(glContext, ComputeContext::DEVICE_TYPE_DEFAULT, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, CCenum deviceType, ExceptionObject& exception)
{
    getPlatforms(exception);
    if (willThrowException(exception))
        return 0;

    // FIXME: Pick a platform that best suites to cl-gl instead of any.
    return createContext(glContext, m_platforms[0].get(), deviceType, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, WebCLPlatform* platform, ExceptionObject& exception)
{
    // FIXME: Pick a device that best suits to cl-gl instead of the default one.
    return createContext(glContext, platform, ComputeContext::DEVICE_TYPE_DEFAULT, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, WebCLPlatform* platform, CCenum deviceType, ExceptionObject& exception)
{
    if (!platform) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PLATFORM, exception);
        return 0;
    }

    if (!WebCLInputChecker::isValidDeviceType(deviceType)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return 0;
    }

    // FIXME: Pick devices that best suits to cl-gl instead of all.
    Vector<RefPtr<WebCLDevice> > devices = platform->getDevices(deviceType, exception);
    if (willThrowException(exception))
        return 0;

    return WebCLContext::create(this, glContext, platform, devices, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, const Vector<RefPtr<WebCLDevice> >& devices, ExceptionObject& exception)
{
    if (!devices.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_DEVICE, exception);
        return 0;
    }

    return WebCLContext::create(this, glContext, devices[0]->platform(), devices, exception);
}

PassRefPtr<WebCLContext> WebCL::createContext(WebGLRenderingContext* glContext, WebCLDevice* device, ExceptionObject& exception)
{
    Vector<RefPtr<WebCLDevice>> devices;
    devices.append(device);

    return createContext(glContext, devices, exception);
}

void WebCL::trackReleaseableWebCLObject(WeakPtr<WebCLObject> object)
{
    m_descendantWebCLObjects.append(object);
}

void WebCL::releaseAll()
{
    for (size_t i = 0; i < m_descendantWebCLObjects.size(); ++i) {
        WebCLObject* object = m_descendantWebCLObjects.at(i).get();
        if (!object)
            continue;

        WebCLContext* context = static_cast<WebCLContext*>(object);
        context->releaseAll();
    }
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

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
#include "WebCLProgram.h"

#include "ComputeDevice.h"
#include "ComputeProgram.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLGetInfo.h"
#include "WebCLImageDescriptor.h"
#include "WebCLInputChecker.h"
#include "WebCLKernel.h"
#include <wtf/text/AtomicStringHash.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/MainThread.h>

namespace WebCore {

WebCLProgram::~WebCLProgram()
{
    releasePlatformObject();
}

PassRefPtr<WebCLProgram> WebCLProgram::create(WebCLContext* context, const String& programSource, ExceptionObject& exception)
{
    CCerror error = 0;

    ComputeProgram* computeProgram = context->computeContext()->createProgram(programSource, error);
    if (error != ComputeContext::SUCCESS) {
        delete computeProgram;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }

    return adoptRef(new WebCLProgram(context, computeProgram, programSource));
}

WebCLProgram::WebCLProgram(WebCLContext*context, ComputeProgram* program, const String& programSource)
    : WebCLObjectImpl(program)
    , m_context(context)
    , m_programSource(programSource)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

WebCLGetInfo WebCLProgram::getInfo(CCenum infoType, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PROGRAM, exception);
        return WebCLGetInfo();
    }

    switch (infoType) {
    case ComputeContext::PROGRAM_NUM_DEVICES:
        return m_context->getInfo(ComputeContext::CONTEXT_NUM_DEVICES, exception);
    case ComputeContext::PROGRAM_SOURCE:
        return WebCLGetInfo(m_programSource);
    case ComputeContext::PROGRAM_CONTEXT:
        return WebCLGetInfo(m_context.get());
    case ComputeContext::PROGRAM_DEVICES:
        return m_context->getInfo(ComputeContext::CONTEXT_DEVICES, exception);
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT_NOT_REACHED();
}

WebCLGetInfo WebCLProgram::getBuildInfo(WebCLDevice* device, CCenum infoType, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PROGRAM, exception);
        return WebCLGetInfo();
    }

    if (!device) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_DEVICE, exception);
        return WebCLGetInfo();
    }

    ComputeDevice* ccDeviceID = device->platformObject();
    ASSERT(ccDeviceID);

    CCerror error = 0;
    switch (infoType) {
    case ComputeContext::PROGRAM_BUILD_OPTIONS:
    case ComputeContext::PROGRAM_BUILD_LOG: {
        Vector<char> buffer;
        error = platformObject()->getBuildInfo(ccDeviceID, infoType, &buffer);
        if (error == ComputeContext::SUCCESS)
            return WebCLGetInfo(String(buffer.data()));
        break;
    }
    case ComputeContext::PROGRAM_BUILD_STATUS: {
        CCBuildStatus buildStatus;
        error = platformObject()->getBuildInfo(ccDeviceID, infoType, &buildStatus);
        if (error == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCint>(buildStatus));
        break;
    }
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(error != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(error, exception);
    return WebCLGetInfo();
}

PassRefPtr<WebCLKernel> WebCLProgram::createKernel(const String& kernelName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PROGRAM, exception);
        return 0;
    }

    return WebCLKernel::create(m_context.get(), this, kernelName, exception);
}

Vector<RefPtr<WebCLKernel> > WebCLProgram::createKernelsInProgram(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PROGRAM, exception);
        return Vector<RefPtr<WebCLKernel> >();
    }

    return WebCLKernel::createKernelsInProgram(m_context.get(), this, exception);
}

/*  Static function to be sent as callback to OpenCL clBuildProgram.
    Must be static and must map a (CCProgram,userData) to corresponding WebCLProgram.
    Here we use the userData as a index to a static Vector. On each call to build with callback,
    append a copy of this pointer to this Vector and send index as userData. On return this static
    member will be used to retrive the WebCLProgram.
    */
void WebCLProgram::callbackProxyOnMainThread(void* userData)
{
    WebCLProgram* callee = static_cast<WebCLProgram*>(userData);

    // spec says "If a callback function is associated with a WebCL
    // object that is subsequently released, the callback will no longer be invoked.
    if (!callee || callee->isPlatformObjectNeutralized())
        return;
    callee->callEvent();
}

void WebCLProgram::callbackProxy(CCProgram, void* userData)
{
    // Callbacks might get called from non-mainthread. When it happens,
    // dispatch it to the mainthread, so that it can call JS back safely.
    if (!isMainThread()) {
        callOnMainThread(callbackProxyOnMainThread, userData);
        return;
    }
    callbackProxyOnMainThread(userData);
}

void WebCLProgram::build(ExceptionObject& exception) {
	build( Vector<RefPtr<WebCLDevice> >(), NULL, NULL, exception);
}
void WebCLProgram::build(const Vector<RefPtr<WebCLDevice> >& devices, const String* _buildOptions, PassRefPtr<WebCLCallback> callback, ExceptionObject& exception)
{
	const String &buildOptions = *_buildOptions;
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_PROGRAM, exception);
        return;
    }

    if (buildOptions.length() > 0) {
        DEFINE_STATIC_LOCAL(AtomicString, buildOptionDashD, ("-D", AtomicString::ConstructFromLiteral));
        DEFINE_STATIC_LOCAL(HashSet<AtomicString>, webCLSupportedBuildOptions, ());
        if (webCLSupportedBuildOptions.isEmpty()) {
            webCLSupportedBuildOptions.add(AtomicString("-cl-opt-disable", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-single-precision-constant", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-denorms-are-zero", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-mad-enable", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-no-signed-zeros", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-unsafe-math-optimizations", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-finite-math-only", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-cl-fast-relaxed-math", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-w", AtomicString::ConstructFromLiteral));
            webCLSupportedBuildOptions.add(AtomicString("-Werror", AtomicString::ConstructFromLiteral));
        }

        Vector<String> webCLBuildOptionsVector;
        buildOptions.split(" ", false /* allowEmptyEntries */, webCLBuildOptionsVector);
        for (size_t i = 0; i < webCLBuildOptionsVector.size(); i++) {
            // Every build option must start with a hyphen.
            if (!webCLBuildOptionsVector[i].startsWith("-")) {
                setExceptionFromComputeErrorCode(ComputeContext::INVALID_BUILD_OPTIONS, exception);
                return;
            }
            if (webCLSupportedBuildOptions.contains(webCLBuildOptionsVector[i]))
                continue;
            /* If the token begins with "-D" it can be one of "-D NAME" or "-D name=definition".
               Currently OpenCL specification does not state any restriction adding spaces in between.
               So on encounter of a token starting with "-D" we skip validation till we reach next build option.
               Pushing the validation of "-D" options to underlying OpenCL. */
            if (webCLBuildOptionsVector[i].startsWith(buildOptionDashD)) {
                size_t j;
                for (j = ++i; j < webCLBuildOptionsVector.size() && !webCLBuildOptionsVector[j].startsWith("-"); ++j) { }
                i = --j;
                continue;
            }
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_BUILD_OPTIONS, exception);
            return;
        }
    }

    Vector<ComputeDevice*> ccDevices;
    ccDeviceListFromWebCLDeviceList(devices, ccDevices, exception);
    if (willThrowException(exception))
        return;

    pfnNotify callbackProxyPtr = 0;
    if (callback) {
        // If previous callback is still valid, calling build again must throw a INVALID_OPERATION.
        if (m_callback) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_OPERATION, exception);
            return;
        }
        m_callback = callback;
        callbackProxyPtr = &callbackProxy;
    }

    CCerror err = platformObject()->buildProgram(ccDevices, buildOptions, callbackProxyPtr, m_callback ? this : 0);
    setExceptionFromComputeErrorCode(err, exception);
}

// FIXME: Guard this change under !VALIDATOR_INTEGRATION.
static void removeComments(const String& inSource, String& outSource)
{
    enum Mode { DEFAULT, BLOCK_COMMENT, LINE_COMMENT };
    Mode currentMode = DEFAULT;

    ASSERT(!inSource.isNull());
    ASSERT(!inSource.isEmpty());

    outSource = inSource;

    for (unsigned i = 0; i < outSource.length(); ++i) {
        if (currentMode == BLOCK_COMMENT) {
            if (outSource[i] == '*' && outSource[i + 1] == '/') {
                outSource.replace(i++, 2, "  ");
                currentMode = DEFAULT;
                continue;
            }
            outSource.replace(i, 1, " ");
            continue;
        }

        if (currentMode == LINE_COMMENT) {
            if (outSource[i] == '\n' || outSource[i] == '\r') {
                currentMode = DEFAULT;
                continue;
            }
            outSource.replace(i, 1, " ");
            continue;
        }

        if(outSource[i] == '/') {
            if(outSource[i + 1] == '*') {
                outSource.replace(i++, 2, "  ");
                currentMode = BLOCK_COMMENT;
                continue;
            }

            if(outSource[i + 1] == '/'){
                outSource.replace(i++, 2, "  ");
                currentMode = LINE_COMMENT;
                continue;
            }
        }
    }
}

const String& WebCLProgram::sourceWithCommentsStripped()
{
    if (m_programSourceWithCommentsStripped.isNull())
        removeComments(m_programSource, m_programSourceWithCommentsStripped);

    return m_programSourceWithCommentsStripped;
}

void WebCLProgram::ccDeviceListFromWebCLDeviceList(const Vector<RefPtr<WebCLDevice> >& devices, Vector<ComputeDevice*>& ccDevices, ExceptionObject& exception)
{
    const Vector<RefPtr<WebCLDevice> >& contextDevices = m_context->devices();
    size_t contextDevicesLength = contextDevices.size();
    bool validDevice;
    for (size_t z, i = 0; i < devices.size(); i++) {
        validDevice = false;
        // Check if the devices[i] is part of programs WebCLContext.
        for (z = 0; z < contextDevicesLength; z++)
            if (contextDevices[z]->platformObject() == devices[i]->platformObject()) {
                validDevice = true;
                break;
            }
        if (!validDevice) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_DEVICE, exception);
            return;
        }
        ccDevices.append(devices[i]->platformObject());
    }
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

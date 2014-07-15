/*
 * Copyright (C) 2011, 2012, 2013 Samsung Electronics Corporation.
 * All rights reserved.
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

#include "WebCLPlatform.h"

#include "ComputePlatform.h"
#include "WebCLGetInfo.h"
#include "WebCLInputChecker.h"

namespace WebCore {

WebCLPlatform::~WebCLPlatform()
{
}

PassRefPtr<WebCLPlatform> WebCLPlatform::create(RefPtr<ComputePlatform> platform)
{
    return adoptRef(new WebCLPlatform(platform));
}

WebCLPlatform::WebCLPlatform(RefPtr<ComputePlatform> platform)
    : WebCLExtensionsAccessor(platform.get())
    , m_platformObject(platform)
    , m_cachedDeviceType(0)
{
}

WebCLGetInfo WebCLPlatform::getInfo(CCenum info, ExceptionObject& exception)
{
    switch(info) {
    case ComputeContext::PLATFORM_PROFILE:
        return WebCLGetInfo(String("WEBCL_PROFILE"));
    case ComputeContext::PLATFORM_VERSION:
        return WebCLGetInfo(String("WebCL 1.0"));
    case ComputeContext::PLATFORM_NAME:
    case ComputeContext::PLATFORM_VENDOR:
    case ComputeContext::PLATFORM_EXTENSIONS:
        return WebCLGetInfo(emptyString());
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT_NOT_REACHED();
    return WebCLGetInfo();
}

Vector<RefPtr<WebCLDevice> > WebCLPlatform::getDevices(CCenum deviceType, ExceptionObject& exception)
{
    if (deviceType && !WebCLInputChecker::isValidDeviceType(deviceType)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_DEVICE_TYPE, exception);
        return Vector<RefPtr<WebCLDevice> >();
    }

    if (!deviceType)
        deviceType = ComputeContext::DEVICE_TYPE_ALL;

    // If cached copy of devices are of same type, return from cache.
    if (m_cachedDeviceType == deviceType && m_webCLDevices.size())
        return m_webCLDevices;

    Vector<RefPtr<ComputeDevice> > ccDevices;
    CCerror error = platformObject()->getDeviceIDs(deviceType, ccDevices);
    if (error != ComputeContext::SUCCESS) {
        setExceptionFromComputeErrorCode(error, exception);
        return Vector<RefPtr<WebCLDevice> >();
    }
    // New device array fetched. Update the cache.
    m_webCLDevices.clear();

    // Store the new device list to cache and also update the type to m_cachedDeviceType.
    toWebCLDeviceArray(this, ccDevices, m_webCLDevices);
    m_cachedDeviceType = deviceType;

    return m_webCLDevices;
}

CCerror getPlatforms(Vector<RefPtr<WebCLPlatform> >& platforms)
{
    Vector<RefPtr<ComputePlatform> > computePlatforms;

    CCerror error = ComputePlatform::getPlatformIDs(computePlatforms);
    if (error != ComputeContext::SUCCESS)
        return error;

    for (size_t i = 0; i < computePlatforms.size(); ++i)
        platforms.append(WebCLPlatform::create(computePlatforms[i]));

    return error;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

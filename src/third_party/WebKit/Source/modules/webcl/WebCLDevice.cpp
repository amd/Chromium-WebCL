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

#include "WebCLDevice.h"

#include "ComputeDevice.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLGetInfo.h"
#include "WebCLInputChecker.h"
#include "WebCLPlatform.h"

namespace WebCore {

WebCLDevice::~WebCLDevice()
{
}

PassRefPtr<WebCLDevice> WebCLDevice::create(RefPtr<ComputeDevice> device, WebCLPlatform* platform)
{
    return adoptRef(new WebCLDevice(device, platform));
}

WebCLDevice::WebCLDevice(RefPtr<ComputeDevice> device, WebCLPlatform* platform)
    : WebCLExtensionsAccessor(device.get())
    , m_device(device)
    , m_platform(platform)
{
}

WebCLGetInfo WebCLDevice::getInfo(CCenum infoType, ExceptionObject& exception)
{
    if (!WebCLInputChecker::isValidDeviceInfoType(infoType)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;
    switch (infoType) {
    case ComputeContext::DEVICE_PROFILE:
        return WebCLGetInfo(String("WEBCL_PROFILE"));
    case ComputeContext::DEVICE_VERSION:
        return WebCLGetInfo(String("WebCL 1.0"));
    case ComputeContext::DEVICE_OPENCL_C_VERSION:
        return WebCLGetInfo(String("WebCL C 1.0"));
    case ComputeContext::DEVICE_VENDOR_ID: // Vendor specific. Will return a empty string.
    case ComputeContext::DRIVER_VERSION:
    case ComputeContext::DEVICE_NAME:
    case ComputeContext::DEVICE_VENDOR:
    case ComputeContext::DEVICE_EXTENSIONS:
        return WebCLGetInfo(emptyString());
    case ComputeContext::DEVICE_IMAGE_SUPPORT:
    case ComputeContext::DEVICE_AVAILABLE:
    case ComputeContext::DEVICE_COMPILER_AVAILABLE:
        return WebCLGetInfo(true);
    case ComputeContext::DEVICE_MAX_WORK_ITEM_SIZES: { // size_t[]
        Vector<size_t> workItemSizes;
        err = platformObject()->getDeviceInfo(infoType, &workItemSizes);
        if (err == ComputeContext::SUCCESS) {
            Vector<CCuint> values;
            for (size_t i = 0; i < workItemSizes.size(); ++i)
                values.append(workItemSizes[i]);
            return WebCLGetInfo(values);
        }
        break;
    }
    case ComputeContext::DEVICE_MAX_WORK_GROUP_SIZE: 
    case ComputeContext::DEVICE_MAX_PARAMETER_SIZE:
    case ComputeContext::DEVICE_PROFILING_TIMER_RESOLUTION:
    case ComputeContext::DEVICE_IMAGE2D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE2D_MAX_WIDTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE3D_MAX_DEPTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_WIDTH: {
        size_t infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCuint>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_MAX_WORK_ITEM_DIMENSIONS:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_MAX_CLOCK_FREQUENCY:
    case ComputeContext::DEVICE_MAX_READ_IMAGE_ARGS:
    case ComputeContext::DEVICE_MAX_WRITE_IMAGE_ARGS:
    case ComputeContext::DEVICE_MAX_SAMPLERS:
    case ComputeContext::DEVICE_MEM_BASE_ADDR_ALIGN:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
    case ComputeContext::DEVICE_MAX_CONSTANT_ARGS:
    case ComputeContext::DEVICE_ADDRESS_BITS:
    case ComputeContext::DEVICE_MAX_COMPUTE_UNITS: {
        CCuint infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCuint>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_LOCAL_MEM_SIZE:
    case ComputeContext::DEVICE_MAX_CONSTANT_BUFFER_SIZE:
    case ComputeContext::DEVICE_GLOBAL_MEM_SIZE:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_SIZE:
    case ComputeContext::DEVICE_MAX_MEM_ALLOC_SIZE: {
        CCulong infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCulong>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_HOST_UNIFIED_MEMORY:
    case ComputeContext::DEVICE_ENDIAN_LITTLE:
    case ComputeContext::DEVICE_ERROR_CORRECTION_SUPPORT: {
        CCbool infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<bool>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_TYPE: {
        CCDeviceMemCachetype type = 0;
        err = platformObject()->getDeviceInfo(infoType, &type);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(type));
        break;
    }
    case ComputeContext::DEVICE_TYPE: {
        CCDeviceType type = 0;
        err = platformObject()->getDeviceInfo(infoType, &type);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(type));
        break;
    }
    case ComputeContext::DEVICE_SINGLE_FP_CONFIG: {
        CCDeviceFPConfig deviceFPConfig = 0;
        err = platformObject()->getDeviceInfo(infoType, &deviceFPConfig);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(deviceFPConfig));
        break;
    }
    case ComputeContext::DEVICE_LOCAL_MEM_TYPE: {
        CCDeviceLocalMemType localMemoryType = 0;
        err = platformObject()->getDeviceInfo(infoType, &localMemoryType);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(localMemoryType));
        break;
    }
    case ComputeContext::DEVICE_QUEUE_PROPERTIES: {
        CCCommandQueueProperties queueProperties = 0;
        err = platformObject()->getDeviceInfo(infoType,  &queueProperties);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(queueProperties));
        break;
    }
    case ComputeContext::DEVICE_EXECUTION_CAPABILITIES:
        return WebCLGetInfo(static_cast<CCenum>(ComputeContext::EXEC_KERNEL));
    case ComputeContext::DEVICE_PLATFORM:
        return WebCLGetInfo(m_platform);
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
    }
    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

void toWebCLDeviceArray(WebCLPlatform* platform, Vector<RefPtr<ComputeDevice> >& ccDevices, Vector<RefPtr<WebCLDevice> >& devices)
{
    for (size_t i = 0; i < ccDevices.size(); i++) {
        RefPtr<WebCLDevice> webCLDevice = WebCLDevice::create(ccDevices[i], platform);
        devices.append(webCLDevice);
    }
}

} // namespace WebCore
#endif // ENABLE(WEBCL)

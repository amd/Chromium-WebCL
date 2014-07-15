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

#include "config.h"
#include "ComputePlatform.h"

#include "ComputeContext.h"

namespace WebCore {

typedef HashMap<CCPlatformID, RefPtr<ComputePlatform> > ComputePlatformPool;
static ComputePlatformPool& computePlatformPool()
{
    static ComputePlatformPool* s_computePlatformPool = new ComputePlatformPool();
    return *s_computePlatformPool;
}

ComputePlatform* ComputePlatform::create(CCPlatformID clPlatform)
{
    if (computePlatformPool().contains(clPlatform))
        return computePlatformPool().get(clPlatform);

    ComputePlatform* platform = new ComputePlatform(clPlatform);
    computePlatformPool().add(clPlatform, adoptRef(platform));
    return platform;
}

ComputePlatform::ComputePlatform(CCPlatformID platform)
    : m_platform(platform)
{
}

CCerror ComputePlatform::getPlatformIDs(Vector<RefPtr<ComputePlatform> >& computePlatforms)
{
    CCuint numberOfPlatforms = 0;
    CCint clError = clGetPlatformIDs(0, 0, &numberOfPlatforms);
    if (clError != CL_SUCCESS)
        return clError;

    Vector<CCPlatformID> clPlatforms;
    //if (!clPlatforms.tryReserveCapacity(numberOfPlatforms))
    //    return OUT_OF_HOST_MEMORY;
    clPlatforms.resize(numberOfPlatforms);

    clError = clGetPlatformIDs(numberOfPlatforms, clPlatforms.data(), 0);
    if (clError != CL_SUCCESS)
        return clError;

    //if (!computePlatforms.tryReserveCapacity(numberOfPlatforms))
    //    return OUT_OF_HOST_MEMORY;
    computePlatforms.resize(numberOfPlatforms);

    for (size_t i = 0; i < numberOfPlatforms; ++i)
        computePlatforms[i] = ComputePlatform::create(clPlatforms[i]);

    return CL_SUCCESS;
}

CCerror ComputePlatform::getDeviceIDs(CCDeviceType deviceType, Vector<RefPtr<ComputeDevice> >& computeDevices)
{
    CCuint numberOfDevices = 0;
    CCint clError = clGetDeviceIDs(m_platform, deviceType, 0, 0, &numberOfDevices);
	// If this type of device is not found, it's not an error
	if (CL_DEVICE_NOT_FOUND == clError && numberOfDevices == 0)
		return CL_SUCCESS;
    if (clError != CL_SUCCESS)
        return clError;

    Vector<CCDeviceID> clDevices;
    //if (!clDevices.tryReserveCapacity(numberOfDevices))
    //    return OUT_OF_HOST_MEMORY;
    clDevices.resize(numberOfDevices);

    clError = clGetDeviceIDs(m_platform, deviceType, numberOfDevices, clDevices.data(), 0);
    if (clError != CL_SUCCESS)
        return clError;

    //if (!computeDevices.tryReserveCapacity(numberOfDevices))
    //    return OUT_OF_HOST_MEMORY;
    computeDevices.resize(numberOfDevices);

    for (size_t i = 0; i < numberOfDevices; ++i)
        computeDevices[i] = ComputeDevice::create(clDevices[i]);

    return clError;
}

CCerror ComputePlatform::getPlatformInfoBase(CCPlatformID platform, CCPlatformInfoType infoType, size_t sizeOfData, void *data, size_t* actualSize)
{
    return clGetPlatformInfo(platform, infoType, sizeOfData, data, actualSize);
}

}

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
#include "ComputeProgram.h"

#include "ComputeContext.h"
#include "ComputeKernel.h"

namespace WebCore {

ComputeProgram::ComputeProgram(ComputeContext* context, const String& programSource, CCerror& error)
{
    const CString& programSourceCString = programSource.utf8();
    const char* programSourcePtr = programSourceCString.data();
    m_program = clCreateProgramWithSource(context->context(), 1, &programSourcePtr, 0, &error);
}

ComputeProgram::~ComputeProgram()
{
    clReleaseProgram(m_program);
}

CCerror ComputeProgram::buildProgram(const Vector<ComputeDevice*>& devices, const String& options, pfnNotify notifyFunction, void* userData)
{
    Vector<CCDeviceID> clDevices;
    for (size_t i = 0; i < devices.size(); ++i)
        clDevices.append(devices[i]->device());

    const CString& optionsCString = options.utf8();
    const char* optionsPtr = optionsCString.data();
    return clBuildProgram(m_program, devices.size(), clDevices.data(), optionsPtr, (void (__stdcall *)(cl_program,void *))notifyFunction, userData);
}

ComputeKernel* ComputeProgram::createKernel(const String& kernelName, CCerror& error)
{
    return new ComputeKernel(this, kernelName.utf8().data(), error);
}

Vector<ComputeKernel*> ComputeProgram::createKernelsInProgram(CCerror& error)
{
    Vector<ComputeKernel* > computeKernels;

    CCuint numberOfKernels = 0;
    Vector<CCKernel> kernels;
    error = clCreateKernelsInProgram(m_program, 0, 0, &numberOfKernels);
    if (error != CL_SUCCESS)
        return computeKernels;

    if (!numberOfKernels) {
        // FIXME: Having '0' kernels is an error?
        return computeKernels;
    }
    //if (!kernels.tryReserveCapacity(numberOfKernels)) {
    //    error = OUT_OF_HOST_MEMORY;
    //    return computeKernels;
    //}
    kernels.resize(numberOfKernels);

    error = clCreateKernelsInProgram(m_program, numberOfKernels, kernels.data(), 0);

    computeKernels.resize(numberOfKernels);
    for (size_t i = 0; i < numberOfKernels; ++i)
        computeKernels[i] = new ComputeKernel(kernels[i]);

    return computeKernels;
}

CCerror ComputeProgram::getProgramInfoBase(CCProgram program, CCProgramInfoType infoType, size_t sizeOfData, void* data, size_t* actualSizeOfData)
{
    return clGetProgramInfo(program, infoType, sizeOfData, data, actualSizeOfData);
}

CCerror ComputeProgram::getBuildInfoBase(CCProgram program, CCDeviceID device, CCProgramBuildInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetProgramBuildInfo(program, device, infoType, sizeOfData, data, retSize);
}

CCerror ComputeProgram::release()
{
    return clReleaseProgram(m_program);
}

}

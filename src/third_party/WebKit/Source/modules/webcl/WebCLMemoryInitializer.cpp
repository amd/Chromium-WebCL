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
#include "WebCLMemoryInitializer.h"

#include "ComputeContext.h"
#include "ComputeDevice.h"
#include "ComputeKernel.h"
#include "ComputeMemoryObject.h"
#include "ComputeProgram.h"
#include "WebCLBuffer.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLMemoryObject.h"

#if ENABLE(WEBCL)

namespace WebCore {

#define RETURN_IF_ERROR(errorCode)                                               \
{                                                                                \
    if (errorCode != ComputeContext::SUCCESS) {                                  \
        setExceptionFromComputeErrorCode(errorCode, exception);                  \
        return;                                                                  \
    }                                                                            \
}

const char* programSource = \
    "__kernel void init(__global char* buffer, unsigned offset, unsigned count) "\
    "{                                                                          "\
    "    unsigned i = get_global_id(0);                                         "\
    "    unsigned displacedI = i + offset;                                      "\
    "    if (displacedI < count)                                                "\
    "        buffer[displacedI] = (char)(0);                                    "\
    "}                                                                          "\
    "__kernel void init16(__global char16* buffer, unsigned count)              "\
    "{                                                                          "\
    "    unsigned i = get_global_id(0);                                         "\
    "    if (i < count)                                                         "\
    "        buffer[i] = (char16)(0);                                           "\
    "}                                                                          ";

WebCLMemoryInitializer::WebCLMemoryInitializer(WebCLContext* context)
    : m_context(context)
    , m_program(0)
    , m_kernelChar(0)
    , m_kernelChar16(0)
{
}

WebCLMemoryInitializer::~WebCLMemoryInitializer()
{
    // FIXME: Use OwnPtr.
    delete m_program;
    delete m_kernelChar;
    delete m_kernelChar16;
}

void WebCLMemoryInitializer::ensureMemoryInitialization(WebCLMemoryObject* memoryObject, WebCLCommandQueue* commandQueue, ExceptionObject& exception)
{
    CCerror error = ComputeContext::SUCCESS;

    if (!m_program) {
        m_program = new ComputeProgram(m_context->computeContext(), programSource, error);
        RETURN_IF_ERROR(error);

        Vector<ComputeDevice*> computeDevices;
        for (size_t i = 0; i < m_context->devices().size(); ++i)
            computeDevices.append(m_context->devices()[i]->platformObject());
        error = m_program->buildProgram(computeDevices, emptyString(), 0 /*pfnNotify*/, 0 /*userData*/);
        RETURN_IF_ERROR(error);

        m_kernelChar16 = new ComputeKernel(m_program, "init16", error);
        RETURN_IF_ERROR(error);

        m_kernelChar = new ComputeKernel(m_program, "init", error);
        RETURN_IF_ERROR(error);
    }

    unsigned count = memoryObject->sizeInBytes() / 16;
    if (count) {
        error = m_kernelChar16->setKernelArg(0, memoryObject->platformObject());
        RETURN_IF_ERROR(error);

        error = m_kernelChar16->setKernelArg(1, sizeof(unsigned), &count);
        RETURN_IF_ERROR(error);

        Vector<size_t> globalWorkSize;
        globalWorkSize.append(count);
        Vector<size_t> globalWorkOffset;
        Vector<size_t> localWorkSize;

        error = commandQueue->platformObject()->enqueueNDRangeKernel(m_kernelChar16, globalWorkSize.size(), globalWorkOffset, globalWorkSize, localWorkSize, Vector<ComputeEvent*>(), 0 /*ComputeEvent*/);
        RETURN_IF_ERROR(error);
    }

    unsigned remainingBytes = memoryObject->sizeInBytes() % 16;
    if (remainingBytes) {
        error = m_kernelChar->setKernelArg(0, memoryObject->platformObject());
        RETURN_IF_ERROR(error);

        unsigned offset = count * 16;
        error = m_kernelChar->setKernelArg(1, sizeof(unsigned), &offset);
        RETURN_IF_ERROR(error);

        unsigned totalSize = memoryObject->sizeInBytes();
        error = m_kernelChar->setKernelArg(2, sizeof(unsigned), &totalSize);
        RETURN_IF_ERROR(error);

        Vector<size_t> globalWorkSize;
        globalWorkSize.append(remainingBytes);
        Vector<size_t> globalWorkOffset;
        Vector<size_t> localWorkSize;

        error = commandQueue->platformObject()->enqueueNDRangeKernel(m_kernelChar, globalWorkSize.size(), globalWorkOffset, globalWorkSize, localWorkSize, Vector<ComputeEvent*>(), 0 /*ComputeEvent*/);
        RETURN_IF_ERROR(error);
    }

    error = commandQueue->platformObject()->finish();
    RETURN_IF_ERROR(error);
}

void WebCLMemoryInitializer::commandQueueCreated(WebCLCommandQueue* queue, ExceptionObject& exception)
{
    if (!queue)
        return;

    m_queuesForMemoryInitialization.append(queue->createWeakPtrForLazyInitialization());
    processPendingMemoryInitializationList(exception);
}

void WebCLMemoryInitializer::bufferCreated(WebCLBuffer* buffer, ExceptionObject& exception)
{
    if (!buffer)
        return;

    initializeOrQueueMemoryInitializationOfMemoryObject(buffer, exception);
}

void WebCLMemoryInitializer::processPendingMemoryInitializationList(ExceptionObject& exception)
{
    if (!m_buffersPendingMemoryInitialization.size())
        return;

    WebCLCommandQueue* queue = validCommandQueueForMemoryInitialization();
    for (size_t i = 0; queue && i < m_buffersPendingMemoryInitialization.size(); ++i) {
        WebCLBuffer* buffer = m_buffersPendingMemoryInitialization[i].get();
        if (buffer && !buffer->isPlatformObjectNeutralized())
            ensureMemoryInitialization(buffer, queue, exception);
    }
    m_buffersPendingMemoryInitialization.clear();
}

WebCLCommandQueue* WebCLMemoryInitializer::validCommandQueueForMemoryInitialization() const
{
    for (size_t i = 0; i < m_queuesForMemoryInitialization.size(); ++i) {
        WebCLCommandQueue* queue = m_queuesForMemoryInitialization[i].get();
        if (queue && !queue->isPlatformObjectNeutralized())
            return queue;
    }
    return 0;
}

void WebCLMemoryInitializer::initializeOrQueueMemoryInitializationOfMemoryObject(WebCLBuffer* buffer, ExceptionObject& exception)
{
    WebCLCommandQueue* queue = validCommandQueueForMemoryInitialization();
    if (!queue) {
        m_buffersPendingMemoryInitialization.append(buffer->createWeakPtrForLazyInitialization());
        return;
    }

    ensureMemoryInitialization(buffer, queue, exception);
}

} // namespace WebCore

#endif

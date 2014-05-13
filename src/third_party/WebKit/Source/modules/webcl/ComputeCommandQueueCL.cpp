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

#include "ComputeCommandQueue.h"

#include "ComputeContext.h"
#include "ComputeDevice.h"
#include "ComputeEvent.h"
#include "ComputeMemoryObject.h"
#include "ComputeKernel.h"

//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace WebCore {

ComputeCommandQueue::ComputeCommandQueue(ComputeContext* context, ComputeDevice* device, CCCommandQueueProperties properties, CCerror& error)
{
    m_commandQueue = clCreateCommandQueue(context->context(), device->device(), properties, &error);
}

ComputeCommandQueue::~ComputeCommandQueue()
{
    clReleaseCommandQueue(m_commandQueue);
}

static CCEvent* ccEventFromComputeEvent(ComputeEvent* event)
{
    if (!event)
        return 0;

    CCEvent* ccEvent = &event->eventRef();
    return ccEvent;
}

static void ccEventListFromComputeEventList(const Vector<ComputeEvent*>& in, Vector<CCEvent>& out)
{
    for (size_t i = 0; i < in.size(); ++i)
        out.append(in[i]->event());
}

CCerror ComputeCommandQueue::enqueueNDRangeKernel(ComputeKernel* kernel, CCuint workItemDimensions,
    const Vector<size_t>& globalWorkOffset, const Vector<size_t>& globalWorkSize, const Vector<size_t>& localWorkSize,
    const Vector<ComputeEvent*>& eventWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventWaitList, ccEventList);

    return clEnqueueNDRangeKernel(m_commandQueue, kernel->kernel(), workItemDimensions,
        globalWorkOffset.data(), globalWorkSize.data(), localWorkSize.data(), ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueBarrier()
{
    CCint error;
#if defined(CL_VERSION_1_2) && CL_VERSION_1_2
    error = clEnqueueBarrierWithWaitList(m_commandQueue, 0 /*eventsWaitListLength*/, 0 /*eventsWaitList*/, 0 /*event*/);
#else
    error = clEnqueueBarrier(m_commandQueue);
#endif
    return error;
}

CCerror ComputeCommandQueue::enqueueMarker(ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    CCint error;
#if defined(CL_VERSION_1_2) && CL_VERSION_1_2
    error = clEnqueueMarkerWithWaitList(m_commandQueue, 0 /*eventsWaitListLength*/, 0 /*eventsWaitList*/, ccEvent);
#else
    error = clEnqueueMarker(m_commandQueue, ccEvent);
#endif
    return error;
}

CCerror ComputeCommandQueue::enqueueTask(ComputeKernel* kernel, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueTask(m_commandQueue, kernel->kernel(), ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueWriteBuffer(ComputeMemoryObject* buffer, CCbool blockingWrite,
    size_t offset, size_t bufferSize, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueWriteBuffer(m_commandQueue, buffer->memoryObject(), blockingWrite, offset,
        bufferSize, baseAddress, ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueWriteBufferRect(ComputeMemoryObject* buffer, CCbool blockingWrite,
    const Vector<size_t>& bufferOriginArray, const Vector<size_t>& hostOriginArray, const Vector<size_t>& regionArray, size_t bufferRowPitch,
    size_t bufferSlicePitch, size_t hostRowPitch, size_t hostSlicePitch, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueWriteBufferRect(m_commandQueue, buffer->memoryObject(), blockingWrite, bufferOriginArray.data(), hostOriginArray.data(), regionArray.data(),
        bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, baseAddress, ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueReadBuffer(ComputeMemoryObject* buffer, CCbool blockingRead, size_t offset, size_t bufferSize, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueReadBuffer(m_commandQueue, buffer->memoryObject(), blockingRead, offset,
        bufferSize, baseAddress, ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueReadBufferRect(ComputeMemoryObject* buffer, CCbool blockingRead,
    const Vector<size_t>& bufferOriginArray, const Vector<size_t>& hostOriginArray, const Vector<size_t>& regionArray, size_t bufferRowPitch,
    size_t bufferSlicePitch, size_t hostRowPitch, size_t hostSlicePitch, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueReadBufferRect(m_commandQueue, buffer->memoryObject(), blockingRead, bufferOriginArray.data(), hostOriginArray.data(), regionArray.data(),
        bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, baseAddress, ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueReadImage(ComputeMemoryObject* image, CCbool blockingRead, const Vector<size_t>& originArray,
    const Vector<size_t>& regionArray, size_t rowPitch, size_t slicePitch, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueReadImage(m_commandQueue, image->memoryObject(), blockingRead, originArray.data(), regionArray.data(), rowPitch, slicePitch, baseAddress,
        ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueWriteImage(ComputeMemoryObject* image, CCbool blockingWrite, const Vector<size_t>& originArray,
    const Vector<size_t>& regionArray, size_t rowPitch, size_t slicePitch, void* baseAddress, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueWriteImage(m_commandQueue, image->memoryObject(), blockingWrite, originArray.data(), regionArray.data(), rowPitch, slicePitch, baseAddress,
        ccEventList.size(), ccEventList.data(), ccEvent);
}

static void ccMemoryObjectListFromComputeMemoryObjectList(const Vector<ComputeMemoryObject*>& in, Vector<PlatformComputeObject>& out)
{
    for (size_t i = 0; i < in.size(); ++i)
        out.append(in[i]->memoryObject());
}

CCerror ComputeCommandQueue::enqueueAcquireGLObjects(const Vector<ComputeMemoryObject*>& objects, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    Vector<PlatformComputeObject> ccMemoryObjectList;
    ccMemoryObjectListFromComputeMemoryObjectList(objects, ccMemoryObjectList);

    return clEnqueueAcquireGLObjects(m_commandQueue, ccMemoryObjectList.size(), ccMemoryObjectList.data(), ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueReleaseGLObjects(const Vector<ComputeMemoryObject*>& objects, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    Vector<PlatformComputeObject> ccMemoryObjectList;
    ccMemoryObjectListFromComputeMemoryObjectList(objects, ccMemoryObjectList);

    return clEnqueueReleaseGLObjects(m_commandQueue, ccMemoryObjectList.size(), ccMemoryObjectList.data(), ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueCopyImage(ComputeMemoryObject* originImage, ComputeMemoryObject* targetImage,
    const Vector<size_t>& sourceOriginArray, const Vector<size_t>& targetOriginArray, const Vector<size_t>& regionArray,
    const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueCopyImage(m_commandQueue, originImage->memoryObject(), targetImage->memoryObject(), sourceOriginArray.data(), targetOriginArray.data(), regionArray.data(),
        ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueCopyImageToBuffer(ComputeMemoryObject* sourceImage, ComputeMemoryObject* targetBuffer,
    const Vector<size_t>& sourceOriginArray, const Vector<size_t>& regionArray, size_t targetOffset, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueCopyImageToBuffer(m_commandQueue, sourceImage->memoryObject(), targetBuffer->memoryObject(), sourceOriginArray.data(), regionArray.data(), targetOffset,
        ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueCopyBufferToImage(ComputeMemoryObject* sourceBuffer, ComputeMemoryObject* targetImage,
    size_t srcOffset, const Vector<size_t>& targetOriginArray, const Vector<size_t>& regionArray, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueCopyBufferToImage(m_commandQueue, sourceBuffer->memoryObject(), targetImage->memoryObject(), srcOffset, targetOriginArray.data(), regionArray.data(),
        ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueCopyBuffer(ComputeMemoryObject* sourceBuffer, ComputeMemoryObject* targetBuffer,
    size_t sourceOffset, size_t targetOffset, size_t sizeInBytes, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueCopyBuffer(m_commandQueue, sourceBuffer->memoryObject(), targetBuffer->memoryObject(), sourceOffset, targetOffset, sizeInBytes,
        ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueCopyBufferRect(ComputeMemoryObject* sourceBuffer, ComputeMemoryObject* targetBuffer,
    const Vector<size_t>& sourceOriginArray, const Vector<size_t>& targetOriginArray, const Vector<size_t>& regionArray, size_t sourceRowPitch,
    size_t sourceSlicePitch, size_t targetRowPitch, size_t targetSlicePitch, const Vector<ComputeEvent*>& eventsWaitList, ComputeEvent* event)
{
    CCEvent* ccEvent = ccEventFromComputeEvent(event);

    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(eventsWaitList, ccEventList);

    return clEnqueueCopyBufferRect(m_commandQueue, sourceBuffer->memoryObject(), targetBuffer->memoryObject(), sourceOriginArray.data(), targetOriginArray.data(), regionArray.data(),
        sourceRowPitch, sourceSlicePitch, targetRowPitch, targetSlicePitch, ccEventList.size(), ccEventList.data(), ccEvent);
}

CCerror ComputeCommandQueue::enqueueWaitForEvents(const Vector<ComputeEvent*>& events)
{
    Vector<CCEvent> ccEventList;
    ccEventListFromComputeEventList(events, ccEventList);

    return clEnqueueWaitForEvents(m_commandQueue, ccEventList.size(), ccEventList.data());
}

CCerror ComputeCommandQueue::releaseCommandQueue()
{
    return clReleaseCommandQueue(m_commandQueue);
}

CCerror ComputeCommandQueue::finish()
{
    return clFinish(m_commandQueue);
}

CCerror ComputeCommandQueue::flush()
{
    return clFlush(m_commandQueue);
}

CCerror ComputeCommandQueue::getCommandQueueInfoBase(CCCommandQueue queue, CCCommandQueueInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetCommandQueueInfo(queue, infoType, sizeOfData, data, retSize);
}


}

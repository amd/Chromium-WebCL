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

#include "WebCLCommandQueue.h"

#include "ComputeDevice.h"
#include "ComputeEvent.h"
#include "ComputeMemoryObject.h"
#include "WebCLBuffer.h"
#include "WebCLContext.h"
#include "WebCLEvent.h"
#include "WebCLGetInfo.h"
#include "WebCLHTMLInterop.h"
#include "WebCLImage.h"
#include "WebCLImageDescriptor.h"
#include "WebCLInputChecker.h"
#include "WebCLKernel.h"
#include "WebCLMemoryObject.h"
#include "WebCLProgram.h"
#include "../../wtf/Int32Array.h"

namespace WebCore {

WebCLCommandQueue::~WebCLCommandQueue()
{
    releasePlatformObject();
}

PassRefPtr<WebCLCommandQueue> WebCLCommandQueue::create(WebCLContext* context, CCenum properties, WebCLDevice* webCLDevice, ExceptionObject& exception)
{
    CCerror error = ComputeContext::SUCCESS;
    ComputeCommandQueue* computeCommandQueue = context->computeContext()->createCommandQueue(webCLDevice->platformObject(), properties, error);
    if (error != ComputeContext::SUCCESS) {
        delete computeCommandQueue;
        setExceptionFromComputeErrorCode(error, exception);
        return 0;
    }

    RefPtr<WebCLCommandQueue> queue = adoptRef(new WebCLCommandQueue(context, computeCommandQueue, webCLDevice));
    return queue.release();
}

WebCLCommandQueue::WebCLCommandQueue(WebCLContext* context, ComputeCommandQueue *computeCommandQueue, WebCLDevice* webCLDevice)
    : WebCLObjectImpl(computeCommandQueue)
    , m_context(context)
    , m_device(webCLDevice)
    , m_weakFactoryForLazyInitialization(this)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

WebCLGetInfo WebCLCommandQueue::getInfo(CCenum paramName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;

    switch (paramName) {
    case ComputeContext::QUEUE_CONTEXT:
        return WebCLGetInfo(m_context.get());
    case ComputeContext::QUEUE_DEVICE:
        return WebCLGetInfo(m_device.get());
    case ComputeContext::QUEUE_PROPERTIES: {
        CCCommandQueueProperties ccCommandQueueProperties = 0;
        err = platformObject()->commandQueueInfo(paramName, &ccCommandQueueProperties);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(ccCommandQueueProperties));
        break;
    }
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

void WebCLCommandQueue::ccEventListFromWebCLEventList(const Vector<RefPtr<WebCLEvent> >& events, Vector<ComputeEvent*>& computeEvents, ExceptionObject& exception, WebCLToCCEventsFilterCriteria criteria)
{
    for (size_t i = 0; i < events.size(); ++i) {
        RefPtr<WebCLEvent> event = events[i];
        if (event->isPlatformObjectNeutralized()
            || !event->holdsValidCLObject()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
            return;
        }

        if (criteria == DoNotAcceptUserEvent && event->isUserEvent()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
            return;
        }

        ASSERT(event->context());
        if (!WebCLInputChecker::compareContext(event->context(), m_context.get())) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
            return;
        }

        computeEvents.append(event->platformObject());
    }
}

ComputeEvent* WebCLCommandQueue::computeEventFromWebCLEventIfApplicable(WebCLEvent* event, ExceptionObject& exception)
{
    if (!event)
        return 0;

    // Throw an exception if:
    // #1 - Event has been released.
    // #2 - Event has been used before
    // #3 - Event is a user event.
    if (event->isPlatformObjectNeutralized()
        || event->holdsValidCLObject()
        || event->isUserEvent()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return 0;
    }

    event->setAssociatedCommandQueue(this);
    return event->platformObject();
}

 __declspec(dllexport) long long g_hostPtrSize = 0;

void WebCLCommandQueue::enqueueWriteBufferBase(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, CCuint numBytes, void* hostPtr, size_t hostPtrLength,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
	if (!numBytes) // to be robust
		return;

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_HOST_PTR, exception);
        return;
    }

    if (hostPtrLength < numBytes
        || buffer->sizeInBytes() < (offset + numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror error = platformObject()->enqueueWriteBuffer(computeMemory, blockingWrite, offset, numBytes, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(error, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, CCuint numBytes, ArrayBufferView* ptr,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(numBytes, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteBufferBase(buffer, blockingWrite, offset, numBytes, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, ImageData* srcPixels,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, pixelSize, hostPtr, pixelSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, HTMLCanvasElement* srcCanvas,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, canvasSize, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, HTMLImageElement* srcImage,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, imageSize, hostPtr, imageSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRectBase(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin,
    const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    g_hostPtrSize= hostPtrLength;

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (bufferOrigin.size() != 3 || hostOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<size_t> bufferOriginCopy, hostOriginCopy, regionCopy;
    bufferOriginCopy.appendVector(bufferOrigin);
    hostOriginCopy.appendVector(hostOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(bufferOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, buffer->sizeInBytes())
        || !WebCLInputChecker::isValidRegionForMemoryObject(hostOriginCopy, regionCopy, hostRowPitch, hostSlicePitch, hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueWriteBufferRect(computeMemory, blockingWrite, bufferOriginCopy,
        hostOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin,
    const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidDataSizeForArrayBufferView(hostSlicePitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch,
        ptr->baseAddress(), ptr->byteLength(), eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    ImageData* srcPixels, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, 0 /* hostRowPitch */, 0 /* hostSlicePitch */,
        hostPtr, pixelSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    HTMLCanvasElement* srcCanvas, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        0 /* hostRowPitch */, 0 /* hostSlicePitch */, hostPtr, canvasSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    HTMLImageElement* srcImage, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, 0 /* hostRowPitch */, 0 /*hostSlicePitch*/,
        hostPtr, imageSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferBase(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes, void* hostPtr, size_t hostPtrLength,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (hostPtrLength < numBytes
        || buffer->sizeInBytes() < (offset + numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = buffer->platformObject();

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadBuffer(computeMemory, blockingRead, offset, numBytes, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(numBytes, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    enqueueReadBufferBase(buffer, blockingRead, offset, numBytes, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes,
    HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadBufferBase(buffer, blockingRead, offset, numBytes, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueReadImageBase(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    CCuint hostRowPitch, void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    g_hostPtrSize = hostPtrLength;

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), image->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(image)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    if (origin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(image->imageDescriptor(), origin, region)
        || !WebCLInputChecker::isValidRegionForHostPtr(region, hostRowPitch, image->imageDescriptor(), hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = image->platformObject();

    Vector<size_t> originCopy, regionCopy;
    originCopy.appendVector(origin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    originCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadImage(computeMemory, blockingRead, originCopy, regionCopy, hostRowPitch, 0 /* slice_pitch */, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadImage(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    CCuint hostRowPitch, ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    enqueueReadImageBase(image, blockingRead, origin, region, hostRowPitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueReadImage(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadImageBase(image, blockingRead, origin, region, 0 /* rowPitch */, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferRectBase(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
	g_hostPtrSize = hostPtrLength;

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (bufferOrigin.size() != 3 || hostOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<size_t> bufferOriginCopy, hostOriginCopy, regionCopy;
    bufferOriginCopy.appendVector(bufferOrigin);
    hostOriginCopy.appendVector(hostOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(hostOriginCopy, regionCopy, hostRowPitch, hostSlicePitch, hostPtrLength)
        || !WebCLInputChecker::isValidRegionForMemoryObject(bufferOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, buffer->sizeInBytes())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadBufferRect(computeMemory, blockingRead, bufferOriginCopy, hostOriginCopy,
        regionCopy, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)
        || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostSlicePitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    enqueueReadBufferRectBase(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        hostRowPitch, hostSlicePitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadBufferRectBase(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        0 /* hostRowPitch */, 0 /* hostSlicePitch */, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, CCuint workDim, const Vector<unsigned>& globalWorkOffsets,
    const Vector<unsigned>& globalWorkSize, const Vector<unsigned>& localWorkSize, const Vector<RefPtr<WebCLEvent> >& events,
    WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(kernel)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_KERNEL, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), kernel->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }
    ComputeKernel* computeKernel = kernel->computeKernel();

    if (workDim > 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_DIMENSION, exception);
        return;
    }

    if (workDim != globalWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_WORK_SIZE, exception);
        return;
    }

    if (globalWorkOffsets.size() && workDim != globalWorkOffsets.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_OFFSET, exception);
        return;
    }
    if (localWorkSize.size() && workDim != localWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_GROUP_SIZE, exception);
        return;
    }

    // FIXME :: Need to add validation if user sent value in each of globalWorkSize, globalWorkOffset and localWorkSize
    // array are valid (not more than 2^32 -1). Currently it is auto clamped by webkit.

    Vector<size_t> globalWorkSizeCopy, localWorkSizeCopy, globalWorkOffsetCopy;
    globalWorkSizeCopy.appendVector(globalWorkSize);
    globalWorkOffsetCopy.appendVector(globalWorkOffsets);
    localWorkSizeCopy.appendVector(localWorkSize);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror computeContextError = platformObject()->enqueueNDRangeKernel(computeKernel,
        workDim, globalWorkOffsetCopy, globalWorkSizeCopy, localWorkSizeCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::enqueueWaitForEvents(const Vector<RefPtr<WebCLEvent> >& events, ExceptionObject& exception)
{
    if (!events.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    CCerror error = platformObject()->enqueueWaitForEvents(computeEvents);
    setExceptionFromComputeErrorCode(error, exception);
}

void WebCLCommandQueue::finish(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->finish();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}


void WebCLCommandQueue::flush(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->flush();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::enqueueWriteImageBase(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    CCuint hostRowPitch, void* hostPtr, const size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
	g_hostPtrSize = hostPtrLength;

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), image->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(image)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (origin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(image->imageDescriptor(), origin, region)
        || !WebCLInputChecker::isValidRegionForHostPtr(region, hostRowPitch, image->imageDescriptor(), hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = image->platformObject();

    Vector<size_t> originCopy, regionCopy;
    originCopy.appendVector(origin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    originCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueWriteImage(computeMemory, blockingWrite, originCopy, regionCopy, hostRowPitch, 0 /* input_slice_pitch */, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    CCuint hostRowPitch, ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteImageBase(image, blockingWrite, origin, region, hostRowPitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    ImageData* srcPixels, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, pixelSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    HTMLCanvasElement* srcCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    HTMLImageElement* srcImage , const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, imageSize, events, event, exception);
}

void WebCLCommandQueue::enqueueCopyImage(WebCLImage* sourceImage, WebCLImage* targetImage, const Vector<unsigned>& sourceOrigin,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceImage->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetImage->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceImage)
        || !WebCLInputChecker::validateWebCLObject(targetImage)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareImageFormat(sourceImage->imageFormat(), targetImage->imageFormat())) {
        setExceptionFromComputeErrorCode(ComputeContext::IMAGE_FORMAT_MISMATCH, exception);
        return;
    }

    if (sourceOrigin.size() != 2 || targetOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(sourceImage->imageDescriptor(), sourceOrigin, region)
        || !WebCLInputChecker::isValidRegionForImage(targetImage->imageDescriptor(), targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
    }

    ComputeMemoryObject* computeSourceImage = sourceImage->platformObject();
    ComputeMemoryObject* computeTargetImage = targetImage->platformObject();

    Vector<size_t> sourceOriginCopy, targetOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    sourceOriginCopy.append(0);
    targetOriginCopy.append(0);
    regionCopy.append(1);

    if (WebCLInputChecker::isRegionOverlapping(sourceImage, targetImage, sourceOrigin, targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyImage(computeSourceImage, computeTargetImage, sourceOriginCopy, targetOriginCopy, regionCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyImageToBuffer(WebCLImage* sourceImage, WebCLBuffer* targetBuffer, const Vector<unsigned>&
    sourceOrigin, const Vector<unsigned>& region, CCuint targetOffset, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceImage->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceImage)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (sourceOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForBuffer(targetBuffer->sizeInBytes(), region, targetOffset, sourceImage->imageDescriptor())
        || !WebCLInputChecker::isValidRegionForImage(sourceImage->imageDescriptor(), sourceOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeSourceImage = sourceImage->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    Vector<size_t> sourceOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    sourceOriginCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyImageToBuffer(computeSourceImage, ccTargetBuffer,
        sourceOriginCopy, regionCopy, targetOffset, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBufferToImage(WebCLBuffer* sourceBuffer, WebCLImage* targetImage, CCuint sourceOffset,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetImage)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetImage->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (targetOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForBuffer(sourceBuffer->sizeInBytes(), region, sourceOffset, targetImage->imageDescriptor())
        || !WebCLInputChecker::isValidRegionForImage(targetImage->imageDescriptor(), targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* computeTargetImage = targetImage->platformObject();

    Vector<size_t> targetOriginCopy, regionCopy;
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    targetOriginCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBufferToImage(ccSourceBuffer, computeTargetImage,
        sourceOffset, targetOriginCopy, regionCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBuffer(WebCLBuffer* sourceBuffer, WebCLBuffer* targetBuffer, CCuint sourceOffset,
    CCuint targetOffset, CCuint sizeInBytes, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (WebCLInputChecker::isRegionOverlapping(sourceBuffer, targetBuffer, sourceOffset, targetOffset, sizeInBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    if ((sourceOffset + sizeInBytes) > sourceBuffer->sizeInBytes()
        || (targetOffset + sizeInBytes) > targetBuffer->sizeInBytes()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBuffer(ccSourceBuffer, ccTargetBuffer,
        sourceOffset, targetOffset, sizeInBytes, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBufferRect(WebCLBuffer* sourceBuffer, WebCLBuffer* targetBuffer, const Vector<unsigned>& sourceOrigin,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, CCuint sourceRowPitch, CCuint sourceSlicePitch, CCuint targetRowPitch,
    CCuint targetSlicePitch, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    if (sourceOrigin.size() != 3 || targetOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    size_t sourceOffset = sourceOrigin[2] * sourceSlicePitch + sourceOrigin[1] * sourceRowPitch + sourceOrigin[0];
    size_t targetOffset = targetOrigin[2] * targetSlicePitch + targetOrigin[1] * targetRowPitch + targetOrigin[0];
    size_t numBytes = region[2] * region[1] * region[0];
    if (WebCLInputChecker::isRegionOverlapping(sourceBuffer, targetBuffer, sourceOffset, targetOffset, numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    Vector<size_t> sourceOriginCopy, targetOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(sourceOriginCopy, regionCopy, sourceRowPitch, sourceSlicePitch, sourceBuffer->sizeInBytes())
        || !WebCLInputChecker::isValidRegionForMemoryObject(targetOriginCopy, regionCopy, targetSlicePitch, targetRowPitch, targetBuffer->sizeInBytes())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }


    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBufferRect(ccSourceBuffer, ccTargetBuffer,
        sourceOriginCopy, targetOriginCopy, regionCopy, sourceRowPitch, sourceSlicePitch, targetRowPitch, targetSlicePitch, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueBarrier(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->enqueueBarrier();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::enqueueMarker(WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(event)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->enqueueMarker(computeEvent);
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

bool WebCLCommandQueue::isExtensionEnabled(WebCLContext* context, const String& name) const
{
    return context->isExtensionEnabled(name);
};

#if ENABLE(WEBGL)
void WebCLCommandQueue::enqueueAcquireGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& memoryObjects, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
	if (this->context()->computeContext()->graphicsContext3D()) this->context()->computeContext()->graphicsContext3D()->flush();
    if (!isExtensionEnabled(m_context.get(), "KHR_gl_sharing")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    Vector<ComputeMemoryObject*> computeMemoryObjects;
    for (size_t i = 0; i < memoryObjects.size(); ++i) {
        if (!memoryObjects[i]->sharesGLResources()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_GL_OBJECT, exception);
            return;
        }
        computeMemoryObjects.append(memoryObjects[i]->platformObject());
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueAcquireGLObjects(computeMemoryObjects, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReleaseGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& memoryObjects, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
	if (this->context()->computeContext()->graphicsContext3D()) this->context()->computeContext()->graphicsContext3D()->flush();
    if (!isExtensionEnabled(m_context.get(), "KHR_gl_sharing")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    Vector<ComputeMemoryObject*> computeMemoryObjects;
    for (size_t i = 0; i < memoryObjects.size(); ++i) {
        if (!memoryObjects[i]->sharesGLResources()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_GL_OBJECT, exception);
            return;
        }
        computeMemoryObjects.append(memoryObjects[i]->platformObject());
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReleaseGLObjects(computeMemoryObjects, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

////
void WebCLCommandQueue::enqueueAcquireGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& objs, ExceptionObject& es) {
	enqueueAcquireGLObjects(objs,  Vector<RefPtr<WebCLEvent> >(), NULL, es);
}
void WebCLCommandQueue::enqueueReleaseGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& objs, ExceptionObject& es) {
	enqueueReleaseGLObjects(objs,  Vector<RefPtr<WebCLEvent> >(), NULL, es);
}
#endif


    ////
    void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buf, CCbool blockingWrite, CCuint bufferOffset, CCuint numBytes, ArrayBufferView* arrayBuf, ExceptionObject& es) {
	    enqueueWriteBuffer(buf, blockingWrite, bufferOffset, numBytes, arrayBuf, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }
    void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buf, CCbool blockingWrite, CCuint bufferOffset, ImageData* data, ExceptionObject& es) {
	    enqueueWriteBuffer(buf, blockingWrite, bufferOffset, data, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }
    void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buf, CCbool blockingWrite, CCuint bufferOffset, HTMLCanvasElement* data, ExceptionObject& es) {
	    enqueueWriteBuffer(buf, blockingWrite, bufferOffset, data, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }
    void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buf, CCbool blockingWrite, CCuint bufferOffset, HTMLImageElement* data, ExceptionObject& es) {
	    enqueueWriteBuffer(buf, blockingWrite, bufferOffset, data, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }

    void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer*clbuf, CCbool blockingWrite, CCuint bufferOffset, CCuint numBytes, ArrayBufferView*abuf,
        ExceptionObject&es) {
			enqueueReadBuffer(clbuf, blockingWrite, bufferOffset, numBytes, abuf, Vector<RefPtr<WebCLEvent> >(), NULL, es);
	}
    void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer*clbuf, CCbool blockingWrite, CCuint bufferOffset, CCuint numBytes, HTMLCanvasElement*elem,
        ExceptionObject& es) {
	    enqueueReadBuffer(clbuf, blockingWrite, bufferOffset, numBytes, elem, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }
    void WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel*k, CCuint workDim, const Vector<unsigned>& globalWorkOffsets, const Vector<unsigned>& globalWorkSize,
        const Vector<unsigned>& localWorkSize, ExceptionObject&es) {
	    enqueueNDRangeKernel(k, workDim, globalWorkOffsets, globalWorkSize, localWorkSize, Vector<RefPtr<WebCLEvent> >(), NULL, es);
    }


    void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>&c, const Vector<CCuint>&d, const Vector<CCuint>&e,
        CCuint f, CCuint g, CCuint h, CCuint i, ArrayBufferView*j, ExceptionObject&k)
	{ enqueueWriteBufferRect(a,b,c,d,e,f,g,h,i,j, Vector<RefPtr<WebCLEvent> >(), NULL, k); }
    void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>&c, const Vector<CCuint>&d, const Vector<CCuint>&e,
        CCuint f, CCuint g, ImageData*h, ExceptionObject&i)
	{ enqueueWriteBufferRect(a,b,c,d,e,f,g,h, Vector<RefPtr<WebCLEvent> >(), NULL, i); }
    void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>&c, const Vector<CCuint>&d, const Vector<CCuint>&e,
        CCuint f, CCuint g, HTMLCanvasElement*h, ExceptionObject&i)
	{ enqueueWriteBufferRect(a,b,c,d,e,f,g,h, Vector<RefPtr<WebCLEvent> >(), NULL, i); }
    void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>&c, const Vector<CCuint>&d, const Vector<CCuint>&e,
        CCuint f, CCuint g, HTMLImageElement*h, ExceptionObject&i)
	{ enqueueWriteBufferRect(a,b,c,d,e,f,g,h, Vector<RefPtr<WebCLEvent> >(), NULL, i); }

    void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>& c, const Vector<CCuint>& d,
        const Vector<CCuint>& e, CCuint f, CCuint g, CCuint h, CCuint i, ArrayBufferView*j, ExceptionObject&k) { enqueueReadBufferRect(a,b,c,d,e,f,g,h,i,j,Vector<RefPtr<WebCLEvent> >(), NULL, k); }
    void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer*a, CCbool b, const Vector<CCuint>& c, const Vector<CCuint>& d,
        const Vector<CCuint>& e, CCuint f, CCuint g, HTMLCanvasElement*h, ExceptionObject&i) { enqueueReadBufferRect(a,b,c,d,e,f,g,h,Vector<RefPtr<WebCLEvent> >(), NULL,i); }

    void WebCLCommandQueue::enqueueReadImage(WebCLImage*a, CCbool b,  const Vector<CCuint>& c, const Vector<CCuint>& d, CCuint e,
        ArrayBufferView*f, ExceptionObject&g) { enqueueReadImage(a,b,c,d,e,f,Vector<RefPtr<WebCLEvent> >(), NULL,g); }
    void WebCLCommandQueue::enqueueReadImage(WebCLImage*a, CCbool b,  const Vector<CCuint>& c, const Vector<CCuint>& d, HTMLCanvasElement*e,
        ExceptionObject&f) { enqueueReadImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }



    void WebCLCommandQueue::enqueueWriteImage(WebCLImage*a, CCbool b,  const Vector<unsigned>& c, const Vector<unsigned>& d, CCuint e,
        ArrayBufferView*f, ExceptionObject&g) { enqueueWriteImage(a,b,c,d,e,f,Vector<RefPtr<WebCLEvent> >(), NULL,g); }
    void WebCLCommandQueue::enqueueWriteImage(WebCLImage*a, CCbool b,  const Vector<unsigned>& c, const Vector<unsigned>& d,
        ImageData*e, ExceptionObject&f) { enqueueWriteImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
    void WebCLCommandQueue::enqueueWriteImage(WebCLImage*a, CCbool b,  const Vector<unsigned>& c, const Vector<unsigned>& d,
        HTMLCanvasElement*e, ExceptionObject&f) { enqueueWriteImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
    void WebCLCommandQueue::enqueueWriteImage(WebCLImage*a, CCbool b,  const Vector<unsigned>& c, const Vector<unsigned>& d,
        HTMLImageElement*e, ExceptionObject&f) { enqueueWriteImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }

    void WebCLCommandQueue::enqueueCopyBuffer(WebCLBuffer*a, WebCLBuffer*b, CCuint c, CCuint d, CCuint e,
        ExceptionObject&f) { enqueueCopyBuffer(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
    void WebCLCommandQueue::enqueueCopyBufferRect(WebCLBuffer*a, WebCLBuffer*b, const Vector<unsigned>& c, const Vector<unsigned>& d,
        const Vector<unsigned>& e, CCuint f, CCuint g, CCuint h, CCuint i,
        ExceptionObject&j) { enqueueCopyBufferRect(a,b,c,d,e,f,g,h,i,Vector<RefPtr<WebCLEvent> >(), NULL,j); }
    void WebCLCommandQueue::enqueueCopyImage(WebCLImage*a, WebCLImage*b, const Vector<unsigned>& c, const Vector<unsigned>& d,
        const Vector<unsigned>& e, ExceptionObject&f) { enqueueCopyImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
    void WebCLCommandQueue::enqueueCopyImageToBuffer(WebCLImage*a, WebCLBuffer*b, const Vector<unsigned>& c, const Vector<unsigned>& d,
        CCuint e, ExceptionObject&f) { enqueueCopyImageToBuffer(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
    void WebCLCommandQueue::enqueueCopyBufferToImage(WebCLBuffer*a, WebCLImage*b, CCuint c, const Vector<unsigned>& d, const Vector<unsigned>& e,
        ExceptionObject&f) { enqueueCopyBufferToImage(a,b,c,d,e,Vector<RefPtr<WebCLEvent> >(), NULL,f); }
} // namespace WebCore

#endif // ENABLE(WEBCL)

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

#if ENABLE(WEBCL)

#include "WebCLInputChecker.h"

#include "WebCLBuffer.h"
#include "WebCLContext.h"
#include "WebCLImage.h"
#include "WebCLImageDescriptor.h"
#include "WebCLKernel.h"
#include "ArrayBufferView.h"

namespace WebCore {
namespace WebCLInputChecker {

bool isValidDeviceType(unsigned long deviceType)
{
    switch (deviceType) {
    case ComputeContext::DEVICE_TYPE_CPU:
    case ComputeContext::DEVICE_TYPE_GPU:
    case ComputeContext::DEVICE_TYPE_ACCELERATOR:
    case ComputeContext::DEVICE_TYPE_DEFAULT:
    case ComputeContext::DEVICE_TYPE_ALL:
        return true;
    }
    return false;
}

bool isValidDeviceInfoType(unsigned long infoType)
{
    switch (infoType) {
    case ComputeContext::DEVICE_EXTENSIONS:
    case ComputeContext::DEVICE_PROFILE:
    case ComputeContext::DEVICE_NAME:
    case ComputeContext::DEVICE_VENDOR:
    case ComputeContext::DEVICE_VENDOR_ID:
    case ComputeContext::DEVICE_VERSION:
    case ComputeContext::DRIVER_VERSION:
    case ComputeContext::DEVICE_OPENCL_C_VERSION:
    case ComputeContext::DEVICE_ADDRESS_BITS:
    case ComputeContext::DEVICE_MAX_CONSTANT_ARGS:
    case ComputeContext::DEVICE_MAX_READ_IMAGE_ARGS:
    case ComputeContext::DEVICE_MAX_SAMPLERS:
    case ComputeContext::DEVICE_MAX_WRITE_IMAGE_ARGS:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_HALF:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_HALF:
    case ComputeContext::DEVICE_MAX_CLOCK_FREQUENCY:
    case ComputeContext::DEVICE_IMAGE2D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE2D_MAX_WIDTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE3D_MAX_WIDTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_DEPTH:
    case ComputeContext::DEVICE_MAX_PARAMETER_SIZE:
    case ComputeContext::DEVICE_MAX_WORK_GROUP_SIZE:
    case ComputeContext::DEVICE_MAX_WORK_ITEM_DIMENSIONS:
    case ComputeContext::DEVICE_LOCAL_MEM_SIZE:
    case ComputeContext::DEVICE_MAX_CONSTANT_BUFFER_SIZE:
    case ComputeContext::DEVICE_MAX_MEM_ALLOC_SIZE:
    case ComputeContext::DEVICE_AVAILABLE:
    case ComputeContext::DEVICE_ENDIAN_LITTLE:
    case ComputeContext::DEVICE_HOST_UNIFIED_MEMORY:
    case ComputeContext::DEVICE_IMAGE_SUPPORT:
    case ComputeContext::DEVICE_TYPE:
    case ComputeContext::DEVICE_QUEUE_PROPERTIES:
    case ComputeContext::DEVICE_PLATFORM:
    case ComputeContext::DEVICE_LOCAL_MEM_TYPE:
    case ComputeContext::DEVICE_MAX_WORK_ITEM_SIZES:
    case ComputeContext::DEVICE_MAX_COMPUTE_UNITS:
    case ComputeContext::DEVICE_GLOBAL_MEM_SIZE:
    case ComputeContext::DEVICE_MEM_BASE_ADDR_ALIGN:
    case ComputeContext::DEVICE_SINGLE_FP_CONFIG:
    case ComputeContext::DEVICE_COMPILER_AVAILABLE:
    case ComputeContext::DEVICE_EXECUTION_CAPABILITIES:
    case ComputeContext::DEVICE_ERROR_CORRECTION_SUPPORT:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_TYPE:
    case ComputeContext::DEVICE_PROFILING_TIMER_RESOLUTION:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_SIZE:
        return true;
    }
    return false;
}

bool isValidMemoryObjectFlag(unsigned long memoryObjectFlag)
{
    switch (memoryObjectFlag) {
    case ComputeContext::MEM_READ_ONLY:
    case ComputeContext::MEM_WRITE_ONLY:
    case ComputeContext::MEM_READ_WRITE:
        return true;
    }

    return false;
}

bool isValidAddressingMode(unsigned long value)
{
    switch(value) {
    case ComputeContext::ADDRESS_CLAMP_TO_EDGE:
    case ComputeContext::ADDRESS_CLAMP:
    case ComputeContext::ADDRESS_REPEAT:
    case ComputeContext::ADDRESS_MIRRORED_REPEAT:
        return true;
    }

    return false;
}

bool isValidFilterMode(unsigned long value)
{
    switch(value) {
    case ComputeContext::FILTER_NEAREST:
    case ComputeContext::FILTER_LINEAR:
        return true;
    }

    return false;

}

bool isValidChannelOrder(unsigned long value)
{
    switch (value) {
    case ComputeContext::R:
    case ComputeContext::A:
    case ComputeContext::RG:
    case ComputeContext::RA:
    case ComputeContext::RGB:
    case ComputeContext::RGBA:
    case ComputeContext::BGRA:
    case ComputeContext::ARGB:
    case ComputeContext::INTENSITY:
    case ComputeContext::LUMINANCE:
    case ComputeContext::Rx:
    case ComputeContext::RGx:
    case ComputeContext::RGBx:
        return true;
    }

    return false;
}

bool isValidChannelType(unsigned long value)
{
    switch (value) {
    case ComputeContext::SNORM_INT8:
    case ComputeContext::SNORM_INT16:
    case ComputeContext::UNORM_INT8:
    case ComputeContext::UNORM_INT16:
    case ComputeContext::UNORM_SHORT_565:
    case ComputeContext::UNORM_SHORT_555:
    case ComputeContext::UNORM_INT_101010:
    case ComputeContext::SIGNED_INT8:
    case ComputeContext::SIGNED_INT16:
    case ComputeContext::SIGNED_INT32:
    case ComputeContext::UNSIGNED_INT8:
    case ComputeContext::UNSIGNED_INT16:
    case ComputeContext::UNSIGNED_INT32:
    case ComputeContext::HALF_FLOAT:
    case ComputeContext::FLOAT:
        return true;
    }

    return false;
}

bool isValidCommandQueueProperty(unsigned long value)
{
    switch (value) {
    case 0: // 0 as integer value CommandQueueProperty is optional.
    case ComputeContext::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
    case ComputeContext::QUEUE_PROFILING_ENABLE:
        return true;
    }
    return false;
}

bool isValidGLTextureInfo(unsigned long value)
{
    switch (value) {
    case ComputeContext::GL_TEXTURE_TARGET:
    case ComputeContext::GL_MIPMAP_LEVEL:
        return true;
    }
    return false;
}

bool isValidKernelArgIndex(WebCLKernel* kernel, unsigned index)
{
    ASSERT(kernel);
    return index < kernel->numberOfArguments();
}

bool isValidDataSizeForArrayBufferView(unsigned long size, ArrayBufferView* arrayBufferView)
{
    ASSERT(arrayBufferView);

    unsigned bytesPerElement = 1;
    switch (arrayBufferView->getType()) {
        case ArrayBufferView::TypeInt8:
        case ArrayBufferView::TypeUint8:
        case ArrayBufferView::TypeUint8Clamped:
            bytesPerElement = 1;
            break;
        case ArrayBufferView::TypeInt16:
        case ArrayBufferView::TypeUint16:
            bytesPerElement = 2;
            break;
        case ArrayBufferView::TypeInt32:
        case ArrayBufferView::TypeUint32:
        case ArrayBufferView::TypeFloat32:
            bytesPerElement = 4;
            break;
        case ArrayBufferView::TypeFloat64:
            bytesPerElement = 8;
            break;
        default:
            ASSERT_NOT_REACHED();
            return false;
    }

    return !(size % bytesPerElement);
}

bool isValidRegionForMemoryObject(const Vector<size_t>& origin, const Vector<size_t>& region, size_t rowPitch, size_t slicePitch, size_t length)
{
    size_t regionArea = region[0] * region[1] * region[2];
    if (!regionArea)
        return false;

    if (rowPitch) {
        // Validate User given rowPitch, region read = rowPitch * number of rows * number of slices.
        // The rowPitch is used to move the pointer to the next read the next row. By default its set to
        // row width. With user sent values we must ensure the read is within the bounds.
        size_t maximumReadPtrValue = rowPitch * region[1] * region[2];
        if (maximumReadPtrValue > length)
            return false;
    }
    if (slicePitch) {
        // Validate User given slicePitch , region read = slicePitch * number of slices.
        // The slicePitch is used to move the pointer for the next slice. Default value is size of slice
        // in bytes ( region[1] * rowPitch). Must be validated identical to rowPitch to avoid out of bound memory access.
        size_t maximumReadPtrValue = slicePitch * region[2];
        if (maximumReadPtrValue > length)
            return false;
    }

    // If row_pitch is 0, row_pitch is computed as region[0].
    rowPitch = rowPitch ? rowPitch : region[0];
    // If slice_pitch is 0, slice_pitch is computed as region[1] * row_pitch.
    slicePitch = slicePitch ? slicePitch : (region[1] * rowPitch);

    // The offset in bytes is computed as origin[2] * host_slice_pitch + origin[1] * rowPitch + origin[0].
    size_t offset = origin[2] * slicePitch + origin[1]  * rowPitch + origin[0];

    return (regionArea + offset) <= length;
}

bool isValidRegionForImage(const WebCLImageDescriptor* descriptor, const Vector<CCuint>& origin, const Vector<CCuint>& region)
{
    size_t regionArea = region[0] * region[1];
    if (!regionArea)
        return false;

    size_t height = descriptor->height();
    size_t width = descriptor->width();
    size_t offsetFromOrigin = origin[1] * height + origin[0];
    return (offsetFromOrigin + regionArea) <= (height * width);
}

bool isValidRegionForBuffer(const size_t bufferLength, const Vector<CCuint>& region, const size_t offset, const WebCLImageDescriptor* descriptor)
{
    // The size in bytes of the region to be copied from buffer is width * height * bytes/image element.
    size_t bytesCopied = region[0] * region[1]
        * WebCLContext::bytesPerChannelType(descriptor->channelType())
        * WebCLContext::numberOfChannelsForChannelOrder(descriptor->channelOrder());

    return (offset+ bytesCopied) <= bufferLength;
}

bool isValidRegionForHostPtr(const Vector<CCuint>& region, size_t rowPitch, const WebCLImageDescriptor* descriptor, size_t length)
{
    /*
    *  Validate the hostPtr length passed to enqueue*Image* API's. Since hostPtr are not validated by OpenCL
    *  Out of Bound access may cause crashes. So validating with rowPitch & region being read.
    *  rowPitch is used to move the pointer to next row for write/read.
    */
    size_t imageBytesPerPixel = WebCLContext::bytesPerChannelType(descriptor->channelType())
        * WebCLContext::numberOfChannelsForChannelOrder(descriptor->channelOrder());
    rowPitch = rowPitch ? rowPitch : region[0];
    if (rowPitch * region[1] * imageBytesPerPixel > length)
        return false;

    size_t regionArea = region[0] * region[1];
    if (!regionArea)
        return false;
    return (regionArea <= length);
}

static bool valueInRange(size_t value, size_t minimum, size_t maximum)
{
    return ((value >= minimum) && (value <= maximum));
}

bool isRegionOverlapping(WebCLImage* source, WebCLImage* destination, const Vector<CCuint>& sourceOrigin,
    const Vector<CCuint>& destinationOrigin, const Vector<CCuint>& region)
{
    if (!source || !destination)
        return false;

    if (sourceOrigin.size() != 2 || destinationOrigin.size() != 2 || region.size() != 2)
        return false;

    if (source->platformObject() != destination->platformObject())
        return false;

    bool xOverlap = valueInRange(destinationOrigin[0], sourceOrigin[0], (region[0] + sourceOrigin[0]))
        || valueInRange(sourceOrigin[0], destinationOrigin[0], (destinationOrigin[0] + region[0]));
    bool yOverlap = valueInRange(destinationOrigin[1], sourceOrigin[1], (region[1] + sourceOrigin[1]))
        || valueInRange(sourceOrigin[1], destinationOrigin[1], (destinationOrigin[1] + region[1]));

    return xOverlap && yOverlap;
}

bool isRegionOverlapping(WebCLBuffer* srcBuffer, WebCLBuffer* destBuffer, const CCuint srcOffset, const CCuint dstOffset, const CCuint numBytes)
{
    if (!srcBuffer || !destBuffer)
        return false;

    if (srcBuffer->platformObject() != destBuffer->platformObject())
        return false;

    return valueInRange(dstOffset, srcOffset, (srcOffset + numBytes))
        || valueInRange(srcOffset, dstOffset, (dstOffset + numBytes));
}

bool compareContext(WebCLContext* context1, WebCLContext* context2)
{
    return context1->platformObject() == context2->platformObject();
}

bool compareImageFormat(const CCImageFormat& srcImageFormat, const CCImageFormat& dstImageFormat)
{
    return (srcImageFormat.image_channel_order == dstImageFormat.image_channel_order) &&
        (srcImageFormat.image_channel_data_type == dstImageFormat.image_channel_data_type);
}

}
}

#endif

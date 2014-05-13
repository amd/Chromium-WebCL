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

#ifndef WebCLInputChecker_h
#define WebCLInputChecker_h

#if ENABLE(WEBCL)

#include "WebCLObject.h"

namespace WTF {
class ArrayBufferView;
}

namespace WebCore {

class WebCLBuffer;
class WebCLContext;
class WebCLImage;
class WebCLImageDescriptor;
class WebCLKernel;

namespace WebCLInputChecker {

bool isValidDeviceType(unsigned long);
bool isValidDeviceInfoType(unsigned long);
bool isValidMemoryObjectFlag(unsigned long);
bool isValidAddressingMode(unsigned long);
bool isValidFilterMode(unsigned long);
bool isValidChannelType(unsigned long);
bool isValidChannelOrder(unsigned long);
bool isValidCommandQueueProperty(unsigned long);
bool isValidGLTextureInfo(unsigned long);
bool isValidKernelArgIndex(WebCLKernel*, unsigned index);
bool isValidDataSizeForArrayBufferView(unsigned long size, ArrayBufferView*);
bool isValidRegionForMemoryObject(const Vector<size_t>& origin, const Vector<size_t>& region, size_t rowPitch, size_t slicePitch, size_t length);
bool isValidRegionForHostPtr(const Vector<CCuint>& region, size_t rowPitch, const WebCLImageDescriptor*, size_t length);
bool isValidRegionForImage(const WebCLImageDescriptor*, const Vector<CCuint>& origin, const Vector<CCuint>& region);
bool isValidRegionForBuffer(const size_t bufferLength, const Vector<CCuint>& region, const size_t offset, const WebCLImageDescriptor*);

bool isRegionOverlapping(WebCLImage*, WebCLImage*, const Vector<CCuint>& srcOrigin, const Vector<CCuint>& dstOrigin, const Vector<CCuint>& region);
bool isRegionOverlapping(WebCLBuffer*, WebCLBuffer*, const CCuint srcOffset, const CCuint dstOffset, const CCuint numBytes);

bool compareContext(WebCLContext*, WebCLContext*);
bool compareImageFormat(const CCImageFormat&, const CCImageFormat&);

template<class T>
bool validateWebCLObject(WebCLObjectImpl<T>* object)
{
    return object && !object->isPlatformObjectNeutralized();
}

}
}

#endif
#endif // WebCLInputChecker

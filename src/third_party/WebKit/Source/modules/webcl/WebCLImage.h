/*
 * Copyright (C) 2011, 2013 Samsung Electronics Corporation. All rights reserved.
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

#ifndef WebCLImage_h
#define WebCLImage_h

#if ENABLE(WEBCL)

#include "WebCLMemoryObject.h"

namespace WebCore {

class WebCLContext;
class WebCLImageDescriptor;
#if ENABLE(WEBGL)
class WebGLObject;
class WebGLRenderbuffer;
class WebGLTexture;
#endif

class WebCLImage : public WebCLMemoryObject {
public:
    ~WebCLImage();
    static PassRefPtr<WebCLImage> create(WebCLContext*, CCenum flags, PassRefPtr<WebCLImageDescriptor>, void*, ExceptionObject&);

#if ENABLE(WEBGL)
    static PassRefPtr<WebCLImage> create(WebCLContext*, CCenum flags, WebGLRenderbuffer* webGLRenderbuffer, ExceptionObject&);
    static PassRefPtr<WebCLImage> create(WebCLContext*, CCenum flags, CCenum textureTarget, CCenum miplevel, WebGLTexture*, ExceptionObject&);

    int getGLTextureInfo(CCenum paramName, ExceptionObject&);
#endif

    WebCLImageDescriptor* getInfo(ExceptionObject&);
    const CCImageFormat& imageFormat() const;
    const WebCLImageDescriptor* imageDescriptor() const
    {
        return m_imageDescriptor.get();
    }

private:
    WebCLImage(WebCLContext*, ComputeMemoryObject*, PassRefPtr<WebCLImageDescriptor>);

#if ENABLE(WEBGL)
    void cacheGLObjectInfo(CCenum type, WebGLObject*);
#endif

    RefPtr<WebCLImageDescriptor> m_imageDescriptor;
};

} // namespace WebCore

#endif
#endif // WebCLImage_h

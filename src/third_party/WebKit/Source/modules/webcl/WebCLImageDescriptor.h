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

#ifndef WebCLImageDescriptor_h
#define WebCLImageDescriptor_h

#if ENABLE(WEBCL)

#include "ComputeTypes.h"
#include "ComputeContext.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebCLImageDescriptor : public RefCounted<WebCLImageDescriptor>
{
public:
    virtual ~WebCLImageDescriptor();

	static PassRefPtr<WebCLImageDescriptor> create(CCuint width, CCuint height);

    static PassRefPtr<WebCLImageDescriptor> create(CCuint width, CCuint height, CCuint rowPitch, const CCImageFormat&/* = {ComputeContext::RGBA, ComputeContext::UNORM_INT8}*/);
    static PassRefPtr<WebCLImageDescriptor> create(const CCImageFormat&);

    void setChannelOrder(CCenum);
    CCenum channelOrder() const;

    void setChannelType(CCenum);
    CCenum channelType() const;

    void setWidth(CCuint);
    CCuint width() const;

    void setHeight(CCuint);
    CCuint height() const;

    void setRowPitch(CCuint);
    CCuint rowPitch() const;

    const CCImageFormat& imageFormat() const;

private:
    WebCLImageDescriptor(CCuint width, CCuint height, CCuint rowPitch, const CCImageFormat&);

    CCuint m_width;
    CCuint m_height;
    CCuint m_rowPitch;
    CCImageFormat m_imageFormat;
};

} // namespace WebCore

#endif
#endif // WebCLImageDescriptor_h

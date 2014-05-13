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

#ifndef WebCLKernelArgInfo_h
#define WebCLKernelArgInfo_h

#if ENABLE(WEBCL)

#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class WebCLKernelArgInfo : public RefCounted<WebCLKernelArgInfo> {
public:
    static PassRefPtr<WebCLKernelArgInfo> create(const String& addressQualifier, const String& accessQualifier, const String& type, const String& name) { return adoptRef(new WebCLKernelArgInfo(addressQualifier, accessQualifier, type, name)); }

    String name() const { return m_name; }
    String typeName() const { return m_type; }
    String addressQualifier() const { return m_addressQualifier; }
    String accessQualifier() const { return m_accessQualifier; }

private:
    WebCLKernelArgInfo(const String& addressQualifier, const String& accessQualifier, const String& type, const String& name)
        : m_addressQualifier(addressQualifier)
        , m_accessQualifier(accessQualifier)
        , m_type(type)
        , m_name(name)
    {
    }
    String m_addressQualifier;
    String m_accessQualifier;
    String m_type;
    String m_name;
};

} // namespace WebCore

#endif
#endif // WebCLKernelArgInfo_h

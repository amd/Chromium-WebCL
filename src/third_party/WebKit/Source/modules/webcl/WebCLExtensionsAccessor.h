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

#ifndef WebCLExtensionsAccessor_h
#define WebCLExtensionsAccessor_h

#if ENABLE(WEBCL)

#include "ComputeExtensions.h"
#include "WebCLException.h"
#include <wtf/HashSet.h>

namespace WebCore {

class WebCLExtension;

// FIXME: When WebCore switches on support for C++11,
// replace NullTypePtr by C++11's nullptr_t.
template <class T = NullTypePtr>
class WebCLExtensionsAccessor {
public:
    virtual ~WebCLExtensionsAccessor()
    {
    }

    T accessor() const
    {
        return m_accessor;
    }

    bool enableExtension(const String& name);
    Vector<String> getSupportedExtensions();
    bool isEnabledExtension(const String& name) const;
    void getEnabledExtensions(HashSet<String>&) const;

protected:
    WebCLExtensionsAccessor(T object)
        : m_accessor(object)
    {
    }
    T m_accessor;

    HashSet<String> m_enabledExtensions;
};

} // WebCore

#endif // ENABLE(WEBCL)

#endif // WebCLExtensionsAccessor_h

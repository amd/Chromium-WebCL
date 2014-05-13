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

#include "ComputePlatform.h"
#include "WebCLExtensionsAccessor.h"

namespace WebCore {

template <class T>
inline bool WebCLExtensionsAccessor<T>::enableExtension(const String& name)
{
    if (equalIgnoringCase(name, "WEBCL_html_image")) {
        m_enabledExtensions.add("WEBCL_html_image");
        return true;
    }
    if (equalIgnoringCase(name, "KHR_gl_sharing")) {
#if ENABLE(WEBGL)
        bool khrGLSharing = m_accessor ? ComputeExtensions::get().supports("cl_khr_gl_sharing", m_accessor)
                                    : ComputeExtensions::get().supports("cl_khr_gl_sharing");
        if (khrGLSharing)
            m_enabledExtensions.add("KHR_gl_sharing");
        return khrGLSharing;
#endif
    }
    return false;
}

template <class T>
Vector<String> WebCLExtensionsAccessor<T>::getSupportedExtensions()
{
    Vector<String> result;
    // WEBCL_html_image is always enabled in WebKit.
    result.append("WEBCL_html_image");

    if (m_accessor) {
#if ENABLE(WEBGL)
        if (ComputeExtensions::get().supports("cl_khr_gl_sharing", m_accessor))
            result.append("KHR_GL_SHARING");
#endif
    } else {
#if ENABLE(WEBGL)
        if (ComputeExtensions::get().supports("cl_khr_gl_sharing"))
            result.append("KHR_GL_SHARING");
#endif
    }

    return result;
}

template <class T>
void WebCLExtensionsAccessor<T>::getEnabledExtensions(HashSet<String>& extensions) const
{
    for (HashSet<String>::iterator i = m_enabledExtensions.begin(); i != m_enabledExtensions.end(); ++i)
        extensions.add(*i);
}

template <class T>
bool WebCLExtensionsAccessor<T>::isEnabledExtension(const String& name) const
{
    return m_enabledExtensions.contains(name);
}

template class WebCLExtensionsAccessor<ComputePlatform*>;
template class WebCLExtensionsAccessor<ComputeDevice*>;
template class WebCLExtensionsAccessor<NullTypePtr>;

} // WebCore

#endif // ENABLE(WEBCL)

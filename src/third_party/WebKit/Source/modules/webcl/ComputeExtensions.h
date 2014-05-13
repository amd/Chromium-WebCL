/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ComputeExtensions_h
#define ComputeExtensions_h

#include "ComputeContext.h"
#include "ComputeTypes.h"

#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/text/StringHash.h>

namespace WebCore {

class ComputePlatform;

// FIXME: When c++11 is enforce in WebCore, nullptr_t should be used here.
typedef void* NullTypePtr;

class ComputeExtensions {
public:
    static ComputeExtensions& get();

    virtual ~ComputeExtensions();

    // Takes full name of extension; for example "cl_khr_gl_sharing".
    bool supports(const String&, NullTypePtr = 0);
    bool supports(const String&, ComputePlatform*);
    bool supports(const String&, ComputeDevice*);

private:
    ComputeExtensions();

    // Global extensions.
    bool supportsExtension(const WTF::String& name) const;
    void cacheGlobalExtensions();
    void initializeGlobalExtensions();
    bool m_initializedGlobalExtensions;
    HashSet<String> m_globalExtensions;

    // Platform extensions.
    bool cacheExtensionsForPlatform(CCPlatformID);
    HashMap<CCPlatformID, HashSet<String> > m_platformExtensions;

    // Device extensions.
    bool cacheExtensionsForDevice(CCDeviceID);
    HashMap<CCDeviceID, HashSet<String> > m_deviceExtensions;

    template <typename Type, typename Cache>
    bool supportsExtension(const WTF::String& name, Type computeType, Cache&) const;
};


} // namespace WebCore

#endif // ComputeExtensions_h

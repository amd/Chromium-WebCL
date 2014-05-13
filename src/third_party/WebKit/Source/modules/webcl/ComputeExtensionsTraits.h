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

#ifndef ComputeExtensionsTraits_h
#define ComputeExtensionsTraits_h

#include "ComputeContext.h"
#include "ComputeTypes.h"

namespace WebCore {

template <typename Func, typename Type>
struct ComputeExtensionFunctor {
    Func function;
    CCerror operator()(Type computeType, int property, Vector<char>* buffer)
    {
        return function(computeType, property, buffer);
    }
};

template <typename Func, typename Type, typename Cache>
struct CacheExtensions
{
    static bool perform(Func getExtensionsFunc, Type computeType, int property, Cache& cache)
    {
        ComputeExtensionFunctor<Func, Type> func = {getExtensionsFunc};

        Vector<char> buffer;
        CCerror error = func(computeType, property, &buffer);
        if (error != ComputeContext::SUCCESS)
            return false;

        HashSet<String> extensionSet;
        if (buffer.isEmpty()) {
            cache.set(computeType, extensionSet);
            return true;
        }

        Vector<String> extensionArray;
        String(buffer.data()).split(" ", extensionArray);

        for (size_t i = 0; i < extensionArray.size(); ++i)
            extensionSet.add(extensionArray[i]);
        cache.set(computeType, extensionSet);
        return true;
    }
};

template <typename Func, typename Type, typename Cache>
bool cacheExtensionsHelper(Func func, Type computeType, int property, Cache& cache)
{
    return CacheExtensions<Func, Type, Cache>::perform(func, computeType, property, cache);
}

} // namespace WebCore

#endif // ComputeExtensionsTraits_h

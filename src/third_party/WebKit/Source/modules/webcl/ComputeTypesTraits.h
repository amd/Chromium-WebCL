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

#ifndef ComputeTypesTraits_h
#define ComputeTypesTraits_h

#include <wtf/Vector.h>

#include "ComputeTypes.h"

namespace WebCore {

// FIXME: We should not duplicate ComputeContext enums
enum {
    SUCCESS = 0,
    OUT_OF_HOST_MEMORY = -6
};

template <typename Func, typename T>
struct Functor {
    Func function;
    cl_int operator()(T computeType, cl_uint name, size_t size, void* param, size_t* retSize) {
        return function(computeType, name, size, param, retSize);
    }
};

template <typename Func, typename T, typename U>
struct ComputeInfo
{
    static cl_int get(Func getInfoFunc, T computeType, cl_uint name, U* param)
    {
        Functor<Func, T> func = {getInfoFunc};
        return func(computeType, name, sizeof(U), static_cast<void*>(param), NULL);
    }
};

template <typename Func, typename T, typename U, size_t inlineCapacity>
struct ComputeInfo<Func, T, Vector<U, inlineCapacity> >
{
    static cl_int get(Func getInfoFunc, T computeType, cl_uint name, Vector<U, inlineCapacity>* param)
    {
        size_t paramSize;
        Functor<Func, T> func = {getInfoFunc};
        cl_int error = func(computeType, name, 0, 0, &paramSize);

        if (error != SUCCESS)
            return error;
        paramSize = paramSize / sizeof(U);
        //if (!param->tryReserveCapacity(paramSize))
        //    return OUT_OF_HOST_MEMORY;
        param->resize(paramSize);

        return func(computeType, name, param->size() * sizeof(U), static_cast<void*>(param->data()), NULL);
    }
};

template <typename Func, typename T, typename U>
cl_int getInfoHelper(Func func, T computeType, cl_uint name, U* param)
{
    return ComputeInfo<Func, T, U>::get(func, computeType, name, param);
}

template <typename Func, typename T, typename U, size_t inlineCapacity>
cl_int getInfoHelper(Func func, T computeType, cl_uint name, Vector<U, inlineCapacity>* param)
{
    return ComputeInfo<Func, T, Vector<U, inlineCapacity> >::get(func, computeType, name, param);
}

// Functor supporting additional ComputeType.
template <typename Func, typename T, typename U>
struct FunctorWithAdditinalCLType {
    Func function;
    cl_int operator()(T computeType, U supportingComputeType, cl_uint name, size_t size, void* param, size_t* retSize)
    {
        return function(computeType, supportingComputeType, name, size, param, retSize);
    }
};

template <typename Func, typename T, typename U, typename V>
struct ComputeInfoWithAdditinalCLType {
    static cl_int get(Func getInfoFunc, T computeType, U supportingComputeType, cl_uint name, V* param)
    {
        FunctorWithAdditinalCLType<Func, T, U> func = {getInfoFunc};
        return func(computeType, supportingComputeType, name, sizeof(V), static_cast<void*>(param), 0);
    }
};

template <typename Func, typename T, typename U, typename V,  size_t inlineCapacity>
struct ComputeInfoWithAdditinalCLType<Func, T, U, Vector<V, inlineCapacity> > {
    static cl_int get(Func getInfoFunc, T computeType, U supportingComputeType, cl_uint name, Vector<V, inlineCapacity>* param)
    {
        size_t paramSize;
        FunctorWithAdditinalCLType<Func, T, U> func = {getInfoFunc};
        cl_int error = func(computeType, supportingComputeType, name, 0, 0, &paramSize);

        if (error != SUCCESS)
            return error;
        paramSize = paramSize / sizeof(V);
        //if (!param->tryReserveCapacity(paramSize))
        //    return OUT_OF_HOST_MEMORY;
        param->resize(paramSize);

        return func(computeType, supportingComputeType, name, param->size() * sizeof(V), static_cast<void*>(param->data()), 0);
    }
};

template <typename Func, typename T, typename U, typename V>
cl_int getInfoHelper(Func func, T computeType, U supportingComputeType, cl_uint name, V* param)
{
    return ComputeInfoWithAdditinalCLType<Func, T, U, V>::get(func, computeType, supportingComputeType, name, param);
}

template <typename Func, typename T, typename U, typename V, size_t inlineCapacity>
cl_int getInfoHelper(Func func, T computeType, U supportingComputeType, cl_uint name, Vector<V, inlineCapacity>* param)
{
    return ComputeInfoWithAdditinalCLType<Func, T, U, Vector<V, inlineCapacity> >::get(func, computeType, supportingComputeType, name, param);
}

}

#endif // ComputeTypesTraits_h

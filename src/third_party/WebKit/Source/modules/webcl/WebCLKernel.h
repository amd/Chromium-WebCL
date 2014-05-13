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

#ifndef WebCLKernel_h
#define WebCLKernel_h

#if ENABLE(WEBCL)

#include "ComputeKernel.h"
#include "WebCLException.h"
#include "WebCLGetInfo.h"
#include "WebCLInputChecker.h"
#include "WebCLKernelArgInfoProvider.h"
#include "WebCLKernelArgInfo.h"
#include "WebCLMemoryObject.h"
#include "WebCLObject.h"

#include <wtf/RefCounted.h>

namespace WTF {
class ArrayBufferView;
}

namespace WebCore {

class WebCLContext;
class WebCLDevice;
class WebCLGetInfo;
class WebCLKernel;
class WebCLKernelArgInfo;
class WebCLMemoryObject;
class WebCLProgram;
class WebCLSampler;

typedef ComputeKernel* ComputeKernelPtr;
class WebCLKernel : public WebCLObjectImpl<ComputeKernelPtr> {
public:
    virtual ~WebCLKernel();
    static PassRefPtr<WebCLKernel> create(WebCLContext*, WebCLProgram*, const String&, ExceptionObject&);
    static Vector<RefPtr<WebCLKernel> > createKernelsInProgram(WebCLContext*, WebCLProgram*, ExceptionObject&);

    WebCLGetInfo getInfo(CCenum, ExceptionObject&);
    WebCLGetInfo getWorkGroupInfo(WebCLDevice*, CCenum, ExceptionObject&);

    void setArg(CCuint index, WebCLMemoryObject*, ExceptionObject&);
    void setArg(CCuint index, WebCLSampler*, ExceptionObject&);
    void setArg(CCuint index, ArrayBufferView*, ExceptionObject&);

    WebCLKernelArgInfo* getArgInfo(CCuint index, ExceptionObject&);

    WebCLProgram* program() const;
    String kernelName() const;

    ComputeKernel* computeKernel() const { return platformObject(); }

    unsigned numberOfArguments();
    WebCLContext* context()
    {
        return m_context.get();
    }

private:
    WebCLKernel(WebCLContext*, WebCLProgram*, ComputeKernel*, const String&);

    static bool isValidVectorLength(size_t arrayLength);

    RefPtr<WebCLContext> m_context;
    RefPtr<WebCLProgram> m_program;
    String m_kernelName;

    WebCLKernelArgInfoProvider m_argumentInfoProvider;
};

} // namespace WebCore

#endif
#endif // WebCLKernel_h

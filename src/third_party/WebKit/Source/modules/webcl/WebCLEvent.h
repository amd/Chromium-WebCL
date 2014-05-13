/*
* Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
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

#ifndef WebCLEvent_h
#define WebCLEvent_h

#if ENABLE(WEBCL)

#include "WebCLCallback.h"
#include "WebCLGetInfo.h"
#include "WebCLObject.h"

namespace WebCore {

class ComputeEvent;
class WebCLCommandQueue;

typedef ComputeEvent* ComputeEventPtr;
class WebCLEvent : public WebCLObjectImpl<ComputeEventPtr> {
public:
    virtual ~WebCLEvent();
    static PassRefPtr<WebCLEvent> create();

    virtual WebCLGetInfo getInfo(CCenum, ExceptionObject&);
    WebCLGetInfo getProfilingInfo(CCenum, ExceptionObject&);

    void setCallback(CCenum, PassRefPtr<WebCLCallback>, ExceptionObject&);
    void setAssociatedCommandQueue(WebCLCommandQueue* commandQueue);

    bool isUserEvent() const;

    virtual WebCLContext* context() const;

    bool holdsValidCLObject() const;

    virtual bool isPlatformObjectNeutralized() const;

protected:
    WebCLEvent(ComputeEvent*);

private:
    typedef Vector<std::pair<CCint, RefPtr<WebCLCallback> > > CallbackDataVector;
    typedef HashMap<RefPtr<WebCLEvent>, OwnPtr<CallbackDataVector> > WebCLEventCallbackRegisterQueue;

    static void callbackProxy(CCEvent, CCint, void*);
    static void callbackProxyOnMainThread(void* userData);
    static WebCLEventCallbackRegisterQueue& callbackRegisterQueue()
    {
        DEFINE_STATIC_LOCAL(WebCLEventCallbackRegisterQueue, instance, ());
        return instance;
    }

    RefPtr<WebCLCommandQueue> m_commandQueue;
};

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // WebCLEvent_h


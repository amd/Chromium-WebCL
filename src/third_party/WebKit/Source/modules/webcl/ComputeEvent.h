/*
 * Copyright (C) 2014 Samsung Electronics Corporation. All rights reserved.
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

#ifndef ComputeEvent_h
#define ComputeEvent_h

#include "ComputeTypes.h"
#include "ComputeTypesTraits.h"

namespace WebCore {

class ComputeContext;

class ComputeEvent {
public:
    ComputeEvent();
    ComputeEvent(ComputeContext*, CCerror&);
    ~ComputeEvent();

    CCerror setEventCallback(CCenum eventCommandExecStatus, pfnEventNotify callback, void* userData);
    CCerror setUserEventStatus(CCint executionStatus);

    template <typename T>
    CCerror getEventInfo(CCEventInfoType infoType, T* data)
    {
        return getInfoHelper(getEventInfoBase, m_event, infoType, data);
    }
    template <typename T>
    CCerror getEventProfilingInfo(CCEventProfilingInfoType infoType, T* data)
    {
        return getInfoHelper(getEventProfilingInfoBase, m_event, infoType, data);
    }
    CCEvent event() const
    {
        return m_event;
    }

    CCEvent& eventRef()
    {
        return m_event;
    }

    bool isUserEvent() const
    {
        return m_isUserEvent;
    }

    CCerror release();

private:
    static CCerror getEventInfoBase(CCEvent, CCEventInfoType, size_t, void *data, size_t* actualSize);
    static CCerror getEventProfilingInfoBase(CCEvent, CCEventProfilingInfoType, size_t, void *data, size_t* actualSize);

private:
    CCEvent m_event;
    bool m_isUserEvent;
};

}

#endif

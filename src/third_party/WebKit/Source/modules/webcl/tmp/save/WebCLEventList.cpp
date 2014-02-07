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

#include "config.h"

#if ENABLE(WEBCL)

#include "WebCLEventList.h"
#include "WebCL.h"


namespace WebCore {

WebCLEventList::WebCLEventList()
{
}

WebCLEventList::~WebCLEventList()
{
}

PassRefPtr<WebCLEventList> WebCLEventList::create(WebCL* ctx , cl_event* eventlist, cl_uint num_events)
{
	
	return adoptRef(new WebCLEventList(ctx ,eventlist, num_events));
}

WebCLEventList::WebCLEventList(WebCL* ctx,cl_event* eventlist, cl_uint num_events ) : 
					m_context(ctx),m_cl_events(eventlist),m_num_events(num_events)
{
	if (m_num_events == 0) {
		printf("Error: Number of events is 0");
	}
	//m_cl_events = new m_cl_events[m_num_events];
	
	for (unsigned int i = 0 ; i < m_num_events; i++) {
		RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, m_cl_events[i]);
		if (o != NULL) {
			m_event_id_list.append(o);
		} else {
			// TODO (siba samal) Error handling
		}
	}
	
}

cl_event* WebCLEventList::getCLEvents()
{
	return m_cl_events;
}

unsigned WebCLEventList::length() const
{
	return m_num_events;
}

WebCLEvent* WebCLEventList::item(unsigned index)
{
	if (index >= m_num_events) {
		printf("Error: Event Index Out of range");
		return 0;
	}
	WebCLEvent* ret = (m_event_id_list[index]).get();
	return ret;
}


} // namespace WebCore

#endif // ENABLE(WEBCL)

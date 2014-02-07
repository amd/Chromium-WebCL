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

#ifndef WebCLContext_h
#define WebCLContext_h

#ifndef THIRD_PARTY_WEBKIT_MODULES_WEBCL // ScalableVision to avoid conflict between TraceEvent.h and trace_event.h
#define THIRD_PARTY_WEBKIT_MODULES_WEBCL
#endif


#include "..\..\bindings\v8\ScriptObject.h"
#include "..\..\bindings\v8\ScriptState.h"

#include "../../core/dom/ActiveDOMObject.h"
#include <wtf/Float32Array.h>
#include <wtf/Int32Array.h>
#include "core/html/ImageData.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLVideoElement.h"
#include "..\..\core\platform\graphics\GraphicsContext3D.h"
#include "..\..\core\html\canvas\WebGLBuffer.h"
#include "..\..\core\html\canvas\WebGLRenderingContext.h"
#include "WebCLGetInfo.h"
//#include "..\..\core\dom\ScriptExecutionContext.h"
#include "..\..\core\dom\Document.h"
//#include "..\..\core\page\DOMWindow.h"
#include "..\..\core\platform\graphics\Image.h"
//#include "..\..\core\platform\SharedBuffer.h"
#include "..\..\core\html\canvas\CanvasRenderingContext2D.h"
#include "..\..\core\platform\graphics\ImageBuffer.h"
//#include "..\..\core\loader\cache\CachedImage.h"
#include <wtf/ArrayBuffer.h>


#include <wtf/OwnPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/Vector.h>
//#include <PlatformString.h>
#if OS(DARWIN)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#include <wtf/ArrayBuffer.h>

namespace WebCore {

class WebCL;
class WebCLCommandQueue;
class WebCLProgram;
class WebCLMem;
class WebCLImage;
class WebCLSampler;
class WebCLEvent;
class ImageData;
class ImageBuffer;
class IntSize;

class WebCLContext : public RefCounted<WebCLContext> {
public:
	virtual ~WebCLContext();
	static PassRefPtr<WebCLContext> create(WebCL*, cl_context);
	
	WebCLGetInfo getInfo(int, ExceptionState&);
	PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDeviceList*, int, ExceptionState&);
	PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDeviceList* deviceList, ExceptionState& ec) {
		return(createCommandQueue(deviceList, NULL, ec));
	}
	PassRefPtr<WebCLCommandQueue> createCommandQueue(ExceptionState& ec) {
		return(createCommandQueue(NULL, NULL, ec));
	}

	//PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDevice*, int, ExceptionState&);
	PassRefPtr<WebCLProgram> createProgram(const String&, ExceptionState&);
	PassRefPtr<WebCLMem> createBuffer(int, int, ExceptionState&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLCanvasElement*, ExceptionState&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLImageElement*, ExceptionState&);
	PassRefPtr<WebCLMem> createImage2D(int, HTMLVideoElement*, ExceptionState&);
	PassRefPtr<WebCLMem> createImage2D(int, ImageData*, ExceptionState&);
	PassRefPtr<WebCLMem> createImage2D(int,unsigned int,unsigned int, ArrayBuffer*, ExceptionState&);
	PassRefPtr<WebCLMem> createImage3D(int, unsigned int, unsigned int, unsigned int,ArrayBuffer*, ExceptionState&);
	PassRefPtr<WebCLSampler> createSampler(bool, int, int, ExceptionState&);
	PassRefPtr<WebCLMem> createFromGLBuffer(int, WebGLBuffer*, ExceptionState&);
	PassRefPtr<WebCLImage> createFromGLRenderBuffer(int, WebGLRenderbuffer*,ExceptionState&);
	PassRefPtr<WebCLMem> createFromGLTexture2D(int, GC3Denum,GC3Dint, GC3Duint, ExceptionState&);
	PassRefPtr<WebCLEvent> createUserEvent(ExceptionState&);	
	void releaseCL( ExceptionState&);
	void setDevice(RefPtr<WebCLDevice>);
	cl_context getCLContext();
	// Fixed-size cache of reusable image buffers for video texImage2D calls.
	class LRUImageBufferCache {
	public:
		LRUImageBufferCache(int capacity);
		// The pointer returned is owned by the image buffer map.
		ImageBuffer* imageBuffer(const IntSize& size);
	private:
		void bubbleToFront(int idx);
		//ScalableVision: OwnArrayPtr is removed
		//OwnArrayPtr<OwnPtr<ImageBuffer> > m_buffers;
		OwnPtr< OwnPtr<ImageBuffer>[] > m_buffers;
		int m_capacity;
	};
	LRUImageBufferCache m_videoCache;

	private:
	WebCLContext(WebCL*, cl_context);
	WebCL* m_context;
	cl_context m_cl_context;
	RefPtr<WebCLDevice> m_device_id;
	
	Vector<RefPtr<WebCLProgram> > m_program_list;
	long m_num_programs;
	Vector<RefPtr<WebCLMem> > m_mem_list;
	long m_num_mems;
	
       Vector<RefPtr<WebCLImage> > m_img_list;
	long m_num_images;


	Vector<RefPtr<WebCLEvent> > m_event_list;
	long m_num_events;
	Vector<RefPtr<WebCLSampler> > m_sampler_list;
	long m_num_samplers;
	Vector<RefPtr<WebCLContext> > m_context_list;
	long m_num_contexts;
	RefPtr<WebCLCommandQueue> m_command_queue;
	
	PassRefPtr<Image> videoFrameToImage(HTMLVideoElement*);

};



} // namespace WebCore

#endif // WebCLContext_h

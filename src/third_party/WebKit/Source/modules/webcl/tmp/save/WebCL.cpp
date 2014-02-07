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

#include "WebCL.h"
#include "ScriptExecutionContext.h"
#include "Document.h"
#include "core/page/DOMWindow.h"
#include "core/platform/graphics/Image.h"
#include "core/platform/SharedBuffer.h"
#include "core/html/canvas/CanvasRenderingContext2D.h"
#include "core/platform/graphics/ImageBuffer.h"
//#include "core/loader/cache/CachedImage.h"
#include <wtf/ArrayBuffer.h>
#include "core/html/HTMLCanvasElement.h"
#include "WebCLException.h"
#include <stdio.h>
#include "core/html/canvas/CanvasRenderingContext.h"
//≈·”®

class CanvasRenderingContext;

//using namespace JSC;

namespace WebCore {   
	//≈·”®
#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;


	WebCL::WebCL(ScriptExecutionContext* context) 
        //: ActiveDOMObject(context, this)
	{
		m_num_mems = 0;
		m_num_programs = 0;
		m_num_events = 0;
		m_num_samplers = 0;
		m_num_contexts = 0;
		m_num_commandqueues = 0;

	}

	PassRefPtr<WebCL> WebCL::create(ScriptExecutionContext* context)
	{
		return adoptRef(new WebCL(context));
	}

	WebCL::~WebCL()
	{
	}

	PassRefPtr<WebCLPlatformList> WebCL::getPlatforms(ExceptionState& ec)
	{
		RefPtr<WebCLPlatformList> o = WebCLPlatformList::create(this);
		if (o != NULL) {
			m_platform_id = o;
			return o;
		} else {
			ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
			return NULL;
		}
	}

	WebCLGetInfo WebCL::getImageInfo(WebCLImage* image, cl_image_info param_name, ExceptionState& ec) 	
	{
		cl_mem cl_Image_id = NULL;
		cl_int err = 0;
		size_t sizet_units = 0;
		if (image != NULL) {
			cl_Image_id = image->getCLImage();
			if (cl_Image_id == NULL) {
				printf("Error: cl_Image_id null\n");
				ec.throwDOMException(WebCLException::INVALID_MEM_OBJECT, "WebCLException::INVALID_MEM_OBJECT");
				return WebCLGetInfo();
			}
		}

		switch(param_name)
		{   
			case IMAGE_ELEMENT_SIZE:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_ELEMENT_SIZE, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_ROW_PITCH:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_ROW_PITCH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_SLICE_PITCH:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_WIDTH:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_WIDTH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_HEIGHT:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_HEIGHT, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
			case IMAGE_DEPTH:
				err=webcl_clGetImageInfo(webcl_channel_, cl_Image_id, CL_IMAGE_DEPTH, sizeof(size_t), &sizet_units, NULL);
				if (err == CL_SUCCESS)
					return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
				break;
				// TODO (siba samal) Handle Image Format & CL_IMAGE_D3D10_SUBRESOURCE_KHR types
			default:
				printf("Error: Unsupported Image Info type\n");
				return WebCLGetInfo();
		}
		switch (err) {
			case CL_INVALID_MEM_OBJECT:
				ec.throwDOMException(WebCLException::INVALID_MEM_OBJECT, "WebCLException::INVALID_MEM_OBJECT");
				printf("Error: CL_INVALID_MEM_OBJECT \n");
				break;
			case CL_INVALID_VALUE:
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
				printf("Error: CL_INVALID_VALUE \n");
				break;
			case CL_OUT_OF_RESOURCES:
				ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
				printf("Error: CL_OUT_OF_RESOURCES\n");
				break; 
			case CL_OUT_OF_HOST_MEMORY:
				ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				break;
		}				
		return WebCLGetInfo();

	}

	void WebCL::waitForEvents(WebCLEventList* events, ExceptionState& ec)
	{
		cl_int err = 0;
		cl_event* cl_event_id = NULL;

		if (events != NULL) {
			cl_event_id = events->getCLEvents();
			if (cl_event_id == NULL) {
				printf("Error: cl_event null\n");
				ec.throwDOMException(WebCLException::INVALID_EVENT, "WebCLException::INVALID_EVENT");
				return;
			}
		}
		err = webcl_clWaitForEvents(webcl_channel_, events->length(), cl_event_id);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_CONTEXT:
					printf("Error: CL_INVALID_CONTEXT \n");
					ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
					break;
				case CL_INVALID_VALUE:
					printf("Error: CL_INVALID_VALUE \n");
					ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
					break;
				case CL_INVALID_EVENT :
					printf("Error: CL_INVALID_EVENT  \n");
					ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
					break;
					//OpenCL 1.1
					//case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
					//	printf("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST \n");
					//	break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES \n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY \n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;

				default:
					printf("Error: Invaild Error Type\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;

			}
		} 
		return;
	}

	PassRefPtr<WebCLContext> WebCL::createContext(ExceptionState& ec)
	{

		cl_int err = 0;
		cl_uint m_num_platforms = 0;
		cl_uint num_devices = 0;
		cl_context cl_context_id = NULL;
		cl_platform_id* m_cl_platforms = NULL;
		cl_device_id* m_cl_devices = NULL;
		
		err = webcl_clGetPlatformIDs(webcl_channel_, 0, NULL, &m_num_platforms);
		
		if(err == CL_SUCCESS) {
			m_cl_platforms = new cl_platform_id[m_num_platforms];
			err = webcl_clGetPlatformIDs(webcl_channel_, m_num_platforms, m_cl_platforms, NULL);
		}
		if(err == CL_SUCCESS) {
			err = webcl_clGetDeviceIDs(webcl_channel_, m_cl_platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, NULL, &num_devices);
		}
		if((num_devices != 0) && (err == CL_SUCCESS)) {
			m_cl_devices = new cl_device_id[num_devices];
			err = webcl_clGetDeviceIDs(webcl_channel_, m_cl_platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, m_cl_devices,
																	&num_devices);
													
		}
		else {
			ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
			printf("Error: Device Type Not Supported \n");
			return NULL;
		}
			
		if(err == CL_SUCCESS) {
			cl_context_id = webcl_clCreateContext(webcl_channel_, NULL, 1, &m_cl_devices[0], NULL, NULL, &err);
		}
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
				case CL_INVALID_VALUE:
					printf("Error: CL_INVALID_VALUE\n");
					ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
					break;					
					//case CL_INVALID_PROPERTY:
					//	printf("Error: CL_INVALID_PROPERTY\n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;
					//case CL_INVALID_D3D10_DEVICE_KHR:
					//	printf("Error: CL_INVALID_D3D10_DEVICE_KHR\n");
					//	break;
					//case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//	printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//	break;

				default:
					printf("Error: Invaild Error Type\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;
			}

		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			o->setDevice(m_device_id_);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				ec.throwDOMException(WebCLException:: FAILURE, "WebCLException:: FAILURE");
					printf("Error: Invaild Error Type Context\n");

				return NULL;
			}
		}
		return NULL;
	}
	
	PassRefPtr<WebCLContext> WebCL::createContext(int contextProperties, 
			WebCLDeviceList* devices, int pfn_notify, int user_data, ExceptionState& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = 0;
		cl_device_id cl_device = NULL;

		if (devices != NULL) {
			cl_device = devices->getCLDevices();
			if (cl_device == NULL) {
				ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
				printf("Error: devices null\n");
				return NULL;
			}
		} else {
			printf("Error: webcl_devices null\n");
			printf("Unused in createContext prop=%d pfn_notify=%d user_data=%d\n", 
					contextProperties, pfn_notify, user_data);

			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = webcl_clCreateContext(webcl_channel_, NULL, 1, &cl_device, NULL, NULL, &err);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("CL_INVALID_PROPERTY  \n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");				
					break;
					// TODO (siba samal) Error handling following Error Types
					//	case CL_INVALID_D3D10_DEVICE_KHR:
					//		printf("Error: CL_INVALID_D3D10_DEVICE_KHR \n");
					//		break;
					//		case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//			printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//			break;

				default:
					printf("Error: Invalid ERROR Type\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;

			}
		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				return NULL;
			}
		}
		return NULL;
	}

	PassRefPtr<WebCLContext> WebCL::createContextFromType(int contextProperties, 
			int device_type, int pfn_notify, int user_data, ExceptionState& ec)
	{

		cl_int err = 0;
		cl_context cl_context_id = 0;

		//TODO (siba samal) Need to handle context properties	
		if((CONTEXT_PLATFORM != contextProperties) &&  (0 != contextProperties))
		{
			ec.throwDOMException(WebCLException::INVALID_PROPERTY, "WebCLException::INVALID_PROPERTY");
			printf("Error: INVALID CONTEXT PROPERTIES\n");
			return NULL;
		}

		if(device_type == DEVICE_TYPE_GPU)
			cl_context_id = webcl_clCreateContextFromType(webcl_channel_, NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
		else if(device_type == DEVICE_TYPE_CPU)
			cl_context_id = webcl_clCreateContextFromType(webcl_channel_, NULL, CL_DEVICE_TYPE_CPU, NULL, NULL, &err);
		else if( device_type == DEVICE_TYPE_ACCELERATOR)
			cl_context_id = webcl_clCreateContextFromType(webcl_channel_, NULL, CL_DEVICE_TYPE_ACCELERATOR, NULL, NULL, &err);
		else if(device_type == DEVICE_TYPE_DEFAULT)
			cl_context_id = webcl_clCreateContextFromType(webcl_channel_, NULL, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, &err);
		//else if(device_type == DEVICE_TYPE_ALL)
		//	cl_context_id = clCreateContextFromType(NULL, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);
		else
			printf("Error:Invalid Device Type \n");
		

		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("CL_INVALID_PROPERTY  \n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;
					// TODO (siba samal) Error handling following Error Types
					//	case CL_INVALID_D3D10_DEVICE_KHR:
					//		printf("Error: CL_INVALID_D3D10_DEVICE_KHR \n");
					//		break;
					//		case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//			printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//			break;

				default:
					printf("Error: Invalid ERROR Type\n");
					printf("CreateContext prop=%d pfn_notify=%d user_data=%d\n", 
											contextProperties, pfn_notify, user_data);
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;

			}
		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				return NULL;
			}
		}
		return NULL;
	}
	PassRefPtr<WebCLContext> WebCL::createSharedContext(int device_type, 
			int pfn_notify, int user_data, ExceptionState& ec)
	{
#if 0

		cl_int err = 0;
		cl_context cl_context_id = NULL;

        // TODO: Add GL support
#if OS(DARWIN)
		cl_context_properties properties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()), 0
            (cl_context_properties)NULL, 0
		};
#endif

#if OS(WINDOWS)
      	cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, 
            (cl_context_properties)wglGetCurrentContext(),//
            CL_WGL_HDC_KHR,
            (cl_context_properties)wglGetCurrentDC(),//
			CL_CONTEXT_PLATFORM, 
            (cl_context_properties)m_platform_id->getCLPlatforms(),
            0
		};   
#endif

#if OS(LINUX)
      	cl_context_properties properties[] = {
            CL_GL_CONTEXT_KHR, 
            (cl_context_properties)glXGetCurrentContext(),
            CL_WGL_HDC_KHR,
            (cl_context_properties)glXGetCurrentDisplay(),
			CL_CONTEXT_PLATFORM, 
            (cl_context_properties)m_platform_id->getCLPlatforms(),
            0
		}; 
#endif


		// TODO (siba samal) Handle NULL parameters
		cl_context_id = webcl_clCreateContext(webcl_channel_, properties, 0, 0, 0, 0, &err);

		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("Error: CL_INVALID_PROPERTY\n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE \n");
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec.throwDOMException(WebCLException:: INVALID_OPERATION, "WebCLException:: INVALID_OPERATION");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;
					//case CL_INVALID_D3D10_DEVICE_KHR:
					//	printf("Error: CL_INVALID_D3D10_DEVICE_KHR\n");
					//	break;
					//case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//	printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//	break;

				default:
					printf("Error: Invalid ERROR type\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;
			}

		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				printf("WebCL::createSharedContext device_type=%d pfn_notify=%d user_data=%d\n",
						device_type, pfn_notify, user_data);
				return NULL;
			}
		}
		return NULL;
#else
		return NULL;
#endif
	}

	PassRefPtr<WebCLContext> WebCL::createContext(int contextProperties, 
			WebCLDevice* device, int pfn_notify, int user_data, ExceptionState& ec)
	{
#if 0
		cl_int err = 0;
		cl_context cl_context_id = NULL;
		cl_device_id cl_device = NULL;

		if (device != NULL) {
			cl_device = device->getCLDevice();
			if (cl_device == NULL) {
				printf("Error: devices null\n");
				return NULL;
			}
			m_device_id_ = device;
		} else {
			printf("Error: webcl_devices null\n");
			return NULL;
		}

		// TODO(won.jeon) - prop, pfn_notify, and user_data need to be addressed later
		cl_context_id = webcl_clCreateContext(webcl_channel_, NULL, 1, &cl_device, NULL, NULL, &err);
		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					printf("Error: CL_INVALID_PLATFORM\n");
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
					//case CL_INVALID_PROPERTY:
					//	printf("Error: CL_INVALID_PROPERTY\n");
					//	break;
				case CL_INVALID_DEVICE:
					printf("Error: CL_INVALID_DEVICE\n");
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_INVALID_OPERATION:
					printf("Error: CL_INVALID_OPERATION\n");
					ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					printf("Error: CL_DEVICE_NOT_AVAILABLE\n");
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					printf("Error: CL_OUT_OF_RESOURCES\n");
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					printf("Error: CL_OUT_OF_HOST_MEMORY\n");
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;
					//case CL_INVALID_D3D10_DEVICE_KHR:
					//	printf("Error: CL_INVALID_D3D10_DEVICE_KHR\n");
					//	break;
					//case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR :
					//	printf("Error: CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");
					//	break;

				default:
					printf("Error: Invaild Error Type\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;
			}

		} else {
			RefPtr<WebCLContext> o = WebCLContext::create(this, cl_context_id);
			o->setDevice(m_device_id_);
			if (o != NULL) {
				m_context = o;
				return o;
			} else {
				ec.throwDOMException(WebCLException:: FAILURE, "WebCLException:: FAILURE");

				printf("WebCL::createContext prop=%d pfn_notify=%d user_data=%d\n", 
						contextProperties, pfn_notify, user_data);
				return NULL;
			}
		}
		return NULL;
#else
		return NULL;
#endif
	}


	// Frome latest WebCL spec
	PassRefPtr<WebCLContext> WebCL::createContext(WebCLContextProperties* properties, ExceptionState& ec)
	{
		cl_platform_id platform_id = 0;
		if (properties && properties->platform()) {
			platform_id = properties->platform()->getCLPlatform();
		} else {
			cl_int err = 0;
			// typically there is only 1 platform
			err = webcl_clGetPlatformIDs(webcl_channel_, 1, &platform_id, NULL);
			if (err != CL_SUCCESS) {
				switch (err) {
					case CL_INVALID_VALUE:
						ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
						break;
					case CL_OUT_OF_HOST_MEMORY:
						ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
						break;
					default:
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						break;
				}
				return NULL;
			}
		}

		// Use default device type if no properties provided.
		cl_device_type device_type = WebCL::DEVICE_TYPE_DEFAULT;
		if (properties)
			device_type = properties->deviceType();

		cl_device_id device_id = 0;
		if (properties && properties->devices())
			device_id = properties->devices()->getCLDevices();

		cl_context cl_context_id = 0;
		bool shareResource = false;
		cl_context_properties sharedContext = 0;
		cl_context_properties sharedDisplay = 0;
#if 0
		if (properties && properties->sharedWebGLContext()) {
			GraphicsContext3D* gc3d = properties->sharedWebGLContext()->graphicsContext3D();
			if (gc3d != NULL) {
				PlatformGraphicsContext3D pgc3d = gc3d->platformGraphicsContext3D();
				PlatformDisplay3D pd3d = gc3d->platformDisplay3D();
				if (!pgc3d || !pd3d) {
					printf("Error: PlatformGraphicsContext3D or PlatformDisplay3D is NULL\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					return NULL;
				}

				sharedContext = (cl_context_properties)pgc3d;
				sharedDisplay = (cl_context_properties)(&pd3d);
				shareResource = true;
			} else {
				printf("Error: No GraphicsContext3D is found with the WebGLContext\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
			}
		}
#endif // #if 0

		cl_int err = 0;
#if 0
		if (shareResource) {
			cl_context_properties context_properties[] = {
#if OS(WINDOWS)
				CL_GL_CONTEXT_KHR, 
				//(cl_context_properties)wglGetCurrentContext(),
				sharedContext,
				CL_WGL_HDC_KHR,
				//CL_EGL_DISPLAY_KHR,
				/*(cl_context_properties)wglGetCurrentDC(),*/
				sharedDisplay,
				//(cl_context_properties)GetDC(NULL),
#endif
#if OS(LINUX)
				CL_GL_CONTEXT_KHR, 
				//(cl_context_properties)glXGetCurrentContext(),
				sharedContext,
				CL_GLX_DISPLAY_KHR,
				//(cl_context_properties)glXGetCurrentDisplay(),
				sharedDisplay,
#endif
#if OS(DARWIN)
				CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
				(cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()),
#endif
				CL_CONTEXT_PLATFORM,
				(cl_context_properties)platform_id,
				0
			};
			//add by peiying
			if (!clGetGLContextInfoKHR) 
			{
				clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform_id, "clGetGLContextInfoKHR");
				if (!clGetGLContextInfoKHR) 
				{
					printf("Failed to query proc address for clGetGLContextInfoKHR\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					return NULL;
				}
			}
			int status;
			cl_device_id interopDevice;
			if(device_type = WebCL::DEVICE_TYPE_GPU)
			{
				status = clGetGLContextInfoKHR( context_properties, 
					CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
					sizeof(cl_device_id), 
					&interopDevice, 
					NULL);
				if(status != CL_SUCCESS)
				{
					printf("clGetGLContextInfoKHR failed!\n");
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					return NULL;
				}
			}
			/*	if(device_id != interopDevice)
			{
			printf("webcl and webgl interOp devive ID is diferent!");
			ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
			return NULL;

			}*/


			if (properties && properties->devices()) {
				// deviceType will be ignored
				cl_context_id = webcl_clCreateContext(webcl_channel_, context_properties, 1, &device_id, NULL, NULL, &err);
			} else {
				// deviceType will be used
				cl_context_id = webcl_clCreateContextFromType(webcl_channel_, context_properties, device_type, NULL, NULL, &err);
			}
		}else
#endif // #if 0

		{
			cl_context_properties context_properties[] = {
				CL_CONTEXT_PLATFORM,
				(cl_context_properties)platform_id,
				0,
			};

			if (properties && properties->devices()) {
				cl_context_id = webcl_clCreateContext(webcl_channel_, context_properties, 1, &device_id, NULL, NULL, &err);
			} else {
				cl_context_id = webcl_clCreateContextFromType(webcl_channel_, context_properties, device_type, NULL, NULL, &err);
			}

		} // shareResource
	

		if (err != CL_SUCCESS) {
			switch (err) {
				case CL_INVALID_PLATFORM:
					ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
					break;
				case CL_INVALID_PROPERTY:
					ec.throwDOMException(WebCLException::INVALID_PROPERTY, "WebCLException::INVALID_PROPERTY");
					break;
				case CL_INVALID_VALUE:
					ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
					break;
				case CL_INVALID_DEVICE:
					ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
					break;
				case CL_DEVICE_NOT_AVAILABLE:
					ec.throwDOMException(WebCLException::DEVICE_NOT_AVAILABLE, "WebCLException::DEVICE_NOT_AVAILABLE");
					break;
				case CL_OUT_OF_RESOURCES:
					ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
					break;
				case CL_OUT_OF_HOST_MEMORY:
					ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
					break;
				default:
					ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
					break;
			}
			return NULL;
		}

		RefPtr<WebCLContext> O = WebCLContext::create(this, cl_context_id);
		if (O != NULL) {
			O->setDevice(properties->devices()->item(0));
			m_context = O;
			return O;
		} else {
			return NULL;
		}
	}

	void WebCL::unloadCompiler(ExceptionState& ec)
	{
    cl_int err;

#if defined(CL_VERSION_1_2)
    cl_platform_id* m_cl_platforms = NULL;
    unsigned int m_num_platforms;
    err = webcl_clGetPlatformIDs(webcl_channel_, 0, NULL, &m_num_platforms);

    if(err == CL_SUCCESS) {
      m_cl_platforms = new cl_platform_id[m_num_platforms];
      err = webcl_clGetPlatformIDs(webcl_channel_, m_num_platforms, m_cl_platforms, NULL);
    }
    err = webcl_clUnloadPlatformCompiler(webcl_channel_, m_cl_platforms[0]);
    delete[] m_cl_platforms;
#else
    err =  clUnloadCompiler();
#endif

		if (err != CL_SUCCESS) {
			printf("Error: Invaild Error Type\n");
			ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE"); 
		}
		else {
		}
		return;
	}

} // namespace WebCore

#endif // ENABLE(WEBCL)

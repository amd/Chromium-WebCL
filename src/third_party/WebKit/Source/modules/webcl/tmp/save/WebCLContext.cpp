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

#include "WebCLContext.h"
#include "WebCLContext.h"
#include "WebCLCommandQueue.h"
#include "WebCLProgram.h"
#include "WebCLKernel.h"
#include "WebCLMem.h"
#include "WebCLEvent.h"
#include "WebCLSampler.h"
#include "WebCL.h"
#include "WebCLException.h"

#include <gl/GL.h>

namespace WebCore {

WebCLContext::~WebCLContext()
{
}

PassRefPtr<WebCLContext> WebCLContext::create(WebCL* compute_context, cl_context context_id)
{
		return adoptRef(new WebCLContext(compute_context, context_id));
}

		WebCLContext::WebCLContext(WebCL* compute_context, cl_context context_id) 
: m_videoCache(4), m_context(compute_context), m_cl_context(context_id)
{
		m_num_programs = 0;
		m_num_mems = 0;
		m_num_events = 0;
		m_num_samplers = 0;
		m_num_contexts = 0;

}

WebCLGetInfo WebCLContext::getInfo(int param_name, ExceptionState& ec)
{
		cl_int err = 0;
		cl_uint uint_units = 0;
		RefPtr<WebCLDeviceList> deviceList  = NULL;
		size_t szParmDataBytes = 0;
		size_t uint_array[1024] = {0};

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return WebCLGetInfo();
		}

		switch(param_name)
		{	
				case WebCL::CONTEXT_REFERENCE_COUNT:
						err = webcl_clGetContextInfo(webcl_channel_, m_cl_context, CL_CONTEXT_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
						if (err == CL_SUCCESS)
								return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
						break;
				# if 0 //OpenCl1.1
				case WebCL::CONTEXT_NUM_DEVICES:
						err = clGetContextInfo(m_cl_context,CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &uint_units, NULL);
						if (err == CL_SUCCESS)
								return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
						break;
				#endif
				case WebCL::CONTEXT_DEVICES:
						cl_device_id* cdDevices;
						webcl_clGetContextInfo(webcl_channel_, m_cl_context, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
						if (err == CL_SUCCESS) {
								int nd = szParmDataBytes / sizeof(cl_device_id);
								cdDevices = (cl_device_id*) malloc(szParmDataBytes);
								webcl_clGetContextInfo(webcl_channel_, m_cl_context, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
								deviceList = WebCLDeviceList::create(m_context, cdDevices, nd);
								printf("Size Vs Size = %lu %d %d \n\n", szParmDataBytes,nd,deviceList->length());
								free(cdDevices);
								return WebCLGetInfo(PassRefPtr<WebCLDeviceList>(deviceList));
						}
						break;	
				case WebCL::CONTEXT_PROPERTIES:
						err = webcl_clGetContextInfo(webcl_channel_, m_cl_context, CL_CONTEXT_PROPERTIES, 0, NULL, &szParmDataBytes);
						if (err == CL_SUCCESS) {
								int nd = szParmDataBytes / sizeof(cl_uint);
								if(nd == 0)	 {	
										printf("No Context Properties defined \n");
										return WebCLGetInfo();
								}
								err = webcl_clGetContextInfo(webcl_channel_, m_cl_context, CL_CONTEXT_PROPERTIES, szParmDataBytes, &uint_array, &szParmDataBytes);
								if (err == CL_SUCCESS) {
										// Should int repacle cl_context_properties
										int values[1024] = {0};
										for(int i=0; i<((int)nd); i++)
										{
												values[i] = (int)uint_array[i];
												printf("%d\n", values[i]);
										}
										return WebCLGetInfo(Int32Array::create(values, nd));
								}	
						}
						break;
				default:
						printf("Error: Unsupported Context Info type\n");
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						return WebCLGetInfo();
		}
		switch (err) {
				case CL_INVALID_CONTEXT:
						ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
						printf("Error: CL_INVALID_CONTEXT \n");
						break;
				case CL_INVALID_VALUE:
						ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
						printf("Error: CL_INVALID_VALUE\n");
						break;
				case CL_OUT_OF_RESOURCES:
						ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
						printf("Error: CL_OUT_OF_RESOURCES \n");
						break;
				case CL_OUT_OF_HOST_MEMORY:
						ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
						printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
						break;
				default:
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						printf("Error: Invaild Error Type\n");
						break;
		}				
		return WebCLGetInfo();
}

PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDeviceList* devices, 
				int command_queue_prop, ExceptionState& ec)
{
		cl_int err = 0;
		cl_device_id cl_device = NULL;
		cl_command_queue cl_command_queue_id = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}
		if (devices != NULL) {
				cl_device = devices->getCLDevices();
				if (cl_device == NULL) {
						ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
						printf("Error: cl_device null\n");
						return NULL;
				}
		}
		switch (command_queue_prop)
		{
				case WebCL::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
						cl_command_queue_id = webcl_clCreateCommandQueue(webcl_channel_, m_cl_context, cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
						break;
				case WebCL::QUEUE_PROFILING_ENABLE:
						cl_command_queue_id = webcl_clCreateCommandQueue(webcl_channel_, m_cl_context, cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
						break;
				default:
						cl_command_queue_id = webcl_clCreateCommandQueue(webcl_channel_, m_cl_context, cl_device, NULL, &err);
						break;
		}
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
						case CL_INVALID_DEVICE:
								printf("Error: CL_INVALID_DEVICE \n");
								ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
								break;
						case CL_INVALID_QUEUE_PROPERTIES:
								printf("Error: CL_INVALID_QUEUE_PROPERTIES \n");
								ec.throwDOMException(WebCLException::INVALID_QUEUE_PROPERTIES, "WebCLException::INVALID_QUEUE_PROPERTIES");
								break;
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
		} else {
				RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
				if (o != NULL) {
						m_command_queue = o;
						return o;
				} else {
						ec.throwDOMException(WebCLException::INVALID_COMMAND_QUEUE, "WebCLException::INVALID_COMMAND_QUEUE");
						return NULL;
				}
		}
		return NULL;
}
/*
PassRefPtr<WebCLCommandQueue> WebCLContext::createCommandQueue(WebCLDevice* device, int command_queue_prop, ExceptionState& ec)
{
		cl_int err = 0;
		cl_device_id cl_device = NULL;
		cl_command_queue cl_command_queue_id = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}
		if (device != NULL) {
				cl_device = device->getCLDevice();
				if (cl_device == NULL) {
						printf("Error: cl_device null\n");
						return NULL;
				}
		}
		switch (command_queue_prop)
		{
				case WebCL::QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE:
						cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
						break;
				case WebCL::QUEUE_PROFILING_ENABLE:
						cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, CL_QUEUE_PROFILING_ENABLE, &err);
						break;
				default:
						cl_command_queue_id = clCreateCommandQueue(m_cl_context, cl_device, NULL, &err);
						break;
		}

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
						case CL_INVALID_DEVICE:
								printf("Error: CL_INVALID_DEVICE  \n");
								ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
								break;
						case CL_INVALID_QUEUE_PROPERTIES:
								printf("Error: CL_INVALID_QUEUE_PROPERTIES  \n");
								ec.throwDOMException(WebCLException::INVALID_QUEUE_PROPERTIES, "WebCLException::INVALID_QUEUE_PROPERTIES");
								break;
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

		} else {
				RefPtr<WebCLCommandQueue> o = WebCLCommandQueue::create(m_context, cl_command_queue_id);
				if (o != NULL) {
						m_command_queue = o;
						return o;
				} else {
						ec.throwDOMException(WebCLException::INVALID_COMMAND_QUEUE, "WebCLException::INVALID_COMMAND_QUEUE");
						return NULL;
				}
		}
		return NULL;
}
*/

PassRefPtr<WebCLProgram> WebCLContext::createProgram(const String& kernelSource, ExceptionState& ec)
{
		cl_int err = 0;
		cl_program cl_program_id = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}
		const char* source = strdup(kernelSource.utf8().data());

		// TODO(won.jeon) - the second and fourth arguments need to be addressed later
		cl_program_id = webcl_clCreateProgramWithSource(webcl_channel_, m_cl_context, 1, (const char**)&source, 
						NULL, &err);

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

		} else {
				RefPtr<WebCLProgram> o = WebCLProgram::create(m_context, cl_program_id);
				o->setDevice(m_device_id);
				if (o != NULL) {
						m_program_list.append(o);
						m_num_programs++;
						return o;
				} else {
						ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
						return NULL;
				}
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createBuffer(int flags, int size, ExceptionState& ec)
{
		cl_int err = 0;	
		cl_mem cl_mem_id = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}
		// TODO(won.jeon) - NULL parameter needs to be addressed later
		switch (flags)
		{
				case WebCL::MEM_READ_ONLY:
						cl_mem_id = webcl_clCreateBuffer(webcl_channel_, m_cl_context, CL_MEM_READ_ONLY, size, NULL, &err);
						break;
				case WebCL::MEM_WRITE_ONLY:
						cl_mem_id = webcl_clCreateBuffer(webcl_channel_, m_cl_context, CL_MEM_WRITE_ONLY, size, NULL, &err);
						break;
				case WebCL::MEM_READ_WRITE:
						cl_mem_id = webcl_clCreateBuffer(webcl_channel_, m_cl_context, CL_MEM_READ_WRITE, size, NULL, &err);
						break;
				default:
						printf("Error: Unsupported Mem Flsg\n");
						//printf("WebCLContext::createBuffer host_ptr = %d\n", host_ptr);
						ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
						break;
		}
		if (err != CL_SUCCESS) {
				printf("Error: clCreateBuffer\n");
				switch(err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_BUFFER_SIZE:
								printf("Error: CL_INVALID_BUFFER_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_BUFFER_SIZE, "WebCLException::INVALID_BUFFER_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}


PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, 
				HTMLCanvasElement* canvasElement, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format = {CL_RGBA, CL_UNSIGNED_INT32};
		cl_uint width = 0;
		cl_uint height = 0;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}	
		ImageBuffer* imageBuffer = NULL;
		RefPtr<Uint8Array> bytearray = NULL;
		if (canvasElement != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				width = (cl_uint) canvasElement->width();
				height = (cl_uint) canvasElement->height();

				imageBuffer = canvasElement->buffer();
				if ( imageBuffer == NULL)
						printf("image is null\n");
				bytearray = imageBuffer->getUnmultipliedImageData(IntRect(0,0,width,height));

				if ( bytearray == NULL)
						printf("bytearray is null\n");
		}

		if(bytearray->data() == NULL)
		{
				printf("bytearray->data() is null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		void* image = (void*) bytearray->data();
		if(image == NULL)
		{
				printf("image is null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
				cl_image_desc clImageDescriptor;
						clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE2D;
						clImageDescriptor.image_width = width;
						clImageDescriptor.image_height = height;
						cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,
										&image_format,&clImageDescriptor,image,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR , 
										&image_format, width, height,0, image, &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
					
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,(CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR |CL_MEM_COPY_HOST_PTR),&image_format,&clImageDescriptor,image,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, (CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR |CL_MEM_COPY_HOST_PTR), 
										&image_format, width, height, 0, image, &err);
#endif
						break;
						// TODO (siba samal) Support other mem_flags & testing 
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}	

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLImageElement* image, ExceptionState& ec)
{
	/*
		cl_int err = 0;
		cl_mem cl_mem_image = 0;
		cl_image_format image_format;
		cl_uint width = 0;
		cl_uint height = 0;

		Image* imagebuf = NULL;
		CachedImage* cachedImage = NULL; 
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
				return NULL;
		}
		if (image != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				cachedImage = image->cachedImage();
				if (cachedImage == NULL) {
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						printf("Error: image null\n");
						return NULL;
				} 
				else {
						width = (cl_uint) image->width();
						height = (cl_uint) image->height();
						imagebuf = cachedImage->image();
						if(imagebuf == NULL)
						{
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								printf("Error: imagebuf null\n");
								return NULL;
						}
				}
		} 
		else {
				printf("Error: imageElement null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		const char* image1  = (const char*)cachedImage->image()->data()->data() ;
		if(image1 == NULL)
		{
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				printf("Error: image data is null\n");
				return NULL;
		}
		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
						cl_image_desc clImageDescriptor;
						clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE2D;
						clImageDescriptor.image_width = width;
						clImageDescriptor.image_height = height;
						cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,&image_format,&clImageDescriptor,(void*)image1,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
										&image_format, width, height, 0, (void*)image1, &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
					
#if defined(CL_VERSION_1_2)
					 
						cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_WRITE_ONLY ,&image_format,&clImageDescriptor,(void*)image1,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
										&image_format, width, height, 0, (void*)image1, &err);
#endif
						break;
						// TODO (siba samal) Support other flags & testing
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break; 
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
*/
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, HTMLVideoElement* video, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format;
		cl_uint width = 0;
		cl_uint height = 0;

		RefPtr<Image> image = NULL;
		SharedBuffer* sharedBuffer = NULL;
		const char* image_data = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		if (video != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				width = (cl_uint) video->width();
				height = (cl_uint) video->height();
				image = videoFrameToImage(video);
				sharedBuffer = image->data();
				if (sharedBuffer == NULL) {
						printf("Error: sharedBuffer null\n");
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						return NULL;
				} else {
						image_data = sharedBuffer->data();

						if (image_data == NULL) {
								printf("Error: image_data null\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								return NULL;
						}
				}

		} else {
				printf("Error: canvasElement null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
						cl_image_desc clImageDescriptor;
						clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE2D;
						clImageDescriptor.image_width = width;
						clImageDescriptor.image_height = height;
						cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,&image_format,&clImageDescriptor,(void*)image_data,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY| CL_MEM_USE_HOST_PTR, 
										&image_format, width, height, 0, (void *)image_data, &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
					  
						cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_WRITE_ONLY,&image_format,&clImageDescriptor,(void*)image_data,&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
										&image_format, width, height, 0, (void *)image_data, &err);
#endif
						break;
						// TODO (siba samal) Support other flags & testing
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags, ImageData* data, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format;
		cl_uint width = 0;
		cl_uint height = 0;

		Uint8ClampedArray* pixelarray = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		if (data != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				pixelarray = data->data();
				if(pixelarray == NULL)
				{
				    return NULL;
				}			
				width = (cl_uint) data->width();
				height = (cl_uint) data->height();
		} else {
				printf("Error: canvasElement null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
					    cl_image_desc clImageDescriptor;
                        clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE2D;
                        clImageDescriptor.image_width = width;
                        clImageDescriptor.image_height = height;
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,&image_format,&clImageDescriptor,(void*)(pixelarray->data()),&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
										&image_format, width, height, 0, (void*)(pixelarray->data()), &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
					 
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_WRITE_ONLY , &image_format,&clImageDescriptor,(void*)(pixelarray->data()),&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
										&image_format, width, height, 0, (void*)(pixelarray->data()), &err);
#endif
						break;
						// TODO (siba samal) Support other flags & testing
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage2D(int flags,unsigned int width, 
				unsigned int height,ArrayBuffer* data, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format;
		cl_uint cl_width = 0;
		cl_uint cl_height = 0;

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		if (data != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				cl_width = width;
				cl_height = height;
		} else {
				printf("Error: canvasElement null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
					    cl_image_desc clImageDescriptor;
                        clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE2D;
                        clImageDescriptor.image_width = width;
                        clImageDescriptor.image_height = height;
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,&image_format,&clImageDescriptor,(void*)(data->data()),&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
										&image_format, cl_width, cl_height, 0, data->data(), &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
					   
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_WRITE_ONLY ,&image_format,&clImageDescriptor,(void*)(data->data()),&err);
#else
						cl_mem_image = clCreateImage2D(m_cl_context, CL_MEM_WRITE_ONLY, 
										&image_format, cl_width, cl_height, 0, data->data(), &err);
#endif
						break;
						// TODO (siba samal) Support other flags & testing
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createImage3D(int flags,unsigned int width, 
				unsigned int height,
				unsigned int depth,
				ArrayBuffer* data, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_image = NULL;
		cl_image_format image_format;
		cl_uint cl_width = 0;
		cl_uint cl_height = 0;
		cl_uint cl_depth = 0;

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		if (data != NULL) {
				image_format.image_channel_data_type = CL_UNSIGNED_INT8;
				image_format.image_channel_order = CL_RGBA;
				cl_width = width;
				cl_height = height;
				cl_depth = depth;
		} else {
				printf("Error: canvasElement null\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
					    cl_image_desc clImageDescriptor;
                        clImageDescriptor.image_type = CL_MEM_OBJECT_IMAGE3D;
                        clImageDescriptor.image_width = width;
                        clImageDescriptor.image_height = height;
						clImageDescriptor.image_depth = depth;
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_READ_ONLY |CL_MEM_USE_HOST_PTR,&image_format,&clImageDescriptor,(void*)(data->data()),&err);
#else
						cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, 
										&image_format, cl_width, cl_height, cl_depth,0, 0, data->data(), &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
					   
					    cl_mem_image = webcl_clCreateImage(webcl_channel_, m_cl_context,CL_MEM_WRITE_ONLY,&image_format,&clImageDescriptor,(void*)(data->data()),&err);
#else
						cl_mem_image = clCreateImage3D(m_cl_context, CL_MEM_WRITE_ONLY, 
										&image_format, cl_width, cl_height, cl_depth, 0, 0,data->data(), &err);
#endif
						break;
						// TODO (siba samal) Support other flags & testing
		}
		if (cl_mem_image == NULL) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_IMAGE_SIZE:
								printf("Error: CL_INVALID_IMAGE_SIZE\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_SIZE, "WebCLException::INVALID_IMAGE_SIZE");
								break;
						case CL_INVALID_HOST_PTR:
								printf("Error: CL_INVALID_HOST_PTR\n");
								ec.throwDOMException(WebCLException::INVALID_HOST_PTR, "WebCLException::INVALID_HOST_PTR");
								break;
						case CL_IMAGE_FORMAT_NOT_SUPPORTED:
								printf("Error: CL_INVALID_FORMAT_NOT_SUPPORTED\n");
								ec.throwDOMException(WebCLException::IMAGE_FORMAT_NOT_SUPPORTED, "WebCLException::IMAGE_FORMAT_NOT_SUPPORTED");
								break;
						case CL_MEM_OBJECT_ALLOCATION_FAILURE:
								printf("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
								ec.throwDOMException(WebCLException::MEM_OBJECT_ALLOCATION_FAILURE, "WebCLException::MEM_OBJECT_ALLOCATION_FAILURE");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_image,false);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLBuffer(int flags, WebGLBuffer* bufobj, ExceptionState& ec)
{
		cl_mem cl_mem_id = NULL;
		cl_int err = 0;
		if (bufobj == NULL || bufobj->object() == 0)
		{
			printf("Invalade GL object!\n");
			return NULL;
		}
		const Platform3DObject& buf_id = bufobj->object();

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		cl_mem_flags clflags;

		switch (flags) {
				case WebCL::MEM_READ_ONLY:
						clflags = CL_MEM_READ_ONLY;
						break;
				case WebCL::MEM_WRITE_ONLY:
						clflags = CL_MEM_WRITE_ONLY;
						break;
				case WebCL::MEM_READ_WRITE:
						clflags = CL_MEM_READ_WRITE;
						break;
				case WebCL::MEM_USE_HOST_PTR:
						clflags = CL_MEM_USE_HOST_PTR;
						break;
				case WebCL::MEM_ALLOC_HOST_PTR:
						clflags = CL_MEM_ALLOC_HOST_PTR;
						break;
				case WebCL::MEM_COPY_HOST_PTR:
						clflags = CL_MEM_COPY_HOST_PTR;
						break;

		}
		cl_mem_id = webcl_clCreateFromGLBuffer(webcl_channel_, m_cl_context, clflags, bufobj->object(), &err);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_GL_OBJECT:
								printf("Error: CL_INVALID_GL_OBJECT\n");
								ec.throwDOMException(WebCLException::INVALID_GL_OBJECT, "WebCLException::INVALID_GL_OBJECT");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
				m_mem_list.append(o);
				m_num_mems++;
				return o;

		}
		return NULL;
}
PassRefPtr<WebCLImage> WebCLContext::createFromGLRenderBuffer(int flags, WebGLRenderbuffer* renderbufferobj, ExceptionState& ec)
{
	cl_mem cl_mem_id = NULL;
	cl_int err = 0;
	GLuint rbuf_id = 0;

	if (m_cl_context == NULL) {
		printf("Error: Invalid CL Context\n");
		ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
		return NULL;
	}
	if (renderbufferobj != NULL) {
       rbuf_id =  renderbufferobj->getInternalFormat();  
	
	}
		switch (flags) {
				case WebCL::MEM_READ_ONLY:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_READ_ONLY, rbuf_id, &err);
						break;
				case WebCL::MEM_WRITE_ONLY:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_WRITE_ONLY, rbuf_id, &err);
						break;
				case WebCL::MEM_READ_WRITE:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_READ_WRITE, rbuf_id, &err);
						break;
				case WebCL::MEM_USE_HOST_PTR:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_USE_HOST_PTR, rbuf_id, &err);
						break;
				case WebCL::MEM_ALLOC_HOST_PTR:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_ALLOC_HOST_PTR, rbuf_id, &err);
						break;
				case WebCL::MEM_COPY_HOST_PTR:
						cl_mem_id = clCreateFromGLRenderbuffer(m_cl_context, CL_MEM_COPY_HOST_PTR, rbuf_id, &err);
						break;

		}
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
								break;
						case CL_INVALID_GL_OBJECT:
								printf("Error: CL_INVALID_GL_OBJECT\n");
								ec.throwDOMException(WebCLException::INVALID_GL_OBJECT, "WebCLException::INVALID_GL_OBJECT");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLImage> o = WebCLImage::create(m_context, cl_mem_id, true);
				m_img_list.append(o);
				m_num_images++;
				return o;

		}
		return NULL;
                     
}

PassRefPtr<WebCLSampler> WebCLContext::createSampler(bool norm_cords, 
				int addr_mode, int fltr_mode, ExceptionState& ec)
{
		cl_int err = 0;
		cl_bool normalized_coords = CL_FALSE;
		cl_addressing_mode addressing_mode = CL_ADDRESS_NONE;
		cl_filter_mode filter_mode = CL_FILTER_NEAREST;
		cl_sampler cl_sampler_id = NULL;

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		if(norm_cords)
				normalized_coords = CL_TRUE;
		switch(addr_mode)
		{
				case WebCL::ADDRESS_NONE:
						addressing_mode = CL_ADDRESS_NONE;
						break;
				case WebCL::ADDRESS_CLAMP_TO_EDGE:
						addressing_mode = CL_ADDRESS_CLAMP_TO_EDGE;
						break;
				case WebCL::ADDRESS_CLAMP:
						addressing_mode = CL_ADDRESS_CLAMP;
						break;
				case WebCL::ADDRESS_REPEAT: 
						addressing_mode = CL_ADDRESS_REPEAT;
						break;
				default:
						printf("Error: Invaild Addressing Mode\n");
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						return NULL;
		}
		switch(fltr_mode)
		{
				case WebCL::FILTER_LINEAR:
						filter_mode = CL_FILTER_LINEAR;
						break;
				case WebCL::FILTER_NEAREST :
						filter_mode = CL_FILTER_NEAREST ;
						break;
				default:
						printf("Error: Invaild Filtering Mode\n");
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						return NULL;
		}
		cl_sampler_id = webcl_clCreateSampler(webcl_channel_, m_cl_context, normalized_coords, addressing_mode, 
						filter_mode, &err);

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
						case CL_INVALID_OPERATION :
								printf("Error: CL_INVALID_OPERATION   \n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION  , "WebCLException::INVALID_OPERATION  ");
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

		} else {
				RefPtr<WebCLSampler> o = WebCLSampler::create(m_context, cl_sampler_id);
				if (o != NULL) {
						m_sampler_list.append(o);
						m_num_samplers++;
						return o;
				} else {
						ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
						return NULL;
				}
		}
		return NULL;
}

PassRefPtr<WebCLMem> WebCLContext::createFromGLTexture2D(int flags, 
				GC3Denum texture_target, GC3Dint miplevel, GC3Duint texture, ExceptionState& ec)
{
		cl_int err = 0;
		cl_mem cl_mem_id = NULL;

		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}
		switch (flags)
		{
				case WebCL::MEM_READ_ONLY:
#if defined(CL_VERSION_1_2)
                        cl_mem_id = clCreateFromGLTexture(m_cl_context, CL_MEM_READ_ONLY, texture_target, miplevel, texture, &err);
#else
						cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_ONLY, 
										texture_target, miplevel, texture, &err);
#endif
						break;
				case WebCL::MEM_WRITE_ONLY:
#if defined(CL_VERSION_1_2)
                        cl_mem_id = clCreateFromGLTexture(m_cl_context, CL_MEM_WRITE_ONLY, texture_target, miplevel, texture, &err);
#else
						cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_WRITE_ONLY,
										texture_target, miplevel, texture, &err);
#endif
						break;
				case WebCL::MEM_READ_WRITE:
#if defined(CL_VERSION_1_2)
                        cl_mem_id = clCreateFromGLTexture(m_cl_context, CL_MEM_READ_WRITE, texture_target, miplevel, texture, &err);
#else				
						cl_mem_id = clCreateFromGLTexture2D(m_cl_context, CL_MEM_READ_WRITE,
										texture_target, miplevel, texture, &err);
#endif
						break;
		}
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_CONTEXT:
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_VALUE:
								printf("Error: CL_INVALID_VALUE\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_INVALID_MIP_LEVEL:
								printf("Error: CL_INVALID_MIP_LEVEL\n");
								ec.throwDOMException(WebCLException::INVALID_MIP_LEVEL, "WebCLException::INVALID_MIP_LEVEL");
								break;
						case CL_INVALID_GL_OBJECT  :
								printf("Error: CL_INVALID_GL_OBJECT\n");
								ec.throwDOMException(WebCLException::INVALID_GL_OBJECT, "WebCLException::INVALID_GL_OBJECT");
								break;

						case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
								printf("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
								ec.throwDOMException(WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR, "WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR");
								break;
						case CL_INVALID_OPERATION:
								printf("Error: CL_INVALID_OPERATION\n");
								ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
								break;
						case CL_OUT_OF_RESOURCES:
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY:
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				RefPtr<WebCLMem> o = WebCLMem::create(m_context, cl_mem_id, true);
				m_mem_list.append(o);
				m_num_mems++;
				return o;
		}
		return NULL;
}

PassRefPtr<WebCLEvent> WebCLContext::createUserEvent( ExceptionState& ec)
{
		cl_int err = -1;	
		cl_event event = NULL;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return NULL;
		}

		//(TODO) To be uncommented for OpenCL1.1
		//event =  clCreateUserEvent(cl_context_id, &err);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_CONTEXT :
								printf("Error: CL_INVALID_CONTEXT \n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_OUT_OF_RESOURCES :
								printf("Error: CCL_OUT_OF_RESOURCES \n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY :
								printf("Error: CCL_OUT_OF_HOST_MEMORY \n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}

		} else {
				RefPtr<WebCLEvent> o = WebCLEvent::create(m_context, event);
				m_event_list.append(o);
				m_num_events++;
				return o;
		}
		return NULL;
}

void WebCLContext::releaseCL( ExceptionState& ec)
{
		cl_int err = 0;
		if (m_cl_context == NULL) {
				printf("Error: Invalid CL Context\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				return;
		}
		err = webcl_clReleaseContext(webcl_channel_, m_cl_context);
		if (err != CL_SUCCESS) {
				switch (err) {
						case CL_INVALID_CONTEXT :
								printf("Error: CL_INVALID_CONTEXT\n");
								ec.throwDOMException(WebCLException::INVALID_CONTEXT, "WebCLException::INVALID_CONTEXT");
								break;
						case CL_OUT_OF_RESOURCES  :
								printf("Error: CL_OUT_OF_RESOURCES\n");
								ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
								break;
						case CL_OUT_OF_HOST_MEMORY  :
								printf("Error: CL_OUT_OF_HOST_MEMORY\n");
								ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
								break;
						default:
								printf("Error: Invaild Error Type\n");
								ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
								break;
				}
		} else {
				m_context = NULL;
				return;
		}
		return;
}

void WebCLContext::setDevice(RefPtr<WebCLDevice> device_id)
{
		m_device_id = device_id;
}
cl_context WebCLContext::getCLContext()
{
		return m_cl_context;
}
PassRefPtr<Image> WebCLContext::videoFrameToImage(HTMLVideoElement* video)
{
		if (!video || !video->videoWidth() || !video->videoHeight()) {
				return 0;
		}
		IntSize size(video->videoWidth(), video->videoHeight());
		ImageBuffer* buf = m_videoCache.imageBuffer(size);
		if (!buf) {
				return 0;
		}
		IntRect destRect(0, 0, size.width(), size.height());
		// FIXME: Turn this into a GPU-GPU texture copy instead of CPU readback.
		video->paintCurrentFrameInContext(buf->context(), destRect);
		return buf->copyImage();
}

		WebCLContext::LRUImageBufferCache::LRUImageBufferCache(int capacity)
		: m_buffers(adoptArrayPtr(new OwnPtr<ImageBuffer>[capacity]))
		  , m_capacity(capacity)
{
}

ImageBuffer* WebCLContext::LRUImageBufferCache::imageBuffer(const IntSize& size)
{
		int i;
		for (i = 0; i < m_capacity; ++i) {
				ImageBuffer* buf = m_buffers[i].get();
				if (!buf)
						break;
				// TODO siba check this call
				//if (buf->size() != size)
				//	continue;
				bubbleToFront(i);
				return buf;
		}

		OwnPtr<ImageBuffer> temp = ImageBuffer::create(size);
		if (!temp)
				return 0;
		i = std::min(m_capacity - 1, i);
		m_buffers[i] = temp.release();

		ImageBuffer* buf = m_buffers[i].get();
		bubbleToFront(i);
		return buf;
}

void WebCLContext::LRUImageBufferCache::bubbleToFront(int idx)
{
		for (int i = idx; i > 0; --i)
				m_buffers[i].swap(m_buffers[i-1]);
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

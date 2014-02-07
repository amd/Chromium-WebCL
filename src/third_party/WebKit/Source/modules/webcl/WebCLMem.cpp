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

#include "WebCLMem.h"
#include "WebCL.h"

namespace WebCore {

WebCLMem::~WebCLMem()
{
}

PassRefPtr<WebCLMem> WebCLMem::create(WebCL* compute_context, 
	cl_mem mem, bool is_shared = false)
{
	return adoptRef(new WebCLMem(compute_context, mem, is_shared));
}

WebCLMem::WebCLMem(WebCL* compute_context, cl_mem mem, bool is_shared) 
		: m_context(compute_context), m_cl_mem(mem), m_shared(is_shared)
{
	m_num_mems = 0;
}

WebCLGetInfo WebCLMem::getInfo(int param_name, ExceptionState& ec)
{
	cl_int err =0;
	cl_uint uint_units = 0;
	size_t sizet_units = 0;
	RefPtr<WebCLContext> contextObj = NULL;
	cl_context cl_context_id = NULL;
	cl_mem_object_type mem_type = 0;
	void* mem_ptr = NULL; 
	if (m_cl_mem == NULL) {
		ec.throwDOMException(WebCL::INVALID_MEM_OBJECT, "WebCL::INVALID_MEM_OBJECT");
		printf("Error: Invalid CLMem\n");
		return WebCLGetInfo();
	}
	switch(param_name)
	{   	
		case WebCL::MEM_MAP_COUNT:
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_MAP_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCL::MEM_REFERENCE_COUNT:
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCL::MEM_SIZE:
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_SIZE, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case WebCL::MEM_TYPE:			
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_TYPE, sizeof(cl_mem_object_type), &mem_type, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(mem_type));
			break;
		case WebCL::MEM_CONTEXT:			
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
			contextObj = WebCLContext::create(m_context, cl_context_id);
			if(contextObj == NULL)
			{
				printf("Error: CL Mem context not NULL\n");
				return WebCLGetInfo();
			}
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
			break;
		case WebCL::MEM_HOST_PTR:			
			err = webcl_clGetMemObjectInfo(webcl_channel_, m_cl_mem, CL_MEM_HOST_PTR, sizeof(mem_ptr), &mem_ptr, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(mem_ptr);
			break;
		default:
			printf("Error: Unsupported Mem Info type\n");
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_VALUE:
			ec.throwDOMException(WebCL::INVALID_VALUE, "WebCL::INVALID_VALUE");
			printf("Error: CL_INVALID_VALUE   \n");
			break;
			// TODO (siba samal) Handle CL_INVALID_D3D10_RESOURCE_KHR Case
			//	case CL_INVALID_D
			//	ec.throwDOMException(WebCL::INVALID_D, "WebCL::INVALID_D");
			//	printf("CL_INVALID_D3D10_RESOURCE_KHR    \n");
			//	break; 
		case CL_INVALID_MEM_OBJECT:
			ec.throwDOMException(WebCL::INVALID_MEM_OBJECT, "WebCL::INVALID_MEM_OBJECT");
			printf("Error: CL_INVALID_MEM_OBJECT    \n");
			break;
		case CL_OUT_OF_RESOURCES:
			ec.throwDOMException(WebCL::OUT_OF_RESOURCES, "WebCL::OUT_OF_RESOURCES");
			printf("Error: CL_OUT_OF_RESOURCES   \n");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			ec.throwDOMException(WebCL::OUT_OF_HOST_MEMORY, "WebCL::OUT_OF_HOST_MEMORY");
			printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
			break;
		default:
			ec.throwDOMException(WebCL::FAILURE, "WebCL::FAILURE");
			printf("Error: Invaild Error Type\n");
			break;
	}				
	return WebCLGetInfo();
}


void WebCLMem::releaseCL( ExceptionState& ec)
{
	cl_int err = 0;
	if (m_cl_mem == NULL) {
		printf("Error: Invalid CLMem\n");
		ec.throwDOMException(WebCL::FAILURE, "WebCL::FAILURE");
		return;
	}
	err = webcl_clReleaseMemObject(webcl_channel_, m_cl_mem);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_MEM_OBJECT:
				printf("Error: CL_INVALID_MEM_OBJECT \n");
				ec.throwDOMException(WebCL::INVALID_MEM_OBJECT, "WebCL::INVALID_MEM_OBJECT");
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				ec.throwDOMException(WebCL::OUT_OF_RESOURCES , "WebCL::OUT_OF_RESOURCES ");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
				ec.throwDOMException(WebCL::OUT_OF_HOST_MEMORY , "WebCL::OUT_OF_HOST_MEMORY ");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec.throwDOMException(WebCL::FAILURE, "WebCL::FAILURE");
				break;
		}
	} else {
		unsigned int i;
		for (i = 0; i < m_mem_list.size(); i++) {
			if ((m_mem_list[i].get())->getCLMem() == m_cl_mem) {				
				m_mem_list.remove(i);
				m_num_mems = m_mem_list.size();
				break;
			}
		}
		return;
	}
	return;
}

cl_mem WebCLMem::getCLMem()
{
	return m_cl_mem;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

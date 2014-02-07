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

#include "WebCLSampler.h"
#include "WebCL.h"
#include "WebCLException.h"

namespace WebCore {

WebCLSampler::~WebCLSampler()
{
}

PassRefPtr<WebCLSampler> WebCLSampler::create(WebCL* compute_context, 
	cl_sampler sampler)
{
	return adoptRef(new WebCLSampler(compute_context, sampler));
}

WebCLSampler::WebCLSampler(WebCL* compute_context, cl_sampler sampler) 
		: m_context(compute_context), m_cl_sampler(sampler)
{
	m_num_samplers = 0;
}

WebCLGetInfo WebCLSampler::getInfo(cl_sampler_info param_name, ExceptionState& ec)
{
	cl_int err = 0;
	cl_uint uint_units = 0;
	cl_bool bool_units = false;
	cl_context cl_context_id = NULL;
	RefPtr<WebCLContext> contextObj =  NULL;
	if (m_cl_sampler == NULL) {
		ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
		printf("Error: Invalid Sampler\n");
		return WebCLGetInfo();
	}
	switch(param_name)
	{
		case WebCL::SAMPLER_REFERENCE_COUNT:
			err = webcl_clGetSamplerInfo (webcl_channel_, m_cl_sampler, CL_SAMPLER_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case WebCL::SAMPLER_NORMALIZED_COORDS:
			err = webcl_clGetSamplerInfo(webcl_channel_, m_cl_sampler, CL_SAMPLER_NORMALIZED_COORDS , sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case WebCL::SAMPLER_CONTEXT:
			err = webcl_clGetSamplerInfo(webcl_channel_, m_cl_sampler, CL_SAMPLER_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
			contextObj = WebCLContext::create(m_context, cl_context_id);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
			break;
		case WebCL::SAMPLER_ADDRESSING_MODE:
			err = webcl_clGetSamplerInfo (webcl_channel_, m_cl_sampler, CL_SAMPLER_ADDRESSING_MODE , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case WebCL::SAMPLER_FILTER_MODE:
			err = webcl_clGetSamplerInfo (webcl_channel_, m_cl_sampler, CL_SAMPLER_FILTER_MODE , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		default:
			printf("Error: Unsupported Sampler Info type\n");
			ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
			return WebCLGetInfo();	
	}
	switch (err) {

		case CL_INVALID_VALUE:
			ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
			printf("Error: CL_INVALID_VALUE \n");
			break;
		case CL_INVALID_SAMPLER:
			ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
			printf("Error: CL_INVALID_SAMPLER\n");
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
			ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
			printf("Error: Invaild Error Type\n");
			break;
	}				
	return WebCLGetInfo();

}
void WebCLSampler::releaseCL( ExceptionState& ec)
{
	cl_int err = 0;
	if (m_cl_sampler == NULL) {
		printf("Error: Invalid Sampler\n");
		ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
		return;
	}
	err = webcl_clReleaseSampler(webcl_channel_, m_cl_sampler);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_SAMPLER :
				printf("Error: CL_INVALID_SAMPLER\n");
				ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
				break;
			case CL_OUT_OF_RESOURCES :
				printf("Error: CL_OUT_OF_RESOURCES\n");
				ec.throwDOMException(WebCLException::OUT_OF_RESOURCES, "WebCLException::OUT_OF_RESOURCES");
				break;
			case CL_OUT_OF_HOST_MEMORY :
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
				break;

			default:
				printf("Error: Invaild Error Type\n");
				ec.throwDOMException(WebCLException::INVALID_SAMPLER, "WebCLException::INVALID_SAMPLER");
				break;
		}
	} else {
		for (int i = 0; i < m_num_samplers; i++) {
			if ((m_sampler_list[i].get())->getCLSampler() == m_cl_sampler) {
				m_sampler_list.remove(i);
				m_num_samplers = m_sampler_list.size();
				break;
			}
		}
		return;
	}
	return;
}

cl_sampler WebCLSampler::getCLSampler()
{
	return m_cl_sampler;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

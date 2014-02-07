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

#include "WebCLPlatform.h"
#include "WebCL.h"
#include "WebCLException.h"

namespace WebCore {

WebCLPlatform::~WebCLPlatform()
{
}

PassRefPtr<WebCLPlatform> WebCLPlatform::create(WebCL* context, cl_platform_id platform_id)
{
	return adoptRef(new WebCLPlatform(context, platform_id)); 
}

WebCLPlatform::WebCLPlatform(WebCL* context, cl_platform_id platform_id)
 : m_context(context), m_cl_platform_id(platform_id)
{
}

WebCLGetInfo WebCLPlatform::getInfo (int platform_info, ExceptionState& ec)
{
	cl_int err = 0;
	if (m_cl_platform_id == NULL) {
			ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
			printf("Error: Invalid Platform ID\n");
			return WebCLGetInfo();
	}

	char platform_string[1024];
	switch(platform_info)
	{
		case WebCL::PLATFORM_PROFILE:
			err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_PROFILE, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCL::PLATFORM_VERSION:
			err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_VERSION, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCL::PLATFORM_NAME:
			err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_NAME, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCL::PLATFORM_VENDOR:
			err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_VENDOR, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		case WebCL::PLATFORM_EXTENSIONS:
			err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_EXTENSIONS, sizeof(platform_string), &platform_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(platform_string));
			break;
		default:
			printf("Error: Unsupported Platform Info type = %d ",platform_info);
			return WebCLGetInfo();
	}

	if(err != CL_SUCCESS)
	{
		switch (err) {
			case CL_INVALID_PLATFORM:
				ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
				printf("Error: CL_INVALID_PLATFORM  \n");
				break;
			case CL_INVALID_VALUE:
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
				printf("Error: CL_INVALID_VALUE\n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY, "WebCLException::OUT_OF_HOST_MEMORY");
				printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
				break;
			default:
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				printf("Invaild Error Type\n");
				break;
		}
	}
	return WebCLGetInfo();
}

PassRefPtr<WebCLDeviceList> WebCLPlatform::getDevices(int device_type, ExceptionState& ec)
{
	if (m_cl_platform_id == NULL) {
		printf("Error: Invalid Platform ID\n");
		ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
		return NULL;
	}
	RefPtr<WebCLDeviceList> o = WebCLDeviceList::create(m_context, m_cl_platform_id, 
			device_type);
	if (o != NULL) {
		//TODO (siba samal) Check if its needed
		//m_device_id = o;
		return o;
	} else {
		ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
		return NULL;
	}
}
Vector<String> WebCLPlatform::getSupportedExtensions(ExceptionState& ec )
{
    char platform_string[1024] = "";
    char extensions[16][64];
    Vector<String> result;
    int count = 0;
    int word_length = 0;
    int i =0;

    if (m_cl_platform_id == NULL) {
        printf("Error: Invalid Platform ID\n");
        ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
        return result;
    }
    cl_int err = webcl_clGetPlatformInfo(webcl_channel_, m_cl_platform_id, CL_PLATFORM_EXTENSIONS, sizeof(platform_string), &platform_string, NULL);
    if (err != CL_SUCCESS) {
		ec.throwDOMException(WebCLException::INVALID_PLATFORM, "WebCLException::INVALID_PLATFORM");
        return result;
    }

    while(platform_string[i] != '\0')
    {
        while(platform_string[i] == ' ')
            ++i;
        while(platform_string[i] !=  ' ' && platform_string[i] != '\0')
        extensions[count][word_length++] = platform_string[i++];
        extensions[count++][word_length] = '\0';  /* Append terminator         */
        word_length = 0;
    }
    for(i = 0 ; i<count ; i++) {
        printf("CL_PLATFORM_EXTENSIONS: %s\n",extensions[i]);
        result.append(String(extensions[i]));
    }

    return result;

}


cl_platform_id WebCLPlatform::getCLPlatform()
{
	return m_cl_platform_id;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

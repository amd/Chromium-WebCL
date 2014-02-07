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

#include "WebCLDeviceList.h"
#include "WebCL.h"
#include "WebCLDevice.h"


namespace WebCore { 

WebCLDeviceList::~WebCLDeviceList()
{
}

PassRefPtr<WebCLDeviceList> WebCLDeviceList::create(WebCL* ctx , cl_device_id* devicelist, int num_devices)
{

        return adoptRef(new WebCLDeviceList(ctx ,devicelist, num_devices));
}

WebCLDeviceList::WebCLDeviceList(WebCL* ctx,cl_device_id* devicelist, int num_devices ) :
                                        m_context(ctx),m_cl_devices(devicelist),m_num_devices(num_devices)
{
        if (m_num_devices == 0) {
                printf("Error: Number of devices is 0");
        }

        for (unsigned int i = 0 ; i < m_num_devices; i++) {
                RefPtr<WebCLDevice> o = WebCLDevice::create(m_context, m_cl_devices[i]);
                if (o != NULL) {
                        m_device_id_list.append(o);
                } else {
                        // TODO (siba samal) Error handling
                }
        }
	printf("iLength of WebCLDevice = %d", num_devices);
}



PassRefPtr<WebCLDeviceList> WebCLDeviceList::create(WebCL* compute_context, 
		cl_platform_id platform_id, int device_type)
{
	return adoptRef(new WebCLDeviceList(compute_context, platform_id, device_type));
}

WebCLDeviceList::WebCLDeviceList(WebCL* compute_context,
		cl_platform_id platform_id, int device_type) : m_context(compute_context)
{
	cl_int err = 0;
	cl_uint num_devices = 0;

	if(device_type == WebCL::DEVICE_TYPE_GPU)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
	else if(device_type ==  WebCL::DEVICE_TYPE_CPU)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_CPU, 1, NULL, &num_devices);
	else if(device_type ==  WebCL::DEVICE_TYPE_ACCELERATOR)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_ACCELERATOR, 1, NULL, &num_devices);
	else if(device_type == WebCL::DEVICE_TYPE_DEFAULT)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_DEFAULT, 1, NULL, &num_devices);
	//else if(device_type == WebCL::DEVICE_TYPE_ALL)
	//	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, NULL, &num_devices);
	else
		printf("Error:Invalid Device Type \n");

	m_num_devices = num_devices;
	if(m_num_devices == 0)
	{
		printf("Error: Device Type Not Supported \n");
		return;
	}
	if (err != CL_SUCCESS)
	{
		switch (err) {
			case CL_INVALID_PLATFORM :
				printf("Error: CL_INVALID_PLATFORM \n");
				break;
			case CL_INVALID_DEVICE_TYPE :
				printf("Error: CL_INVALID_DEVICE_TYPE \n");
				break;
			case CL_INVALID_VALUE :
				printf("Error: CL_INVALID_VALUE \n");
				break;
			case CL_DEVICE_NOT_FOUND :
				printf("Error: CL_DEVICE_NOT_FOUND \n");
				break;
			case CL_OUT_OF_RESOURCES :
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				break;
		} 
		return;
	}

	m_cl_devices = new cl_device_id[num_devices];
	
	if(device_type == WebCL::DEVICE_TYPE_GPU)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_GPU, 1, m_cl_devices, &num_devices);
	else if(device_type == WebCL::DEVICE_TYPE_CPU)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_CPU, 1, m_cl_devices, &num_devices);
	else if(device_type == WebCL::DEVICE_TYPE_ACCELERATOR)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_ACCELERATOR, 1, m_cl_devices, &num_devices);
	else if(device_type == WebCL::DEVICE_TYPE_DEFAULT)
		err = webcl_clGetDeviceIDs(webcl_channel_, platform_id, CL_DEVICE_TYPE_DEFAULT, 1, m_cl_devices, &num_devices);
	//else if(device_type == WebCL::DEVICE_TYPE_ALL)
	//	err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, m_cl_devices , &num_devices);
	else
		printf("Error:Invalid Device Type \n");

	if (err != CL_SUCCESS)
	{
		printf("Error ACCELERATOR 2\n");
		switch (err) {
			case CL_INVALID_PLATFORM :
				printf("Error: CL_INVALID_PLATFORM \n");
				break;
			case CL_INVALID_DEVICE_TYPE :
				printf("Error: CL_INVALID_DEVICE_TYPE \n");
				break;
			case CL_INVALID_VALUE :
				printf("Error: CL_INVALID_VALUE \n");
				break;
			case CL_DEVICE_NOT_FOUND :
				printf("Error: CL_DEVICE_NOT_FOUND \n");
				break;
			case CL_OUT_OF_RESOURCES :
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY\n");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				break;
		} 
		return;
	}

	for (unsigned int i = 0; i < m_num_devices; i++) {
		RefPtr<WebCLDevice> o = WebCLDevice::create(m_context, m_cl_devices[i]);
		if (o != NULL) {
			m_device_id_list.append(o);
		} else {
		}
	}
}

cl_device_id WebCLDeviceList::getCLDevices()
{
	return *m_cl_devices;
}

unsigned WebCLDeviceList::length() const
{
	return m_num_devices;
}

WebCLDevice* WebCLDeviceList::item(unsigned index)
{
	if (index >= m_num_devices) {
		return 0;
	}
	WebCLDevice* ret = (m_device_id_list[index]).get();
	return ret;

}

} // namespace WebCore

#endif // ENABLE(WEBCL)

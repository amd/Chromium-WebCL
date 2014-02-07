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

#include "WebCLProgram.h"
#include "WebCLGetInfo.h"
#include "WebCLKernel.h"
#include "WebCLKernelList.h"
#include "WebCL.h"
#include "WebCLException.h"

namespace WebCore {

WebCLProgram::~WebCLProgram()
{
}

PassRefPtr<WebCLProgram> WebCLProgram::create(WebCL* 
											compute_context, cl_program program)
{
	return adoptRef(new WebCLProgram(compute_context, program));
}

WebCLProgram::WebCLProgram(WebCL* compute_context, 
		cl_program program) : m_context(compute_context), m_cl_program(program)
{
	m_num_programs = 0;
	m_num_kernels = 0; 
}

WebCLGetInfo WebCLProgram::getInfo(int param_name, ExceptionState& ec)
{
	cl_int err = 0;
	cl_uint uint_units = 0;
	size_t sizet_units = 0;
	char program_string[4096];
	cl_context cl_context_id = NULL;
	RefPtr<WebCLContext> contextObj  = NULL;
	RefPtr<WebCLDeviceList> deviceList =  NULL;
	size_t szParmDataBytes = 0;
	if (m_cl_program == NULL) {
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			printf("Error: Invalid program object\n");
			return WebCLGetInfo();
	}

	switch(param_name)
	{   
		case WebCL::PROGRAM_REFERENCE_COUNT:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_REFERENCE_COUNT , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case WebCL::PROGRAM_NUM_DEVICES:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_NUM_DEVICES , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case WebCL::PROGRAM_BINARY_SIZES:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case WebCL::PROGRAM_SOURCE:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_SOURCE, sizeof(program_string), &program_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(program_string));
			break;
		case WebCL::PROGRAM_BINARIES:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_BINARIES, sizeof(program_string), &program_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(program_string));
			break;
		case WebCL::PROGRAM_CONTEXT:
			err = webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_CONTEXT, sizeof(cl_context), &cl_context_id, NULL);
			contextObj = WebCLContext::create(m_context, cl_context_id);
			if(contextObj == NULL)
			{
				printf("Error : CL program context not NULL\n");
				return WebCLGetInfo();
			}
			if (err == CL_SUCCESS)
				return WebCLGetInfo(PassRefPtr<WebCLContext>(contextObj));
			break;
		case WebCL::PROGRAM_DEVICES:
			cl_device_id* cdDevices;
                        webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_DEVICES, 0, NULL, &szParmDataBytes);
                        if (err == CL_SUCCESS) {
                                int nd = szParmDataBytes / sizeof(cl_device_id);
                                cdDevices = (cl_device_id*) malloc(szParmDataBytes);
                                webcl_clGetProgramInfo(webcl_channel_, m_cl_program, CL_PROGRAM_DEVICES, szParmDataBytes, cdDevices, NULL);
                                deviceList = WebCLDeviceList::create(m_context, cdDevices, nd);
                                printf("Size Vs Size = %lu %d %d \n\n", szParmDataBytes,nd,deviceList->length());
                                free(cdDevices);
                                return WebCLGetInfo(PassRefPtr<WebCLDeviceList>(deviceList));
                        }
                        break;

			// TODO (siba samal)- Handle Array of cl_device_id 
			//case WebCL::PROGRAM_DEVICES:
			//  size_t numDevices;
			//  clGetProgramInfo( m_cl_program, CL_PROGRAM_DEVICES, 0, 0, &numDevices );
			//  cl_device_id *devices = new cl_device_id[numDevices];
			//  clGetProgramInfo( m_cl_program, CL_PROGRAM_DEVICES, numDevices, devices, &numDevices );
			//  return WebCLGetInfo(PassRefPtr<WebCLContext>(obj));
		default:
			printf("Error: UNSUPPORTED program Info type = %d ",param_name);
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			return WebCLGetInfo();
	}
	switch (err) {
		case CL_INVALID_PROGRAM:
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			printf("Error: CL_INVALID_PROGRAM \n");
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
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			printf("Error: Invaild Error Type\n");
			break;
	}
	return WebCLGetInfo();
}


WebCLGetInfo WebCLProgram::getBuildInfo(WebCLDevice* device, int param_name, ExceptionState& ec)
{
	cl_device_id device_id = NULL;
	cl_uint err = 0;
	char buffer[8192];
	size_t len = 0;

	if (m_cl_program == NULL) {
			printf("Error: Invalid program object\n");
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			return WebCLGetInfo();
	}
	if (device != NULL) {
		device_id = device->getCLDevice();
		if (device_id == NULL) {
			ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
			printf("Error: device_id null\n");
			return WebCLGetInfo();
		}
	}

	switch (param_name) {
		case WebCL::PROGRAM_BUILD_LOG:
			err = webcl_clGetProgramBuildInfo(webcl_channel_, m_cl_program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(buffer));
			break;
		case WebCL::PROGRAM_BUILD_OPTIONS:
			err = webcl_clGetProgramBuildInfo(webcl_channel_, m_cl_program, device_id, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), &buffer, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(buffer));
			break;
		case WebCL::PROGRAM_BUILD_STATUS:
			cl_build_status build_status;
			err = webcl_clGetProgramBuildInfo(webcl_channel_, m_cl_program, device_id, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(build_status));
			break;
		default:
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			printf("Error: UNSUPPORTED Program Build Info   Type = %d ",param_name);
			return WebCLGetInfo();			
	}
	switch (err) {
		case CL_INVALID_DEVICE:
			ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
			printf("Error: CL_INVALID_DEVICE   \n");
			break;
		case CL_INVALID_VALUE:
			ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
			printf("Error: CL_INVALID_VALUE \n");
			break;
		case CL_INVALID_PROGRAM:
			ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
			printf("Error: CL_INVALID_PROGRAM  \n");
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

PassRefPtr<WebCLKernel> WebCLProgram::createKernel(	const String& kernel_name,
							ExceptionState& ec)
{
	cl_int err = 0;	
	cl_kernel cl_kernel_id = NULL;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
		return NULL;
	}
	// TODO(siba samal) - more detailed error code need to be addressed later
	const char* kernel_name_str = strdup(kernel_name.utf8().data());
	cl_kernel_id = webcl_clCreateKernel(webcl_channel_, m_cl_program, kernel_name_str, &err);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM_EXECUTABLE, "WebCLException::INVALID_PROGRAM_EXECUTABLE");
				break;
			case CL_INVALID_KERNEL_NAME:
				printf("Error: CL_INVALID_KERNEL_NAME\n");
				ec.throwDOMException(WebCLException::INVALID_KERNEL_NAME, "WebCLException::INVALID_KERNEL_NAME");
				break;
			case CL_INVALID_KERNEL_DEFINITION:
				printf("Error: CL_INVALID_KERNEL_DEFINITION\n");
				ec.throwDOMException(WebCLException::INVALID_KERNEL_DEFINITION, "WebCLException::INVALID_KERNEL_DEFINITION");
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
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
		RefPtr<WebCLKernel> o = WebCLKernel::create(m_context, cl_kernel_id);
		o->setDevice(m_device_id);
		m_kernel_list.append(o);
		m_num_kernels++;

		return o;
	}
	return NULL;
}

PassRefPtr<WebCLKernelList> WebCLProgram::createKernelsInProgram( ExceptionState& ec)
{
	cl_int err = 0;
	cl_kernel* kernelBuf = NULL;
	cl_uint num = 0;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
		return NULL;
	}
	err = webcl_clCreateKernelsInProgram (webcl_channel_, m_cl_program, NULL, NULL, &num);
	if (err != CL_SUCCESS) {
		//TODO (siba samal) Deatiled error check
		printf("Error: clCreateKernelsInProgram \n");
		ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
		return NULL;
	}
	if(num == 0) {
		printf("Warning: createKernelsInProgram - Number of Kernels is 0 \n");
		ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
		return NULL;
	}
	// TODO - Free the below malloc
	kernelBuf = (cl_kernel*)malloc (sizeof(cl_kernel) * num);
	if (!kernelBuf) {
		return NULL;
	}

	err = webcl_clCreateKernelsInProgram (webcl_channel_, m_cl_program, num, kernelBuf, NULL);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_INVALID_PROGRAM_EXECUTABLE:
				printf("Error: CL_INVALID_PROGRAM_EXECUTABLE\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM_EXECUTABLE, "WebCLException::INVALID_PROGRAM_EXECUTABLE");
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
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
		RefPtr<WebCLKernelList> o = WebCLKernelList::create(m_context, kernelBuf, num);
		printf("WebCLKernelList Size = %d \n\n\n\n", num);
		//m_kernel_list = o;
		m_num_kernels = num;
		return o;
	}
	return NULL;
}

void WebCLProgram::buildProgram(int options, int pfn_notify,
		int user_data, ExceptionState& ec)
{
	cl_int err = 0;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
		return;
	}

	cl_device_id device_id = m_device_id->getCLDevice();

	// TODO(siba samal) - needs to be addressed later
	//err = webcl_clBuildProgram(webcl_channel_, m_cl_program, 1, &device_id, NULL, NULL, NULL);
err = webcl_clBuildProgram(webcl_channel_, m_cl_program, 0, NULL, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec.throwDOMException(WebCLException::INVALID_BINARY, "WebCLException::INVALID_BINARY");
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec.throwDOMException(WebCLException::INVALID_BUILD_OPTIONS, "WebCLException::INVALID_BUILD_OPTIONS");
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec.throwDOMException(WebCLException::COMPILER_NOT_AVAILABLE, "WebCLException::COMPILER_NOT_AVAILABLE");
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec.throwDOMException(WebCLException::BUILD_PROGRAM_FAILURE, "WebCLException::BUILD_PROGRAM_FAILURE");
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
				printf("WebCL::buildProgram normal options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::buildProgram(WebCLDevice* device_id,int options, 
		int pfn_notify, int user_data, ExceptionState& ec)
{
	cl_int err = 0;
	cl_device_id cl_device = NULL;
	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
		return;
	}
	cl_device = device_id->getCLDevice();
	if (cl_device == NULL) {
		printf("Error: devices null\n");
		ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
		return;
	}

	// TODO(siba samal) - NULL parameters needs to be addressed later
	err = webcl_clBuildProgram(webcl_channel_, m_cl_program, 1, (const cl_device_id*)&cl_device, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec.throwDOMException(WebCLException::INVALID_BINARY, "WebCLException::INVALID_BINARY");
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec.throwDOMException(WebCLException::INVALID_BUILD_OPTIONS, "WebCLException::INVALID_BUILD_OPTIONS");
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec.throwDOMException(WebCLException::COMPILER_NOT_AVAILABLE, "WebCLException::COMPILER_NOT_AVAILABLE");
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec.throwDOMException(WebCLException::BUILD_PROGRAM_FAILURE, "WebCLException::BUILD_PROGRAM_FAILURE");
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
				printf("WebCL::buildProgram WebCLDevice options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::buildProgram( WebCLDeviceList* cl_devices, int options, 
		int pfn_notify, int user_data, ExceptionState& ec)
{
	cl_int err = 0;	
	cl_device_id cl_device = NULL;

	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
		return ;
	}
	if (cl_devices != NULL) {
		cl_device = cl_devices->getCLDevices();
		if (cl_device == NULL) {
			ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
			printf("Error: devices null\n");
			return;
		}
	} else {
		ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
		printf("Error: webcl_devices null\n");
		return;
	}

	// TODO(siba samal) - NULL parameters needs to be addressed later
	err = webcl_clBuildProgram(webcl_channel_, m_cl_program, cl_devices->length(), (const cl_device_id*)&cl_device, NULL, NULL, NULL);

	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM\n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_INVALID_VALUE:
				printf("Error: CL_INVALID_VALUE\n");
				ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
				break;
			case CL_INVALID_DEVICE:
				printf("Error: CL_INVALID_DEVICE\n");
				ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
				break;
			case CL_INVALID_BINARY:
				printf("Error: CL_INVALID_BINARY\n");
				ec.throwDOMException(WebCLException::INVALID_BINARY, "WebCLException::INVALID_BINARY");
				break;
			case CL_INVALID_OPERATION:
				printf("Error: CL_INVALID_OPERATION\n");
				ec.throwDOMException(WebCLException::INVALID_OPERATION, "WebCLException::INVALID_OPERATION");
				break;
			case CL_INVALID_BUILD_OPTIONS :
				printf("Error: CL_INVALID_BUILD_OPTIONS\n");
				ec.throwDOMException(WebCLException::INVALID_BUILD_OPTIONS, "WebCLException::INVALID_BUILD_OPTIONS");
				break;
			case CL_COMPILER_NOT_AVAILABLE:
				printf("Error: CL_COMPILER_NOT_AVAILABLE\n");
				ec.throwDOMException(WebCLException::COMPILER_NOT_AVAILABLE, "WebCLException::COMPILER_NOT_AVAILABLE");
				break;
			case CL_BUILD_PROGRAM_FAILURE:
				printf("Error: CL_BUILD_PROGRAM_FAILURE\n");
				ec.throwDOMException(WebCLException::BUILD_PROGRAM_FAILURE, "WebCLException::BUILD_PROGRAM_FAILURE");
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
				printf("WebCL::buildProgram WebCLDeviceList  options=%d pfn_notify=%d user_data=%d\n", 
						options, pfn_notify, user_data);
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				break;

		}

	} else {
		return;
	}
	return;
}

void WebCLProgram::releaseCL( ExceptionState& ec)
{
	cl_int err = 0;

	if (m_cl_program == NULL) {
		printf("Error: Invalid program object\n");
		ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
		return;
	}
	err = webcl_clReleaseProgram(webcl_channel_, m_cl_program);
	if (err != CL_SUCCESS) {
		switch (err) {
			case CL_INVALID_PROGRAM:
				printf("Error: CL_INVALID_PROGRAM  \n");
				ec.throwDOMException(WebCLException::INVALID_PROGRAM, "WebCLException::INVALID_PROGRAM");
				break;
			case CL_OUT_OF_RESOURCES:
				printf("Error: CL_OUT_OF_RESOURCES  \n");
				ec.throwDOMException(WebCLException::OUT_OF_RESOURCES , "WebCLException::OUT_OF_RESOURCES ");
				break;
			case CL_OUT_OF_HOST_MEMORY:
				printf("Error: CL_OUT_OF_HOST_MEMORY  \n");
				ec.throwDOMException(WebCLException::OUT_OF_HOST_MEMORY , "WebCLException::OUT_OF_HOST_MEMORY ");
				break;
			default:
				printf("Error: Invaild Error Type\n");
				ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
				break;
		}
	} else {
		for (int i = 0; i < m_num_programs; i++) {
			if ((m_program_list[i].get())->getCLProgram() == m_cl_program) {
				m_program_list.remove(i);
				m_num_programs = m_program_list.size();
				break;
			}
		}
		return;
	}
	return;
}

	
void WebCLProgram::setDevice(RefPtr<WebCLDevice> m_device_id_)
{
	m_device_id = m_device_id_;
}

cl_program WebCLProgram::getCLProgram()
{
	return m_cl_program;
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

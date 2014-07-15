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

#include "WebCLDevice.h"

#include "ComputeDevice.h"
#include "WebCLCommandQueue.h"
#include "WebCLContext.h"
#include "WebCLGetInfo.h"
#include "WebCLInputChecker.h"
#include "WebCLPlatform.h"

#include "../../wtf/Int32Array.h"

namespace WebCore {

WebCLDevice::~WebCLDevice()
{
}

PassRefPtr<WebCLDevice> WebCLDevice::create(RefPtr<ComputeDevice> device, WebCLPlatform* platform)
{
    return adoptRef(new WebCLDevice(device, platform));
}

WebCLDevice::WebCLDevice(RefPtr<ComputeDevice> device, WebCLPlatform* platform)
    : WebCLExtensionsAccessor(device.get())
    , m_device(device)
    , m_platform(platform)
{
}

WebCLGetInfo WebCLDevice::getInfo(CCenum infoType, ExceptionObject& exception)
{
    if (!WebCLInputChecker::isValidDeviceInfoType(infoType)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }
#if 0
    CCerror err = 0;
    switch (infoType) {
    case ComputeContext::DEVICE_PROFILE:
        return WebCLGetInfo(String("WEBCL_PROFILE"));
    case ComputeContext::DEVICE_VERSION:
        return WebCLGetInfo(String("WebCL 1.0"));
    case ComputeContext::DEVICE_OPENCL_C_VERSION:
        return WebCLGetInfo(String("WebCL C 1.0"));
    case ComputeContext::DEVICE_VENDOR_ID: // Vendor specific. Will return a empty string.
    case ComputeContext::DRIVER_VERSION:
    case ComputeContext::DEVICE_NAME:
    case ComputeContext::DEVICE_VENDOR:
    case ComputeContext::DEVICE_EXTENSIONS:
        return WebCLGetInfo(emptyString());
    case ComputeContext::DEVICE_IMAGE_SUPPORT:
    case ComputeContext::DEVICE_AVAILABLE:
    case ComputeContext::DEVICE_COMPILER_AVAILABLE:
        return WebCLGetInfo(true);
    case ComputeContext::DEVICE_MAX_WORK_ITEM_SIZES: { // size_t[]
        Vector<size_t> workItemSizes;
        err = platformObject()->getDeviceInfo(infoType, &workItemSizes);
        if (err == ComputeContext::SUCCESS) {
            Vector<CCuint> values;
            for (size_t i = 0; i < workItemSizes.size(); ++i)
                values.append(workItemSizes[i]);
            return WebCLGetInfo(values);
        }
        break;
    }
    case ComputeContext::DEVICE_MAX_WORK_GROUP_SIZE: 
    case ComputeContext::DEVICE_MAX_PARAMETER_SIZE:
    case ComputeContext::DEVICE_PROFILING_TIMER_RESOLUTION:
    case ComputeContext::DEVICE_IMAGE2D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE2D_MAX_WIDTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_HEIGHT:
    case ComputeContext::DEVICE_IMAGE3D_MAX_DEPTH:
    case ComputeContext::DEVICE_IMAGE3D_MAX_WIDTH: {
        size_t infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCuint>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_MAX_WORK_ITEM_DIMENSIONS:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_CHAR:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_SHORT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_INT:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_LONG:
    case ComputeContext::DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:
    case ComputeContext::DEVICE_MAX_CLOCK_FREQUENCY:
    case ComputeContext::DEVICE_MAX_READ_IMAGE_ARGS:
    case ComputeContext::DEVICE_MAX_WRITE_IMAGE_ARGS:
    case ComputeContext::DEVICE_MAX_SAMPLERS:
    case ComputeContext::DEVICE_MEM_BASE_ADDR_ALIGN:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
    case ComputeContext::DEVICE_MAX_CONSTANT_ARGS:
    case ComputeContext::DEVICE_ADDRESS_BITS:
    case ComputeContext::DEVICE_MAX_COMPUTE_UNITS: {
        CCuint infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCuint>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_LOCAL_MEM_SIZE:
    case ComputeContext::DEVICE_MAX_CONSTANT_BUFFER_SIZE:
    case ComputeContext::DEVICE_GLOBAL_MEM_SIZE:
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_SIZE:
    case ComputeContext::DEVICE_MAX_MEM_ALLOC_SIZE: {
        CCulong infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCulong>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_HOST_UNIFIED_MEMORY:
    case ComputeContext::DEVICE_ENDIAN_LITTLE:
    case ComputeContext::DEVICE_ERROR_CORRECTION_SUPPORT: {
        CCbool infoValue = 0;
        err = platformObject()->getDeviceInfo(infoType, &infoValue);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<bool>(infoValue));
        break;
    }
    case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_TYPE: {
        CCDeviceMemCachetype type = 0;
        err = platformObject()->getDeviceInfo(infoType, &type);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(type));
        break;
    }
    case ComputeContext::DEVICE_TYPE: {
        CCDeviceType type = 0;
        err = platformObject()->getDeviceInfo(infoType, &type);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(type));
        break;
    }
    case ComputeContext::DEVICE_SINGLE_FP_CONFIG: {
        CCDeviceFPConfig deviceFPConfig = 0;
        err = platformObject()->getDeviceInfo(infoType, &deviceFPConfig);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(deviceFPConfig));
        break;
    }
    case ComputeContext::DEVICE_LOCAL_MEM_TYPE: {
        CCDeviceLocalMemType localMemoryType = 0;
        err = platformObject()->getDeviceInfo(infoType, &localMemoryType);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(localMemoryType));
        break;
    }
    case ComputeContext::DEVICE_QUEUE_PROPERTIES: {
        CCCommandQueueProperties queueProperties = 0;
        err = platformObject()->getDeviceInfo(infoType,  &queueProperties);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(queueProperties));
        break;
    }
    case ComputeContext::DEVICE_EXECUTION_CAPABILITIES:
        return WebCLGetInfo(static_cast<CCenum>(ComputeContext::EXEC_KERNEL));
    case ComputeContext::DEVICE_PLATFORM:
        return WebCLGetInfo(m_platform);
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
    }
    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
#else
	cl_int err = 0;
	char device_string[1024];
	cl_uint uint_units = 0;
	size_t sizet_units = 0;
	size_t sizet_array[1024] = {0};
	cl_ulong  ulong_units = 0;
	cl_bool bool_units = false;
	cl_device_type type = 0;
	cl_device_mem_cache_type global_type = 0;
	cl_command_queue_properties queue_properties = 0;
	cl_device_exec_capabilities exec = NULL;
	cl_device_local_mem_type local_type = 0;

	cl_device_id device_id = platformObject()->device();

	switch(infoType)
	{

		case ComputeContext::DEVICE_EXTENSIONS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));	
			break;
		case ComputeContext::DEVICE_NAME:
			err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));	
			break;
		case ComputeContext::DEVICE_PROFILE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PROFILE, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));	
			break;
		case ComputeContext::DEVICE_VENDOR:
			err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));
			break;
		case ComputeContext::DEVICE_VERSION:
			err = clGetDeviceInfo(device_id, CL_DEVICE_VERSION, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));
			break;
		case ComputeContext::DRIVER_VERSION:
			err = clGetDeviceInfo(device_id, CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(String(device_string));	
			break;
		case ComputeContext::DEVICE_ADDRESS_BITS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_ADDRESS_BITS , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MAX_CLOCK_FREQUENCY:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case ComputeContext::DEVICE_MAX_CONSTANT_ARGS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MAX_READ_IMAGE_ARGS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));	
			break;
		case ComputeContext::DEVICE_MAX_SAMPLERS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_SAMPLERS, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MAX_WRITE_IMAGE_ARGS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MEM_BASE_ADDR_ALIGN:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_VENDOR_ID:
			err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR_ID, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_INT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &uint_units,NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_MAX_COMPUTE_UNITS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS , sizeof(cl_uint), &uint_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(uint_units));
			break;
		case ComputeContext::DEVICE_IMAGE2D_MAX_HEIGHT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_IMAGE2D_MAX_WIDTH:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_IMAGE3D_MAX_DEPTH:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_IMAGE3D_MAX_HEIGHT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_IMAGE3D_MAX_WIDTH:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_MAX_PARAMETER_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_MAX_WORK_GROUP_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_MAX_WORK_ITEM_SIZES:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &sizet_units, NULL);
			if(err == CL_SUCCESS) {
				err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, 1024, &sizet_array, NULL);
				// TODO (siba samal) Check support for SizeTArray/Int16Array in WebCLGetInfo
				if (err == CL_SUCCESS) {
					int values[3] = {0,0,0};
					for(int i=0; i<((int)sizet_units); i++)
					{
						printf("%d\n", values[i]);
						values[i] = (int)sizet_array[i];
					}
					return WebCLGetInfo(Int32Array::create(values, 3));
					
						
				}
			}
			break;
		case ComputeContext::DEVICE_PROFILING_TIMER_RESOLUTION:
			err = clGetDeviceInfo(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_MAX_WORK_ITEM_DIMENSIONS:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &sizet_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(sizet_units));
			break;
		case ComputeContext::DEVICE_LOCAL_MEM_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		case ComputeContext::DEVICE_MAX_CONSTANT_BUFFER_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		case ComputeContext::DEVICE_MAX_MEM_ALLOC_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		case ComputeContext::DEVICE_GLOBAL_MEM_SIZE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &ulong_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(ulong_units));
			break;
		case ComputeContext::DEVICE_AVAILABLE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_AVAILABLE , sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case ComputeContext::DEVICE_COMPILER_AVAILABLE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_COMPILER_AVAILABLE , sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case ComputeContext::DEVICE_ENDIAN_LITTLE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case ComputeContext::DEVICE_ERROR_CORRECTION_SUPPORT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case ComputeContext::DEVICE_IMAGE_SUPPORT:
			err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &bool_units, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<bool>(bool_units));
			break;
		case ComputeContext::DEVICE_TYPE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(type));
			break;
		case ComputeContext::DEVICE_QUEUE_PROPERTIES:
			err = clGetDeviceInfo(device_id, CL_DEVICE_QUEUE_PROPERTIES, 
					sizeof(cl_command_queue_properties), &queue_properties, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(queue_properties));
			break;

			//cl_device_fp_config fp_config = 0;
			// Part of cl_ext.h (which isn't available in Khronos)
			//case ComputeContext::DEVICE_DOUBLE_FP_CONFIG:
			//clGetDeviceInfo(device_id, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
			//case ComputeContext::DEVICE_HALF_FP_CONFIG:
			//clGetDeviceInfo(device_id, CL_DEVICE_HALF_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
			//case vDEVICE_SINGLE_FP_CONFIG:
			//clGetDeviceInfo(device_id, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(cl_device_fp_config), &fp_config, NULL);
			//return WebCLGetInfo(static_cast<unsigned int>(fp_config));


			//Platform ID is not Supported
			//case ComputeContext::DEVICE_PLATFORM:
			//cl_platform_id platform_id = NULL;
			//clGetDeviceInfo(device_id, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform_id, NULL);
			//return WebCLGetInfo(static_cast<unsigned int>(platform_id));

		case ComputeContext::DEVICE_EXECUTION_CAPABILITIES:
			err = clGetDeviceInfo(device_id, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(cl_device_exec_capabilities), &exec, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(exec));
			break;
		case ComputeContext::DEVICE_GLOBAL_MEM_CACHE_TYPE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &global_type, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(global_type));
			break;
		case ComputeContext::DEVICE_LOCAL_MEM_TYPE:
			err = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &local_type, NULL);
			if (err == CL_SUCCESS)
				return WebCLGetInfo(static_cast<unsigned int>(local_type));
			break;
		default:
			//ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
			printf("Error:UNSUPPORTED DEVICE TYPE = %d ",infoType);
			return WebCLGetInfo();
	}

	setExceptionFromComputeErrorCode(err, exception);

	/*
	switch (err) {
		case CL_INVALID_DEVICE:
			ec.throwDOMException(WebCLException::INVALID_DEVICE, "WebCLException::INVALID_DEVICE");
			printf("Error: CL_INVALID_DEVICE \n");
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
			printf("Error: Invaild Error Type\n");
			ec.throwDOMException(WebCLException::FAILURE, "WebCLException::FAILURE");
			break;
	}
	*/
	return WebCLGetInfo();

#endif
}

void toWebCLDeviceArray(WebCLPlatform* platform, Vector<RefPtr<ComputeDevice> >& ccDevices, Vector<RefPtr<WebCLDevice> >& devices)
{
    for (size_t i = 0; i < ccDevices.size(); i++) {
        RefPtr<WebCLDevice> webCLDevice = WebCLDevice::create(ccDevices[i], platform);
        devices.append(webCLDevice);
    }
}

} // namespace WebCore
#endif // ENABLE(WEBCL)

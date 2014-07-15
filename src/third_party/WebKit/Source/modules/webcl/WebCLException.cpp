/*
 * Copyright (C) 2011, 2012 Samsung Electronics Corporation. All rights reserved.
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

#include "WebCLException.h"

#include "ComputeContext.h"

namespace WebCore {

// This should be an array of structs to pair the names and descriptions. ??
static const char* const exceptionNames[] = {
    "SUCCESS",
    "DEVICE_NOT_FOUND",
    "DEVICE_NOT_AVAILABLE",
    "COMPILER_NOT_AVAILABLE",
    "MEM_OBJECT_ALLOCATION_FAILURE",
    "OUT_OF_RESOURCES",
    "OUT_OF_HOST_MEMORY",
    "PROFILING_INFO_NOT_AVAILABLE",
    "MEM_COPY_OVERLAP",
    "IMAGE_FORMAT_MISMATCH",
    "IMAGE_FORMAT_NOT_SUPPORTED",
    "BUILD_PROGRAM_FAILURE",
    "MAP_FAILURE",
    "MISALIGNED_SUB_BUFFER_OFFSET",
    "EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
    "INVALID_VALUE",
    "INVALID_DEVICE_TYPE",
    "INVALID_PLATFORM",
    "INVALID_DEVICE",
    "INVALID_CONTEXT",
    "INVALID_QUEUE_PROPERTIES",
    "INVALID_COMMAND_QUEUE",
    "INVALID_HOST_PTR",
    "INVALID_MEM_OBJECT",
    "INVALID_IMAGE_FORMAT_DESCRIPTOR",
    "INVALID_IMAGE_SIZE",
    "INVALID_SAMPLER",
    "INVALID_BINARY",
    "INVALID_BUILD_OPTIONS",
    "INVALID_PROGRAM",
    "INVALID_PROGRAM_EXECUTABLE",
    "INVALID_KERNEL_NAME",
    "INVALID_KERNEL_DEFINITION",
    "INVALID_KERNEL",
    "INVALID_ARG_INDEX",
    "INVALID_ARG_VALUE",
    "INVALID_ARG_SIZE",
    "INVALID_KERNEL_ARGS",
    "INVALID_WORK_DIMENSION",
    "INVALID_WORK_GROUP_SIZE",
    "INVALID_WORK_ITEM_SIZE",
    "INVALID_GLOBAL_OFFSET",
    "INVALID_EVENT_WAIT_LIST",
    "INVALID_EVENT",
    "INVALID_OPERATION",
    "INVALID_GL_OBJECT",
    "INVALID_BUFFER_SIZE",
    "INVALID_MIP_LEVEL",
    "INVALID_GLOBAL_WORK_SIZE",
    "INVALID_PROPERTY",
    "WEBCL_EXTENSION_NOT_ENABLED",
    "WEBCL_IMPLEMENTATION_FAILURE",
};
// Messages are not proper
static const char* const exceptionDescriptions[] = {
    "WEBCL_IMPL_SUCCESS",
    "WEBCL_IMPL_DEVICE_NOT_FOUND",
    "WEBCL_IMPL_DEVICE_NOT_AVAILABLE",
    "WEBCL_IMPL_COMPILER_NOT_AVAILABLE",
    "WEBCL_IMPL_MEM_OBJECT_ALLOCATION_FAILURE",
    "WEBCL_IMPL_OUT_OF_RESOURCES",
    "WEBCL_IMPL_OUT_OF_HOST_MEMORY",
    "WEBCL_IMPL_PROFILING_INFO_NOT_AVAILABLE",
    "WEBCL_IMPL_MEM_COPY_OVERLAP",
    "WEBCL_IMPL_IMAGE_FORMAT_MISMATCH",
    "WEBCL_IMPL_IMAGE_FORMAT_NOT_SUPPORTED",
    "WEBCL_IMPL_BUILD_PROGRAM_FAILURE",
    "WEBCL_IMPL_MAP_FAILURE",
    "WEBCL_IMPL_MISALIGNED_SUB_BUFFER_OFFSET",
    "WEBCL_IMPL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
    "WEBCL_IMPL_INVALID_VALUE",
    "WEBCL_IMPL_INVALID_DEVICE_TYPE",
    "WEBCL_IMPL_INVALID_PLATFORM",
    "WEBCL_IMPL_INVALID_DEVICE",
    "WEBCL_IMPL_INVALID_CONTEXT",
    "WEBCL_IMPL_INVALID_QUEUE_PROPERTIES",
    "WEBCL_IMPL_INVALID_COMMAND_QUEUE",
    "WEBCL_IMPL_INVALID_HOST_PTR",
    "WEBCL_IMPL_INVALID_MEM_OBJECT",
    "WEBCL_IMPL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    "WEBCL_IMPL_INVALID_IMAGE_SIZE",
    "WEBCL_IMPL_INVALID_SAMPLER",
    "WEBCL_IMPL_INVALID_BINARY",
    "WEBCL_IMPL_INVALID_BUILD_OPTIONS",
    "WEBCL_IMPL_INVALID_PROGRAM",
    "WEBCL_IMPL_INVALID_PROGRAM_EXECUTABLE",
    "WEBCL_IMPL_INVALID_KERNEL_NAME",
    "WEBCL_IMPL_INVALID_KERNEL_DEFINITION",
    "WEBCL_IMPL_INVALID_KERNEL",
    "WEBCL_IMPL_INVALID_ARG_INDEX",
    "WEBCL_IMPL_INVALID_ARG_VALUE",
    "WEBCL_IMPL_INVALID_ARG_SIZE",
    "WEBCL_IMPL_INVALID_KERNEL_ARGS",
    "WEBCL_IMPL_INVALID_WORK_DIMENSION",
    "WEBCL_IMPL_INVALID_WORK_GROUP_SIZE",
    "WEBCL_IMPL_INVALID_WORK_ITEM_SIZE",
    "WEBCL_IMPL_INVALID_GLOBAL_OFFSET",
    "WEBCL_IMPL_INVALID_EVENT_WAIT_LIST",
    "WEBCL_IMPL_INVALID_EVENT",
    "WEBCL_IMPL_INVALID_OPERATION",
    "WEBCL_IMPL_INVALID_GL_OBJECT",
    "WEBCL_IMPL_INVALID_BUFFER_SIZE",
    "WEBCL_IMPL_INVALID_MIP_LEVEL",
    "WEBCL_IMPL_INVALID_GLOBAL_WORK_SIZE",
    "WEBCL_IMPL_INVALID_PROPERTY",
    "WEBCL_EXTENSION_NOT_ENABLED",
    "WEBCL_IMPLEMENTATION_FAILURE",
};

COMPILE_ASSERT(WTF_ARRAY_LENGTH(exceptionNames) == WTF_ARRAY_LENGTH(exceptionDescriptions), WebCLExceptionTablesMustMatch);

WebCLException::WebCLException(const ExceptionCodeDescription& desc) {
    ASSERT(desc.name);
    m_code = desc.code;
    m_name = desc.name;
	m_message = desc.description;

    ScriptWrappable::init(this);
}

RefPtr<WebCLException> WebCLException::create(int errorCode)
    {
		ExceptionCodeDescription desc(errorCode);
        return adoptRef(new WebCLException(desc));
    }

bool WebCLException::initializeDescription(ExceptionCode ec, ExceptionCodeDescription* description)
{
    if (ec < WebCLExceptionOffset || ec > WebCLExceptionMax)
        return false;

    description->typeName = "DOM WebCL";

    // As INVALID_VALUE=-30 No error code between -15 to -30
    if(ec < (14 + WebCLExceptionOffset))
        description->code =  WebCLExceptionOffset - ec;
    else
        description->code =  WebCLExceptionOffset - ec - 15;

    // Change the type
    description->type = WebCLExceptionType;

    size_t tableSize = WTF_ARRAY_LENGTH(exceptionNames);
    size_t tableIndex = ec - WebCLExceptionOffset;
    description->name = tableIndex < tableSize ? exceptionNames[tableIndex] : 0;
    description->description = tableIndex < tableSize ? exceptionDescriptions[tableIndex] : 0;
    return true;
}

WebCLException::WebCLExceptionCode WebCLException::computeContextErrorToWebCLExceptionCode(int computeContextError)
{
    switch (computeContextError) {
    case ComputeContext::SUCCESS:
        return WebCLException::SUCCESS;
    case ComputeContext::INVALID_PROGRAM_EXECUTABLE:
        return WebCLException::INVALID_PROGRAM_EXECUTABLE;
    case ComputeContext::INVALID_COMMAND_QUEUE:
        return WebCLException::INVALID_COMMAND_QUEUE;
    case ComputeContext::INVALID_KERNEL:
        return WebCLException::INVALID_KERNEL;
    case ComputeContext::INVALID_CONTEXT:
        return WebCLException::INVALID_CONTEXT;
    case ComputeContext::INVALID_KERNEL_ARGS:
        return WebCLException::INVALID_KERNEL_ARGS;
    case ComputeContext::INVALID_WORK_DIMENSION:
        return WebCLException::INVALID_WORK_DIMENSION;
    case ComputeContext::INVALID_GLOBAL_WORK_SIZE:
        return WebCLException::INVALID_GLOBAL_WORK_SIZE;
    case ComputeContext::INVALID_GLOBAL_OFFSET:
        return WebCLException::INVALID_GLOBAL_OFFSET;
    case ComputeContext::INVALID_WORK_GROUP_SIZE:
        return WebCLException::INVALID_WORK_GROUP_SIZE;
    case ComputeContext::MISALIGNED_SUB_BUFFER_OFFSET:
        return WebCLException::MISALIGNED_SUB_BUFFER_OFFSET;
    case ComputeContext::INVALID_WORK_ITEM_SIZE:
        return WebCLException::INVALID_WORK_ITEM_SIZE;
    case ComputeContext::INVALID_IMAGE_SIZE:
        return WebCLException::INVALID_IMAGE_SIZE;
    case ComputeContext::MEM_OBJECT_ALLOCATION_FAILURE:
        return WebCLException::MEM_OBJECT_ALLOCATION_FAILURE;
    case ComputeContext::INVALID_EVENT_WAIT_LIST:
        return WebCLException::INVALID_EVENT_WAIT_LIST;
    case ComputeContext::OUT_OF_RESOURCES:
        return WebCLException::OUT_OF_RESOURCES;
    case ComputeContext::OUT_OF_HOST_MEMORY:
        return WebCLException::OUT_OF_HOST_MEMORY;
    case ComputeContext::DEVICE_NOT_FOUND:
        return WebCLException::DEVICE_NOT_FOUND;
    case ComputeContext::DEVICE_NOT_AVAILABLE:
        return WebCLException::DEVICE_NOT_AVAILABLE;
    case ComputeContext::COMPILER_NOT_AVAILABLE:
        return WebCLException::COMPILER_NOT_AVAILABLE;
    case ComputeContext::PROFILING_INFO_NOT_AVAILABLE:
        return WebCLException::PROFILING_INFO_NOT_AVAILABLE;
    case ComputeContext::MEM_COPY_OVERLAP:
        return WebCLException::MEM_COPY_OVERLAP;
    case ComputeContext::IMAGE_FORMAT_MISMATCH:
       return WebCLException::IMAGE_FORMAT_MISMATCH;
    case ComputeContext::IMAGE_FORMAT_NOT_SUPPORTED:
        return WebCLException::IMAGE_FORMAT_NOT_SUPPORTED;
    case ComputeContext::BUILD_PROGRAM_FAILURE:
        return WebCLException::BUILD_PROGRAM_FAILURE;
    case ComputeContext::MAP_FAILURE:
        return WebCLException::MAP_FAILURE;
    case ComputeContext::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        return WebCLException::EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
    case ComputeContext::INVALID_VALUE:
        return WebCLException::INVALID_VALUE;
    case ComputeContext::INVALID_DEVICE_TYPE:
        return WebCLException::INVALID_DEVICE_TYPE;
    case ComputeContext::INVALID_PLATFORM:
        return WebCLException::INVALID_PLATFORM;
    case ComputeContext::INVALID_DEVICE:
        return WebCLException::INVALID_DEVICE;
    case ComputeContext::INVALID_QUEUE_PROPERTIES:
        return WebCLException::INVALID_QUEUE_PROPERTIES;
    case ComputeContext::INVALID_HOST_PTR:
        return WebCLException::INVALID_HOST_PTR;
    case ComputeContext::INVALID_MEM_OBJECT:
        return WebCLException::INVALID_MEM_OBJECT;
    case ComputeContext::INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return WebCLException::INVALID_IMAGE_FORMAT_DESCRIPTOR;
    case ComputeContext::INVALID_SAMPLER:
        return WebCLException::INVALID_SAMPLER;
    case ComputeContext::INVALID_BINARY:
        return WebCLException::INVALID_BINARY;
    case ComputeContext::INVALID_BUILD_OPTIONS:
        return WebCLException::INVALID_BUILD_OPTIONS;
    case ComputeContext::INVALID_PROGRAM:
        return WebCLException::INVALID_PROGRAM;
    case ComputeContext::INVALID_KERNEL_NAME:
        return WebCLException::INVALID_KERNEL_NAME;
    case ComputeContext::INVALID_KERNEL_DEFINITION:
        return WebCLException::INVALID_KERNEL_DEFINITION;
    case ComputeContext::INVALID_ARG_INDEX:
        return WebCLException::INVALID_ARG_INDEX;
    case ComputeContext::INVALID_ARG_VALUE:
        return WebCLException::INVALID_ARG_VALUE;
    case ComputeContext::INVALID_ARG_SIZE:
        return WebCLException::INVALID_ARG_SIZE;
    case ComputeContext::INVALID_EVENT:
        return WebCLException::INVALID_EVENT;
    case ComputeContext::INVALID_OPERATION:
        return WebCLException::INVALID_OPERATION;
    case ComputeContext::INVALID_GL_OBJECT:
        return WebCLException::INVALID_GL_OBJECT;
    case ComputeContext::INVALID_BUFFER_SIZE:
        return WebCLException::INVALID_BUFFER_SIZE;
    case ComputeContext::INVALID_MIP_LEVEL:
        return WebCLException::INVALID_MIP_LEVEL;
    case ComputeContext::INVALID_PROPERTY:
        return WebCLException::INVALID_PROPERTY;
    default:
        return WebCLException::WEBCL_IMPLEMENTATION_FAILURE;
    }
}

void setExceptionFromComputeErrorCode(int computeContextError, ExceptionState& es /* ExceptionCode& ec */) {
	if (computeContextError == ComputeContext::SUCCESS)
		return;
    int errorCode = WebCLException::computeContextErrorToWebCLExceptionCode(computeContextError);
	ExceptionCodeDescription desc(errorCode);
	
	es.throwWebCLException(errorCode, WTF::String(desc.name)+"^"+desc.description);
}
void setExtensionsNotEnabledException(ExceptionState & es/*ExceptionCode& ec*/) {
	/*ec = */es.throwWebCLException(WebCLException::WEBCL_EXTENSION_NOT_ENABLED, "");
}
bool willThrowException(ExceptionState & es/*ExceptionCode& ec*/) {
    //return (ec != WebCLException::SUCCESS);
	return es.hadException();
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

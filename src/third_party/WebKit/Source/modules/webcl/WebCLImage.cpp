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

#include "WebCLImage.h"
#include "WebCLException.h"
#include "WebCLContext.h"
#include "WebCL.h"

namespace WebCore {

WebCLImage::~WebCLImage()
{
}

PassRefPtr<WebCLImage> WebCLImage::create(WebCL* compute_context, 
	cl_mem image, bool is_shared = false)
{
	return adoptRef(new WebCLImage(compute_context, image, is_shared));
}

WebCLImage::WebCLImage(WebCL* compute_context, cl_mem image, bool is_shared) 
		: m_context(compute_context), m_cl_mem(image), m_shared(is_shared)
{
}

cl_mem WebCLImage::getCLImage()
{
	return m_cl_mem;
}

int WebCLImage::getGLtextureInfo(int paramNameobj, ExceptionState& ec)
{
      cl_int err = 0;
      
      if (m_cl_mem == NULL) {
		printf("Error: Invalid CL Context\n");
		ec.throwDOMException(WebCLException::INVALID_MEM_OBJECT, "WebCLException::INVALID_MEM_OBJECT");
		return NULL;
	  }
	  cl_int int_units = 0;
	  
	  switch(paramNameobj)
	  {
            case WebCL::TEXTURE_TARGET:
			err = clGetGLTextureInfo(m_cl_mem, CL_GL_TEXTURE_TARGET, sizeof(cl_int), &int_units, NULL);
			if (err == CL_SUCCESS)
			return ((int)int_units);
			break;
		
            case WebCL::MIPMAP_LEVEL:
			err = clGetGLTextureInfo(m_cl_mem, CL_GL_MIPMAP_LEVEL, sizeof(cl_int), &int_units, NULL);
			if (err == CL_SUCCESS)
			return ((int)int_units);
			break;
			
	     default:
			printf("Error: Unsupported paramName Info type = %d ",paramNameobj);
			return (NULL);
                          
      }
      if(err != CL_SUCCESS)
	  {
        	switch (err) {
        		case CL_INVALID_MEM_OBJECT:
        			ec.throwDOMException(WebCLException::INVALID_MEM_OBJECT, "WebCLException::INVALID_MEM_OBJECT");
        			printf("Error: CL_INVALID_MEM_OBJECT  \n");
        			break;
        		case CL_INVALID_GL_OBJECT:
        			ec.throwDOMException(WebCLException::INVALID_GL_OBJECT, "WebCLException::INVALID_GL_OBJECT");
        			printf("Error: CL_INVALID_GL_OBJECT \n");
        			break;
        		case CL_INVALID_VALUE:
        			ec.throwDOMException(WebCLException::INVALID_VALUE, "WebCLException::INVALID_VALUE");
        			printf("Error: CL_INVALID_VALUE \n");
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
        			printf("Invaild Error Type\n");
        			break;
        	}
	  }
	  
      return (NULL);
      //clGetGLObjectInfo (cl_mem memobj,cl_gl_object_type *gl_object_type,GLuint *gl_object_name)
      
      
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

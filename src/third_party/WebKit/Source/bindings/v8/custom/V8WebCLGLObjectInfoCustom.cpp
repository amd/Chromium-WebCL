#include "config.h"

#if ENABLE(WEBCL)

#include "V8WebCLCustom.h"
#include "V8WebCLGLObjectInfo.h"

namespace WebCore {

void V8WebCLGLObjectInfo::glObjectAttributeGetterCustom(const v8::PropertyCallbackInfo<v8::Value>& args)
{
  
    ExceptionState es(args.GetIsolate());
    WebCLGLObjectInfo* info = V8WebCLGLObjectInfo::toNative(args.Holder()); 

	/* TODO 
    switch(info->type()) {
    case ComputeContext::GL_OBJECT_BUFFER:
    v8SetReturnValue(args, toV8Object(static_cast<WebGLBuffer*>(info->glObject()), args.Holder(),args.GetIsolate()));
    break;
    case ComputeContext::GL_OBJECT_TEXTURE2D:
    v8SetReturnValue(args, toV8Object(static_cast<WebGLTexture*>(info->glObject()), args.Holder(),args.GetIsolate()));
    break;
    case ComputeContext::GL_OBJECT_RENDERBUFFER:
    v8SetReturnValue(args, toV8Object(static_cast<WebGLRenderBuffer*>(info->glObject()), args.Holder(),args.GetIsolate()));
    break;
    }
	*/

}


}
#endif


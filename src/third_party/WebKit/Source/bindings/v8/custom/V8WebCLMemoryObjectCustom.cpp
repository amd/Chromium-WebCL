#include "config.h"

#if ENABLE(WEBCL)

#include "V8WebCLCustom.h"
#include "V8WebCLMemoryObject.h"

namespace WebCore {

void V8WebCLMemoryObject::getInfoMethodCustom(const v8::Arguments& args)
{
  
    if (args.Length() != 1)
        { throwNotEnoughArgumentsError(args.GetIsolate()); return; }

    ExceptionState es(args.GetIsolate());
    WebCLMemoryObject* memObj = V8WebCLMemoryObject::toNative(args.Holder()); 
    unsigned mem_info = toInt32(args[0]);
    WebCLGetInfo info = memObj->getInfo(mem_info, es);

    v8SetReturnValue(args, toV8Object(info, args.Holder(),args.GetIsolate()));
}


}
#endif
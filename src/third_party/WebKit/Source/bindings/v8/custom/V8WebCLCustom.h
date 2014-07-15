/*
 * Copyright (C) 2012 Intel Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE
 * INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEBCL)

// To avoid conflict between TraceEvent.h and trace_event.h
#define THIRD_PARTY_WEBKIT_MODULES_WEBCL

#include "V8Float32ArrayCustom.h"
#include "V8Int32ArrayCustom.h"
#include "..\V8Binding.h"
#include "V8WebCLCommandQueue.h"
#include "V8WebCLContext.h"
#include "V8WebCLDevice.h"
#include "V8WebCLPlatform.h"
//#include "V8WebCLDeviceList.h"
#include "V8WebCLProgram.h"
#include "V8WebCLMemoryObject.h"
#include "V8WebCLImageDescriptor.h"

#include "modules/webcl/WebCLCommandQueue.h"
//#include "..\..\core\platform\NotImplemented.h"

#include "V8Uint32ArrayCustom.h"

namespace v8 {
	typedef FunctionCallbackInfo<v8::Value> Arguments;
}

#define throwNotEnoughArgumentsError(x) //ScalableVision, to be impl'd

namespace WebCore {
#if 0 //ScalableVision
	inline const uint16_t* fromWebCoreString(const String& str)
 {
        return reinterpret_cast<const uint16_t*>(str.characters());
 }
#else
	inline const char* fromWebCoreString(const String& str)
 {
        return reinterpret_cast<const char*>(str.characters8());
 }
#endif
static v8::Handle<v8::Value> toV8Object(const WebCLGetInfo& info,v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    switch (info.getType()) {
        case WebCLGetInfo::kTypeBool:
            return v8::Boolean::New(info.getBool());
        /*case WebCLGetInfo::kTypeBoolArray: {
            const Vector<bool>& value = info.getBoolArray();
            v8::Local<v8::Array> array = v8::Array::New(value.size());
            for (size_t ii = 0; ii < value.size(); ++ii)
                array->Set(v8::Integer::New(ii), v8::Boolean::New(value[ii]));
            return array;
        case WebCLGetInfo::kTypeFloat:
            return v8::Number::New(info.getFloat());
	*/
        case WebCLGetInfo::kTypeInt:
            return v8::Integer::New(info.getInt());
        case WebCLGetInfo::kTypeNull:
            return v8::Null(isolate);
        case WebCLGetInfo::kTypeString:
            return v8::String::New(fromWebCoreString(info.getString()), info.getString().length());
        case WebCLGetInfo::kTypeUnsignedInt:
            return v8::Integer::NewFromUnsigned(info.getUnsignedInt());
        case WebCLGetInfo::kTypeUnsignedLong:
            //return v8::Interger::NewFromUnsigned(info.getUnsignedInt());
        /*case WebCLGetInfo::kTypeWebCLFloatArray:
            return toV8(info.getWebCLFloatArray(),creationContext, isolate);*/
        case WebCLGetInfo::kTypeWebCLIntArray:
			{

				Vector<CCuint> uint_arr = info.getWebCLUintArray();
				/*
				PassRefPtr<Uint32Array> v8arr = Uint32Array::create(&uint_arr[0], uint_arr.size());

				return toV8(v8arr,creationContext, isolate);
				*/
				// We will be creating temporary handles so we use a handle scope
				v8::Isolate* isolate = v8::Isolate::GetCurrent();
				v8::HandleScope handle_scope(isolate);

				// Create a new empty array.
				v8::Handle<v8::Array> array = v8::Array::New(uint_arr.size());

				// Return an empty result if there was an error creating the array.
				if (array.IsEmpty())
					return v8::Handle<v8::Array>();

				// Fill out the values
				for (int i=0; i<(int)uint_arr.size(); i++)
					array->Set(i, v8::Integer::New(uint_arr[i]));

				// Return the value through Close.
				return handle_scope.Close(array);

			}

    case WebCLGetInfo::kTypeWebCLImageDescriptor: //TODO
        return v8::Undefined();//return toV8(info.getWebCLImageDescriptor(), creationContext, isolate);

        case WebCLGetInfo::kTypeWebCLProgram:
            return toV8(info.getWebCLProgram(),creationContext, isolate);
        case WebCLGetInfo::kTypeWebCLContext:
            return toV8(info.getWebCLContext(), creationContext,isolate);
        case WebCLGetInfo::kTypeWebCLCommandQueue:
            return toV8(info.getWebCLCommandQueue(),creationContext, isolate);
        case WebCLGetInfo::kTypeWebCLDevice:
            return toV8(info.getWebCLDevice(), creationContext,isolate);
        /*case WebCLGetInfo::kTypeWebCLDeviceList:
            return toV8(info.getWebCLDeviceList(),creationContext, isolate);*/
    case WebCLGetInfo::kTypeWebCLDevices: //TODO
        {
			Vector<RefPtr<WebCLDevice> > devices = info.getWebCLDevices();
				v8::Isolate* isolate = v8::Isolate::GetCurrent();
				v8::HandleScope handle_scope(isolate);

				// Create a new empty array.
				v8::Handle<v8::Array> array = v8::Array::New(devices.size());

				// Return an empty result if there was an error creating the array.
				if (array.IsEmpty())
					return v8::Handle<v8::Array>();

				// Fill out the values
				for (int i=0; i<(int)devices.size(); i++)
					array->Set(i, toV8(devices[i], creationContext, isolate));
				return handle_scope.Close(array);
		}

    case WebCLGetInfo::kTypeWebCLMemoryObject: //TODO
		return v8::Undefined(); //return toV8(info.getWebCLMemoryObject(), creationContext, isolate);
    case WebCLGetInfo::kTypeWebCLPlatform: //TODO
        return toV8(info.getWebCLPlatform(), creationContext, isolate);

        default:
            // notImplemented(); //ScalableVision
            return v8::Undefined();
    }
}

}

#endif // ENABLE(WEBCL)

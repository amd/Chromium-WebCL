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
#include "WebCLKernelTypes.h"
#include "WebCLException.h"

#if ENABLE(WEBCL)

#include <wtf/DecimalNumber.h>

namespace WebCore {

bool WebCLKernelTypeValue::asNumber(char*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(unsigned char*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(short*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(unsigned short*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(float*) const
{
    return false;
}
bool WebCLKernelTypeValue::asNumber(long*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(int*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(unsigned long*) const
{
    return false;
}

bool WebCLKernelTypeValue::asNumber(unsigned int*) const
{
    return false;
}

bool WebCLKernelTypeValue::asValue(RefPtr<WebCLKernelTypeValue>* output)
{
    *output = this;
    return true;
}

bool WebCLKernelTypeValue::asObject(RefPtr<WebCLKernelTypeObject>*)
{
    return false;
}

bool WebCLKernelTypeValue::asVector(RefPtr<WebCLKernelTypeVector>*)
{
    return false;
}

PassRefPtr<WebCLKernelTypeObject> WebCLKernelTypeValue::asObject()
{
    return 0;
}

PassRefPtr<WebCLKernelTypeVector> WebCLKernelTypeValue::asVector()
{
    return 0;
}

bool WebCLKernelTypeBasicValue::asNumber(char* output) const
{
	// TODO (siba samal) Need to Check if conversion works !!
    if (type() != TypeNumber)
        return false;
    *output = static_cast<char>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(unsigned char* output) const
{
	// TODO (siba samal) Need to Check if conversion works !!
    if (type() != TypeNumber)
        return false;
    *output = static_cast<unsigned char>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(float* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<float>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(long* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<long>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(int* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<int>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(unsigned int* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<unsigned int>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(short* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<short>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(unsigned long* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<unsigned long>(m_doubleValue);
    return true;
}

bool WebCLKernelTypeBasicValue::asNumber(unsigned short* output) const
{
    if (type() != TypeNumber)
        return false;
    *output = static_cast<unsigned short>(m_doubleValue);
    return true;
}

WebCLKernelTypeObject::~WebCLKernelTypeObject()
{
}

bool WebCLKernelTypeObject::asObject(RefPtr<WebCLKernelTypeObject>* output)
{
    *output = this;
    return true;
}

PassRefPtr<WebCLKernelTypeObject> WebCLKernelTypeObject::asObject()
{
    return this;
}

PassRefPtr<WebCLKernelTypeObject> WebCLKernelTypeObject::getObject(const String& name) const
{
    PassRefPtr<WebCLKernelTypeValue> value = get(name);
    if (!value)
        return 0;
    return value->asObject();
}

PassRefPtr<WebCLKernelTypeVector> WebCLKernelTypeObject::getVector(const String& name) const
{
    PassRefPtr<WebCLKernelTypeValue> value = get(name);
    if (!value)
        return 0;
    return value->asVector();
}

PassRefPtr<WebCLKernelTypeValue> WebCLKernelTypeObject::get(const String& name) const
{
    Dictionary::const_iterator it = m_data.find(name);
    if (it == m_data.end())
        return 0;
    return it->value;
}

void WebCLKernelTypeObject::remove(const String& name)
{
    m_data.remove(name);
    for (size_t i = 0; i < m_order.size(); ++i) {
        if (m_order[i] == name) {
            m_order.remove(i);
            break;
        }
    }
}

WebCLKernelTypeObject::WebCLKernelTypeObject()
    : WebCLKernelTypeValue(TypeObject)
    , m_data()
    , m_order()
{
}

WebCLKernelTypeVector::~WebCLKernelTypeVector()
{
}

bool WebCLKernelTypeVector::asVector(RefPtr<WebCLKernelTypeVector>* output)
{
    *output = this;
    return true;
}

PassRefPtr<WebCLKernelTypeVector> WebCLKernelTypeVector::asVector()
{
    return this;
}

WebCLKernelTypeVector::WebCLKernelTypeVector()
    : WebCLKernelTypeValue(TypeVector)
    , m_data()
{
}

PassRefPtr<WebCLKernelTypeValue> WebCLKernelTypeVector::get(size_t index)
{
    ASSERT(index < m_data.size());
    return m_data[index];
}

} // namespace WebCore

#endif // ENABLE(WEBCL)

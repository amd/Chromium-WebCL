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
#ifndef WebCLKernelTypes_h
#define WebCLKernelTypes_h

#ifndef THIRD_PARTY_WEBKIT_MODULES_WEBCL // ScalableVision to avoid conflict between TraceEvent.h and trace_event.h
#define THIRD_PARTY_WEBKIT_MODULES_WEBCL
#endif

#if ENABLE(WEBCL)

//#include "PlatformString.h"

#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/text/StringHash.h>

namespace WebCore {

class WebCLKernelTypeVector;
class WebCLKernelTypeObject;

class WebCLKernelTypeValue : public RefCounted<WebCLKernelTypeValue> {
public:
    WebCLKernelTypeValue() : m_type(TypeNull) { }
    virtual ~WebCLKernelTypeValue() { }

    static PassRefPtr<WebCLKernelTypeValue> null()
    {
        return adoptRef(new WebCLKernelTypeValue());
    }

    typedef enum {
        TypeNull = 0,
        TypeNumber,
        TypeObject,
        TypeVector
    } Type;

    Type type() const { return m_type; }

    bool isNull() const { return m_type == TypeNull; }

	virtual bool asNumber(char* output) const;
	virtual bool asNumber(unsigned char* output) const;
	virtual bool asNumber(float* output) const;
    virtual bool asNumber(long* output) const;
    virtual bool asNumber(int* output) const;
	virtual bool asNumber(short* output) const;
    virtual bool asNumber(unsigned long* output) const;
    virtual bool asNumber(unsigned int* output) const;
	virtual bool asNumber(unsigned short* output) const;
	
    virtual bool asValue(RefPtr<WebCLKernelTypeValue>* output);
    virtual bool asObject(RefPtr<WebCLKernelTypeObject>* output);
    virtual bool asVector(RefPtr<WebCLKernelTypeVector>* output);
    virtual PassRefPtr<WebCLKernelTypeObject> asObject();
    virtual PassRefPtr<WebCLKernelTypeVector> asVector();

protected:
    explicit WebCLKernelTypeValue(Type type) : m_type(type) { }

private:
    Type m_type;
};

class WebCLKernelTypeBasicValue : public WebCLKernelTypeValue {
public:

    static PassRefPtr<WebCLKernelTypeBasicValue> create(int value)
    {
        return adoptRef(new WebCLKernelTypeBasicValue(value));
    }

    static PassRefPtr<WebCLKernelTypeBasicValue> create(double value)
    {
        return adoptRef(new WebCLKernelTypeBasicValue(value));
    }

	virtual bool asNumber(char* output) const;
	virtual bool asNumber(unsigned char* output) const;
	virtual bool asNumber(float* output) const;
    virtual bool asNumber(long* output) const;
    virtual bool asNumber(int* output) const;
	virtual bool asNumber(short* output) const;
    virtual bool asNumber(unsigned long* output) const;
    virtual bool asNumber(unsigned int* output) const;
	virtual bool asNumber(unsigned short* output) const;

private:
	explicit WebCLKernelTypeBasicValue(int value) : WebCLKernelTypeValue(TypeNumber), m_doubleValue((double)value) { }
    explicit WebCLKernelTypeBasicValue(double value) : WebCLKernelTypeValue(TypeNumber), m_doubleValue(value) { }

    union {
        double m_doubleValue;
    };
};

class WebCLKernelTypeObject : public WebCLKernelTypeValue {
private:
    typedef HashMap<String, RefPtr<WebCLKernelTypeValue> > Dictionary;

public:
    typedef Dictionary::iterator iterator;
    typedef Dictionary::const_iterator const_iterator;

public:
    static PassRefPtr<WebCLKernelTypeObject> create()
    {
        return adoptRef(new WebCLKernelTypeObject());
    }
    ~WebCLKernelTypeObject();

    virtual bool asObject(RefPtr<WebCLKernelTypeObject>* output);
    virtual PassRefPtr<WebCLKernelTypeObject> asObject();

    void setNumber(const String& name, double);
    void setValue(const String& name, PassRefPtr<WebCLKernelTypeValue>);
    void setObject(const String& name, PassRefPtr<WebCLKernelTypeObject>);
    void setVector(const String& name, PassRefPtr<WebCLKernelTypeVector>);

    iterator find(const String& name);
    const_iterator find(const String& name) const;
    template<class T> bool getNumber(const String& name, T* output) const
    {
        RefPtr<WebCLKernelTypeValue> value = get(name);
        if (!value)
            return false;
        return value->asNumber(output);
    }
    bool getString(const String& name, String* output) const;
    PassRefPtr<WebCLKernelTypeObject> getObject(const String& name) const;
    PassRefPtr<WebCLKernelTypeVector> getVector(const String& name) const;
    PassRefPtr<WebCLKernelTypeValue> get(const String& name) const;

    void remove(const String& name);

    iterator begin() { return m_data.begin(); }
    iterator end() { return m_data.end(); }
    const_iterator begin() const { return m_data.begin(); }
    const_iterator end() const { return m_data.end(); }

private:
    WebCLKernelTypeObject();
    Dictionary m_data;
    Vector<String> m_order;
};

class WebCLKernelTypeVector : public WebCLKernelTypeValue {
public:
    static PassRefPtr<WebCLKernelTypeVector> create()
    {
        return adoptRef(new WebCLKernelTypeVector());
    }
    ~WebCLKernelTypeVector();

    virtual bool asVector(RefPtr<WebCLKernelTypeVector>* output);
    virtual PassRefPtr<WebCLKernelTypeVector> asVector();

    void pushNumber(double);
    void pushValue(PassRefPtr<WebCLKernelTypeValue>);
    void pushObject(PassRefPtr<WebCLKernelTypeObject>);
    void pushVector(PassRefPtr<WebCLKernelTypeVector>);
    unsigned length() const { return m_data.size(); }

    PassRefPtr<WebCLKernelTypeValue> get(size_t index);


private:
    WebCLKernelTypeVector();
    Vector<RefPtr<WebCLKernelTypeValue> > m_data;
};

inline WebCLKernelTypeObject::iterator WebCLKernelTypeObject::find(const String& name)
{
    return m_data.find(name);
}

inline WebCLKernelTypeObject::const_iterator WebCLKernelTypeObject::find(const String& name) const
{
    return m_data.find(name);
}


inline void WebCLKernelTypeObject::setNumber(const String& name, double value)
{
    setValue(name, WebCLKernelTypeBasicValue::create(value));
}

inline void WebCLKernelTypeObject::setValue(const String& name, PassRefPtr<WebCLKernelTypeValue> value)
{
    if (m_data.set(name, value).iterator->value/*second*/)
        m_order.append(name);
}

inline void WebCLKernelTypeObject::setObject(const String& name, PassRefPtr<WebCLKernelTypeObject> value)
{
	if (m_data.set(name, value).iterator->value/*->second*/)
        m_order.append(name);
}

inline void WebCLKernelTypeObject::setVector(const String& name, PassRefPtr<WebCLKernelTypeVector> value)
{
    if (m_data.set(name, value).iterator->value/*second*/)
        m_order.append(name);
}

inline void WebCLKernelTypeVector::pushNumber(double value)
{
    m_data.append(WebCLKernelTypeBasicValue::create(value));
}

inline void WebCLKernelTypeVector::pushValue(PassRefPtr<WebCLKernelTypeValue> value)
{
    m_data.append(value);
}

inline void WebCLKernelTypeVector::pushObject(PassRefPtr<WebCLKernelTypeObject> value)
{
    m_data.append(value);
}

inline void WebCLKernelTypeVector::pushVector(PassRefPtr<WebCLKernelTypeVector> value)
{
    m_data.append(value);
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // !defined(WebCLKernelTypes_h)

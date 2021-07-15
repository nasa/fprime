#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Prm/PrmString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    ParamString::ParamString(const char* src) : StringBase()  {
        this->copyBuff(src,this->getCapacity());
    }

    ParamString::ParamString(const StringBase& src) : StringBase()  {
        this->copyBuff(src.toChar(),this->getCapacity());
    }

    ParamString::ParamString(const ParamString& src) : StringBase()  {
        this->copyBuff(src.m_buf,this->getCapacity());
    }

    ParamString::ParamString() : StringBase()  {
        this->m_buf[0] = 0;
    }

    ParamString::~ParamString() {
    }

    NATIVE_UINT_TYPE ParamString::length() const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* ParamString::toChar() const {
        return this->m_buf;
    }

    SerializeStatus ParamString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    SerializeStatus ParamString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE ParamString::getCapacity() const {
        return FW_PARAM_STRING_MAX_SIZE;
    }

    void ParamString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

    const ParamString& ParamString::operator=(const ParamString& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

}

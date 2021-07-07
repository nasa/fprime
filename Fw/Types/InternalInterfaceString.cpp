#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/InternalInterfaceString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    InternalInterfaceString::InternalInterfaceString(const char* src) : StringBase()  {
        this->copyBuff(src, sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString(const StringBase& src) : StringBase()  {
        this->copyBuff(src.toChar(), sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString(const InternalInterfaceString& src) : StringBase()  {
        this->copyBuff(src.m_buf, sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString() : StringBase()  {
        this->m_buf[0] = 0;
    }

    InternalInterfaceString::~InternalInterfaceString() {
    }

    NATIVE_UINT_TYPE InternalInterfaceString::length() const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* InternalInterfaceString::toChar() const {
        return this->m_buf;
    }

    SerializeStatus InternalInterfaceString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    SerializeStatus InternalInterfaceString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE InternalInterfaceString::getCapacity() const {
        return FW_INTERNAL_INTERFACE_STRING_MAX_SIZE;
    }

    const InternalInterfaceString& InternalInterfaceString::operator=(const InternalInterfaceString& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

    void InternalInterfaceString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

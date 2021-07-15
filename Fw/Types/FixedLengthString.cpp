#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/FixedLengthString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    FixedLengthString::FixedLengthString(const char* src) : StringBase() {
        this->copyBuff(src, sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString(const StringBase& src) : StringBase() {
        this->copyBuff(src.toChar(), sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString(const FixedLengthString& src) : StringBase() {
        this->copyBuff(src.m_buf, sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString() : StringBase() {
        this->m_buf[0] = 0;
    }

    FixedLengthString::~FixedLengthString() {
    }

    NATIVE_UINT_TYPE FixedLengthString::length() const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* FixedLengthString::toChar() const {
        return this->m_buf;
    }

    const FixedLengthString& FixedLengthString::operator=(const FixedLengthString& other) {
        this->copyBuff(other.m_buf,sizeof(this->m_buf));
        return *this;
    }

    SerializeStatus FixedLengthString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    SerializeStatus FixedLengthString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE FixedLengthString::getCapacity() const {
        return STRING_SIZE;
    }

    void FixedLengthString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

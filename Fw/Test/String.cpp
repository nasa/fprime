#include <Fw/Test/String.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Test {

    String::String(const char* src) : StringBase() {
        this->copyBuff(src, sizeof(this->m_buf));
    }

    String::String(const StringBase& src) : StringBase() {
        this->copyBuff(src.toChar(), sizeof(this->m_buf));
    }

    String::String(const String& src) : StringBase() {
        this->copyBuff(src.m_buf, sizeof(this->m_buf));
    }

    String::String(void) : StringBase() {
        this->m_buf[0] = 0;
    }

    String::~String(void) {
    }

    NATIVE_UINT_TYPE String::length(void) const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* String::toChar(void) const {
        return this->m_buf;
    }

    const String& String::operator=(const String& other) {
        this->copyBuff(other.m_buf,sizeof(this->m_buf));
        return *this;
    }

    Fw::SerializeStatus String::serialize(Fw::SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    Fw::SerializeStatus String::deserialize(Fw::SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        Fw::SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE String::getCapacity(void) const {
        return STRING_SIZE;
    }

    void String::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

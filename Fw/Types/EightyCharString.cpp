#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    EightyCharString::EightyCharString(const char* src) : StringBase() {
        this->copyBuff(src, sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const StringBase& src) : StringBase() {
        this->copyBuff(src.toChar(), sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const EightyCharString& src) : StringBase() {
        this->copyBuff(src.m_buf, sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString() : StringBase() {
        this->m_buf[0] = 0;
    }

    EightyCharString::~EightyCharString() {
    }

    NATIVE_UINT_TYPE EightyCharString::length() const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* EightyCharString::toChar() const {
        return this->m_buf;
    }

    const EightyCharString& EightyCharString::operator=(const EightyCharString& other) {
        this->copyBuff(other.m_buf,sizeof(this->m_buf));
        return *this;
    }

    SerializeStatus EightyCharString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    SerializeStatus EightyCharString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE EightyCharString::getCapacity() const {
        return STRING_SIZE;
    }

    void EightyCharString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

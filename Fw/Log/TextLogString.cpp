#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Log/TextLogString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    TextLogString::TextLogString(const char* src) : StringBase() {
        this->copyBuff(src,sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const StringBase& src): StringBase()  {
        this->copyBuff(src.toChar(),sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const TextLogString& src): StringBase()  {
        this->copyBuff(src.m_buf,sizeof(this->m_buf));
    }

    TextLogString::TextLogString(void): StringBase()  {
        this->m_buf[0] = 0;
    }

    TextLogString::~TextLogString(void) {
    }

    NATIVE_UINT_TYPE TextLogString::length(void) const {
        return (NATIVE_UINT_TYPE) strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* TextLogString::toChar(void) const {
        return this->m_buf;
    }

    void TextLogString::copyBuff(const char* buff, NATIVE_UINT_TYPE size) {
        FW_ASSERT(buff);
        // check for self copy
        if (buff != this->m_buf) {
            (void)strncpy(this->m_buf,buff,size);
            // NULL terminate
            this->terminate(sizeof(this->m_buf));
        }
    }
    
    SerializeStatus TextLogString::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string
        return buffer.serialize((U8*)this->m_buf,strSize);
    }
    
    SerializeStatus TextLogString::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE TextLogString::getCapacity(void) const {
        return FW_LOG_TEXT_BUFFER_SIZE;
    }
    
    void TextLogString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

    const TextLogString& TextLogString::operator=(const TextLogString& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

}

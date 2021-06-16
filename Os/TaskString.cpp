#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/TaskString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Os {

    TaskString::TaskString(const char* src) : StringBase()  {
        this->copyBuff(src,sizeof(this->m_buf));
    }

    TaskString::TaskString(const StringBase& src) : StringBase()  {
        this->copyBuff(src.toChar(),sizeof(this->m_buf));
    }

    TaskString::TaskString(const TaskString& src) : StringBase()  {
        this->copyBuff(src.m_buf,sizeof(this->m_buf));
    }

    TaskString::TaskString(void) {
        this->m_buf[0] = 0;
    }

    TaskString::~TaskString(void) {
    }

    NATIVE_UINT_TYPE TaskString::length(void) const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* TaskString::toChar(void) const {
        return this->m_buf;
    }

    const TaskString& TaskString::operator=(const TaskString& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

    NATIVE_UINT_TYPE TaskString::getCapacity(void) const {
        return FW_TASK_NAME_MAX_SIZE;
    }

    Fw::SerializeStatus TaskString::serialize(Fw::SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string as buffer
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    Fw::SerializeStatus TaskString::deserialize(Fw::SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        Fw::SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    void TaskString::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

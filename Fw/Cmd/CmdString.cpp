#include <Fw/Types/StringType.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Cmd/CmdString.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Fw {

    CmdStringArg::CmdStringArg(const char* src) : StringBase() {
        this->copyBuff(src,this->getCapacity());
    }

    CmdStringArg::CmdStringArg(const StringBase& src) : StringBase()  {
        this->copyBuff(src.toChar(),this->getCapacity());
    }

    CmdStringArg::CmdStringArg(const CmdStringArg& src) : StringBase()  {
        this->copyBuff(src.m_buf,this->getCapacity());
    }

    CmdStringArg::CmdStringArg(void) : StringBase() {
        this->m_buf[0] = 0;
    }

    CmdStringArg::~CmdStringArg(void) {
    }

    NATIVE_UINT_TYPE CmdStringArg::length(void) const {
        return strnlen(this->m_buf,sizeof(this->m_buf));
    }

    const char* CmdStringArg::toChar(void) const {
        return this->m_buf;
    }

    const CmdStringArg& CmdStringArg::operator=(const CmdStringArg& other) {
        this->copyBuff(other.m_buf,this->getCapacity());
        return *this;
    }

    SerializeStatus CmdStringArg::serialize(SerializeBufferBase& buffer) const {
        NATIVE_UINT_TYPE strSize = strnlen(this->m_buf,sizeof(this->m_buf));
        // serialize string
        return buffer.serialize((U8*)this->m_buf,strSize);
    }

    SerializeStatus CmdStringArg::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = sizeof(this->m_buf);
        // deserialize string
        SerializeStatus stat = buffer.deserialize((U8*)this->m_buf,maxSize);
        // make sure it is null-terminated
        this->terminate(maxSize);

        return stat;
    }

    NATIVE_UINT_TYPE CmdStringArg::getCapacity(void) const {
        return FW_CMD_STRING_MAX_SIZE;
    }

    void CmdStringArg::terminate(NATIVE_UINT_TYPE size) {
        // null terminate the string
        this->m_buf[size < sizeof(this->m_buf)?size:sizeof(this->m_buf)-1] = 0;
    }

}

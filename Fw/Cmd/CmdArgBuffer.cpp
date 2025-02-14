#include <Fw/Cmd/CmdArgBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    CmdArgBuffer::CmdArgBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = this->setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    CmdArgBuffer::CmdArgBuffer() {
    }

    CmdArgBuffer::~CmdArgBuffer() {
    }

    CmdArgBuffer::CmdArgBuffer(const CmdArgBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = this->setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    CmdArgBuffer& CmdArgBuffer::operator=(const CmdArgBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = this->setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE CmdArgBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* CmdArgBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* CmdArgBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


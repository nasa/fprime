#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TlmBuffer::TlmBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    TlmBuffer::TlmBuffer() {
    }

    TlmBuffer::~TlmBuffer() {
    }

    TlmBuffer::TlmBuffer(const TlmBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    TlmBuffer& TlmBuffer::operator=(const TlmBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE TlmBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* TlmBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* TlmBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


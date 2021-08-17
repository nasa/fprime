#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TlmBuffer::TlmBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }
    
    TlmBuffer::TlmBuffer() {
    }

    TlmBuffer::~TlmBuffer() {
    }

    TlmBuffer::TlmBuffer(const TlmBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    const TlmBuffer& TlmBuffer::operator=(const TlmBuffer& other) {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE TlmBuffer::getBuffCapacity(void) const {
        return sizeof(this->m_bufferData);
    }

    const U8* TlmBuffer::getBuffAddr(void) const {
        return this->m_bufferData;
    }

    U8* TlmBuffer::getBuffAddr(void) {
        return this->m_bufferData;
    }

}


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
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    const TlmBuffer& TlmBuffer::operator=(const TlmBuffer& other) {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE TlmBuffer::getBuffCapacity() const {
        return sizeof(this->m_data);
    }

    const U8* TlmBuffer::getBuffAddr() const {
        return this->m_data;
    }

    U8* TlmBuffer::getBuffAddr() {
        return this->m_data;
    }

}


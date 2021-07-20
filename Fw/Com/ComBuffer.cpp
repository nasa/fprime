#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    ComBuffer::ComBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }
    
    ComBuffer::ComBuffer() {
    }

    ComBuffer::~ComBuffer() {
    }

    ComBuffer::ComBuffer(const ComBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    const ComBuffer& ComBuffer::operator=(const ComBuffer& other) {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE ComBuffer::getBuffCapacity(void) const {
        return sizeof(this->m_bufferData);
    }

    const U8* ComBuffer::getBuffAddr(void) const {
        return this->m_bufferData;
    }

    U8* ComBuffer::getBuffAddr(void) {
        return this->m_bufferData;
    }

}


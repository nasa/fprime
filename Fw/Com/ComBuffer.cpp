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
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    const ComBuffer& ComBuffer::operator=(const ComBuffer& other) {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_data,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE ComBuffer::getBuffCapacity() const {
        return sizeof(this->m_data);
    }

    const U8* ComBuffer::getBuffAddr() const {
        return this->m_data;
    }

    U8* ComBuffer::getBuffAddr() {
        return this->m_data;
    }

}


#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    ComBuffer::ComBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    ComBuffer::ComBuffer() {
    }

    ComBuffer::~ComBuffer() {
    }

    ComBuffer::ComBuffer(const ComBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
    }

    ComBuffer& ComBuffer::operator=(const ComBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE ComBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* ComBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* ComBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


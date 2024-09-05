#include <Fw/Sm/SmSignalBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    SmSignalBuffer::SmSignalBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SmSignalBuffer::SmSignalBuffer() : m_bufferData{} {
    }

    SmSignalBuffer::~SmSignalBuffer() {
    }

    SmSignalBuffer::SmSignalBuffer(const SmSignalBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SmSignalBuffer& SmSignalBuffer::operator=(const SmSignalBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE SmSignalBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* SmSignalBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* SmSignalBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


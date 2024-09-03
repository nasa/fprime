#include <Fw/Sm/SMSignalBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    SMSignalBuffer::SMSignalBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SMSignalBuffer::SMSignalBuffer() {
    }

    SMSignalBuffer::~SMSignalBuffer() {
    }

    SMSignalBuffer::SMSignalBuffer(const SMSignalBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    SMSignalBuffer& SMSignalBuffer::operator=(const SMSignalBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE SMSignalBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* SMSignalBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* SMSignalBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    LogBuffer::LogBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_UINT_TYPE>(stat));
    }
    
    LogBuffer::LogBuffer() {
    }

    LogBuffer::~LogBuffer() {
    }

    LogBuffer::LogBuffer(const LogBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
    }

    const LogBuffer& LogBuffer::operator=(const LogBuffer& other) {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE LogBuffer::getBuffCapacity(void) const {
        return sizeof(this->m_bufferData);
    }

    const U8* LogBuffer::getBuffAddr(void) const {
        return this->m_bufferData;
    }

    U8* LogBuffer::getBuffAddr(void) {
        return this->m_bufferData;
    }

}


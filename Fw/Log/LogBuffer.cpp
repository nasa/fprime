#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    LogBuffer::LogBuffer(const U8 *args, NATIVE_UINT_TYPE size) {
        SerializeStatus stat = SerializeBufferBase::setBuff(args,size);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    LogBuffer::LogBuffer() {
    }

    LogBuffer::~LogBuffer() {
    }

    LogBuffer::LogBuffer(const LogBuffer& other) : Fw::SerializeBufferBase() {
        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    LogBuffer& LogBuffer::operator=(const LogBuffer& other) {
        if(this == &other) {
            return *this;
        }

        SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData,other.getBuffLength());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
        return *this;
    }

    NATIVE_UINT_TYPE LogBuffer::getBuffCapacity() const {
        return sizeof(this->m_bufferData);
    }

    const U8* LogBuffer::getBuffAddr() const {
        return this->m_bufferData;
    }

    U8* LogBuffer::getBuffAddr() {
        return this->m_bufferData;
    }

}


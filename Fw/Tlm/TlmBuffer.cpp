#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

TlmBuffer::TlmBuffer(const U8* args, FwSizeType size) {
    SerializeStatus stat = SerializeBufferBase::setBuff(args, size);
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

TlmBuffer::TlmBuffer() {}

TlmBuffer::~TlmBuffer() {}

TlmBuffer::TlmBuffer(const TlmBuffer& other) : Fw::SerializeBufferBase() {
    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

TlmBuffer& TlmBuffer::operator=(const TlmBuffer& other) {
    if (this == &other) {
        return *this;
    }

    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

FwSizeType TlmBuffer::getCapacity() const {
    return sizeof(this->m_bufferData);
}

FwSizeType TlmBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

const U8* TlmBuffer::getBuffAddr() const {
    return this->m_bufferData;
}

U8* TlmBuffer::getBuffAddr() {
    return this->m_bufferData;
}

}  // namespace Fw

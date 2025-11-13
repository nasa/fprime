#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

ComBuffer::ComBuffer(const U8* args, FwSizeType size) {
    SerializeStatus stat = SerializeBufferBase::setBuff(args, size);
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

ComBuffer::ComBuffer() {}

ComBuffer::~ComBuffer() {}

ComBuffer::ComBuffer(const ComBuffer& other) : Fw::SerializeBufferBase() {
    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

ComBuffer& ComBuffer::operator=(const ComBuffer& other) {
    if (this == &other) {
        return *this;
    }

    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

FwSizeType ComBuffer::getCapacity() const {
    return sizeof(this->m_bufferData);
}

FwSizeType ComBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

const U8* ComBuffer::getBuffAddr() const {
    return this->m_bufferData;
}

U8* ComBuffer::getBuffAddr() {
    return this->m_bufferData;
}

}  // namespace Fw

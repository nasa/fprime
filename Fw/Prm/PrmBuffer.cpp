#include <Fw/Prm/PrmBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

ParamBuffer::ParamBuffer(const U8* args, FwSizeType size) {
    SerializeStatus stat = SerializeBufferBase::setBuff(args, size);
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

ParamBuffer::ParamBuffer() {}

ParamBuffer::~ParamBuffer() {}

ParamBuffer::ParamBuffer(const ParamBuffer& other) : Fw::SerializeBufferBase() {
    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

ParamBuffer& ParamBuffer::operator=(const ParamBuffer& other) {
    if (this == &other) {
        return *this;
    }

    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

FwSizeType ParamBuffer::getCapacity() const {
    return sizeof(this->m_bufferData);
}

FwSizeType ParamBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

const U8* ParamBuffer::getBuffAddr() const {
    return this->m_bufferData;
}

U8* ParamBuffer::getBuffAddr() {
    return this->m_bufferData;
}

}  // namespace Fw

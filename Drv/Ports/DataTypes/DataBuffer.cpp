#include <Drv/Ports/DataTypes/DataBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Drv {

DataBuffer::DataBuffer(const U8* args, FwSizeType size) : Fw::LinearBufferBase(m_data, sizeof(m_data)) {
    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(args, size);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

DataBuffer::DataBuffer() : Fw::LinearBufferBase(m_data, sizeof(m_data)) {}

DataBuffer::~DataBuffer() {}

// m_data contents are copied via setBuff below
// cppcheck-suppress missingMemberCopy
DataBuffer::DataBuffer(const DataBuffer& other) : Fw::LinearBufferBase(m_data, sizeof(m_data)) {
    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(other.m_data, other.getSize());
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

DataBuffer& DataBuffer::operator=(const DataBuffer& other) {
    if (this == &other) {
        return *this;
    }

    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(other.m_data, other.getSize());
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

FwSizeType DataBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

}  // namespace Drv

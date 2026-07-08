#include <Drv/Ports/DataTypes/DataBuffer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Drv {

DataBuffer::DataBuffer(const U8* args, FwSizeType size) {
    this->m_buffAddr = this->m_data;
    this->m_capacity = sizeof(this->m_data);
    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(args, size);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

DataBuffer::DataBuffer() {
    this->m_buffAddr = this->m_data;
    this->m_capacity = sizeof(this->m_data);
}

DataBuffer::~DataBuffer() {}

// m_data contents are copied via setBuff below
// cppcheck-suppress missingMemberCopy
DataBuffer::DataBuffer(const DataBuffer& other) : Fw::LinearBufferBase() {
    this->m_buffAddr = this->m_data;
    this->m_capacity = sizeof(this->m_data);
    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(other.m_data, other.m_serLoc);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

DataBuffer& DataBuffer::operator=(const DataBuffer& other) {
    if (this == &other) {
        return *this;
    }

    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(other.m_data, other.m_serLoc);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

FwSizeType DataBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

}  // namespace Drv

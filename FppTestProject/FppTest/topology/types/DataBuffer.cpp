// ======================================================================
// \title  DataBuffer.hpp
// \author tumbar
// \brief  cpp file for DataBuffer implementation class
// ======================================================================

#include "DataBuffer.hpp"

#include <cstring>
#include <iomanip>

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/StringTemplate.hpp"

namespace FppTest {
namespace FrameworkPortData {

DataBuffer::DataBuffer() : m_data{}, m_size{0} {}

DataBuffer::DataBuffer(const DataBuffer& buffer) : DataBuffer() {
    std::memcpy(this->m_data, buffer.m_data, sizeof(m_data));
    m_size = buffer.m_size;
}
DataBuffer::DataBuffer(const Fw::LinearBufferBase& buffer) {
    FW_ASSERT(buffer.getSize() <= sizeof(m_data), static_cast<FwAssertArgType>(buffer.getSize()), sizeof(m_data));
    std::memcpy(this->m_data, buffer.getBuffAddr(), buffer.getSize());
    m_size = buffer.getSize();
}

DataBuffer::DataBuffer(const U8* data, FwSizeType size) : DataBuffer() {
    FW_ASSERT(size < sizeof(m_data), static_cast<FwAssertArgType>(size), sizeof(m_data));
    std::memcpy(this->m_data, data, size);
    m_size = size;
}
bool DataBuffer::operator==(const DataBuffer& src) const {
    if (this->m_size != src.m_size) {
        return false;
    }

    const auto thisData = this->m_data;
    const auto srcData = src.m_data;
    for (FwSizeType i = 0; i < this->m_size; i++) {
        if (thisData[i] != srcData[i]) {
            return false;
        }
    }

    return true;
}
DataBuffer& DataBuffer::operator=(const DataBuffer& src) {
    std::memcpy(this->m_data, src.m_data, sizeof(m_data));
    m_size = src.m_size;
    return *this;
}

Fw::SerializeStatus DataBuffer::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    auto status = buffer.serializeFrom(m_size, mode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = buffer.serializeFrom(m_data, m_size);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}
Fw::SerializeStatus DataBuffer::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    auto status = buffer.deserializeTo(m_size, mode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = buffer.deserializeTo(m_data, m_size);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

#if FW_SERIALIZABLE_TO_STRING
void DataBuffer::toString(Fw::StringBase& text) const {
    static auto formatString = "data [%u]";
    text.format(formatString, m_size);

    Fw::StringTemplate<10> scratch;

    for (FwSizeType i = 0; i < m_size; i++) {
        if (i % 10 == 0) {
            text += "\n    ";
        }
        scratch.format(" 0x%02X", this->m_data[i]);
        text += scratch;
    }
    text += "\n";
}
#endif

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const DataBuffer& obj) {
    os << "[" << obj.m_size << "]";

    for (FwSizeType i = 0; i < obj.m_size; i++) {
        os << " 0x" << std::hex << static_cast<int>(obj.m_data[i]);
    }

    return os;
}
#endif
}  // namespace FrameworkPortData
}  // namespace FppTest

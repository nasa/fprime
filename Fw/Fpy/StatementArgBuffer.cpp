#include <Fw/Fpy/StatementArgBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringBase.hpp>

namespace Fw {

StatementArgBuffer::StatementArgBuffer(const U8* args, FwSizeType size)
    : Fw::LinearBufferBase(m_bufferData, sizeof(m_bufferData)) {
    SerializeStatus stat = LinearBufferBase::setBuff(args, size);
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

StatementArgBuffer::StatementArgBuffer() : Fw::LinearBufferBase(m_bufferData, sizeof(m_bufferData)) {}

StatementArgBuffer::~StatementArgBuffer() {}

// m_bufferData contents are copied via setBuff below
// cppcheck-suppress missingMemberCopy
StatementArgBuffer::StatementArgBuffer(const StatementArgBuffer& other)
    : Fw::LinearBufferBase(m_bufferData, sizeof(m_bufferData)) {
    SerializeStatus stat = LinearBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

StatementArgBuffer& StatementArgBuffer::operator=(const StatementArgBuffer& other) {
    if (this == &other) {
        return *this;
    }

    SerializeStatus stat = LinearBufferBase::setBuff(other.m_bufferData, other.getSize());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

Serializable::SizeType StatementArgBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

bool StatementArgBuffer::operator==(const StatementArgBuffer& other) const {
    if (this->getSize() != other.getSize()) {
        return false;
    }

    const U8* us = this->getBuffAddr();
    const U8* them = other.getBuffAddr();

    FW_ASSERT(us != nullptr);
    FW_ASSERT(them != nullptr);

    const Serializable::SizeType size = this->getSize();
    for (Serializable::SizeType byte = 0; byte < size; byte++) {
        if (us[byte] != them[byte]) {
            return false;
        }
    }

    return true;
}

#if FW_SERIALIZABLE_TO_STRING
void StatementArgBuffer::toString(Fw::StringBase& text) const {
    static const char* formatString = "(data = %p, size = %" PRI_FwSizeType ")";
    (void)text.format(formatString, &this->m_bufferData, this->getSize());  // display string may safely truncate
}
#endif
}  // namespace Fw

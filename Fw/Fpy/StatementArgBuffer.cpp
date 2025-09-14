#include <Fw/Fpy/StatementArgBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringBase.hpp>

namespace Fw {

StatementArgBuffer::StatementArgBuffer(const U8* args, FwSizeType size) {
    SerializeStatus stat = SerializeBufferBase::setBuff(args, size);
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

StatementArgBuffer::StatementArgBuffer() {}

StatementArgBuffer::~StatementArgBuffer() {}

StatementArgBuffer::StatementArgBuffer(const StatementArgBuffer& other) : Fw::SerializeBufferBase() {
    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getBuffLength());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

StatementArgBuffer& StatementArgBuffer::operator=(const StatementArgBuffer& other) {
    if (this == &other) {
        return *this;
    }

    SerializeStatus stat = SerializeBufferBase::setBuff(other.m_bufferData, other.getBuffLength());
    FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    return *this;
}

Serializable::SizeType StatementArgBuffer::getBuffCapacity() const {
    return sizeof(this->m_bufferData);
}

const U8* StatementArgBuffer::getBuffAddr() const {
    return this->m_bufferData;
}

U8* StatementArgBuffer::getBuffAddr() {
    return this->m_bufferData;
}

bool StatementArgBuffer::operator==(const StatementArgBuffer& other) const {
    if (this->getBuffLength() != other.getBuffLength()) {
        return false;
    }

    const U8* us = this->getBuffAddr();
    const U8* them = other.getBuffAddr();

    FW_ASSERT(us);
    FW_ASSERT(them);

    for (Serializable::SizeType byte = 0; byte < this->getBuffLength(); byte++) {
        if (us[byte] != them[byte]) {
            return false;
        }
    }

    return true;
}

#if FW_SERIALIZABLE_TO_STRING
void StatementArgBuffer::toString(Fw::StringBase& text) const {
    static const char* formatString = "(data = %p, size = %" PRI_FwSizeType ")";
    text.format(formatString, &this->m_bufferData, this->getBuffLength());
}
#endif
}  // namespace Fw

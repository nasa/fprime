#include <Fw/Types/ObjectName.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

ObjectName::ObjectName(const CHAR* src) : StringBase() {
    (void)Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
}

ObjectName::ObjectName(const StringBase& src) : StringBase() {
    (void)Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
}

ObjectName::ObjectName(const ObjectName& src) : StringBase() {
    (void)Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
}

ObjectName::ObjectName() : StringBase() {
    this->m_buf[0] = 0;
}

ObjectName& ObjectName::operator=(const ObjectName& other) {
    if (this == &other) {
        return *this;
    }

    (void)Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
    return *this;
}

ObjectName& ObjectName::operator=(const StringBase& other) {
    if (this == &other) {
        return *this;
    }

    (void)Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
    return *this;
}

ObjectName& ObjectName::operator=(const CHAR* other) {
    (void)Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
    return *this;
}

ObjectName::~ObjectName() {}

const CHAR* ObjectName::toChar() const {
    return this->m_buf;
}

NATIVE_UINT_TYPE ObjectName::getCapacity() const {
    return STRING_SIZE;
}
}  // namespace Fw

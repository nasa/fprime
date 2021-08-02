#include <Fw/Types/FixedLengthString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    FixedLengthString::FixedLengthString(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString(const StringBase& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString(const FixedLengthString& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    FixedLengthString::FixedLengthString() : StringBase() {
        this->m_buf[0] = 0;
    }

    FixedLengthString& FixedLengthString::operator=(const FixedLengthString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    FixedLengthString& FixedLengthString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    FixedLengthString& FixedLengthString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    FixedLengthString::~FixedLengthString() {
    }

    const char* FixedLengthString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE FixedLengthString::getCapacity() const {
        return STRING_SIZE;
    }
}

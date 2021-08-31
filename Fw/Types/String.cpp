#include <Fw/Types/String.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    String::String(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    String::String(const StringBase& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String(const String& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String() : StringBase() {
        this->m_buf[0] = 0;
    }

    String& String::operator=(const String& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    String& String::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    String& String::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    String::~String() {
    }

    const char* String::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE String::getCapacity() const {
        return STRING_SIZE;
    }
}

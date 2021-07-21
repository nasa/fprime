#include <Fw/Test/String.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Test {

    String::String(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    String::String(const StringBase& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String(const String& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String(void) : StringBase() {
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

    String::~String(void) {
    }

    const char* String::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE String::getCapacity(void) const {
        return STRING_SIZE;
    }
}

#include <Fw/Test/String.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Test {

    String::String(const char* src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    String::String(const StringBase& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String(const String& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    String::String() : StringBase() {
        this->m_buf[0] = 0;
    }

    String& String::operator=(const String& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    String& String::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    String& String::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
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

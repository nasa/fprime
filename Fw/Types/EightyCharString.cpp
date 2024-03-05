#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    EightyCharString::EightyCharString(const char* src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const StringBase& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const EightyCharString& src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString() : StringBase() {
        this->m_buf[0] = 0;
    }

    EightyCharString& EightyCharString::operator=(const EightyCharString& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    EightyCharString& EightyCharString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    EightyCharString& EightyCharString::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    EightyCharString::~EightyCharString() {
    }

    const char* EightyCharString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE EightyCharString::getCapacity() const {
        return STRING_SIZE;
    }
}

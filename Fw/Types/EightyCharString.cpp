#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    EightyCharString::EightyCharString(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const StringBase& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(const EightyCharString& src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    EightyCharString::EightyCharString(void) : StringBase() {
        this->m_buf[0] = 0;
    }

    EightyCharString& EightyCharString::operator=(const EightyCharString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    EightyCharString& EightyCharString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    EightyCharString& EightyCharString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    EightyCharString::~EightyCharString(void) {
    }

    const char* EightyCharString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE EightyCharString::getCapacity(void) const {
        return STRING_SIZE;
    }
}

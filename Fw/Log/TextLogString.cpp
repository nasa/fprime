#include <Fw/Log/TextLogString.hpp>
#include <Fw/Types/StringUtils.hpp>
namespace Fw {

    TextLogString::TextLogString(const char* src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const StringBase& src): StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const TextLogString& src): StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TextLogString::TextLogString(): StringBase()  {
        this->m_buf[0] = 0;
    }

    TextLogString& TextLogString::operator=(const TextLogString& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TextLogString& TextLogString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TextLogString& TextLogString::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    TextLogString::~TextLogString() {
    }

    const char* TextLogString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE TextLogString::getCapacity() const {
        return FW_LOG_TEXT_BUFFER_SIZE;
    }
}

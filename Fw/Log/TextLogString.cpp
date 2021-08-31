#include <Fw/Log/TextLogString.hpp>
#include <Fw/Types/StringUtils.hpp>
namespace Fw {

    TextLogString::TextLogString(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const StringBase& src): StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TextLogString::TextLogString(const TextLogString& src): StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TextLogString::TextLogString(void): StringBase()  {
        this->m_buf[0] = 0;
    }

    TextLogString& TextLogString::operator=(const TextLogString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TextLogString& TextLogString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TextLogString& TextLogString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    TextLogString::~TextLogString(void) {
    }

    const char* TextLogString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE TextLogString::getCapacity(void) const {
        return FW_LOG_TEXT_BUFFER_SIZE;
    }
}

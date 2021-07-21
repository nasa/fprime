#include <Os/QueueString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Os {

    QueueString::QueueString(const char* src) : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    QueueString::QueueString(const StringBase& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    QueueString::QueueString(const QueueString& src)  : StringBase() {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    QueueString::QueueString(void) : StringBase()  {
        this->m_buf[0] = 0;
    }

    QueueString& QueueString::operator=(const QueueString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    QueueString& QueueString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    QueueString& QueueString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    QueueString::~QueueString(void) {
    }

    const char* QueueString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE QueueString::getCapacity(void) const {
        return FW_QUEUE_NAME_MAX_SIZE;
    }
}

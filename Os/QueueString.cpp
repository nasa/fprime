#include <Os/QueueString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Os {

    QueueString::QueueString(const char* src) : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    QueueString::QueueString(const StringBase& src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    QueueString::QueueString(const QueueString& src)  : StringBase() {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    QueueString::QueueString() : StringBase()  {
        this->m_buf[0] = 0;
    }

    QueueString& QueueString::operator=(const QueueString& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    QueueString& QueueString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    QueueString& QueueString::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    QueueString::~QueueString() {
    }

    const char* QueueString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE QueueString::getCapacity() const {
        return FW_QUEUE_NAME_MAX_SIZE;
    }
}

#include <Os/TaskString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Os {

    TaskString::TaskString(const char* src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    TaskString::TaskString(const StringBase& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TaskString::TaskString(const TaskString& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    TaskString::TaskString() {
        this->m_buf[0] = 0;
    }

    TaskString& TaskString::operator=(const TaskString& other) {
        if(this == &other) {
            return *this;
        }

        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TaskString& TaskString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TaskString& TaskString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    TaskString::~TaskString() {
    }

    const char* TaskString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE TaskString::getCapacity() const {
        return FW_TASK_NAME_MAX_SIZE;
    }
}

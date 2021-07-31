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

    TaskString::TaskString(void) {
        this->m_buf[0] = 0;
    }

    TaskString& TaskString::operator=(const TaskString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TaskString& TaskString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    TaskString& TaskString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    TaskString::~TaskString(void) {
    }

    const char* TaskString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE TaskString::getCapacity(void) const {
        return FW_TASK_NAME_MAX_SIZE;
    }
}

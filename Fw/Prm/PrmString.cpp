#include <Fw/Prm/PrmString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    ParamString::ParamString(const char* src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    ParamString::ParamString(const StringBase& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    ParamString::ParamString(const ParamString& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    ParamString::ParamString(void) : StringBase()  {
        this->m_buf[0] = 0;
    }

    ParamString& ParamString::operator=(const ParamString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    ParamString& ParamString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    ParamString& ParamString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    ParamString::~ParamString(void) {
    }

    const char* ParamString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE ParamString::getCapacity(void) const {
        return FW_PARAM_STRING_MAX_SIZE;
    }
}

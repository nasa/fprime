#include <Fw/Prm/PrmString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    ParamString::ParamString(const char* src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    ParamString::ParamString(const StringBase& src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    ParamString::ParamString(const ParamString& src) : StringBase()  {
        (void) Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    ParamString::ParamString() : StringBase()  {
        this->m_buf[0] = 0;
    }

    ParamString& ParamString::operator=(const ParamString& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    ParamString& ParamString::operator=(const StringBase& other) {
        if(this == &other) {
            return *this;
        }

        (void) Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    ParamString& ParamString::operator=(const char* other) {
        (void) Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    ParamString::~ParamString() {
    }

    const char* ParamString::toChar() const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE ParamString::getCapacity() const {
        return FW_PARAM_STRING_MAX_SIZE;
    }
}

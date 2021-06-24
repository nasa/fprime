#include <Fw/Types/InternalInterfaceString.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

    InternalInterfaceString::InternalInterfaceString(const char* src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src, sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString(const StringBase& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString(const InternalInterfaceString& src) : StringBase()  {
        Fw::StringUtils::string_copy(this->m_buf, src.toChar(), sizeof(this->m_buf));
    }

    InternalInterfaceString::InternalInterfaceString(void) : StringBase()  {
        this->m_buf[0] = 0;
    }

    InternalInterfaceString& InternalInterfaceString::operator=(const InternalInterfaceString& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    InternalInterfaceString& InternalInterfaceString::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(this->m_buf, other.toChar(), sizeof(this->m_buf));
        return *this;
    }

    InternalInterfaceString& InternalInterfaceString::operator=(const char* other) {
        Fw::StringUtils::string_copy(this->m_buf, other, sizeof(this->m_buf));
        return *this;
    }

    InternalInterfaceString::~InternalInterfaceString(void) {
    }

    const char* InternalInterfaceString::toChar(void) const {
        return this->m_buf;
    }

    NATIVE_UINT_TYPE InternalInterfaceString::getCapacity(void) const {
        return FW_INTERNAL_INTERFACE_STRING_MAX_SIZE;
    }
}

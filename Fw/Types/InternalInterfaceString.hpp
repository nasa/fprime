// ======================================================================
// @file   InternalInterfaceString.hpp
// @author F Prime
// @brief  A string sized for an internal port argument
// ======================================================================

#ifndef FW_INTERNAL_INTERFACE_STRING_HPP
#define FW_INTERNAL_INTERFACE_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

class InternalInterfaceString final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_INTERNAL_INTERFACE_STRING,
        STRING_SIZE = FW_INTERNAL_INTERFACE_STRING_MAX_SIZE,
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE)
    };

    InternalInterfaceString() : StringBase() { *this = ""; }

    explicit InternalInterfaceString(const InternalInterfaceString& src) : StringBase() { *this = src; }

    explicit InternalInterfaceString(const StringBase& src) : StringBase() { *this = src; }

    explicit InternalInterfaceString(const char* src) : StringBase() { *this = src; }

    ~InternalInterfaceString() {}

    InternalInterfaceString& operator=(const InternalInterfaceString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    InternalInterfaceString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    InternalInterfaceString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[BUFFER_SIZE(STRING_SIZE)];
};
}  // namespace Fw

#endif

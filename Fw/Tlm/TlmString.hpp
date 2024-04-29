// ======================================================================
// @file   TlmString.hpp
// @author F Prime
// @brief  A string sized for a telemetry channel
// ======================================================================

#ifndef FW_TLM_STRING_HPP
#define FW_TLM_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

class TlmString final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_TLM_STR,
        STRING_SIZE = FW_TLM_STRING_MAX_SIZE,
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType)
    };

    TlmString() : StringBase() { *this = ""; }

    TlmString(const TlmString& src) : StringBase() { *this = src; }

    TlmString(const StringBase& src) : StringBase() { *this = src; }

    TlmString(const char* src) : StringBase() { *this = src; }

    ~TlmString() {}

    TlmString& operator=(const TlmString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    TlmString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    TlmString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[TlmString::STRING_SIZE];
};
}  // namespace Fw

#endif

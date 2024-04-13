// ======================================================================
// @file   TlmString.hpp
// @author F Prime
// @brief  A string sized for a telemetry channel
// ======================================================================

#ifndef FW_TLM_STRING_TYPE_HPP
#define FW_TLM_STRING_TYPE_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class TlmString : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_TLM_STR,
        SERIALIZED_SIZE = FW_TLM_STRING_MAX_SIZE + sizeof(FwSizeStoreType)
    };

    //!< zero-argument constructor
    TlmString() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const TlmString& constructor
    TlmString(const TlmString& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    TlmString(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    TlmString(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! Operator= (const String&)
    TlmString& operator=(const TlmString& other) {
        static_cast<StringBase*>(this)->operator=(other);
        return *this;
    }

    //! destructor
    ~TlmString() {}

  private:
    char m_buf[FW_TLM_STRING_MAX_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

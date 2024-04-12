// ======================================================================
// @file   LogString.hpp
// @author F Prime
// @brief  A string sized for an event log entry
// ======================================================================

#ifndef FW_LOG_STRING_TYPE_HPP
#define FW_LOG_STRING_TYPE_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class LogStringArg : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
        SERIALIZED_SIZE = FW_LOG_STRING_MAX_SIZE + sizeof(FwSizeStoreType)
    };

    //!< zero-argument constructor
    LogStringArg() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const LogStringArg& constructor
    LogStringArg(const LogStringArg& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    LogStringArg(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    LogStringArg(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~LogStringArg() {}

  private:
    char m_buf[FW_LOG_STRING_MAX_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

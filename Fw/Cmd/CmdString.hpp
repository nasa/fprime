// ======================================================================
// @file   CmdString.hpp
// @author F Prime
// @brief  A string sized for a command argument
// ======================================================================

#ifndef FW_CMD_STRING_TYPE_HPP
#define FW_CMD_STRING_TYPE_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class CmdStringArg : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_CMD_STR,
        SERIALIZED_SIZE = FW_CMD_STRING_MAX_SIZE + sizeof(FwSizeStoreType)
    };

    //!< zero-argument constructor
    CmdStringArg() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const CmdStringArg& constructor
    CmdStringArg(const CmdStringArg& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    CmdStringArg(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    CmdStringArg(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~CmdStringArg() {}

  private:
    char m_buf[FW_CMD_STRING_MAX_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

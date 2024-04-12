// ======================================================================
// @file   PrmString.hpp
// @author F Prime
// @brief  A string sized for a parameter
// ======================================================================

#ifndef FW_PARAM_STRING_HPP
#define FW_PARAM_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class ParamString : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_PRM_STR,
        SERIALIZED_SIZE = FW_PARAM_STRING_MAX_SIZE + sizeof(FwSizeStoreType)
    };

    //!< zero-argument constructor
    ParamString() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const ParamString& constructor
    ParamString(const ParamString& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    ParamString(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    ParamString(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~ParamString() {}

  private:
    char m_buf[FW_PARAM_STRING_MAX_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

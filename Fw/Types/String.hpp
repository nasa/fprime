// ======================================================================
// @file   String.hpp
// @author F Prime
// @brief  A general purpose string backed by a fixed-length buffer
// ======================================================================

#ifndef FW_STRING_HPP
#define FW_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class String : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_FIXED_LENGTH_STRING,     //!< typeid for string type
        STRING_SIZE = FW_FIXED_LENGTH_STRING_SIZE,              //!< storage for string
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwBuffSizeType)  //!< static serialized size is size of buffer + size field
    };

    //!< zero-argument constructor
    String() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const String& constructor
    String(const String& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    String(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    String(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~String() {}

  private:
    char m_buf[STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

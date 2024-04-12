// ======================================================================
// @file   Test/String.hpp
// @author F Prime
// @brief  A longer string for testing
// ======================================================================

#ifndef FW_TEST_STRING_HPP
#define FW_TEST_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Test {

class String : public ExternalString {
  public:
    enum {
        STRING_SIZE = 256,
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType)
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
    char m_buf[String::STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

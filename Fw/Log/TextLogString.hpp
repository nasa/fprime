// ======================================================================
// @file   TextLogString.hpp
// @author F Prime
// @brief  A string sized for an event log entry
// ======================================================================

#ifndef FW_TEXT_LOG_STRING_TYPE_HPP
#define FW_TEXT_LOG_STRING_TYPE_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class TextLogString : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
        SERIALIZED_SIZE = FW_LOG_TEXT_BUFFER_SIZE + sizeof(FwSizeStoreType)
    };

    //!< zero-argument constructor
    TextLogString() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const TextLogString& constructor
    TextLogString(const TextLogString& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    TextLogString(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    TextLogString(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! Operator= (const String&)
    TextLogString& operator=(const TextLogString& other) {
        static_cast<StringBase*>(this)->operator=(other);
        return *this;
    }

    //! destructor
    ~TextLogString() {}

  private:
    char m_buf[FW_LOG_TEXT_BUFFER_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

// ======================================================================
// @file   InternalInterfaceString.hpp
// @author F Prime
// @brief  A string sized for an internal port argument
// ======================================================================

#ifndef FW_INTERNAL_INTERFACE_STRING_HPP
#define FW_INTERNAL_INTERFACE_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

class InternalInterfaceString : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_INTERNAL_INTERFACE_STRING,  //!< type id for Fw::String
        STRING_SIZE = FW_INTERNAL_INTERFACE_STRING_MAX_SIZE,       //!< storage for string
        SERIALIZED_SIZE =
            STRING_SIZE + sizeof(FwSizeStoreType)  //!< static serialized size is size of buffer + size of size field
    };

    //! zero-argument constructor
    InternalInterfaceString() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const InternalInterfaceString& constructor
    InternalInterfaceString(const InternalInterfaceString& src)
        : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    InternalInterfaceString(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const char* source constructor
    InternalInterfaceString(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! Operator= (const String&)
    InternalInterfaceString& operator=(const InternalInterfaceString& other) {
        static_cast<StringBase*>(this)->operator=(other);
        return *this;
    }

    //! destructor
    ~InternalInterfaceString() {}

  private:
    char m_buf[InternalInterfaceString::STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

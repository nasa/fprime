// ======================================================================
// @file   String.hpp
// @author F Prime
// @brief  A string sized to store an object name
// ======================================================================

#ifndef FW_OBJECT_NAME_HPP
#define FW_OBJECT_NAME_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"

namespace Fw {

class ObjectName : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_OBJECT_NAME,  //!< typeid for Fw::ObjectName
        STRING_SIZE = FW_OBJ_NAME_MAX_SIZE,          //!< size of the buffer that stores the string
        SERIALIZED_SIZE =
            STRING_SIZE + sizeof(FwSizeStoreType)  //!< static serialized size is size of buffer + size of size field
    };

    //!< zero-argument constructor
    ObjectName() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const ObjectName& constructor
    ObjectName(const ObjectName& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    ObjectName(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    ObjectName(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~ObjectName() {}

  private:
    char m_buf[ObjectName::STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

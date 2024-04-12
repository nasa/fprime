// ======================================================================
// @file   FileNameString.hpp
// @author F Prime
// @brief  A string sized to store a file name
// ======================================================================

#ifndef FW_FILE_NAME_STRING_HPP
#define FW_FILE_NAME_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

class FileNameString : public ExternalString {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_FILE_NAME_STRING,  //!< type id for Fw::String
        STRING_SIZE = FileNameStringSize,                 //!< storage for string
        SERIALIZED_SIZE =
            STRING_SIZE + sizeof(FwSizeStoreType)  //!< static serialized size is size of buffer + size of size field
    };

    //!< zero-argument constructor
    FileNameString() : ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const FileNameString& constructor
    FileNameString(const FileNameString& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    FileNameString(const StringBase& src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    FileNameString(const char* src) : ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~FileNameString() {}

  private:
    char m_buf[FileNameString::STRING_SIZE];  //!< storage for string data
};
}  // namespace Fw

#endif

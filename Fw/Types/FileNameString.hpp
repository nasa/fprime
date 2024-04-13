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
        SERIALIZED_TYPE_ID = FW_TYPEID_FILE_NAME_STRING,
        SERIALIZED_SIZE = sizeof FileNameString::m_buf + sizeof(FwSizeStoreType)
    };

    FileNameString() : StringBase() { *this = ""; }

    FileNameString(const FileNameString& src) : StringBase() { *this = src; }

    FileNameString(const StringBase& src) : StringBase() { *this = src; }

    FileNameString(const char* src) : StringBase() { *this = src; }

    FileNameString& operator=(const FileNameString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    FileNameString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    FileNameString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[FileNameStringSize];
};
}  // namespace Fw

#endif

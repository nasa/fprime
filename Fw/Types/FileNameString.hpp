// ======================================================================
// @file   FileNameString.hpp
// @author F Prime
// @brief  A string sized to store a file name
// ======================================================================

#ifndef FW_FILE_NAME_STRING_HPP
#define FW_FILE_NAME_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

class FileNameString final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_FILE_NAME_STRING,
        STRING_SIZE = FileNameStringSize,
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE)
    };

    FileNameString() : StringBase() { *this = ""; }

    explicit FileNameString(const FileNameString& src) : StringBase() { *this = src; }

    explicit FileNameString(const StringBase& src) : StringBase() { *this = src; }

    explicit FileNameString(const char* src) : StringBase() { *this = src; }

    ~FileNameString() {}

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
    char m_buf[BUFFER_SIZE(STRING_SIZE)];
};
}  // namespace Fw

#endif

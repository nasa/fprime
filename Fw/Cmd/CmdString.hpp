// ======================================================================
// @file   CmdString.hpp
// @author F Prime
// @brief  A string sized for a command argument
// ======================================================================

#ifndef FW_CMD_STRING_HPP
#define FW_CMD_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

class CmdStringArg final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_CMD_STR,
        STRING_SIZE = FW_CMD_STRING_MAX_SIZE,
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE),
    };

    CmdStringArg() : StringBase() { *this = ""; }

    explicit CmdStringArg(const CmdStringArg& src) : StringBase() { *this = src; }

    explicit CmdStringArg(const StringBase& src) : StringBase() { *this = src; }

    explicit CmdStringArg(const char* src) : StringBase() { *this = src; }

    ~CmdStringArg() {}

    CmdStringArg& operator=(const CmdStringArg& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    CmdStringArg& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    CmdStringArg& operator=(const char* src) {
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

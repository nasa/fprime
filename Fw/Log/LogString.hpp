// ======================================================================
// @file   LogString.hpp
// @author F Prime
// @brief  A string sized for an event log entry
// ======================================================================

#ifndef FW_LOG_STRING_HPP
#define FW_LOG_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

class LogStringArg final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_LOG_STR,
        STRING_SIZE = FW_LOG_STRING_MAX_SIZE,
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType)
    };

    LogStringArg() : StringBase() { *this = ""; }

    LogStringArg(const LogStringArg& src) : StringBase() { *this = src; }

    LogStringArg(const StringBase& src) : StringBase() { *this = src; }

    LogStringArg(const char* src) : StringBase() { *this = src; }

    ~LogStringArg() {}

    LogStringArg& operator=(const LogStringArg& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    LogStringArg& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    LogStringArg& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[LogStringArg::STRING_SIZE];
};
}  // namespace Fw

#endif

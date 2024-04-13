// ======================================================================
// @file   PrmString.hpp
// @author F Prime
// @brief  A string sized for a parameter
// ======================================================================

#ifndef FW_PARAM_STRING_HPP
#define FW_PARAM_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

class ParamString : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_PRM_STR,
        STRING_SIZE = FW_PARAM_STRING_MAX_SIZE,
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType)
    };

    ParamString() : StringBase() { *this = ""; }

    ParamString(const ParamString& src) : StringBase() { *this = src; }

    ParamString(const StringBase& src) : StringBase() { *this = src; }

    ParamString(const char* src) : StringBase() { *this = src; }

    ~ParamString() {}

    ParamString& operator=(const ParamString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    ParamString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    ParamString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[ParamString::STRING_SIZE];
};
}  // namespace Fw

#endif

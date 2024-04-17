// ======================================================================
// @file   Test/String.hpp
// @author F Prime
// @brief  A longer string for testing
// ======================================================================

#ifndef FW_TEST_STRING_HPP
#define FW_TEST_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Test {

class String : public Fw::StringBase {
  public:
    enum {
        STRING_SIZE = 256,
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE),
    };

    String() : StringBase() { *this = ""; }

    String(const String& src) : StringBase() { *this = src; }

    String(const StringBase& src) : StringBase() { *this = src; }

    String(const char* src) : StringBase() { *this = src; }

    String& operator=(const String& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    String& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    String& operator=(const char* src) {
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

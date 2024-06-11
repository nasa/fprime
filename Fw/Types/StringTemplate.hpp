// ======================================================================
// @file   StringTemplate.hpp
// @author Rob Bocchino
// @brief  A string template parameterized by size
// ======================================================================

#ifndef FW_STRING_TEMPLATE_HPP
#define FW_STRING_TEMPLATE_HPP

#include <FpConfig.hpp>

#include "Fw/Types/StringBase.hpp"

namespace Fw {

template<Fw::StringBase::SizeType size> class StringTemplate final : public StringBase {
  public:
    enum {
        STRING_SIZE = size,
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE),
    };

    StringTemplate() : StringBase() { *this = ""; }

    explicit StringTemplate(const StringTemplate& src) : StringBase() { *this = src; }

    explicit StringTemplate(const StringBase& src) : StringBase() { *this = src; }

    explicit StringTemplate(const char* src) : StringBase() { *this = src; }

    ~StringTemplate() {}

    StringTemplate& operator=(const StringTemplate& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    StringTemplate& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    StringTemplate& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[BUFFER_SIZE(size)];
};
}  // namespace Fw

#endif

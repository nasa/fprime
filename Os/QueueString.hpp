// ======================================================================
// @file   QueueString.hpp
// @author F Prime
// @brief  A string sized for an OS queue name
// ======================================================================

#ifndef OS_QUEUE_STRING_HPP
#define OS_QUEUE_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/StringBase.hpp"

namespace Os {

class QueueString final : public Fw::StringBase {
  public:
    enum { STRING_SIZE = FW_QUEUE_NAME_BUFFER_SIZE, SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(STRING_SIZE) };

    QueueString() : StringBase() { *this = ""; }

    explicit QueueString(const QueueString& src) : StringBase() { *this = src; }

    explicit QueueString(const StringBase& src) : StringBase() { *this = src; }

    explicit QueueString(const char* src) : StringBase() { *this = src; }

    ~QueueString() {}

    QueueString& operator=(const QueueString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    QueueString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    QueueString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[BUFFER_SIZE(STRING_SIZE)];
};
}  // namespace Os

#endif

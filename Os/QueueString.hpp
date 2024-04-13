// ======================================================================
// @file   QueueString.hpp
// @author F Prime
// @brief  A string sized for an OS queue name
// ======================================================================

#ifndef OS_QUEUE_STRING_HPP
#define OS_QUEUE_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/ExternalString.hpp"

namespace Os {

class QueueString : public Fw::ExternalString {
  public:
    //!< zero-argument constructor
    QueueString() : Fw::ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const QueueString& constructor
    QueueString(const QueueString& src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    QueueString(const Fw::StringBase& src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    QueueString(const char* src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! Operator= (const String&)
    QueueString& operator=(const QueueString& other) {
        static_cast<StringBase*>(this)->operator=(other);
        return *this;
    }

    //! destructor
    ~QueueString() {}

  private:
    char m_buf[FW_QUEUE_NAME_MAX_SIZE];  //!< storage for string data
};
}  // namespace Os

#endif

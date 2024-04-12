// ======================================================================
// @file   TaskString.hpp
// @author F Prime
// @brief  A string sized for an OS task name
// ======================================================================

#ifndef OS_TASK_STRING_HPP
#define OS_TASK_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/ExternalString.hpp"

namespace Os {

class TaskString : public Fw::ExternalString {
  public:
    //!< zero-argument constructor
    TaskString() : Fw::ExternalString(this->m_buf, sizeof this->m_buf) {}

    //! const TaskString& constructor
    TaskString(const TaskString& src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! const StringBase& constructor
    TaskString(const Fw::StringBase& src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //!< const char* source constructor
    TaskString(const char* src) : Fw::ExternalString(this->m_buf, sizeof this->m_buf, src) {}

    //! destructor
    ~TaskString() {}

  private:
    char m_buf[FW_TASK_NAME_MAX_SIZE];  //!< storage for string data
};
}  // namespace Os

#endif

// ======================================================================
// @file   TaskString.hpp
// @author F Prime
// @brief  A string sized for an OS task name
// ======================================================================

#ifndef OS_TASK_STRING_HPP
#define OS_TASK_STRING_HPP

#include <FpConfig.hpp>

#include "Fw/Types/StringBase.hpp"

namespace Os {

class TaskString final : public Fw::StringBase {
  public:
    enum { STRING_SIZE = FW_TASK_NAME_MAX_SIZE, SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType) };

    TaskString() : StringBase() { *this = ""; }

    TaskString(const TaskString& src) : StringBase() { *this = src; }

    TaskString(const StringBase& src) : StringBase() { *this = src; }

    TaskString(const char* src) : StringBase() { *this = src; }

    ~TaskString() {}

    TaskString& operator=(const TaskString& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    TaskString& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    TaskString& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[TaskString::STRING_SIZE];
};
}  // namespace Os

#endif

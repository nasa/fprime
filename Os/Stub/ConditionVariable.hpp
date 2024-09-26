// ======================================================================
// \title Os/Stub/ConditionVariable.hpp
// \brief Stub definitions for Os::ConditionVariable
// ======================================================================
#ifndef OS_STUB_CONDITION_VARIABLE_HPP
#define OS_STUB_CONDITION_VARIABLE_HPP
#include <Os/Condition.hpp>

namespace Os {
namespace Stub {
namespace Mutex {

struct StubConditionVariableHandle : public ConditionVariableHandle {};

//! \brief Stub implementation of Os::ConditionVariable
//!
//! Stub implementation of `ConditionVariable` for use as a delegate class handling error-only file operations.
//!
class StubConditionVariable : public ConditionVariableInterface {
  public:
    //! \brief constructor
    //!
    StubConditionVariable() = default;

    //! \brief destructor
    //!
    ~StubConditionVariable() override = default;

    //! \brief wait releasing mutex
    void wait(Os::Mutex& mutex) override;

    //! \brief notify a single waiter
    void notify() override;

    //! \brief notify all current waiters
    void notifyAll() override;

    //! \brief get handle
    ConditionVariableHandle* getHandle() override;

  private:
    //! Handle for PosixMutex
    StubConditionVariableHandle m_handle;
};

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
#endif  // OS_STUB_CONDITION_VARIABLE_HPP

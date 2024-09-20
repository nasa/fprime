// ======================================================================
// \title Os/Posix/ConditionVariable.hpp
// \brief Posix definitions for Os::ConditionVariable
// ======================================================================
#ifndef OS_POSIX_CONDITION_VARIABLE_HPP
#define OS_POSIX_CONDITION_VARIABLE_HPP
#include <pthread.h>
#include <Os/Condition.hpp>

namespace Os {
namespace Posix {
namespace Mutex {

struct PosixConditionVariableHandle : public ConditionVariableHandle {
    pthread_cond_t m_condition; //! The condition variable
};

//! \brief Posix implementation of Os::Mutex
//!
//! Posix implementation of `MutexInterface` for use as a delegate class handling error-only file operations.
//!
class PosixConditionVariable : public ConditionVariableInterface {
  public:
    //! \brief constructor
    //!
    PosixConditionVariable();

    //! \brief destructor
    //!
    ~PosixConditionVariable() override;

    void wait(Os::Mutex& mutex) override;
    void notify() override;
    void notifyAll() override;

  private:
    //! Handle for PosixMutex
    PosixConditionVariableHandle m_handle;
};

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_MUTEX_HPP

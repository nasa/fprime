// ======================================================================
// \title Os/ConditionVariableInterface.hpp
// \brief Os::ConditionVariableHandle and Os::ConditionVariableInterface definitions
// ======================================================================
#ifndef OS_CONDITIONVARIABLEINTERFACE_HPP_
#define OS_CONDITIONVARIABLEINTERFACE_HPP_

#include <type_traits>

#include "Fw/Types/Assert.hpp"
#include "Os/Mutex.hpp"
#include "Os/Os.hpp"
#include "config/OsDelegateMutex.hpp"  // defines Os::ConditionVariable alias and OS_CONDITION_VARIABLE_HEADER consumed by Os/Condition.hpp; do not remove

namespace Os {

class DelegateMutex;
class DelegateConditionVariable;

// A condition variable implementation operates on the handle of the configured Os::Mutex. Selecting a
// compile-time Mutex while leaving Os::ConditionVariable on the link-time delegate (or vice versa) hands the
// CMake-chosen implementation a handle of the wrong type, so both must be delegates or neither may be.
static_assert(std::is_same<Os::Mutex, Os::DelegateMutex>::value ==
                  std::is_same<Os::ConditionVariable, Os::DelegateConditionVariable>::value,
              "Os::Mutex and Os::ConditionVariable must be overridden together in config/OsDelegateMutex.hpp");

//! \brief Condition variable handle parent
class ConditionVariableHandle {};

//! \brief interface for condition variables
//!
//! Condition variables allow a program to block on a condition while atomically releasing an Os::Mutex and atomically
//! reacquiring the mutex once the condition has been notified.
class ConditionVariableInterface {
  public:
    enum Status {
        OP_OK,                  //!<  Operation was successful
        ERROR_MUTEX_NOT_HELD,   //!< When trying to wait but we don't hold the mutex
        ERROR_DIFFERENT_MUTEX,  //!< When trying to use a different mutex than expected mutex
        ERROR_NOT_IMPLEMENTED,  //!< When trying to use a feature that isn't implemented
        NOT_SUPPORTED,          //!< ConditionVariable does not support operation
        ERROR_OTHER             //!< All other errors
    };

    //! Default constructor
    ConditionVariableInterface() = default;
    //! Default destructor
    virtual ~ConditionVariableInterface() = default;

    //! \brief copy constructor is forbidden
    ConditionVariableInterface(const ConditionVariableInterface& other) = delete;

    //! \brief assignment operator is forbidden
    virtual ConditionVariableInterface& operator=(const ConditionVariableInterface& other) = delete;

    //! \brief wait on a condition variable
    //!
    //! Wait on a condition variable. This function will atomically unlock the provided mutex and block on the condition
    //! in one step. Blocking will occur until a future `notify` or `notifyAll` call is made to this variable on another
    //! thread of execution.
    //!
    //! \param mutex: mutex to unlock as part of this operation
    //! \return status of the conditional wait
    virtual Status pend(Os::Mutex& mutex) = 0;

    //! \brief notify a single waiter on this condition variable
    //!
    //! Notify a single waiter on this condition variable. It is not necessary to hold the mutex supplied by the waiters
    //! and it is advantageous not to hold the lock to prevent immediate re-blocking.
    virtual void notify() = 0;

    //! \brief notify all waiters on this condition variable
    //!
    //! Notify all waiters on this condition variable. It is not necessary to hold the mutex supplied by the waiters
    //! and it is advantageous not to hold the lock to prevent immediate re-blocking.
    virtual void notifyAll() = 0;

    //! \brief return the underlying condition variable handle (implementation specific).
    //! \return internal task handle representation
    virtual ConditionVariableHandle* getHandle() = 0;

    //! \brief provide a pointer to a ConditionVariable delegate object
    //!
    //! \note ERROR_DIFFERENT_MUTEX is reported by the link-time Os::DelegateConditionVariable, which records the mutex
    //! first supplied to pend(); an implementation selected directly as Os::ConditionVariable does not perform that
    //! check.
    static ConditionVariableInterface* getDelegate(ConditionVariableHandleStorage& aligned_new_memory);

    // ------------------------------------------------------------------
    // Common (non-virtual) functions built on top of pend(). Defined inline
    // on the interface so they are available regardless of which
    // implementation Os::ConditionVariable is configured to be.
    // ------------------------------------------------------------------

    //! \brief wait on a condition variable and assert success
    //!
    //! Wait on a condition variable. This function will atomically unlock the provided mutex and block on the condition
    //! in one step. Blocking will occur until a future `notify` or `notifyAll` call is made to this variable on another
    //! thread of execution.
    //!
    //! \warning it is invalid to supply a mutex different from those supplied by others
    //! \warning conditions *must* be rechecked after the condition variable unlocks
    //! \warning the mutex must be locked by the calling task
    //!
    //! \param mutex: mutex to unlock as part of this operation
    void wait(Os::Mutex& mutex) {
        const Status status = this->pend(mutex);
        FW_ASSERT(status == Status::OP_OK,
                  static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
    }
};

}  // namespace Os

#endif  // OS_CONDITIONVARIABLEINTERFACE_HPP_

// ======================================================================
// \title Os/Condition.hpp
// \brief common function definitions for Os::ConditionVariables
// ======================================================================
#include "Os/Mutex.hpp"
#include "Os/Os.hpp"

#ifndef OS_CONDITION_HPP_
#define OS_CONDITION_HPP_

namespace Os {

//! \brief Condition variable handle parent
class ConditionVariableHandle {};

//! \brief interface for condition variables
//!
//! Condition variables allow a program to block on a condition while atomically releasing an Os::Mutex and atomically
//! reacquiring the mutex once the condition has been notified.
class ConditionVariableInterface {
  public:
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
    virtual void wait(Os::Mutex& mutex) = 0;

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

    //! \brief provide a pointer to a Mutex delegate object
    static ConditionVariableInterface* getDelegate(ConditionVariableHandleStorage& aligned_new_memory);
};

//! \brief condition variable implementation
//!
//! Condition variables allow a program to block on a condition while atomically releasing an Os::Mutex and atomically
//! reacquiring the mutex once the condition has been notified.
class ConditionVariable final : public ConditionVariableInterface {
  public:
    //! \brief default constructor
    ConditionVariable();

    //! \brief default virtual destructor
    ~ConditionVariable() final;

    //! \brief copy constructor is forbidden
    ConditionVariable(const ConditionVariableInterface& other) = delete;

    //! \brief copy constructor is forbidden
    ConditionVariable(const ConditionVariableInterface* other) = delete;

    //! \brief assignment operator is forbidden
    ConditionVariableInterface& operator=(const ConditionVariableInterface& other) override = delete;

    //! \brief wait on a condition variable
    //!
    //! Wait on a condition variable. This function will atomically unlock the provided mutex and block on the condition
    //! in one step. Blocking will occur until a future `notify` or `notifyAll` call is made to this variable on another
    //! thread of execution. This function delegates to the underlying implementation.
    //!
    //! \warning it is invalid to supply a mutex different from those supplied by others
    //! \warning conditions *must* be rechecked after the condition variable unlocks
    //! \note unlocked mutexes will be locked before waiting and will be relocked before this function returns
    //!
    //! \param mutex: mutex to unlock as part of this operation
    void wait(Os::Mutex& mutex) override;

    //! \brief notify a single waiter on this condition variable
    //!
    //! Notify a single waiter on this condition variable. It is not necessary to hold the mutex supplied by the waiters
    //! and it is advantageous not to hold the lock to prevent immediate re-blocking. This function delegates to the
    //! underlying implementation.
    void notify() override;

    //! \brief notify all waiters on this condition variable
    //!
    //! Notify all waiters on this condition variable. It is not necessary to hold the mutex supplied by the waiters
    //! and it is advantageous not to hold the lock to prevent immediate re-blocking. This function delegates to the
    //! underlying implementation.
    void notifyAll() override;

    //! \brief return the underlying condition variable handle (implementation specific). Delegates to implementation.
    //! \return internal task handle representation
    ConditionVariableHandle* getHandle() override;

  private:
    //! Pointer to mutex object previously used
    Os::Mutex* m_lock = nullptr;

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) ConditionVariableHandleStorage m_handle_storage; //!< Storage for aligned FileHandle data
    ConditionVariableInterface& m_delegate;                                       //!< Delegate for the real implementation
};
}
#endif //OS_CONDITION_HPP_

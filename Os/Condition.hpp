#include "Os/Mutex.hpp"
#include "Os/Os.hpp"

#ifndef OS_CONDITION_HPP_
#define OS_CONDITION_HPP_

namespace Os {

class ConditionVariableHandle {};

class ConditionVariableInterface {
  public:
    ConditionVariableInterface() = default;
    virtual ~ConditionVariableInterface() = default;

    virtual void wait(Os::Mutex& mutex) = 0;
    virtual void notify() = 0;
    virtual void notifyAll() = 0;

    //! \brief provide a pointer to a Mutex delegate object
    static ConditionVariableInterface* getDelegate(ConditionVariableHandleStorage& aligned_new_memory);
};

class ConditionVariable final : public ConditionVariableInterface {
  public:
    //! \brief default constructor
    ConditionVariable();

    //! \brief default virtual destructor
    ~ConditionVariable() final;

    //! \brief copy constructor is forbidden
    ConditionVariable(const ConditionVariable& other) = delete;

    //! \brief assignment operator is forbidden
    ConditionVariable& operator=(const ConditionVariable& other) = delete;

    void wait(Os::Mutex& mutex);
    void notify();
    void notifyAll();

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< Storage for aligned FileHandle data
    ConditionVariableInterface& m_delegate;                       //!< Delegate for the real implementation
};
}

#endif //OS_CONDITION_HPP_
// ======================================================================
// \title Os/MutexInterface.hpp
// \brief Os::MutexHandle, Os::MutexInterface, and Os::ScopeLock definitions
// ======================================================================
#ifndef OS_MUTEXINTERFACE_HPP_
#define OS_MUTEXINTERFACE_HPP_

#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Os.hpp>
#include "config/OsDelegateMutex.hpp"

namespace Os {

struct MutexHandle {};

class MutexInterface {
  public:
    enum Status {
        OP_OK,           //!< Operation was successful
        ERROR_BUSY,      //!< Mutex is busy
        ERROR_DEADLOCK,  //!< Deadlock condition detected
        NOT_SUPPORTED,   //!< Mutex does not support operation
        ERROR_OTHER      //!< All other errors
    };

    //! \brief default constructor
    MutexInterface() = default;

    //! \brief default virtual destructor
    virtual ~MutexInterface() = default;

    //! \brief copy constructor is forbidden
    MutexInterface(const MutexInterface& other) = delete;

    //! \brief copy constructor is forbidden
    MutexInterface(const MutexInterface* other) = delete;

    //! \brief assignment operator is forbidden
    MutexInterface& operator=(const MutexInterface& other) = delete;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    virtual MutexHandle* getHandle() = 0;

    //! \brief provide a pointer to a Mutex delegate object
    static MutexInterface* getDelegate(MutexHandleStorage& aligned_new_memory);

    // ------------------------------------------------------------------
    // Mutex operations to be implemented by an OSAL implementation
    // ------------------------------------------------------------------

    virtual Status take() = 0;     //!<  lock the mutex return status
    virtual Status release() = 0;  //!<  unlock the mutex return status

    // ------------------------------------------------------------------
    // Common (non-virtual) functions built on top of take()/release().
    // Located on the interface (per fprime#5249) so they are available
    // regardless of which implementation Os::Mutex is configured to be.
    // Implemented in DelegateMutex.cpp to keep Mutex code in one TU.
    // ------------------------------------------------------------------

    void lock();                       //!<  lock the mutex and assert success
    void unLock();                     //!<  unlock the mutex and assert success
    void unlock() { this->unLock(); }  //!<  alias for unLock to meet BasicLockable requirements
};

//! \brief locks a mutex within the current scope
//!
//! The scope lock will lock the associated mutex immediately and will ensure the mutex is unlock when the scope lock
//! is destroyed.
//!
//! \warning it is unadvisable to dynamically allocate ScopeLock as this violates the implied usage.
class ScopeLock {
  public:
    //! \brief construct the scope lock
    //!
    //! Will lock the supplied mutex and will unlock the mutex when this object goes out of scope.
    //! \param mutex
    explicit ScopeLock(MutexInterface& mutex);

    //!\brief unlock the scoped mutex
    ~ScopeLock();

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock& other) = delete;

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock* other) = delete;

    //! \brief assignment operator is forbidden
    ScopeLock& operator=(const ScopeLock& other) = delete;

  private:
    MutexInterface& m_mutex;  //!< Stores the mutex reference
};

}  // namespace Os

#endif  // OS_MUTEXINTERFACE_HPP_

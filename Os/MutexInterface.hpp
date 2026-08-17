// ======================================================================
// \title Os/MutexInterface.hpp
// \brief Os::MutexHandle and Os::MutexInterface definitions
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

    virtual Status take() = 0;     //!<  lock the mutex return status
    virtual Status release() = 0;  //!<  unlock the mutex return status

    // ------------------------------------------------------------------
    // Common virtual functions built on top of OS-specific functions
    // ------------------------------------------------------------------

    virtual void lock();               //!<  lock the mutex and assert success
    virtual void unLock();             //!<  unlock the mutex and assert success
    void unlock() { this->unLock(); }  //!<  alias for unLock to meet BasicLockable requirements
};
}  // namespace Os

#endif  // OS_MUTEXINTERFACE_HPP_

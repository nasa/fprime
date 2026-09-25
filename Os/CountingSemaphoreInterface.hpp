// ======================================================================
// \title Os/CountingSemaphoreInterface.hpp
// \brief Os::CountingSemaphoreHandle and Os::CountingSemaphoreInterface definitions
// ======================================================================
#ifndef OS_COUNTINGSEMAPHOREINTERFACE_HPP_
#define OS_COUNTINGSEMAPHOREINTERFACE_HPP_
#include "Fw/Time/TimeInterval.hpp"
#include "Os/Os.hpp"
#include "config/OsDelegateCountingSemaphore.hpp"

namespace Os {

class CountingSemaphoreHandle {};

class CountingSemaphoreInterface {
  public:
    enum Status {
        OP_OK,                  //!< Operation was successful
        ERROR_TIMEOUT,          //!< Timeout occurred during wait
        ERROR_INVALID,          //!< Invalid semaphore or argument
        ERROR_NOT_IMPLEMENTED,  //!< Feature not implemented
        NOT_SUPPORTED,          //!< CountingSemaphore does not support operation
        ERROR_OTHER             //!< All other errors
    };

    //! \brief default constructor
    CountingSemaphoreInterface() = default;

    //! \brief default virtual destructor
    virtual ~CountingSemaphoreInterface() = default;

    //! \brief copy constructor is forbidden
    CountingSemaphoreInterface(const CountingSemaphoreInterface& other) = delete;

    //! \brief assignment operator is forbidden
    CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) = delete;

    //! \brief wait (decrement) the semaphore, blocking if count is zero
    //! \return status of the operation
    virtual Status wait() = 0;

    //! \brief wait on the semaphore with a timeout
    //! \param interval maximum time to wait
    //! \return OP_OK on success, ERROR_TIMEOUT if the interval elapsed
    virtual Status waitTimeout(const Fw::TimeInterval& interval) = 0;

    //! \brief non-blocking attempt to decrement the semaphore
    //! \return OP_OK if decremented, ERROR_TIMEOUT if count was zero
    virtual Status tryWait() = 0;

    //! \brief post (increment) the semaphore, potentially waking a waiting thread
    //! \return status of the operation
    virtual Status post() = 0;

    //! \brief return the underlying semaphore handle (implementation specific)
    //! \return internal semaphore handle representation
    virtual CountingSemaphoreHandle* getHandle() = 0;

    //! \brief provide a pointer to a CountingSemaphore delegate object
    static CountingSemaphoreInterface* getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                   U32 initial_count);
};
}  // namespace Os
#endif  // OS_COUNTINGSEMAPHOREINTERFACE_HPP_

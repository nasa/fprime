// ======================================================================
// \title Os/Mutex.hpp
// \brief common definitions for Os::Mutex
// ======================================================================
#ifndef Os_Mutex_hpp
#define Os_Mutex_hpp

#include <FpConfig.hpp>
#include <Os/Os.hpp>

namespace Os {

struct MutexHandle {};

class MutexInterface {
  public:
    enum Status { 
      OP_OK, //!<  Operation was successful
      ERROR_BUSY, //!<  Mutex is busy
      ERROR_DEADLOCK, //!< Deadlock condition detected
      ERROR_OTHER //!< All other errors
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
};

class Mutex final : public MutexInterface {
  public:
    Mutex();         //!<  Constructor. Mutex is unlocked when created
    ~Mutex() final;  //!<  Destructor

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;            //!<  lock the mutex and get return status
    Status release() override;         //!<  unlock the mutex and get return status
    void lock();                       //!<  lock the mutex and assert success
    void unLock();                     //!<  unlock the mutex and assert success
    void unlock() { this->unLock(); }  //!<  alias for unLock to meet BasicLockable requirements

  private:
    // This section is used to store the implementation-defined mutex handle. To Os::Mutex and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) MutexHandleStorage m_handle_storage;  //!< Mutex handle storage
    MutexInterface& m_delegate;                                        //!< Delegate for the real implementation
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
    explicit ScopeLock(Mutex& mutex);

    //!\brief unlock the scoped mutex
    ~ScopeLock();

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock& other) = delete;

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock* other) = delete;

    //! \brief assignment operator is forbidden
    ScopeLock& operator=(const ScopeLock& other) = delete;

  private:
    Mutex& m_mutex; //!< Stores the mutex reference
};
}  // namespace Os

#endif

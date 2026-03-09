// ======================================================================
// \title Os/RwMutex.hpp
// \brief common definitions for Os::RwMutex
// ======================================================================
#ifndef OS_RWMUTEX_HPP
#define OS_RWMUTEX_HPP

#include <Os/Mutex.hpp>

namespace Os {

class RwMutexInterface : public MutexInterface {
  public:
    //! \brief provide a pointer to a platform-specific RwMutex delegate object
    //! \param aligned_new_memory Pre-allocated storage with FW_HANDLE_ALIGNMENT
    //! \return Pointer to the constructed delegate implementation
    static RwMutexInterface* getDelegate(RwMutexHandleStorage& aligned_new_memory);

    //! \brief Acquire a shared (read) lock on the mutex
    //! \return Status::OP_OK on success, or error code (ERROR_BUSY, ERROR_DEADLOCK, etc.)
    virtual Status takeRead() = 0;

    //! \brief Release a shared (read) lock on the mutex
    //! \return Status::OP_OK on success, or error code
    virtual Status releaseRead() = 0;

    // Note: Inherited take() and release() provide exclusive (write) lock semantics.
    // Writers block all other readers and writers.
};

// RwMutex: RAII wrapper for reader-writer mutex (delegation pattern)
class RwMutex final : public RwMutexInterface {
  public:
    //! \brief Default constructor
    //!
    //! Constructs the mutex in an unlocked state. The underlying platform-specific
    //! implementation is initialized via the delegate pattern.
    RwMutex();

    //! \brief Destructor
    //!
    //! Releases system resources associated with the mutex.
    ~RwMutex() override;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return Pointer to the internal mutex handle representation
    MutexHandle* getHandle() override;

    // ------------------------------------
    // Exclusive (write) lock operations
    // ------------------------------------

    Status take() override;            //!<  Acquire an exclusive (write) lock and return status
    Status release() override;         //!<  Release an exclusive (write) lock and return status
    void lock();                       //!<  Acquire an exclusive (write) lock and assert success
    void unLock();                     //!<  Release an exclusive (write) lock and assert success
    void unlock() { this->unLock(); }  //!<  alias for unLock to meet BasicLockable requirements

    // ------------------------------------
    // Shared (read) lock operations
    // ------------------------------------

    Status takeRead() override;        //!<  Acquire a shared (read) lock and return status
    Status releaseRead() override;     //!<  Release a shared (read) lock and return status
    void lockRead();                   //!<  Acquire a shared (read) lock and assert success
    void unLockRead();                 //!<  Release a shared (read) lock and assert success

  private:
    // This section is used to store the implementation-defined reader-writer mutex handle. 
    // To Os::RwMutex (and Os::Mutex) and F' Prime, this type is opaque and thus normal
    // allocation cannot be done. Instead, we allow the implementor to store then 
    // handle in the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) RwMutexHandleStorage m_handle_storage; //!< Mutex handle storage
    RwMutexInterface& m_delegate;                                       //!< Delegate for the real implementation
};
//! \brief Acquires a shared (read) lock on a reader-writer mutex for the current scope
//!
//! This guard automatically acquires a read lock on the supplied RwMutex upon construction
//! and releases it upon destruction. Multiple ReadLock instances may hold the mutex
//! simultaneously for concurrent read access.
//!
//! \warning it is unadvisable to dynamically allocate ReadLock as this violates the implied usage.
class ReadLock {
  public:
    //! \brief Construct the guard and acquire a read lock
    //!
    //! Immediately attempts to acquire a shared (read) lock on the provided mutex.
    //! The lock will be released automatically when this object is destroyed.
    //! \param mutex The reader-writer mutex to lock for reading
    explicit ReadLock(RwMutex& mutex);

    //!\brief Destructor: releases the read lock
    ~ReadLock();

    //! \brief Copy constructor is forbidden
    ReadLock(const ReadLock& other) = delete;

    //! \brief Copy assignment operator is forbidden
    ReadLock& operator=(const ReadLock& other) = delete;

  private:
    RwMutex& m_mutex;  //!< Stores the reader-writer mutex reference
};
//! \brief Acquires an exclusive (write) lock on a reader-writer mutex for the current scope
//!
//! This guard class automatically acquires a write lock on the supplied RwMutex upon construction
//! and releases it upon destruction. Only one WriteLock instance may hold the mutex at a time;
//! it blocks all other readers and writers.
//!
//! \warning it is unadvisable to dynamically allocate WriteLock as this violates the implied usage.
class WriteLock {
  public:
    //! \brief construct the scope lock
    //!
    //! Immediately attempts to acquire an exclusive (write) lock on the provided mutex.
    //! The lock will be released automatically when this object is destroyed.
    //! \param mutex
    explicit WriteLock(RwMutex& mutex);

    //!\brief Destructor: releases the write lock
    ~WriteLock();

    //! \brief Copy constructor is forbidden
    WriteLock(const WriteLock& other) = delete;

    //! \brief Copy assignment operator is forbidden
    WriteLock& operator=(const WriteLock& other) = delete;

  private:
    RwMutex& m_mutex;  //!< Stores the reader-writer mutex reference
};

}  // namespace Os
#endif  // OS_RWMUTEX_HPP

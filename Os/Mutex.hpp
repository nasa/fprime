#ifndef _Mutex_hpp_
#define _Mutex_hpp_

#include <FpConfig.hpp>
#include <Os/Os.hpp>

namespace Os {

struct MutexHandle {};

class MutexInterface {
  public:
    //! \brief default constructor
    MutexInterface() = default;

    // Question: Why is this virtual?
    //! \brief default virtual destructor
    virtual ~MutexInterface() = default;

    //! \brief copy constructor is forbidden
    MutexInterface(const MutexInterface& other) = delete;

    //! \brief assignment operator is forbidden
    MutexInterface& operator=(const MutexInterface& other) = delete;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    virtual MutexHandle* getHandle() = 0;

    //! \brief provide a pointer to a Mutex delegate object
    static MutexInterface* getDelegate(HandleStorage& aligned_new_memory);  // TODO

    virtual void lock() = 0;    //!<  lock the mutex
    virtual void unLock() = 0;  //!<  unlock the mutex
};

class Mutex final : public MutexInterface {
  public:
    Mutex();           //!<  Constructor. Mutex is unlocked when created
    ~Mutex() final;  //!<  Destructor


    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    void lock() override;              //!<  lock the mutex
    void unLock() override;            //!<  unlock the mutex
    void unlock() { this->unLock(); }  //!<  alias for unLock to meet BasicLockable requirements

  private:
    // POINTER_CAST m_handle;  //!<  Stored handle to mutex

    // This section is used to store the implementation-defined mutex handle. To Os::Mutex and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< Mutex handle storage
    MutexInterface& m_delegate;  //!< Delegate for the real implementation

};
}  // namespace Os

#endif

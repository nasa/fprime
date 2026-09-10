// ======================================================================
// \title Os/DelegateMutex.hpp
// \brief Define the Os::DelegateMutex class
// ======================================================================
#ifndef OS_DELEGATEMUTEX_HPP_
#define OS_DELEGATEMUTEX_HPP_

#include "Os/MutexInterface.hpp"

namespace Os {

//! \brief Link-time delegating Mutex implementation.
//!
//! Stores an implementation-defined mutex handle in a byte array and forwards all operations to a
//! delegate constructed (via placement-new) by MutexInterface::getDelegate(). Which getDelegate()
//! is linked selects the concrete implementation at link time. This is the default binding of the
//! Os::Mutex alias; platforms may instead alias Os::Mutex directly to a concrete implementation for
//! compile-time selection (see config/OsDelegateMutex.hpp).
class DelegateMutex final : public MutexInterface {
  public:
    DelegateMutex();         //!<  Constructor. Mutex is unlocked when created
    ~DelegateMutex() final;  //!<  Destructor

    //! \brief copy constructor is forbidden
    DelegateMutex(const DelegateMutex& other) = delete;

    //! \brief assignment operator is forbidden
    DelegateMutex& operator=(const DelegateMutex& other) = delete;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;     //!<  lock the mutex and get return status
    Status release() override;  //!<  unlock the mutex and get return status

  private:
    // This section is used to store the implementation-defined mutex handle. To Os::Mutex and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store the handle in
    // the byte-array here and set `m_delegate` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) MutexHandleStorage m_handle_storage;  //!< Mutex handle storage
    MutexInterface& m_delegate;                                        //!< Delegate for the real implementation
};

}  // namespace Os

#endif  // OS_DELEGATEMUTEX_HPP_

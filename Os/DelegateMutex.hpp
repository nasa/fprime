// ======================================================================
// \title Os/DelegateMutex.hpp
// \brief Define the Os::DelegateMutex class
// ======================================================================
#ifndef OS_DELEGATEMUTEX_HPP_
#define OS_DELEGATEMUTEX_HPP_

#include "Os/MutexInterface.hpp"

namespace Os {

class DelegateMutex final : public MutexInterface {
  public:
    DelegateMutex();         //!<  Constructor. Mutex is unlocked when created
    ~DelegateMutex() final;  //!<  Destructor

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;     //!<  lock the mutex and get return status
    Status release() override;  //!<  unlock the mutex and get return status
    void lock() override;       //!<  lock the mutex and assert success
    void unLock() override;     //!<  unlock the mutex and assert success

  private:
    // This section is used to store the implementation-defined mutex handle. To Os::Mutex and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) MutexHandleStorage m_handle_storage;  //!< Mutex handle storage
    MutexInterface& m_delegate;                                        //!< Delegate for the real implementation
};
}  // namespace Os

#endif  // OS_DELEGATEMUTEX_HPP_

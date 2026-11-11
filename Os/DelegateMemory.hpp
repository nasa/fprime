// ======================================================================
// \title Os/DelegateMemory.hpp
// \brief defines a delegate-based implementation of MemoryInterface
// ======================================================================
#ifndef OS_DELEGATEMEMORY_HPP_
#define OS_DELEGATEMEMORY_HPP_

#include <Os/MemoryInterface.hpp>

namespace Os {

//! \brief memory delegate implementation
class DelegateMemory final : public MemoryInterface {
  public:
    //! \brief default constructor
    DelegateMemory();

    //! \brief default virtual destructor
    ~DelegateMemory() final;

    //! \brief copy constructor is forbidden
    DelegateMemory(const MemoryInterface& other) = delete;

    //! \brief copy constructor is forbidden
    DelegateMemory(const MemoryInterface* other) = delete;

    //! \brief assignment operator is forbidden
    MemoryInterface& operator=(const MemoryInterface& other) override = delete;

    //-----------------------------------------------------------------------------
    // Interface methods
    //-----------------------------------------------------------------------------
  public:
    //! \brief initialize the singleton
    static void init();

    //! \brief return singleton
    static DelegateMemory& getSingleton();

    //-----------------------------------------------------------------------------
    // Delegating methods
    //-----------------------------------------------------------------------------

    //! \brief get system memory usage
    //!
    //! This method delegates to the underlying implementation.
    //!
    //! \param memory_usage: (output) data structure used to store memory usage
    //! \return:  ERROR when error occurs, OK otherwise.
    Status _getUsage(Usage& memory_usage) override;

    //! \brief return the underlying memory handle (implementation specific).
    //! \return internal task handle representation
    MemoryHandle* getHandle() override;

  private:
    // This section is used to store the implementation-defined memory handle. To Os::Memory and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) MemoryHandleStorage m_handle_storage;  //!< Storage for aligned data
    MemoryInterface& m_delegate;                                        //!< Delegate for the real implementation
};
}  // namespace Os
#endif  // OS_DELEGATEMEMORY_HPP_

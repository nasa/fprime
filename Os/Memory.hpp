// ======================================================================
// \title Os/Memory.hpp
// \brief common function definitions for Os::Memory
// ======================================================================
#include "Os/Os.hpp"

#ifndef OS_MEMORY_HPP_
#define OS_MEMORY_HPP_

namespace Os {

//! \brief Memory variable handle parent
class MemoryHandle {};

//! \brief interface for memory implementation
class MemoryInterface {
  public:
    using Status = Os::Generic::Status;
    using Usage = Os::Generic::UsedTotal;

    //! Default constructor
    MemoryInterface() = default;
    //! Default destructor
    virtual ~MemoryInterface() = default;

    //! \brief copy constructor is forbidden
    MemoryInterface(const MemoryInterface& other) = delete;

    //! \brief assignment operator is forbidden
    virtual MemoryInterface& operator=(const MemoryInterface& other) = delete;

    //! \brief get system memory usage
    //!
    //! \param memory_usage: (output) data structure used to store memory usage
    //! \return: ERROR when error occurs, OK otherwise.
    virtual Status _getUsage(Usage& memory_usage) = 0;

    //! \brief return the underlying memory handle (implementation specific).
    //! \return internal task handle representation
    virtual MemoryHandle* getHandle() = 0;

    //! \brief provide a pointer to a Mutex delegate object
    static MemoryInterface* getDelegate(MemoryHandleStorage& aligned_new_memory);
};

//! \brief memory implementation
class Memory final : public MemoryInterface {
  public:
    //! \brief default constructor
    Memory();

    //! \brief default virtual destructor
    ~Memory() final;

    //! \brief copy constructor is forbidden
    Memory(const MemoryInterface& other) = delete;

    //! \brief copy constructor is forbidden
    Memory(const MemoryInterface* other) = delete;

    //! \brief assignment operator is forbidden
    MemoryInterface& operator=(const MemoryInterface& other) override = delete;

    //-----------------------------------------------------------------------------
    // Interface methods
    //-----------------------------------------------------------------------------
  public:

    //! \brief initialize the singleton
    void init();

    //! \brief return singleton
    static Memory& getSingleton();

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

    //-----------------------------------------------------------------------------
    // Static interface (singleton) methods
    //-----------------------------------------------------------------------------

    //! \brief get system memory usage
    //!
    //! This method wraps delegates to the underlying implementation.
    //!
    //! \param memory_usage: (output) data structure used to store memory usage
    //! \return: ERROR when error occurs, OK otherwise.
    static Status getUsage(Usage& memory);

  private:

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) MemoryHandleStorage m_handle_storage; //!< Storage for aligned data
    MemoryInterface& m_delegate;                                       //!< Delegate for the real implementation
};
}
#endif //OS_CONDITION_HPP_

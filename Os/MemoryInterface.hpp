// ======================================================================
// \title Os/MemoryInterface.hpp
// \brief Os::MemoryHandle and Os::MemoryInterface definitions
// ======================================================================
#ifndef OS_MEMORYINTERFACE_HPP_
#define OS_MEMORYINTERFACE_HPP_

#include "Os/Os.hpp"
#include "config/OsDelegateMemory.hpp"

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
    MemoryInterface(const MemoryInterface& other) = delete;  // NO_CODESONAR (cpp:S3657)

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

    //! \brief provide a pointer to a Memory delegate object
    static MemoryInterface* getDelegate(MemoryHandleStorage& aligned_new_memory);

    // ------------------------------------------------------------------
    // Convenience static wrappers
    // ------------------------------------------------------------------
    // Implemented in terms of the virtual instance function above, and inherited
    // (via ordinary class-scope lookup) by every concrete Os::Memory backend,
    // regardless of whether that backend is a link-time delegate or a direct
    // compile-time alias.

    //! \brief get system memory usage
    //!
    //! \param memory_usage: (output) data structure used to store memory usage
    //! \return: ERROR when error occurs, OK otherwise.
    static Status getUsage(Usage& memory);
};
}  // namespace Os
#endif  // OS_MEMORYINTERFACE_HPP_

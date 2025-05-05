// ======================================================================
// \title Os/Darwin/Memory.hpp
// \brief Darwin implementation for Os::Memory, header and test definitions
// ======================================================================
#include <Os/Memory.hpp>
#ifndef OS_Darwin_Memory_HPP
#define OS_Darwin_Memory_HPP

namespace Os {
namespace Darwin {
namespace Memory {

//! MemoryHandle class definition for stub implementations.
//!
struct DarwinMemoryHandle : public MemoryHandle {
};

//! \brief stub implementation of Os::MemoryInterface
//!
//! Darwin implementation of `MemoryInterface` for use as a delegate class handling stub console operations.
//!
class DarwinMemory : public MemoryInterface {
  public:
    //! \brief constructor
    //!
    DarwinMemory() = default;

    //! \brief copy constructor
    DarwinMemory(const DarwinMemory& other) = delete;

    //! \brief default copy assignment
    MemoryInterface& operator=(const MemoryInterface& other) override = delete;

    //! \brief destructor
    //!
    ~DarwinMemory() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------
  public:

    //! \brief get system memory usage
    //!
    //! This method delegates to the underlying implementation.
    //!
    //! \param memory_usage: (output) data structure used to store memory usage
    //! \return:  ERROR when error occurs, OK otherwise.
    Status _getUsage(Os::Memory::Usage& memory_usage) override;

    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    MemoryHandle *getHandle() override;
  private:
    //! File handle for PosixFile
    DarwinMemoryHandle m_handle;
};
} // namespace Memory
} // namespace Darwin
} // namespace Os

#endif // OS_Darwin_Memory_HPP

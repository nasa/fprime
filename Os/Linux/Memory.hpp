// ======================================================================
// \title Os/Linux/Memory.hpp
// \brief Linux implementation for Os::Memory, header and test definitions
// ======================================================================
#include <Os/Memory.hpp>
#ifndef OS_Linux_Memory_HPP
#define OS_Linux_Memory_HPP

namespace Os {
namespace Linux {
namespace Memory {

//! MemoryHandle class definition for stub implementations.
//!
struct LinuxMemoryHandle : public MemoryHandle {
};

//! \brief stub implementation of Os::MemoryInterface
//!
//! Linux implementation of `MemoryInterface` for use as a delegate class handling stub console operations.
//!
class LinuxMemory : public MemoryInterface {
  public:
    //! \brief constructor
    //!
    LinuxMemory() = default;

    //! \brief copy constructor
    LinuxMemory(const LinuxMemory& other) = delete;

    //! \brief default copy assignment
    MemoryInterface& operator=(const MemoryInterface& other) override = delete;

    //! \brief destructor
    //!
    ~LinuxMemory() override = default;

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
    LinuxMemoryHandle m_handle;
};
} // namespace Memory
} // namespace Linux
} // namespace Os

#endif // OS_Linux_Memory_HPP

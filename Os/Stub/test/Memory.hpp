// ======================================================================
// \title Os/Stub/test/Memory.hpp
// \brief test stub implementation for Os::Memory, header and test definitions
// ======================================================================
#include <cstdio>
#include <Os/Memory.hpp>
#ifndef OS_Stub_Test_Memory_HPP
#define OS_Stub_Test_Memory_HPP

namespace Os {
namespace Stub {
namespace Memory {
namespace Test {

class TestMemory;

//! Data that supports the stubbed File implementation.
//!/
struct StaticData {
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        USAGE_FN,
        HANDLE_FN
    };
    //! Last function called
    LastFn lastCalled = NONE_FN;

    //! Count test
    FwSizeType count = 0;
    //! Count test
    FwSizeType index = 0;

    //! Ticks test
    Os::Memory::Usage usage;

    //! Status out
    MemoryInterface::Status status_out;

    // Singleton data
    static StaticData data;
};

//! MemoryHandle class definition for stub implementations.
//!
struct TestMemoryHandle : public MemoryHandle {
};

//! \brief stub implementation of Os::MemoryInterface
//!
//! Stub implementation of `MemoryInterface` for use as a delegate class handling stub console operations.
//!
class TestMemory : public MemoryInterface {
  public:
    //! \brief constructor
    //!
    TestMemory();

    //! \brief copy constructor
    TestMemory(const TestMemory& other) = delete;

    //! \brief assignment operator that copies the internal representation
    MemoryInterface& operator=(const MemoryInterface& other) override = delete;

    //! \brief destructor
    //!
    ~TestMemory() override;

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
    MemoryHandle* getHandle() override;

  private:
    //! File handle for PosixFile
    TestMemoryHandle m_handle;
};
} // namespace Test
} // namespace Memory
} // namespace Stub
} // namespace Os

#endif // OS_Stub_Test_Memory_HPP

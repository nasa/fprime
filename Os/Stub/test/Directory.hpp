// ======================================================================
// \title Os/Stub/test/Directory.cpp
// \brief definitions for TestDirectory stubs for interface testing
// ======================================================================
#ifndef OS_STUB_Directory_TEST_HPP
#define OS_STUB_Directory_TEST_HPP

#include "Os/Directory.hpp"

namespace Os {
namespace Stub {
namespace Directory {
namespace Test {

//! Data that supports the stubbed Directory implementation.
//!/
struct StaticData {
    //! Enumeration of last function called
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        OPEN_FN,
        REWIND_FN,
        READ_FN,
        CLOSE_FN,
        GET_HANDLE_FN,
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;

    // Singleton data
    static StaticData data;
};

//! Test task handle
class TestDirectoryHandle : public DirectoryHandle {};

//! Implementation of task
class TestDirectory : public DirectoryInterface {
  public:
    //! Constructor
    TestDirectory();

    //! Destructor
    ~TestDirectory() override;


    // ------------------------------------------------------------
    // Implementation-specific Directory member functions
    // ------------------------------------------------------------
    Status open(const char* path, OpenMode mode) override;
    Status rewind() override;
    Status read(char * fileNameBuffer, FwSizeType bufSize) override;
    void close() override;

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal task handle representation
    DirectoryHandle* getHandle() override;

};

}
}
}
}
#endif // End OS_STUB_MUTEX_TEST_HPP

// ======================================================================
// \title Os/Stub/test/FileSystem.cpp
// \brief definitions for TestFileSystem stubs for interface testing
//
// These classes are here to test that delegation of Os::FileSystem calls
// the selected implementation of a FileSystemInterface.
// ======================================================================
#ifndef OS_STUB_FILESYSTEM_TEST_HPP
#define OS_STUB_FILESYSTEM_TEST_HPP

#include "Os/FileSystem.hpp"

namespace Os {
namespace Stub {
namespace FileSystem {
namespace Test {

//! Data that supports the stubbed FileSystem implementation.
//!/
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        CREATE_DIR_FN,
        REMOVE_DIR_FN,
        REMOVE_FILE_FN,
        RENAME_FN,
        GET_CWD_FN,
        CHANGE_CWD_FN,
        GET_FREESPACE_FN,
        GET_HANDLE_FN,
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;

    Os::FileSystem::Status lastStatus = Os::FileSystem::Status::OP_OK;

    // Singleton data
    static StaticData data;
};

//! Test task handle
class TestFileSystemHandle : public FileSystemHandle {};

//! Implementation of task
class TestFileSystem : public FileSystemInterface {
  public:
    //! Constructor
    TestFileSystem();

    //! Destructor
    ~TestFileSystem() override;

    Status _removeDirectory(const char* path) override;
    Status _removeFile(const char* path) override;
    Status _rename(const char* originPath, const char* destPath) override;
    Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;
    Status _changeWorkingDirectory(const char* path) override;
    Status _getWorkingDirectory(char* path, FwSizeType size) override;

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal task handle representation
    FileSystemHandle* getHandle() override;

};

}
}
}
}
#endif // End OS_STUB_MUTEX_TEST_HPP

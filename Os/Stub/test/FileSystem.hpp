// ======================================================================
// \title Os/Stub/test/FileSystem.cpp
// \brief definitions for TestFileSystem stubs for interface testing
// ======================================================================
#include "Os/FileSystem.hpp"

#ifndef OS_STUB_FileSystem_TEST_HPP
#define OS_STUB_FileSystem_TEST_HPP
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
        READ_DIR_FN,
        REMOVE_FILE_FN,
        MOVE_FILE_FN,
        COPY_FILE_FN,
        APPEND_FILE_FN,
        GET_SIZE_FN,
        GET_COUNT_FN,
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

    Status _createDirectory(const char* path) override;
    Status _removeDirectory(const char* path) override;
    Status _removeFile(const char* path) override;
    Status _moveFile(const char* originPath, const char* destPath) override;
    Status _copyFile(const char* originPath, const char* destPath) override;
    Status _appendFile(const char* originPath, const char* destPath, bool createMissingDest=false) override;
    Status _getFileSize(const char* path, FwSignedSizeType& size) override;
    Status _getFileCount(const char* directory, U32& fileCount) override;
    Status _changeWorkingDirectory(const char* path) override;
    Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal task handle representation
    FileSystemHandle* getHandle() override;

};

}
}
}
}
#endif // End OS_STUB_MUTEX_TEST_HPP

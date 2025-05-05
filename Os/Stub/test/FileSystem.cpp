// ======================================================================
// \title Os/Stub/test/FileSystem.cpp
// \brief implementation for TestFileSystem stubs for interface testing
// ======================================================================

#include "Os/Stub/test/FileSystem.hpp"
#include "Os/FileSystem.hpp"

namespace Os {
namespace Stub {
namespace FileSystem {
namespace Test {

StaticData StaticData::data;

TestFileSystem::TestFileSystem() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestFileSystem::~TestFileSystem() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

TestFileSystem::Status TestFileSystem::_removeDirectory(const char* path) {
    StaticData::data.lastCalled = StaticData::LastFn::REMOVE_DIR_FN;
    return Status::OP_OK;
}

TestFileSystem::Status TestFileSystem::_removeFile(const char* path) {
    StaticData::data.lastCalled = StaticData::LastFn::REMOVE_FILE_FN;
    return Status::OP_OK;
}

TestFileSystem::Status TestFileSystem::_rename(const char* originPath, const char* destPath) {
    StaticData::data.lastCalled = StaticData::LastFn::RENAME_FN;
    return Status::OP_OK;
}

TestFileSystem::Status TestFileSystem::_getWorkingDirectory(char* path, FwSizeType size) {
    StaticData::data.lastCalled = StaticData::LastFn::GET_CWD_FN;
    return Status::OP_OK;
}

TestFileSystem::Status TestFileSystem::_changeWorkingDirectory(const char* path) {
    StaticData::data.lastCalled = StaticData::LastFn::CHANGE_CWD_FN;
    return Status::OP_OK;
}

TestFileSystem::Status TestFileSystem::_getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    StaticData::data.lastCalled = StaticData::LastFn::GET_FREESPACE_FN;
    return Status::OP_OK;
}

Os::FileSystemHandle *TestFileSystem::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return nullptr;
}


}
}
}
}

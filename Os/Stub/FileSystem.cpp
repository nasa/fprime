// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include "Os/Stub/FileSystem.hpp"

namespace Os {
namespace Stub {
namespace FileSystem {

StubFileSystem::Status StubFileSystem::_removeDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_removeFile(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_rename(const char* originPath, const char* destPath) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_changeWorkingDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return Status::NOT_SUPPORTED;
}

FileSystemHandle* StubFileSystem::getHandle() {
    return &this->m_handle;
}

} // namespace File
} // namespace Stub
} // namespace Os

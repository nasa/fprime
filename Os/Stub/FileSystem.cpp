// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include "Os/Stub/FileSystem.hpp"

namespace Os {
namespace Stub {
namespace FileSystem {


StubFileSystem::Status StubFileSystem::_createDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_removeDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_removeFile(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::_moveFile(const char* originPath, const char* destPath) {
    return Status::NOT_SUPPORTED;
}

// StubFileSystem::Status StubFileSystem::_copyFile(const char* originPath, const char* destPath) {
//     return Status::NOT_SUPPORTED;
// }

// StubFileSystem::Status StubFileSystem::_appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
//     return Status::NOT_SUPPORTED;
// }

StubFileSystem::Status StubFileSystem::_getFileSize(const char* path, FwSignedSizeType& size) {
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

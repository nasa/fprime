// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include "Os/Stub/FileSystem.hpp"

namespace Os {
namespace Stub {
namespace FileSystem {


StubFileSystem::Status StubFileSystem::createDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::removeDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::readDirectory(const char* path,  const U32 maxNum, Fw::FileNameString fileArray[], U32& numFiles) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::removeFile(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::moveFile(const char* originPath, const char* destPath) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::copyFile(const char* originPath, const char* destPath) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::getFileSize(const char* path, FwSignedSizeType& size) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::getFileCount(const char* directory, U32& fileCount) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::changeWorkingDirectory(const char* path) {
    return Status::NOT_SUPPORTED;
}

StubFileSystem::Status StubFileSystem::getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return Status::NOT_SUPPORTED;
}

FileSystemHandle* StubFileSystem::getHandle() {
    return &this->m_handle;
}

} // namespace File
} // namespace Stub
} // namespace Os

// ======================================================================
// \title Os/DelegateFileSystem.cpp
// \brief common function implementation for Os::FileSystem
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateFileSystem.hpp>
#include <Os/FileSystem.hpp>

namespace Os {

DelegateFileSystem::DelegateFileSystem()
    : m_handle_storage(), m_delegate(*FileSystemInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
}

DelegateFileSystem::~DelegateFileSystem() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    m_delegate.~FileSystemInterface();
}

FileSystemHandle* DelegateFileSystem::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateFileSystem::Status DelegateFileSystem::_removeDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    return this->m_delegate._removeDirectory(path);
}

DelegateFileSystem::Status DelegateFileSystem::_removeFile(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    return this->m_delegate._removeFile(path);
}

DelegateFileSystem::Status DelegateFileSystem::_rename(const char* sourcePath, const char* destPath) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(sourcePath != nullptr);
    FW_ASSERT(destPath != nullptr);
    return this->m_delegate._rename(sourcePath, destPath);
}

DelegateFileSystem::Status DelegateFileSystem::_getPathType(const char* path, PathType& pathType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    return this->m_delegate._getPathType(path, pathType);
}

DelegateFileSystem::Status DelegateFileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    FW_ASSERT(bufferSize > 0);  // because bufferSize=0 would trigger a malloc in some implementations (e.g. Posix)
    return this->m_delegate._getWorkingDirectory(path, bufferSize);
}

DelegateFileSystem::Status DelegateFileSystem::_changeWorkingDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    return this->m_delegate._changeWorkingDirectory(path);
}

DelegateFileSystem::Status DelegateFileSystem::_getFreeSpace(const char* path,
                                                             FwSizeType& totalBytes,
                                                             FwSizeType& freeBytes) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    return this->m_delegate._getFreeSpace(path, totalBytes, freeBytes);
}

void DelegateFileSystem::init() {
    // Force trigger on the fly singleton setup
    (void)DelegateFileSystem::getSingleton();
}

DelegateFileSystem& DelegateFileSystem::getSingleton() {
    static DelegateFileSystem s_singleton;
    return s_singleton;
}

}  // namespace Os

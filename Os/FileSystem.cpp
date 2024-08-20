// ======================================================================
// \title Os/FileSystem.cpp
// \brief common function implementation for Os::FileSystem
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/FileSystem.hpp>

namespace Os {
FileSystem* FileSystem::s_singleton;


FileSystem::FileSystem() : m_handle_storage(), m_delegate(*FileSystemInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
}

FileSystem::~FileSystem() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    m_delegate.~FileSystemInterface();
}

FileSystemHandle* FileSystem::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

FileSystem::Status FileSystem::_createDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._createDirectory(path);
}

FileSystem::Status FileSystem::_removeDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._removeDirectory(path);
}

FileSystem::Status FileSystem::_readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._readDirectory(path, maxNum, fileArray, numFiles);
}

FileSystem::Status FileSystem::_removeFile(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._removeFile(path);
}

FileSystem::Status FileSystem::_moveFile(const char* originPath, const char* destPath) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._moveFile(originPath, destPath);
}

FileSystem::Status FileSystem::_copyFile(const char* originPath, const char* destPath) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._copyFile(originPath, destPath);
}

FileSystem::Status FileSystem::_appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._appendFile(originPath, destPath, createMissingDest);
}

FileSystem::Status FileSystem::_getFileSize(const char* path, FwSignedSizeType& size) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getFileSize(path, size);
}

FileSystem::Status FileSystem::_getFileCount(const char* directory, U32& fileCount) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getFileCount(directory, fileCount);
}

FileSystem::Status FileSystem::_changeWorkingDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._changeWorkingDirectory(path);
}

FileSystem::Status FileSystem::_getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getFreeSpace(path, totalBytes, freeBytes);
}

void FileSystem::init() {
    // Force trigger on the fly singleton setup
    (void) FileSystem::getSingleton();
}

FileSystem& FileSystem::getSingleton() {
    // On the fly construction of singleton
    if (FileSystem::s_singleton == nullptr) {
        FileSystem::s_singleton = new FileSystem();
    }
    return *FileSystem::s_singleton;
}

FileSystem::Status FileSystem::createDirectory(const char* path) {
    return FileSystem::getSingleton()._createDirectory(path);
}

FileSystem::Status FileSystem::removeDirectory(const char* path) {
    return FileSystem::getSingleton()._removeDirectory(path);
}
FileSystem::Status FileSystem::readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles) {
    return FileSystem::getSingleton()._readDirectory(path, maxNum, fileArray, numFiles);
}
FileSystem::Status FileSystem::removeFile(const char* path) {
    return FileSystem::getSingleton()._removeFile(path);
}
FileSystem::Status FileSystem::moveFile(const char* originPath, const char* destPath) {
    return FileSystem::getSingleton()._moveFile(originPath, destPath);
}
FileSystem::Status FileSystem::copyFile(const char* originPath, const char* destPath) {
    return FileSystem::getSingleton()._copyFile(originPath, destPath);
}
FileSystem::Status FileSystem::appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
    return FileSystem::getSingleton()._appendFile(originPath, destPath, createMissingDest);
}
FileSystem::Status FileSystem::getFileSize(const char* path, FwSignedSizeType& size) {
    return FileSystem::getSingleton()._getFileSize(path, size);
}
FileSystem::Status FileSystem::getFileCount(const char* directory, U32& fileCount) {
    return FileSystem::getSingleton()._getFileCount(directory, fileCount);
}
FileSystem::Status FileSystem::changeWorkingDirectory(const char* path) {
    return FileSystem::getSingleton()._changeWorkingDirectory(path);
}
FileSystem::Status FileSystem::getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return FileSystem::getSingleton()._getFreeSpace(path, totalBytes, freeBytes);
}


}  // namespace Os

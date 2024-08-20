// ======================================================================
// \title Os/FileSystem.cpp
// \brief common function implementation for Os::FileSystem
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/FileSystem.hpp>
#include <Os/Directory.hpp>

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

FileSystem::Status FileSystem::readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles) {
    Directory::Status dirStatus = Directory::Status::OP_OK;
    FileSystem::Status status = FileSystem::OP_OK;

    FW_ASSERT(fileArray != nullptr);
    FW_ASSERT(path != nullptr);

    Os::Directory directory;
    dirStatus = directory.open(path);

    // Open directory failed:
    if (dirStatus != Directory::Status::OP_OK) {
        return FileSystem::Status::OTHER_ERROR;
    }

    U32 arrayIdx = 0;
    U32 limitCount = 0;
    const U32 loopLimit = std::numeric_limits<U32>::max();

    char fileName[FPP_CONFIG_FILENAME_MAX_SIZE];

    // Read the directory contents and store in passed in array:
    while (arrayIdx < maxNum && limitCount < loopLimit) {
        ++limitCount;

        dirStatus = directory.read(fileName, FPP_CONFIG_FILENAME_MAX_SIZE);

        if (dirStatus == Directory::Status::NO_MORE_FILES) {
            break;
        }
        if (dirStatus != Directory::Status::OP_OK) {
            return FileSystem::OTHER_ERROR;
        }

        FW_ASSERT(arrayIdx < maxNum, static_cast<NATIVE_INT_TYPE>(arrayIdx),
                    static_cast<NATIVE_INT_TYPE>(maxNum));

        fileArray[arrayIdx++] = Fw::String(fileName);
    }

    if (limitCount == loopLimit) {
        status = FileSystem::Status::FILE_LIMIT;
    }

    directory.close();

    numFiles = arrayIdx;

    return status;

}

}  // namespace Os

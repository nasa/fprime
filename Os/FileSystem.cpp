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

FileSystem::Status FileSystem::_removeDirectory(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._removeDirectory(path);
}

FileSystem::Status FileSystem::_removeFile(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._removeFile(path);
}

FileSystem::Status FileSystem::_rename(const char* sourcePath, const char* destPath) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._rename(sourcePath, destPath);
}

FileSystem::Status FileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getWorkingDirectory(path, bufferSize);
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


// ------------------------------------------------------------
// Static functions calling implementation-specific operations
// ------------------------------------------------------------

FileSystem::Status FileSystem::removeDirectory(const char* path) {
    return FileSystem::getSingleton()._removeDirectory(path);
}

FileSystem::Status FileSystem::removeFile(const char* path) {
    return FileSystem::getSingleton()._removeFile(path);
}

FileSystem::Status FileSystem::rename(const char* sourcePath, const char* destPath) {
    return FileSystem::getSingleton()._rename(sourcePath, destPath);
}

FileSystem::Status FileSystem::getWorkingDirectory(char* path, FwSizeType bufferSize) {
    return FileSystem::getSingleton()._getWorkingDirectory(path, bufferSize);
}

FileSystem::Status FileSystem::changeWorkingDirectory(const char* path) {
    return FileSystem::getSingleton()._changeWorkingDirectory(path);
}

FileSystem::Status FileSystem::getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return FileSystem::getSingleton()._getFreeSpace(path, totalBytes, freeBytes);
}


// ------------------------------------------------------------
// Additional functions built on top of OS-specific operations
// ------------------------------------------------------------

FileSystem::Status FileSystem::createDirectory(const char* path, bool errorIfAlreadyExists) {
    Status status = Status::OP_OK;
    Os::Directory dir;
    // If errorIfAlreadyExists is true, use CREATE_EXCLUSIVE mode, otherwise use CREATE_IF_MISSING
    Directory::OpenMode mode = errorIfAlreadyExists ? Directory::OpenMode::CREATE_EXCLUSIVE : Directory::OpenMode::CREATE_IF_MISSING;
    Directory::Status dirStatus = dir.open(path, mode);
    if (dirStatus != Directory::OP_OK) {
        return FileSystem::handleDirectoryError(dirStatus);
    }
    dir.close();
    return status;
}

FileSystem::Status FileSystem::touch(const char* path) {
    Status status = Status::OP_OK;
    Os::File file;
    File::Status file_status = file.open(path, Os::File::OPEN_WRITE);
    if (file_status != File::OP_OK) {
        status = FileSystem::handleFileError(file_status);
    }
    file.close();
    return status;
}

bool FileSystem::exists(const char* path) {
    Os::File file;
    File::Status file_status = file.open(path, Os::File::OPEN_READ);
    if (file_status == File::OP_OK) {
        file.close();
        return true;
    }
    Os::Directory dir;
    Directory::Status dir_status = dir.open(path, Os::Directory::OpenMode::READ);
    if (dir_status == Directory::Status::OP_OK) {
        dir.close();
        return true;
    }
    return false;
} // end exists

FileSystem::Status FileSystem::copyFile(const char* sourcePath, const char* destPath) {
    Os::File source, destination;
    Os::File::Status fileStatus = source.open(sourcePath, Os::File::OPEN_READ);
    if (fileStatus != Os::File::OP_OK) {
        return FileSystem::handleFileError(fileStatus);
    }
    fileStatus = destination.open(destPath, Os::File::OPEN_WRITE);
    if (fileStatus != Os::File::OP_OK) {
        return FileSystem::handleFileError(fileStatus);
    }

    FwSignedSizeType sourceFileSize = 0;
    FileSystem::Status fs_status = FileSystem::getFileSize(sourcePath, sourceFileSize);
    if (fs_status != FileSystem::Status::OP_OK) {
        return fs_status;
    }

    fs_status = FileSystem::copyFileData(source, destination, sourceFileSize);

    return fs_status;
} // end copyFile

FileSystem::Status FileSystem::appendFile(const char* sourcePath, const char* destPath, bool createMissingDest) {
    Os::File source, destination;

    // If requested, check if destination file exists and exit if does not exist
    if (not createMissingDest) {
        if (not FileSystem::exists(destPath)) {
            return Status::DOESNT_EXIST;
        }
    }

    Os::File::Status fileStatus = source.open(sourcePath, Os::File::OPEN_READ);
    if (fileStatus != Os::File::OP_OK) {
        return FileSystem::handleFileError(fileStatus);
    }
    fileStatus = destination.open(destPath, Os::File::OPEN_APPEND);
    if (fileStatus != Os::File::OP_OK) {
        return FileSystem::handleFileError(fileStatus);
    }

    FileSystem::Status fs_status = FileSystem::OP_OK;

    FwSignedSizeType sourceFileSize = 0;
    fs_status = FileSystem::getFileSize(sourcePath, sourceFileSize);
    if (fs_status != FileSystem::Status::OP_OK) {
        return fs_status;
    }

    fs_status = FileSystem::copyFileData(source, destination, sourceFileSize);

    return fs_status;
} // end appendFile

FileSystem::Status FileSystem::moveFile(const char* source, const char* destination) {
    Status status = Status::OP_OK;

    // Try to simply rename the file
    status = FileSystem::rename(source, destination);

    // If rename fails because of cross-device rename, attempt to copy and remove instead
    if (status == Status::EXDEV_ERROR) {
        status = FileSystem::copyFile(source, destination);
        if (status != Status::OP_OK) {
            return status;
        }
        // REVIEW NOTE: what to do if removeFile fails?
        status = FileSystem::removeFile(source);
    }

    return status;
}

FileSystem::Status FileSystem::getFileSize(const char* path, FwSignedSizeType& size) {
    Os::File file;
    Os::File::Status status = file.open(path, Os::File::OPEN_READ);
    if (status != File::Status::OP_OK) {
        return FileSystem::handleFileError(status);
    }
    status = file.size(size);
    if (status != File::Status::OP_OK) {
        return FileSystem::handleFileError(status);
    }
    return FileSystem::OP_OK;
}


// ------------------------------------------------------------
// Internal helper functions
// ------------------------------------------------------------

FileSystem::Status FileSystem::handleFileError(File::Status fileStatus) {
    FileSystem::Status status = FileSystem::OTHER_ERROR;

    switch (fileStatus) {
        case File::NO_SPACE:
            status = FileSystem::NO_SPACE;
            break;
        case File::NO_PERMISSION:
            status = FileSystem::NO_PERMISSION;
            break;
        case File::DOESNT_EXIST:
            status = FileSystem::DOESNT_EXIST;
            break;
        default:
            status = FileSystem::OTHER_ERROR;
    }
    return status;
} // end handleFileError

FileSystem::Status FileSystem::handleDirectoryError(Directory::Status dirStatus) {
    FileSystem::Status status = FileSystem::OTHER_ERROR;

    switch (dirStatus) {
        case Directory::DOESNT_EXIST:
            status = FileSystem::DOESNT_EXIST;
            break;
        case Directory::NO_PERMISSION:
            status = FileSystem::NO_PERMISSION;
            break;
        case Directory::ALREADY_EXISTS:
            status = FileSystem::ALREADY_EXISTS;
            break;
        case Directory::NOT_SUPPORTED:
            status = FileSystem::NOT_SUPPORTED;
            break;
        default:
            status = FileSystem::OTHER_ERROR;
    }
    return status;
} // end handleFileError

FileSystem::Status FileSystem::copyFileData(File& source, File& destination, FwSignedSizeType size) {
    static_assert(FILE_SYSTEM_FILE_CHUNK_SIZE != 0, "FILE_SYSTEM_FILE_CHUNK_SIZE must be >0");
    U8 fileBuffer[FILE_SYSTEM_FILE_CHUNK_SIZE];
    File::Status file_status;

    FwSignedSizeType copiedSize = 0;
    FwSignedSizeType chunkSize = FILE_SYSTEM_FILE_CHUNK_SIZE;

    // Copy the file in chunks - loop until all data is copied
    for (copiedSize = 0; copiedSize < size; copiedSize += chunkSize) {
        // chunkSize is FILE_SYSTEM_FILE_CHUNK_SIZE unless size-copiedSize is less than that
        // in which case chunkSize is size-copiedSize, ensuring the last chunk reads the remaining data
        chunkSize = FW_MIN(FILE_SYSTEM_FILE_CHUNK_SIZE, size - copiedSize);
        file_status = source.read(fileBuffer, chunkSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return FileSystem::handleFileError(file_status);
        }
        file_status = destination.write(fileBuffer, chunkSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return FileSystem::handleFileError(file_status);
        }
    }

    return FileSystem::OP_OK;
}  // end copyFileData

}  // namespace Os

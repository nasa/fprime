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

FileSystem::Status FileSystem::_moveFile(const char* sourcePath, const char* destPath) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<FileSystemInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._moveFile(sourcePath, destPath);
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

FileSystem::Status FileSystem::removeDirectory(const char* path) {
    return FileSystem::getSingleton()._removeDirectory(path);
}

FileSystem::Status FileSystem::removeFile(const char* path) {
    return FileSystem::getSingleton()._removeFile(path);
}

FileSystem::Status FileSystem::moveFile(const char* sourcePath, const char* destPath) {
    return FileSystem::getSingleton()._moveFile(sourcePath, destPath);
}

FileSystem::Status FileSystem::changeWorkingDirectory(const char* path) {
    return FileSystem::getSingleton()._changeWorkingDirectory(path);
}

FileSystem::Status FileSystem::getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return FileSystem::getSingleton()._getFreeSpace(path, totalBytes, freeBytes);
}

FileSystem::Status FileSystem::createDirectory(const char* path) {
    Status status = Status::OP_OK;
    Os::Directory dir;
    Directory::Status dirStatus = dir.open(path, Os::Directory::OpenMode::CREATE);
    if (dirStatus != Directory::OP_OK) {
        return FileSystem::Status::INVALID_PATH; // TODO error handling
    }
    dir.close();
    return status;
}

FileSystem::Status FileSystem::touch(const char* path) {
    Status status = Status::OP_OK;
    Os::File file;
    // is this actually touching the file?
    // see https://chris-sharpe.blogspot.com/2013/05/better-than-systemtouch.html
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
    if (!createMissingDest) {
        if (!FileSystem::exists(destPath)) {
            return Status::INVALID_PATH;
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


// ######################################################################
// #################         Helper Functions           #################
// ######################################################################

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
            status = FileSystem::INVALID_PATH;
            break;
        default:
            status = FileSystem::OTHER_ERROR;
    }
    return status;
} // end handleFileError

FileSystem::Status FileSystem::copyFileData(File& source, File& destination, FwSignedSizeType size) {
    static_assert(FILE_SYSTEM_CHUNK_SIZE != 0, "FILE_SYSTEM_CHUNK_SIZE must be >0");
    U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];
    File::Status file_status;

    // Set loop limit
    const FwSignedSizeType copyLoopLimit = (size / FILE_SYSTEM_CHUNK_SIZE) + 2;

    FwSignedSizeType chunkSize;
    // REVIEW NOTE: implementation was changed from while to for loop
    // REVIEW NOTE 2: should assert source and destination are not the same file?
    for (FwIndexType i = 0; i < copyLoopLimit; i++) {
        chunkSize = FILE_SYSTEM_CHUNK_SIZE;
        file_status = source.read(fileBuffer, chunkSize, Os::File::WaitType::NO_WAIT);
        if (file_status != File::OP_OK) {
            return FileSystem::handleFileError(file_status);
        }

        if (chunkSize == 0) {
            // file has been successfully copied
            break;
        }

        file_status = destination.write(fileBuffer, chunkSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return FileSystem::handleFileError(file_status);
        }
    }

    return FileSystem::OP_OK;
}  // end copyFileData

}  // namespace Os

// ======================================================================
// \title Os/FileSystemInterface.cpp
// \brief common convenience-wrapper implementation for Os::FileSystem
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Directory.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <algorithm>

namespace Os {

namespace {

FileSystemInterface::Status handleFileError(File::Status fileStatus) {
    FileSystemInterface::Status status = FileSystemInterface::OTHER_ERROR;

    switch (fileStatus) {
        case File::NO_SPACE:
            status = FileSystemInterface::NO_SPACE;
            break;
        case File::NO_PERMISSION:
            status = FileSystemInterface::NO_PERMISSION;
            break;
        case File::DOESNT_EXIST:
            status = FileSystemInterface::DOESNT_EXIST;
            break;
        default:
            status = FileSystemInterface::OTHER_ERROR;
    }
    return status;
}  // end handleFileError

FileSystemInterface::Status handleDirectoryError(Directory::Status dirStatus) {
    FileSystemInterface::Status status = FileSystemInterface::OTHER_ERROR;

    switch (dirStatus) {
        case Directory::DOESNT_EXIST:
            status = FileSystemInterface::DOESNT_EXIST;
            break;
        case Directory::NO_PERMISSION:
            status = FileSystemInterface::NO_PERMISSION;
            break;
        case Directory::ALREADY_EXISTS:
            status = FileSystemInterface::ALREADY_EXISTS;
            break;
        case Directory::NOT_SUPPORTED:
            status = FileSystemInterface::NOT_SUPPORTED;
            break;
        default:
            status = FileSystemInterface::OTHER_ERROR;
    }
    return status;
}  // end handleDirectoryError

FileSystemInterface::Status copyFileData(File& source, File& destination, FwSizeType size) {
    static_assert(FileSystemInterface::FILE_SYSTEM_FILE_CHUNK_SIZE != 0, "FILE_SYSTEM_FILE_CHUNK_SIZE must be >0");
    U8 fileBuffer[FileSystemInterface::FILE_SYSTEM_FILE_CHUNK_SIZE];
    File::Status file_status;

    FwSizeType copiedSize = 0;
    FwSizeType chunkSize = FileSystemInterface::FILE_SYSTEM_FILE_CHUNK_SIZE;

    // Loop up to 2 times for each by, bounded to prevent infinite loop
    const FwSizeType maximum =
        (size > (std::numeric_limits<FwSizeType>::max() / 2)) ? std::numeric_limits<FwSizeType>::max() : size * 2;

    // Copy the file in chunks - loop until all data is copied
    FwSizeType i = 0;
    for (copiedSize = 0; (copiedSize < size) && (i < maximum); i++) {
        // chunkSize is FILE_SYSTEM_FILE_CHUNK_SIZE unless size-copiedSize is less than that
        // in which case chunkSize is size-copiedSize, ensuring the last chunk reads the remaining data
        // static_cast needed to avoid ODR linker issue with static constexpr passed by reference to std::min
        chunkSize =
            std::min(static_cast<FwSizeType>(FileSystemInterface::FILE_SYSTEM_FILE_CHUNK_SIZE), size - copiedSize);
        FwSizeType readSize = chunkSize;
        file_status = source.read(fileBuffer, readSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return handleFileError(file_status);
        }
        // A zero-byte read means the source ended before the expected size (e.g. truncated mid-copy)
        if (readSize == 0) {
            break;
        }
        FwSizeType writeSize = readSize;
        file_status = destination.write(fileBuffer, writeSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return handleFileError(file_status);
        }
        // A short write means the destination did not receive all the data (e.g. device full)
        if (writeSize != readSize) {
            return FileSystemInterface::OTHER_ERROR;
        }
        copiedSize += writeSize;
    }

    // A copy that did not transfer the full expected size must not report success
    if (copiedSize != size) {
        return FileSystemInterface::OTHER_ERROR;
    }

    return FileSystemInterface::OP_OK;
}  // end copyFileData

}  // namespace

// ------------------------------------------------------------
// Convenience static wrappers built directly on the virtual instance API
// ------------------------------------------------------------

FileSystemInterface::Status FileSystemInterface::removeDirectory(const char* path) {
    return Os::FileSystem::getSingleton()._removeDirectory(path);
}

FileSystemInterface::Status FileSystemInterface::removeFile(const char* path) {
    return Os::FileSystem::getSingleton()._removeFile(path);
}

FileSystemInterface::Status FileSystemInterface::rename(const char* sourcePath, const char* destPath) {
    return Os::FileSystem::getSingleton()._rename(sourcePath, destPath);
}

FileSystemInterface::Status FileSystemInterface::getWorkingDirectory(char* path, FwSizeType bufferSize) {
    return Os::FileSystem::getSingleton()._getWorkingDirectory(path, bufferSize);
}

FileSystemInterface::Status FileSystemInterface::changeWorkingDirectory(const char* path) {
    return Os::FileSystem::getSingleton()._changeWorkingDirectory(path);
}

FileSystemInterface::Status FileSystemInterface::getFreeSpace(const char* path,
                                                              FwSizeType& totalBytes,
                                                              FwSizeType& freeBytes) {
    return Os::FileSystem::getSingleton()._getFreeSpace(path, totalBytes, freeBytes);
}

// ------------------------------------------------------------
// Additional functions built on top of OS-specific operations
// ------------------------------------------------------------

FileSystemInterface::Status FileSystemInterface::createDirectory(const char* path, bool errorIfAlreadyExists) {
    FW_ASSERT(path != nullptr);
    Status status = Status::OP_OK;
    Os::Directory dir;
    // If errorIfAlreadyExists is true, use CREATE_EXCLUSIVE mode, otherwise use CREATE_IF_MISSING
    Directory::OpenMode mode =
        errorIfAlreadyExists ? Directory::OpenMode::CREATE_EXCLUSIVE : Directory::OpenMode::CREATE_IF_MISSING;
    Directory::Status dirStatus = dir.open(path, mode);
    dir.close();
    if (dirStatus != Directory::OP_OK) {
        return handleDirectoryError(dirStatus);
    }
    return status;
}

FileSystemInterface::Status FileSystemInterface::touch(const char* path) {
    FW_ASSERT(path != nullptr);
    Status status = Status::OP_OK;
    Os::File file;
    File::Status file_status = file.open(path, Os::File::OPEN_WRITE);
    file.close();
    if (file_status != File::OP_OK) {
        status = handleFileError(file_status);
    }
    return status;
}

FileSystemInterface::PathType FileSystemInterface::getPathType(const char* path) {
    FW_ASSERT(path != nullptr);
    PathType pathType;
    Status status = Os::FileSystem::getSingleton()._getPathType(path, pathType);
    if (status != Status::OP_OK) {
        return PathType::NOT_EXIST;
    }
    return pathType;
}  // end getPathType

bool FileSystemInterface::exists(const char* path) {
    return FileSystemInterface::getPathType(path) != PathType::NOT_EXIST;
}  // end exists

FileSystemInterface::Status FileSystemInterface::copyFile(const char* sourcePath, const char* destPath) {
    FW_ASSERT(sourcePath != nullptr);
    FW_ASSERT(destPath != nullptr);
    Os::File source;
    Os::File destination;
    Os::File::Status fileStatus = source.open(sourcePath, Os::File::OPEN_READ);
    if (fileStatus != Os::File::OP_OK) {
        return handleFileError(fileStatus);
    }
    fileStatus = destination.open(destPath, Os::File::OPEN_WRITE);
    if (fileStatus != Os::File::OP_OK) {
        return handleFileError(fileStatus);
    }

    FwSizeType sourceFileSize = 0;
    FileSystemInterface::Status fs_status = FileSystemInterface::getFileSize(sourcePath, sourceFileSize);
    if (fs_status != FileSystemInterface::Status::OP_OK) {
        return fs_status;
    }

    fs_status = copyFileData(source, destination, sourceFileSize);

    return fs_status;
}  // end copyFile

FileSystemInterface::Status FileSystemInterface::appendFile(const char* sourcePath,
                                                            const char* destPath,
                                                            bool createMissingDest) {
    Os::File source;
    Os::File destination;

    // If requested, check if destination file exists and exit if does not exist
    const bool destExists = FileSystemInterface::exists(destPath);
    if (not createMissingDest and not destExists) {
        return Status::DOESNT_EXIST;
    }

    Os::File::Status fileStatus = source.open(sourcePath, Os::File::OPEN_READ);
    if (fileStatus != Os::File::OP_OK) {
        return handleFileError(fileStatus);
    }
    fileStatus = destination.open(destPath, Os::File::OPEN_APPEND);
    if (fileStatus != Os::File::OP_OK) {
        return handleFileError(fileStatus);
    }

    FileSystemInterface::Status fs_status = FileSystemInterface::OP_OK;

    FwSizeType sourceFileSize = 0;
    fs_status = FileSystemInterface::getFileSize(sourcePath, sourceFileSize);
    if (fs_status != FileSystemInterface::Status::OP_OK) {
        return fs_status;
    }

    fs_status = copyFileData(source, destination, sourceFileSize);

    return fs_status;
}  // end appendFile

FileSystemInterface::Status FileSystemInterface::moveFile(const char* source, const char* destination) {
    Status status = Status::OP_OK;

    // Try to rename the file
    status = FileSystemInterface::rename(source, destination);

    // If rename fails because of cross-device rename, attempt to copy and remove instead
    if (status == Status::EXDEV_ERROR) {
        status = FileSystemInterface::copyFile(source, destination);
        if (status != Status::OP_OK) {
            return status;
        }
        status = FileSystemInterface::removeFile(source);
    }

    return status;
}

FileSystemInterface::Status FileSystemInterface::getFileSize(const char* path, FwSizeType& size) {
    Os::File file;
    Os::File::Status status = file.open(path, Os::File::OPEN_READ);
    if (status != File::Status::OP_OK) {
        return handleFileError(status);
    }
    status = file.size(size);
    if (status != File::Status::OP_OK) {
        return handleFileError(status);
    }
    return FileSystemInterface::OP_OK;
}

}  // namespace Os

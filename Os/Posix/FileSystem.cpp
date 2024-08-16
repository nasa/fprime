// ======================================================================
// \title Os/Posix/File.cpp
// \brief Posix implementation for Os::File
// ======================================================================
#include "Os/Posix/FileSystem.hpp"
#include "Os/Posix/error.hpp"

#include <dirent.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

#include <cerrno>

namespace Os {
namespace Posix {
namespace FileSystem {


PosixFileSystem::Status PosixFileSystem::createDirectory(const char* path) {
    Status status = OP_OK;
    if (::mkdir(path, S_IRWXU) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::removeDirectory(const char* path) {
    Status status = OP_OK;
    if (::rmdir(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::readDirectory(const char* path,  const U32 maxNum, Fw::FileNameString fileArray[], U32& numFiles) {
    Status dirStat = OP_OK;
    DIR* dirPtr = nullptr;
    struct dirent* direntData = nullptr;

    FW_ASSERT(fileArray != nullptr);
    FW_ASSERT(path != nullptr);

        // Open directory failed:
    if ((dirPtr = ::opendir(path)) == nullptr) {
        switch (errno) {
            case EACCES:
                dirStat = NO_PERMISSION;
                break;
            case ENOENT:
                dirStat = INVALID_PATH;
                break;
            case ENOTDIR:
                dirStat = NOT_DIR;
                break;
            default:
                dirStat = OTHER_ERROR;
                break;
        }
        return dirStat;
    }

    // Set errno to 0 so we know why we exited readdir
    errno = 0;
    U32 arrayIdx = 0;
    U32 limitCount = 0;
    const U32 loopLimit = std::numeric_limits<U32>::max();

    // Read the directory contents and store in passed in array:
    while (arrayIdx < maxNum && limitCount < loopLimit) {
        ++limitCount;

        if ((direntData = ::readdir(dirPtr)) != nullptr) {
            // We are only care about regular files
            if (direntData->d_type == DT_REG) {
                FW_ASSERT(arrayIdx < maxNum, static_cast<NATIVE_INT_TYPE>(arrayIdx),
                          static_cast<NATIVE_INT_TYPE>(maxNum));

                Fw::FileNameString str(direntData->d_name);
                fileArray[arrayIdx++] = str;
            }
        } else {
            // readdir failed, did it error or did we run out of files?
            if (errno != 0) {
                // Only error from readdir is EBADF
                dirStat = OTHER_ERROR;
            }
            break;
        }
    }

    if (limitCount == loopLimit) {
        dirStat = FILE_LIMIT;
    }

    if (::closedir(dirPtr) == -1) {
        // Only error from closedir is EBADF
        dirStat = OTHER_ERROR;
    }

    numFiles = arrayIdx;

    return dirStat;

}

PosixFileSystem::Status PosixFileSystem::removeFile(const char* path) {
    Status status = OP_OK;
    if (::unlink(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::moveFile(const char* originPath, const char* destPath) {
    Status status = OP_OK;
    if (::rename(originPath, destPath) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

//! \brief Helper namespace for file operations, local to Os::Posix::FileSystem
namespace FileOperationHelper {
PosixFileSystem::Status handleFileError(File::Status fileStatus) {
    PosixFileSystem::Status status = PosixFileSystem::OTHER_ERROR;

    switch (fileStatus) {
        case File::NO_SPACE:
            status = PosixFileSystem::NO_SPACE;
            break;
        case File::NO_PERMISSION:
            status = PosixFileSystem::NO_PERMISSION;
            break;
        case File::DOESNT_EXIST:
            status = PosixFileSystem::INVALID_PATH;
            break;
        default:
            status = PosixFileSystem::OTHER_ERROR;
    }
    return status;
} // end handleFileError

/**
 * A helper function that returns an "OP_OK" status if the given file
 * exists and can be read from, otherwise returns an error status.
 *
 * If provided, will also initialize the given stat struct with file
 * information.
 */
PosixFileSystem::Status initAndCheckFileStats(const char* filePath, struct stat* fileInfo = nullptr) {
    PosixFileSystem::Status fs_status;
    struct stat local_info;
    if (!fileInfo) {
        // No external struct given, so use the local one
        fileInfo = &local_info;
    }
    if (::stat(filePath, fileInfo) == -1) {
        switch (errno) {
            case EACCES:
                fs_status = PosixFileSystem::NO_PERMISSION;
                break;
            case ELOOP:
            case ENOENT:
            case ENAMETOOLONG:
                fs_status = PosixFileSystem::INVALID_PATH;
                break;
            case ENOTDIR:
                fs_status = PosixFileSystem::NOT_DIR;
                break;
            default:
                fs_status = PosixFileSystem::OTHER_ERROR;
                break;
        }
        return fs_status;
    }
    // Make sure the origin is a regular file
    if (!S_ISREG(fileInfo->st_mode)) {
        return PosixFileSystem::INVALID_PATH;
    }
    return PosixFileSystem::OP_OK;
}

/**
 * A helper function that writes all the file information in the source
 * file to the destination file (replaces/appends to end/etc. depending
 * on destination file mode).
 *
 * Files must already be open and will remain open after this function
 * completes.
 *
 * @param source File to copy data from
 * @param destination File to copy data to
 * @param size The number of bytes to copy
 */
PosixFileSystem::Status copyFileData(File& source, File& destination, FwSignedSizeType size) {
    static_assert(FILE_SYSTEM_CHUNK_SIZE != 0, "FILE_SYSTEM_CHUNK_SIZE must be >0");
    U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];
    File::Status file_status;

    // Set loop limit
    const FwSignedSizeType copyLoopLimit = (size / FILE_SYSTEM_CHUNK_SIZE) + 2;

    FwSignedSizeType loopCounter = 0;
    FwSignedSizeType chunkSize;
    while (loopCounter < copyLoopLimit) {
        chunkSize = FILE_SYSTEM_CHUNK_SIZE;
        file_status = source.read(fileBuffer, chunkSize, Os::File::WaitType::NO_WAIT);
        if (file_status != File::OP_OK) {
            return handleFileError(file_status);
        }

        if (chunkSize == 0) {
            // file has been successfully copied
            break;
        }

        file_status = destination.write(fileBuffer, chunkSize, Os::File::WaitType::WAIT);
        if (file_status != File::OP_OK) {
            return handleFileError(file_status);
        }
        loopCounter++;
    }
    FW_ASSERT(loopCounter < copyLoopLimit);

    return PosixFileSystem::OP_OK;
}  // end copyFileData

}  // end FileOperationHelper


PosixFileSystem::Status PosixFileSystem::copyFile(const char* originPath, const char* destPath) {
    PosixFileSystem::Status fs_status;
    File::Status file_status;

    FwSignedSizeType fileSize = 0;

    File source;
    File destination;

    fs_status = FileOperationHelper::initAndCheckFileStats(originPath);
    if (PosixFileSystem::OP_OK != fs_status) {
        return fs_status;
    }

    // Get the file size:
    fs_status =
        PosixFileSystem::getFileSize(originPath, fileSize);  //!< gets the size of the file (in bytes) at location path
    if (PosixFileSystem::OP_OK != fs_status) {
        return fs_status;
    }

    file_status = source.open(originPath, File::OPEN_READ);
    if (file_status != File::OP_OK) {
        return FileOperationHelper::handleFileError(file_status);
    }

    file_status = destination.open(destPath, File::OPEN_WRITE);
    if (file_status != File::OP_OK) {
        return FileOperationHelper::handleFileError(file_status);
    }

    fs_status = FileOperationHelper::copyFileData(source, destination, fileSize);

    (void)source.close();
    (void)destination.close();

    return fs_status;
}

PosixFileSystem::Status PosixFileSystem::appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
    PosixFileSystem::Status fs_status;
    File::Status file_status;
    FwSignedSizeType fileSize = 0;

    File source;
    File destination;

    fs_status = FileOperationHelper::initAndCheckFileStats(originPath);
    if (PosixFileSystem::OP_OK != fs_status) {
        return fs_status;
    }

    // Get the file size (bytes)
    fs_status = PosixFileSystem::getFileSize(originPath, fileSize);
    if (PosixFileSystem::OP_OK != fs_status) {
        return fs_status;
    }

    file_status = source.open(originPath, File::OPEN_READ);
    if (file_status != File::OP_OK) {
        return FileOperationHelper::handleFileError(file_status);
    }

    // If needed, check if destination file exists (and exit if not)
    if (!createMissingDest) {
        fs_status = FileOperationHelper::initAndCheckFileStats(destPath);
        if (PosixFileSystem::OP_OK != fs_status) {
            return fs_status;
        }
    }

    file_status = destination.open(destPath, File::OPEN_APPEND);
    if (file_status != File::OP_OK) {
        return FileOperationHelper::handleFileError(file_status);
    }

    fs_status = FileOperationHelper::copyFileData(source, destination, fileSize);

    (void)source.close();
    (void)destination.close();

    return fs_status;
}

PosixFileSystem::Status PosixFileSystem::getFileSize(const char* path, FwSignedSizeType& size) {
    Status fileStat = OP_OK;
    struct stat fileStatStruct;

    fileStat = FileOperationHelper::initAndCheckFileStats(path, &fileStatStruct);
    if (PosixFileSystem::OP_OK == fileStat) {
        // Only check size if struct was initialized successfully
        size = fileStatStruct.st_size;
        if (static_cast<off_t>(size) != fileStatStruct.st_size) {
            return PosixFileSystem::OTHER_ERROR;
        }
    }
    return fileStat;
}

PosixFileSystem::Status PosixFileSystem::getFileCount(const char* directory, U32& fileCount) {
    Status status = OP_OK;
    DIR* dirPtr = nullptr;
    struct dirent* direntData = nullptr;
    U32 limitCount;
    const U32 loopLimit = std::numeric_limits<U32>::max();
    fileCount = 0;
    if ((dirPtr = ::opendir(directory)) == nullptr) {
        status = errno_to_filesystem_status(errno);
        return status;
    }
    // Set errno to 0 so we know why we exited readdir
    errno = 0;
    for (limitCount = 0; limitCount < loopLimit; limitCount++) {
        if ((direntData = ::readdir(dirPtr)) != nullptr) {
            // We are only counting regular files
            if (direntData->d_type == DT_REG) {
                fileCount++;
            }
        } else {
            // readdir failed, did it error or did we run out of files?
            if (errno != 0) {
                // Only error from readdir is EBADF
                status = Status::OTHER_ERROR;
            }
            break;
        }
    }
    if (limitCount == loopLimit) {
        status = Status::FILE_LIMIT;
    }
    if (::closedir(dirPtr) == -1) {
        // Only error from closedir is EBADF
        status = Status::OTHER_ERROR;
    }

    return status;
}

PosixFileSystem::Status PosixFileSystem::changeWorkingDirectory(const char* path) {
    Status status = OP_OK;
    if (::chdir(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    Status stat = OP_OK;

    struct statvfs fsStat;
    int ret = statvfs(path, &fsStat);
    if (ret) {
        return errno_to_filesystem_status(errno);
    }

    const FwSizeType block_size = static_cast<FwSizeType>(fsStat.f_frsize);
    const FwSizeType free_blocks = static_cast<FwSizeType>(fsStat.f_bfree);
    const FwSizeType total_blocks = static_cast<FwSizeType>(fsStat.f_blocks);

    // Check for casting and type error
    if (((block_size <= 0) || (static_cast<unsigned long>(block_size) != fsStat.f_frsize)) ||
        ((free_blocks <= 0) || (static_cast<fsblkcnt_t>(free_blocks) != fsStat.f_bfree)) ||
        ((total_blocks <= 0) || (static_cast<fsblkcnt_t>(block_size) != fsStat.f_blocks))) {
        return OTHER_ERROR;
    }
    // Check for overflow in multiplication
    if (free_blocks > (std::numeric_limits<FwSizeType>::max() / block_size) ||
        total_blocks > (std::numeric_limits<FwSizeType>::max() / block_size)) {
        return OTHER_ERROR;
    }
    freeBytes = free_blocks * block_size;
    totalBytes = total_blocks * block_size;
    return stat;
}

FileSystemHandle* PosixFileSystem::getHandle() {
    return &this->m_handle;
}

} // namespace File
} // namespace Posix
} // namespace Os

// ======================================================================
// \title Os/Posix/FileSystem.cpp
// \brief Posix implementation for Os::FileSystem
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


PosixFileSystem::Status PosixFileSystem::_removeDirectory(const char* path) {
    Status status = OP_OK;
    if (::rmdir(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::_removeFile(const char* path) {
    Status status = OP_OK;
    if (::unlink(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::_rename(const char* originPath, const char* destPath) {
    Status status = OP_OK;
    if (::rename(originPath, destPath) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::_getWorkingDirectory(char* path, FwSizeType bufferSize) {
    Status status = OP_OK;
    if (::getcwd(path, bufferSize) == nullptr) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::_changeWorkingDirectory(const char* path) {
    Status status = OP_OK;
    if (::chdir(path) == -1) {
        status = errno_to_filesystem_status(errno);
    }
    return status;
}

PosixFileSystem::Status PosixFileSystem::_getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    Status stat = OP_OK;
    static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<fsblkcnt_t>::max(), "FwSizeType must be able to hold fsblkcnt_t");
    static_assert(std::numeric_limits<FwSizeType>::min() <= std::numeric_limits<fsblkcnt_t>::min(), "FwSizeType must be able to hold fsblkcnt_t");
    static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<unsigned long>::max(), "FwSizeType must be able to hold unsigned long");
    struct statvfs fsStat;
    int ret = statvfs(path, &fsStat);
    if (ret) {
        return errno_to_filesystem_status(errno);
    }

    const FwSizeType block_size = static_cast<FwSizeType>(fsStat.f_frsize);
    const FwSizeType free_blocks = static_cast<FwSizeType>(fsStat.f_bfree);
    const FwSizeType total_blocks = static_cast<FwSizeType>(fsStat.f_blocks);

    // Check for overflow in multiplication
    if (free_blocks > (std::numeric_limits<FwSizeType>::max() / block_size) ||
        total_blocks > (std::numeric_limits<FwSizeType>::max() / block_size)) {
        return OVERFLOW_ERROR;
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

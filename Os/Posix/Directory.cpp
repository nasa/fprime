// ======================================================================
// \title Os/Posix/Directory.cpp
// \brief Posix implementation for Os::Directory
// ======================================================================
#include <sys/errno.h>
#include <sys/stat.h>
#include <cstring>

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/Posix/Directory.hpp>
#include <Os/Posix/error.hpp>

namespace Os {
namespace Posix {
namespace Directory {

PosixDirectory::PosixDirectory() : Os::DirectoryInterface(), m_handle() {}

DirectoryHandle* PosixDirectory::getHandle() {
    return &this->m_handle;
}

PosixDirectory::Status PosixDirectory::open(const char* path, OpenMode mode) {
    Status status = Status::OP_OK;

    // If one of the CREATE mode, attempt to create the directory
    if (mode == OpenMode::CREATE_EXCLUSIVE || mode == OpenMode::CREATE_IF_MISSING) {
        if (::mkdir(path, S_IRWXU) == -1) {
            status = errno_to_directory_status(errno);
            // If error is not ALREADY_EXISTS, return the error
            // If any error and mode CREATE_EXCLUSIVE, return the error
            // Else, we keep going with OP_OK
            if (status != Status::ALREADY_EXISTS || mode == OpenMode::CREATE_EXCLUSIVE) {
                return status;
            } else {
                status = Status::OP_OK;
            }
        }
    }

    DIR* dir = ::opendir(path);

    if (dir == nullptr) {
        status = errno_to_directory_status(errno);
    }

    this->m_handle.m_dir_descriptor = dir;
    return status;
}

PosixDirectory::Status PosixDirectory::rewind() {
    Status status = Status::OP_OK;
    // no errors defined in man page for rewinddir
    ::rewinddir(this->m_handle.m_dir_descriptor);
    return status;
}

PosixDirectory::Status PosixDirectory::read(char* fileNameBuffer, FwSizeType bufSize) {
    FW_ASSERT(fileNameBuffer);

    Status status = Status::OP_OK;

    // Set errno to 0 so we know why we exited readdir
    // This is recommended by the manual pages (man 3 readdir)
    errno = 0;

    struct dirent* direntData = nullptr;
    while ((direntData = ::readdir(this->m_handle.m_dir_descriptor)) != nullptr) {
        // Skip . and .. directory entries
        if ((direntData->d_name[0] == '.' and direntData->d_name[1] == '\0') or
            (direntData->d_name[0] == '.' and direntData->d_name[1] == '.' and direntData->d_name[2] == '\0')) {
            continue;
        } else {
            (void)Fw::StringUtils::string_copy(fileNameBuffer, direntData->d_name, bufSize);
            break;
        }
    }
    if (direntData == nullptr) {
        // loop ended because readdir failed, did it error or did we run out of files?
        if (errno != 0) {
            // Only error from readdir is EBADF
            status = Status::BAD_DESCRIPTOR;
        } else {
            status = Status::NO_MORE_FILES;
        }
    }
    return status;
}

void PosixDirectory::close() {
    // ::closedir errors if dir descriptor is nullptr
    if (this->m_handle.m_dir_descriptor != nullptr) {
        (void)::closedir(this->m_handle.m_dir_descriptor);
    }
    this->m_handle.m_dir_descriptor = nullptr;
}

}  // namespace Directory
}  // namespace Posix
}  // namespace Os

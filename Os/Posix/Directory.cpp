// ======================================================================
// \title Os/Posix/Directory.cpp
// \brief Posix implementation for Os::Directory
// ======================================================================
#include <cstring>
#include <sys/errno.h>

#include <Fw/Types/Assert.hpp>
#include <Os/Posix/Directory.hpp>
#include <Os/Posix/error.hpp>

namespace Os {
namespace Posix {
namespace Directory {

PosixDirectory::PosixDirectory() : Os::DirectoryInterface(), m_handle() {}

PosixDirectory::~PosixDirectory() {
    this->close();
}

DirectoryHandle* PosixDirectory::getHandle() {
    return &this->m_handle;
}

PosixDirectory::Status PosixDirectory::open(const char* path) {
    Status status = Status::OP_OK;

    DIR* dir = ::opendir(path);

    if (dir == nullptr) {
        return errno_to_directory_status(errno);
    }

    this->m_handle.m_dir_descriptor = dir;
    return status;
}

bool PosixDirectory::isOpen() {
    return this->m_handle.m_dir_descriptor != nullptr;
}

PosixDirectory::Status PosixDirectory::rewind() {
    // make sure it has been opened
    if (!this->isOpen()) {
        return Status::NOT_OPENED;
    }
    Status status = Status::OP_OK;
    // no errors defined
    ::rewinddir(this->m_handle.m_dir_descriptor);
    return status;
}

PosixDirectory::Status PosixDirectory::read(char* fileNameBuffer, U32 bufSize) {
    I64 unused;
    return this->read(fileNameBuffer, bufSize, unused);
}

PosixDirectory::Status PosixDirectory::read(char* fileNameBuffer, U32 bufSize, I64& inode) {

        FW_ASSERT(fileNameBuffer);

        // make sure it has been opened
        if (!this->isOpen()) {
            return Status::NOT_OPENED;
        }

        Status status = Status::OP_OK;

        // Set errno to 0 so we know why we exited readdir
        errno = 0;

        struct dirent *direntData = nullptr;
        while ((direntData = ::readdir(this->m_handle.m_dir_descriptor)) != nullptr) {
            // Skip hidden files
            if (direntData->d_name[0] != '.') {
                strncpy(fileNameBuffer, direntData->d_name, bufSize);
                inode = static_cast<I64>(direntData->d_ino);
                break;
            }
        }

        if (direntData == nullptr) {
            // loop ended because readdir failed, did it error or did we run out of files?
            if(errno != 0) {
                // Only error from readdir is EBADF
                status = Status::BAD_DESCRIPTOR;
            }
            else {
                status = Status::NO_MORE_FILES;
            }
        }

        return status;
    }

void PosixDirectory::close() {
    if (this->isOpen()) {
        (void)::closedir(this->m_handle.m_dir_descriptor);
    }
    this->m_handle.m_dir_descriptor = nullptr;
}

}  // namespace Directory
}  // namespace Posix
}  // namespace Os
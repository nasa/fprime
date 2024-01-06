// ======================================================================
// \title Os/Posix/File.cpp
// \brief posix implementation for Os::File
// ======================================================================
#include <cerrno>
#include <new>
#include <fcntl.h>
#include <unistd.h>
#include <limits>

#include <Os/File.hpp>
#include <Os/Posix/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Posix/errno.hpp>

namespace Os {

// Os specific direct writing support
#ifdef __linux__
#define DIRECT_FLAGS O_DIRECT
#else
#define DIRECT_FLAGS 0
#endif

#if defined(S_IREAD) && defined(S_IWRITE)
#define USER_FLAGS (S_IREAD | S_IWRITE)
#else
#define USER_FLAGS (0)
#endif

FileHandle::FileHandle() : file_descriptor(-1) {}

void File::constructInternal() {
    // Placement-new the file handle into the opaque file-handle storage
    this->handle = new (&this->handle_storage[0]) FileHandle;
}

void File::destructInternal() {}

File::Status File::openInternal(const char* path, File::Mode requested_mode, bool overwrite) {
    PlatformIntType mode_flags = 0;
    Status status = OP_OK;

    switch (requested_mode) {
        case OPEN_READ:
            mode_flags = O_RDONLY;
            break;
        case OPEN_WRITE:
            mode_flags = O_WRONLY | O_CREAT;
            break;
        case OPEN_SYNC_WRITE:
            mode_flags = O_WRONLY | O_CREAT | O_SYNC;
            break;
        case OPEN_SYNC_DIRECT_WRITE:
            mode_flags = O_WRONLY | O_CREAT | O_DSYNC | DIRECT_FLAGS;
            break;
        case OPEN_CREATE:
            mode_flags = O_WRONLY | O_CREAT | O_TRUNC | (overwrite ? 0 : O_EXCL);
            break;
        case OPEN_APPEND:
            mode_flags = O_WRONLY | O_CREAT | O_APPEND;
            break;
        default:
            FW_ASSERT(0, requested_mode);
            break;
    }

    PlatformIntType descriptor = ::open(path, mode_flags, USER_FLAGS);
    if (-1 == descriptor) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    this->handle->file_descriptor = descriptor;
    return status;
}

void File::closeInternal() {
    // Only close file handles that are not open
    if (-1 != this->handle->file_descriptor) {
        (void)::close(this->handle->file_descriptor);
        this->handle->file_descriptor = -1;
    }
    this->mode = OPEN_NO_MODE;
}


File::Status File::preallocateInternal(FwSizeType offset, FwSizeType length) {
    File::Status status = NOT_SUPPORTED;
// Efficient linux/gnu implementation
#ifdef _GNU_SOURCE
    PlatformIntType errno_status = ::fallocate(this->handle->file_descriptor, 0, offset, length);
    status = Os::Posix::errno_to_file_status(errno_status);
#endif
    // When the operation is not supported, fallback to base algorithm
    if (NOT_SUPPORTED == status) {
        FwSizeType size = 0;
        // FwSizeType size = this->getSize();

        // Check for integer overflow
        if ((std::numeric_limits<FwSizeType>::max() - offset - length) < 0) {
            status = File::NO_SPACE;
        } else if (size < (offset + length)) {
            const FwSizeType write_start = FW_MAX(offset, size);
            const FwSizeType write_length = (offset + length) - write_start;
            this->seek(write_start);
            // Fill in zeros past size of file to ensure compatibility with fallocate
            for (FwSizeType i = 0; i < write_length; i++) {
               FwSizeType write_size = 1;
                status = this->write("\0", write_size, false);
                if (Status::OP_OK != status) {
                    break;
                }
            }
        }
    }
    return status;
}

static_assert(sizeof(FwSizeType) >= sizeof(off_t), "FwSizeType is not large enough to store off_t");
static_assert(sizeof(FwSizeType) >= sizeof(size_t), "FwSizeType is not large enough to store size_t");
static_assert(sizeof(FwSizeType) >= sizeof(ssize_t), "FwSizeType is not large enough to store ssize_t");

File::Status File::seekInternal(FwSizeType offset, bool absolute) {
    Status status = OP_OK;
    off_t actual = ::lseek(this->handle->file_descriptor, offset, absolute ? SEEK_SET : SEEK_CUR);
    if (actual == -1) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    } else if (actual != offset) {
        status = Os::File::Status::BAD_SIZE;
    }
    return status;
}

File::Status File::flushInternal() {
    File::Status status = OP_OK;
    if (-1 == fsync(this->handle->file_descriptor)) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    return status;
}


File::Status File::readInternal(U8* buffer, FwSizeType &size, bool wait) {
    Status status = OP_OK;
    FwSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSizeType maximum = (size > (std::numeric_limits<FwSizeType>::max()/2)) ? std::numeric_limits<FwSizeType>::max() : size * 2;

    for (FwSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t read_size = ::read(this->handle->file_descriptor, reinterpret_cast<CHAR*>(buffer + accumulated), size - accumulated);
        // Non-interrupt error
        if (-1 == read_size) {
            PlatformIntType errno_store = errno;
            // Interrupted w/o read, try again
            if (EINTR != errno_store) {
                continue;
            }
            status = Os::Posix::errno_to_file_status(errno_store);
            break;
        }
        // End-of-file
        else if (read_size == 0) {
            break;
        }
        accumulated += read_size;
        // Stop looping when we had a good read and are not waiting
        if (not wait) {
            break;
        }
    }
    size = accumulated;
    return status;
}

File::Status File::writeInternal(const void* buffer_in, FwSizeType &size, bool wait) {
    Status status = OP_OK;
    FwSizeType accumulated = 0;
    U8* buffer = reinterpret_cast<U8*>(const_cast<void *>(buffer_in));
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSizeType maximum = (size > (std::numeric_limits<FwSizeType>::max()/2)) ? std::numeric_limits<FwSizeType>::max() : size * 2;

    for (FwSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t write_size = ::write(this->handle->file_descriptor, reinterpret_cast<CHAR*>(buffer + accumulated), size - accumulated);
        // Non-interrupt error
        if (-1 == write_size) {
            PlatformIntType errno_store = errno;
            // Interrupted w/o read, try again
            if (EINTR != errno_store) {
                continue;
            }
            status = Os::Posix::errno_to_file_status(errno_store);
            break;
        }
        accumulated += write_size;
    }
    size = accumulated;
    // When waiting, sync to disk
    if (wait) {
       PlatformIntType fsync_return = fsync(this->handle->file_descriptor);
       if (-1 == fsync_return) {
            PlatformIntType errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
       }
    }
    return status;
}

}
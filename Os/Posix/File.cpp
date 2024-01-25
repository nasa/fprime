// ======================================================================
// \title Os/Posix/File.cpp
// \brief posix implementation for Os::File
// ======================================================================
#include <cerrno>
#include <new>
#include <fcntl.h>
#include <unistd.h>
#include <limits>
// remove these
#include <cstdio>
#include <cstring>

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



// Ensure size of FwSizeType is large enough to fit eh necessary range
static_assert(sizeof(FwSignedSizeType) >= sizeof(off_t), "FwSizeType is not large enough to store values of type off_t");
static_assert(sizeof(FwSignedSizeType) >= sizeof(ssize_t), "FwSizeType is not large enough to store values of type ssize_t");

// Now check ranges of FwSizeType
static_assert(std::numeric_limits<FwSignedSizeType>::max() >= std::numeric_limits<off_t>::max(),
              "Maximum value of FwSizeType less than the maximum value of off_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::max() >= std::numeric_limits<ssize_t>::max(),
              "Maximum value of FwSizeType less than the maximum value of ssize_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::min() <= std::numeric_limits<off_t>::min(),
              "Minimum value of FwSizeType larger than the minimum value of off_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::min() <= std::numeric_limits<ssize_t>::min(),
              "Minimum value of FwSizeType larger than the minimum value of ssize_t. Configure a larger type.");


FileHandle::FileHandle() : file_descriptor(-1) {}

void File::constructInternal() {
    // Placement-new the file handle into the opaque file-handle storage
    this->m_handle = new (&this->m_handle_storage[0]) FileHandle;
}

void File::destructInternal() {}

File::Status File::openInternal(const char* filepath, File::Mode requested_mode, bool overwrite) {
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

    PlatformIntType descriptor = ::open(filepath, mode_flags, USER_FLAGS);
    if (-1 == descriptor) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    } else {
        this->m_path = filepath;
    }
    this->m_handle->file_descriptor = descriptor;
    return status;
}

void File::closeInternal() {
    // Only close file handles that are not open
    if (-1 != this->m_handle->file_descriptor) {
        (void)::close(this->m_handle->file_descriptor);
        this->m_handle->file_descriptor = -1;
    }
    this->m_mode = OPEN_NO_MODE;
}

File::Status  File::sizeInternal(FwSignedSizeType& size_result) {
    FwSignedSizeType current_position = 0;
    Status status = this->positionInternal(current_position);
    size_result = 0;
    if (Os::File::Status::OP_OK == status) {
        // Seek to the end of the file to determine size
        off_t end_of_file = ::lseek(this->m_handle->file_descriptor, 0, SEEK_END);
        if (-1 == end_of_file) {
            PlatformIntType errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
        } else {
            // Return the file pointer back to the original position
            off_t original = ::lseek(this->m_handle->file_descriptor, current_position, SEEK_SET);
            if ((-1 == original) || (current_position != original)) {
                PlatformIntType errno_store = errno;
                status = Os::Posix::errno_to_file_status(errno_store);
            }
        }
        size_result = end_of_file;
    }
    return status;
}

File::Status  File::positionInternal(FwSignedSizeType &position_result) {
    Status status = OP_OK;
    position_result = 0;
    off_t actual = ::lseek(this->m_handle->file_descriptor, 0, SEEK_CUR);
    if (-1 == actual) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    position_result = static_cast<FwSignedSizeType>(actual);
    return status;
}

File::Status File::preallocateInternal(FwSignedSizeType offset, FwSignedSizeType length) {
    File::Status status = Os::File::Status::NOT_SUPPORTED;
// Efficient linux/gnu implementation
#if _POSIX_C_SOURCE >= 200112L
    PlatformIntType errno_status = ::posix_fallocate(this->m_handle->file_descriptor, offset, length);
    status = Os::Posix::errno_to_file_status(errno_status);
#endif
    // When the operation is not supported, fallback to base algorithm
    if (Os::File::Status::NOT_SUPPORTED == status) {
        // Calculate size
        FwSignedSizeType file_size = 0;
        status = this->size(file_size);
        if (Os::File::Status::OP_OK == status) {
            // Calculate current position
            FwSignedSizeType file_position = 0;
            status = this->position(file_position);
            if (Os::File::Status::OP_OK == status) {
                // Check for integer overflow
                if ((std::numeric_limits<FwSignedSizeType>::max() - offset - length) < 0) {
                    status = File::NO_SPACE;
                } else if (file_size < (offset + length)) {
                    const FwSignedSizeType write_length = (offset + length) - file_size;
                    status = this->seek(file_size, true);
                    if (Os::File::Status::OP_OK == status) {
                        // Fill in zeros past size of file to ensure compatibility with fallocate
                        for (FwSignedSizeType i = 0; i < write_length; i++) {
                            FwSignedSizeType write_size = 1;
                            status = this->write("\0", write_size, false);
                            if (Status::OP_OK != status || write_size != 1) {
                                break;
                            }
                        }
                        // Return to original position
                        if (Os::File::Status::OP_OK == status) {
                            status = this->seek(file_position, true);
                        }
                    }
                }
            }
        }
    }
    return status;
}

File::Status File::seekInternal(FwSignedSizeType offset, bool absolute) {
    Status status = OP_OK;
    off_t actual = ::lseek(this->m_handle->file_descriptor, offset, absolute ? SEEK_SET : SEEK_CUR);
    PlatformIntType errno_store = errno;
    if (actual == -1) {
        status = Os::Posix::errno_to_file_status(errno_store);
    } else if (absolute && (actual != offset)) {
        status = Os::File::Status::OTHER_ERROR;
    }
    return status;
}

File::Status File::flushInternal() {
    File::Status status = OP_OK;
    if (-1 == fsync(this->m_handle->file_descriptor)) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    return status;
}


File::Status File::readInternal(U8* buffer, FwSignedSizeType &size, bool wait) {
    Status status = OP_OK;
    FwSignedSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSignedSizeType maximum = (size > (std::numeric_limits<FwSignedSizeType>::max()/2)) ? std::numeric_limits<FwSignedSizeType>::max() : size * 2;

    for (FwSignedSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t read_size = ::read(this->m_handle->file_descriptor, reinterpret_cast<CHAR*>(buffer + accumulated), size - accumulated);
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

File::Status File::writeInternal(const void* buffer_in, FwSignedSizeType &size, bool wait) {
    Status status = OP_OK;
    FwSignedSizeType accumulated = 0;
    U8* buffer = reinterpret_cast<U8*>(const_cast<void *>(buffer_in));
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSignedSizeType maximum = (size > (std::numeric_limits<FwSignedSizeType>::max()/2)) ? std::numeric_limits<FwSignedSizeType>::max() : size * 2;

    for (FwSignedSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t write_size = ::write(this->m_handle->file_descriptor, reinterpret_cast<CHAR*>(buffer + accumulated), size - accumulated);
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
       PlatformIntType fsync_return = fsync(this->m_handle->file_descriptor);
       if (-1 == fsync_return) {
            PlatformIntType errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
       }
    }
    return status;
}

}
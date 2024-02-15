// ======================================================================
// \title Os/Posix/File.cpp
// \brief posix implementation for Os::File
// ======================================================================
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <limits>

#include <Os/File.hpp>
#include <Os/Posix/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Posix/errno.hpp>

namespace Os {
namespace Posix {
namespace File {

// Sets up the default file permission as user read + user write
// Some posix systems (e.g. Darwin) use the older S_IREAD and S_IWRITE flags while other systems (e.g. Linux) use the
// newer S_IRUSR and S_IWUSR flags, and some don't support these flags at all. Hence, we look if flags are defined then
// set USER_FLAGS to be the set of flags supported or 0 in the case neither is defined.
#if defined(S_IREAD) && defined(S_IWRITE)
#define USER_FLAGS (S_IREAD | S_IWRITE)
#elif defined(S_IRUSR) && defined(S_IWUSR)
#define USER_FLAGS (S_IRUSR | S_IWUSR)
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

//!\brief default copy constructor
PosixFile::PosixFile(const PosixFile& other) {
    // Must properly duplicate the file handle
    this->m_handle.m_file_descriptor = ::dup(other.m_handle.m_file_descriptor);
};

PosixFile& PosixFile::operator=(const PosixFile& other) {
    if (this != &other) {
        this->m_handle.m_file_descriptor = ::dup(other.m_handle.m_file_descriptor);
    }
    return *this;
}

PosixFile::Status PosixFile::open(const char* filepath, PosixFile::Mode requested_mode, PosixFile::OverwriteType overwrite) {
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
        case OPEN_CREATE:
            mode_flags = O_WRONLY | O_CREAT | O_TRUNC | ((overwrite == PosixFile::OverwriteType::OVERWRITE) ? 0 : O_EXCL);
            break;
        case OPEN_APPEND:
            mode_flags = O_WRONLY | O_CREAT | O_APPEND;
            break;
        default:
            FW_ASSERT(0, requested_mode);
            break;
    }
    PlatformIntType descriptor = ::open(filepath, mode_flags, USER_FLAGS);
    if (PosixFileHandle::INVALID_FILE_DESCRIPTOR == descriptor) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    this->m_handle.m_file_descriptor = descriptor;
    return status;
}

void PosixFile::close() {
    // Only close file handles that are not open
    if (PosixFileHandle::INVALID_FILE_DESCRIPTOR != this->m_handle.m_file_descriptor) {
        (void)::close(this->m_handle.m_file_descriptor);
        this->m_handle.m_file_descriptor = PosixFileHandle::INVALID_FILE_DESCRIPTOR;
    }
}

PosixFile::Status  PosixFile::size(FwSignedSizeType& size_result) {
    FwSignedSizeType current_position = 0;
    Status status = this->position(current_position);
    size_result = 0;
    if (Os::File::Status::OP_OK == status) {
        // Seek to the end of the file to determine size
        off_t end_of_file = ::lseek(this->m_handle.m_file_descriptor, 0, SEEK_END);
        if (PosixFileHandle::ERROR_RETURN_VALUE == end_of_file) {
            PlatformIntType errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
        } else {
            // Return the file pointer back to the original position
            off_t original = ::lseek(this->m_handle.m_file_descriptor, current_position, SEEK_SET);
            if ((PosixFileHandle::ERROR_RETURN_VALUE == original) || (current_position != original)) {
                PlatformIntType errno_store = errno;
                status = Os::Posix::errno_to_file_status(errno_store);
            }
        }
        size_result = end_of_file;
    }
    return status;
}

PosixFile::Status  PosixFile::position(FwSignedSizeType &position_result) {
    Status status = OP_OK;
    position_result = 0;
    off_t actual = ::lseek(this->m_handle.m_file_descriptor, 0, SEEK_CUR);
    if (PosixFileHandle::ERROR_RETURN_VALUE == actual) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    position_result = static_cast<FwSignedSizeType>(actual);
    return status;
}

PosixFile::Status PosixFile::preallocate(FwSignedSizeType offset, FwSignedSizeType length) {
    PosixFile::Status status = Os::File::Status::NOT_SUPPORTED;
    // posix_fallocate is only available with the posix C-API post version 200112L, however; it is not guaranteed that
    // this call is properly implemented. This code starts with a status of "NOT_SUPPORTED".  When the standard is met
    // an attempt will be made to called posix_fallocate, and should that still return NOT_SUPPORTED then fallback
    // code is engaged to synthesize this behavior.
#if _POSIX_C_SOURCE >= 200112L
    PlatformIntType errno_status = ::posix_fallocate(this->m_handle.m_file_descriptor, offset, length);
    status = Os::Posix::errno_to_file_status(errno_status);
#endif
    // When the operation is not supported or posix-API is not sufficient, fallback to a slower algorithm
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
                    status = PosixFile::NO_SPACE;
                } else if (file_size < (offset + length)) {
                    const FwSignedSizeType write_length = (offset + length) - file_size;
                    status = this->seek(file_size, PosixFile::SeekType::ABSOLUTE);
                    if (Os::File::Status::OP_OK == status) {
                        // Fill in zeros past size of file to ensure compatibility with fallocate
                        for (FwSignedSizeType i = 0; i < write_length; i++) {
                            FwSignedSizeType write_size = 1;
                            status = this->write(reinterpret_cast<const U8*>("\0"), write_size, PosixFile::WaitType::NO_WAIT);
                            if (Status::OP_OK != status || write_size != 1) {
                                break;
                            }
                        }
                        // Return to original position
                        if (Os::File::Status::OP_OK == status) {
                            status = this->seek(file_position, PosixFile::SeekType::ABSOLUTE);
                        }
                    }
                }
            }
        }
    }
    return status;
}

PosixFile::Status PosixFile::seek(FwSignedSizeType offset, PosixFile::SeekType seekType) {
    Status status = OP_OK;
    off_t actual = ::lseek(this->m_handle.m_file_descriptor, offset, (seekType == SeekType::ABSOLUTE) ? SEEK_SET : SEEK_CUR);
    PlatformIntType errno_store = errno;
    if (actual == PosixFileHandle::ERROR_RETURN_VALUE) {
        status = Os::Posix::errno_to_file_status(errno_store);
    } else if ((seekType == SeekType::ABSOLUTE) && (actual != offset)) {
        status = Os::File::Status::OTHER_ERROR;
    }
    return status;
}

PosixFile::Status PosixFile::flush() {
    PosixFile::Status status = OP_OK;
    if (PosixFileHandle::ERROR_RETURN_VALUE == ::fsync(this->m_handle.m_file_descriptor)) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    return status;
}

PosixFile::Status PosixFile::read(U8* buffer, FwSignedSizeType &size, PosixFile::WaitType wait) {
    Status status = OP_OK;
    FwSignedSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSignedSizeType maximum = (size > (std::numeric_limits<FwSignedSizeType>::max()/2)) ? std::numeric_limits<FwSignedSizeType>::max() : size * 2;

    for (FwSignedSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t read_size = ::read(this->m_handle.m_file_descriptor, reinterpret_cast<CHAR*>(&buffer[accumulated]), size - accumulated);
        // Non-interrupt error
        if (PosixFileHandle::ERROR_RETURN_VALUE == read_size) {
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

PosixFile::Status PosixFile::write(const U8* buffer, FwSignedSizeType &size, PosixFile::WaitType wait) {
    Status status = OP_OK;
    FwSignedSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSignedSizeType maximum = (size > (std::numeric_limits<FwSignedSizeType>::max()/2)) ? std::numeric_limits<FwSignedSizeType>::max() : size * 2;

    for (FwSignedSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t write_size = ::write(this->m_handle.m_file_descriptor, reinterpret_cast<const CHAR*>(&buffer[accumulated]), size - accumulated);
        // Non-interrupt error
        if (PosixFileHandle::ERROR_RETURN_VALUE == write_size) {
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
       PlatformIntType fsync_return = ::fsync(this->m_handle.m_file_descriptor);
       if (PosixFileHandle::ERROR_RETURN_VALUE == fsync_return) {
            PlatformIntType errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
       }
    }
    return status;
}

FileHandle* PosixFile::getHandle() {
    return &this->m_handle;
}

} // namespace File
} // namespace Posix
} // namespace Os


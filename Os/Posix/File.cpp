// ======================================================================
// \title Os/Posix/File.cpp
// \brief posix implementation for Os::File
// ======================================================================
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <limits>
#include <type_traits>

#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/Posix/File.hpp>
#include <Os/Posix/error.hpp>

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

// Create constants for the max limits of the signed types
// These constants are used for comparisons with complementary unsigned types to avoid sign-compare warning
using UnsignedOffT = std::make_unsigned<off_t>::type;
static const UnsignedOffT OFF_T_MAX_LIMIT = static_cast<UnsignedOffT>(std::numeric_limits<off_t>::max());
using UnsignedSSizeT = std::make_unsigned<ssize_t>::type;
static const UnsignedSSizeT SSIZE_T_MAX_LIMIT = static_cast<UnsignedSSizeT>(std::numeric_limits<ssize_t>::max());

// Ensure size of FwSizeType is large enough to fit eh necessary range
static_assert(sizeof(FwSignedSizeType) >= sizeof(off_t),
              "FwSignedSizeType is not large enough to store values of type off_t");
static_assert(sizeof(FwSignedSizeType) >= sizeof(ssize_t),
              "FwSignedSizeType is not large enough to store values of type ssize_t");
static_assert(sizeof(FwSizeType) >= sizeof(size_t), "FwSizeType is not large enough to store values of type size_t");

// Now check ranges of FwSizeType
static_assert(std::numeric_limits<FwSignedSizeType>::max() >= std::numeric_limits<off_t>::max(),
              "Maximum value of FwSignedSizeType less than the maximum value of off_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSizeType>::max() >= OFF_T_MAX_LIMIT,
              "Maximum value of FwSizeType less than the maximum value of off_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::max() >= std::numeric_limits<ssize_t>::max(),
              "Maximum value of FwSignedSizeType less than the maximum value of ssize_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::min() <= std::numeric_limits<off_t>::min(),
              "Minimum value of FwSignedSizeType larger than the minimum value of off_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSignedSizeType>::min() <= std::numeric_limits<ssize_t>::min(),
              "Minimum value of FwSizeType larger than the minimum value of ssize_t. Configure a larger type.");
static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<size_t>::max(),
              "Maximum value of FwSizeType less than the maximum value of size_t. Configure a larger type.");

//!\brief default copy constructor
PosixFile::PosixFile(const PosixFile& other) {
    // Must properly duplicate the file handle
    this->m_handle.m_file_descriptor = fcntl(other.m_handle.m_file_descriptor, F_DUPFD, 0);
}

PosixFile& PosixFile::operator=(const PosixFile& other) {
    if (this != &other) {
        this->m_handle.m_file_descriptor = fcntl(other.m_handle.m_file_descriptor, F_DUPFD, 0);
    }
    return *this;
}

PosixFile::Status PosixFile::open(const char* filepath,
                                  PosixFile::Mode requested_mode,
                                  PosixFile::OverwriteType overwrite) {
    int mode_flags = 0;
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
            mode_flags =
                O_WRONLY | O_CREAT | O_TRUNC | ((overwrite == PosixFile::OverwriteType::OVERWRITE) ? 0 : O_EXCL);
            break;
        case OPEN_APPEND:
            mode_flags = O_WRONLY | O_CREAT | O_APPEND;
            break;
        default:
            FW_ASSERT(0, requested_mode);
            break;
    }
    int descriptor = ::open(filepath, mode_flags, USER_FLAGS);
    if (PosixFileHandle::INVALID_FILE_DESCRIPTOR == descriptor) {
        int errno_store = errno;
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

PosixFile::Status PosixFile::size(FwSizeType& size_result) {
    FwSizeType current_position = 0;
    Status status = this->position(current_position);
    size_result = 0;
    if (Os::File::Status::OP_OK == status) {
        // Must be a coding error if current_position is larger than off_t max in Posix File
        FW_ASSERT(current_position <= OFF_T_MAX_LIMIT);
        // Seek to the end of the file to determine size
        off_t end_of_file = ::lseek(this->m_handle.m_file_descriptor, 0, SEEK_END);
        if (PosixFileHandle::ERROR_RETURN_VALUE == end_of_file) {
            int errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
        }
        // Return to original position
        (void)::lseek(this->m_handle.m_file_descriptor, static_cast<off_t>(current_position), SEEK_SET);
        size_result = static_cast<FwSizeType>(end_of_file);
    }
    return status;
}

PosixFile::Status PosixFile::position(FwSizeType& position_result) {
    Status status = OP_OK;
    position_result = 0;
    off_t actual = ::lseek(this->m_handle.m_file_descriptor, 0, SEEK_CUR);
    if (PosixFileHandle::ERROR_RETURN_VALUE == actual) {
        int errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    // Protected by static assertion (FwSizeType >= off_t)
    position_result = static_cast<FwSizeType>(actual);
    return status;
}

PosixFile::Status PosixFile::preallocate(FwSizeType offset, FwSizeType length) {
    PosixFile::Status status = Os::File::Status::NOT_SUPPORTED;
    // Check for larger size than posix supports
    if ((length > OFF_T_MAX_LIMIT) || (offset > OFF_T_MAX_LIMIT) ||
        (std::numeric_limits<off_t>::max() - length) < offset) {
        status = Os::File::Status::BAD_SIZE;
    }
    // posix_fallocate is only available with the posix C-API post version 200112L, however; it is not guaranteed that
    // this call is properly implemented. This code starts with a status of "NOT_SUPPORTED".  When the standard is met
    // an attempt will be made to called posix_fallocate, and should that still return NOT_SUPPORTED then fallback
    // code is engaged to synthesize this behavior.
#if _POSIX_C_SOURCE >= 200112L
    else {
        int errno_status =
            ::posix_fallocate(this->m_handle.m_file_descriptor, static_cast<off_t>(offset), static_cast<off_t>(length));
        status = Os::Posix::errno_to_file_status(errno_status);
    }
#endif
    // When the operation is not supported or posix-API is not sufficient, fallback to a slower algorithm
    if (Os::File::Status::NOT_SUPPORTED == status) {
        // Calculate size
        FwSizeType file_size = 0;
        status = this->size(file_size);
        if (Os::File::Status::OP_OK == status) {
            // Calculate current position
            FwSizeType file_position = 0;
            status = this->position(file_position);
            // Check for overflow in seek calls
            if (file_position > static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max()) ||
                file_size > static_cast<FwSizeType>(std::numeric_limits<FwSignedSizeType>::max())) {
                status = Os::File::Status::BAD_SIZE;
            }
            // Only allocate when the file is smaller than the allocation
            else if ((Os::File::Status::OP_OK == status) && (file_size < (offset + length))) {
                const FwSizeType write_length = (offset + length) - file_size;
                status = this->seek(static_cast<FwSignedSizeType>(file_size), PosixFile::SeekType::ABSOLUTE);
                if (Os::File::Status::OP_OK == status) {
                    // Fill in zeros past size of file to ensure compatibility with fallocate
                    for (FwSizeType i = 0; i < write_length; i++) {
                        FwSizeType write_size = 1;
                        status =
                            this->write(reinterpret_cast<const U8*>("\0"), write_size, PosixFile::WaitType::NO_WAIT);
                        if (Status::OP_OK != status || write_size != 1) {
                            break;
                        }
                    }
                    // Return to original position
                    if (Os::File::Status::OP_OK == status) {
                        status =
                            this->seek(static_cast<FwSignedSizeType>(file_position), PosixFile::SeekType::ABSOLUTE);
                    }
                }
            }
        }
    }
    return status;
}

PosixFile::Status PosixFile::seek(FwSignedSizeType offset, PosixFile::SeekType seekType) {
    Status status = OP_OK;
    if (offset > std::numeric_limits<off_t>::max()) {
        status = BAD_SIZE;
    } else {
        off_t actual = ::lseek(this->m_handle.m_file_descriptor, static_cast<off_t>(offset),
                               (seekType == SeekType::ABSOLUTE) ? SEEK_SET : SEEK_CUR);
        int errno_store = errno;
        if (actual == PosixFileHandle::ERROR_RETURN_VALUE) {
            status = Os::Posix::errno_to_file_status(errno_store);
        } else if ((seekType == SeekType::ABSOLUTE) && (actual != offset)) {
            status = Os::File::Status::OTHER_ERROR;
        }
    }
    return status;
}

PosixFile::Status PosixFile::flush() {
    PosixFile::Status status = OP_OK;
    if (PosixFileHandle::ERROR_RETURN_VALUE == ::fsync(this->m_handle.m_file_descriptor)) {
        int errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
    }
    return status;
}

PosixFile::Status PosixFile::read(U8* buffer, FwSizeType& size, PosixFile::WaitType wait) {
    Status status = OP_OK;
    FwSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSizeType maximum =
        (size > (std::numeric_limits<FwSizeType>::max() / 2)) ? std::numeric_limits<FwSizeType>::max() : size * 2;
    // POSIX APIs are implementation dependent when dealing with sizes larger than the signed return value
    // thus we ensure a clear decision: BAD_SIZE
    if (size > SSIZE_T_MAX_LIMIT) {
        return BAD_SIZE;
    }

    for (FwSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t read_size = ::read(this->m_handle.m_file_descriptor, reinterpret_cast<CHAR*>(&buffer[accumulated]),
                                   static_cast<size_t>(size - accumulated));
        // Non-interrupt error
        if (PosixFileHandle::ERROR_RETURN_VALUE == read_size) {
            int errno_store = errno;
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
        accumulated += static_cast<FwSizeType>(read_size);
        // Stop looping when we had a good read and are not waiting
        if (not wait) {
            break;
        }
    }
    size = accumulated;
    return status;
}

PosixFile::Status PosixFile::write(const U8* buffer, FwSizeType& size, PosixFile::WaitType wait) {
    Status status = OP_OK;
    FwSizeType accumulated = 0;
    // Loop up to 2 times for each by, bounded to prevent overflow
    const FwSizeType maximum =
        (size > (std::numeric_limits<FwSizeType>::max() / 2)) ? std::numeric_limits<FwSizeType>::max() : size * 2;
    // POSIX APIs are implementation dependent when dealing with sizes larger than the signed return value
    // thus we ensure a clear decision: BAD_SIZE
    if (size > SSIZE_T_MAX_LIMIT) {
        return BAD_SIZE;
    }

    for (FwSizeType i = 0; i < maximum && accumulated < size; i++) {
        // char* for some posix implementations
        ssize_t write_size =
            ::write(this->m_handle.m_file_descriptor, reinterpret_cast<const CHAR*>(&buffer[accumulated]),
                    static_cast<size_t>(size - accumulated));
        // Non-interrupt error
        if (PosixFileHandle::ERROR_RETURN_VALUE == write_size || write_size < 0) {
            int errno_store = errno;
            // Interrupted w/o write, try again
            if (EINTR != errno_store) {
                continue;
            }
            status = Os::Posix::errno_to_file_status(errno_store);
            break;
        }
        accumulated += static_cast<FwSizeType>(write_size);
    }
    size = accumulated;
    // When waiting, sync to disk
    if (wait) {
        int fsync_return = ::fsync(this->m_handle.m_file_descriptor);
        if (PosixFileHandle::ERROR_RETURN_VALUE == fsync_return) {
            int errno_store = errno;
            status = Os::Posix::errno_to_file_status(errno_store);
        }
    }
    return status;
}

FileHandle* PosixFile::getHandle() {
    return &this->m_handle;
}

}  // namespace File
}  // namespace Posix
}  // namespace Os

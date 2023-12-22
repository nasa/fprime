// ======================================================================
// \title Os/Posix/File.cpp
// \brief posix implementation for Os::File
// ======================================================================
#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Posix/errno.hpp>
#include <cerrno>
#include <fcntl.h>

namespace Os {

// Os specific direct writing support
#ifdef __linux__
#define DIRECT_FLAGS O_DIRECT
#else
#define DIRECT_FLAGS 0
#endif

File::Status File::openInternal(const char* path, File::Mode mode, bool overwrite) {

    PlatformIntType mode_flags = 0;
    Status status = OP_OK;

    switch (mode) {
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
            FW_ASSERT(0, mode);
            break;
    }

    PlatformIntType fd = ::open(path, mode_flags, S_IREAD | S_IWRITE);
    if (-1 == fd) {
        PlatformIntType errno_store = errno;
        status = Os::Posix::errno_to_file_status(errno_store);
        this->m_lastError = errno_store;
    }
    this->m_mode = mode;
    this->m_fd = fd;
    return status;
}
}
// ======================================================================
// \title Os/Posix/error.cpp
// \brief implementation for posix errno conversion
// ======================================================================
#include <cerrno>
#include "Os/Posix/error.hpp"

namespace Os {
namespace Posix {

File::Status errno_to_file_status(PlatformIntType errno_input) {
    File::Status status = File::Status::OP_OK;
    switch (errno_input) {
        case 0:
            status = File::Status::OP_OK;
            break;
        // Fallthrough intended
        case ENOSPC:
        case EFBIG:
            status = File::Status::NO_SPACE;
            break;
        case ENOENT:
            status = File::Status::DOESNT_EXIST;
            break;
        // Fallthrough intended
        case EPERM:
        case EACCES:
            status = File::Status::NO_PERMISSION;
            break;
        case EEXIST:
            status = File::Status::FILE_EXISTS;
            break;
        case EBADF:
            status = File::Status::NOT_OPENED;
            break;
        // Fallthrough intended
        case ENOSYS:
        case EOPNOTSUPP:
            status = File::Status::NOT_SUPPORTED;
            break;
        case EINVAL:
            status = File::Status::INVALID_ARGUMENT;
            break;
        default:
            status = File::Status::OTHER_ERROR;
            break;
    }
    return status;
}

Task::Status posix_status_to_task_status(PlatformIntType posix_status) {
    Task::Status status = Task::Status::OP_OK;
    switch (posix_status) {
        case 0:
            status = Task::Status::OP_OK;
            break;
        case EINVAL:
            status = Task::Status::INVALID_PARAMS;
            break;
        case EPERM:
            status = Task::Status::ERROR_PERMISSION;
            break;
        case EAGAIN:
            status = Task::Status::ERROR_RESOURCES;
            break;
        default:
            status = Task::Status::UNKNOWN_ERROR;
            break;
    }
    return status;
}

}
}

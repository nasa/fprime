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

FileSystem::Status errno_to_filesystem_status(PlatformIntType errno_input) {
    FileSystem::Status status = FileSystem::Status::OP_OK;
    switch (errno_input) {
        case 0:
            status = FileSystem::Status::OP_OK;
            break;
        // All fall through are intended to fallback on OTHER_ERROR
        case EACCES:
            status = FileSystem::Status::NO_PERMISSION;
            break;
        case EPERM:
        case EROFS:
        case EFAULT:
            status = FileSystem::Status::NO_PERMISSION;
            break;
        case EEXIST:
            status = FileSystem::Status::ALREADY_EXISTS;
            break;
        case ELOOP:
        case ENOENT:
            status = FileSystem::Status::DOESNT_EXIST;
            break;
        case ENAMETOOLONG:
            status = FileSystem::Status::INVALID_PATH;
            break;
        case ENOTDIR:
            status = FileSystem::Status::NOT_DIR;
            break;
        case EDQUOT:
            status = FileSystem::Status::NO_SPACE;
            break;
        case EMLINK:
            status = FileSystem::Status::FILE_LIMIT;
            break;
        case ENOSPC:
        case EFBIG:
            status = FileSystem::Status::NO_SPACE;
            break;
        case ENOSYS:
        case EOPNOTSUPP:
            status = FileSystem::Status::NOT_SUPPORTED;
            break;
        case ERANGE:
            status = FileSystem::Status::BUFFER_TOO_SMALL;
            break;
        case EXDEV:
            status = FileSystem::Status::EXDEV_ERROR;
            break;
        default:
            status = FileSystem::Status::OTHER_ERROR;
            break;
    }
    return status;
}

Directory::Status errno_to_directory_status(PlatformIntType errno_input) {
    Directory::Status status = Directory::Status::OP_OK;
    switch (errno_input) {
        case 0:
            status = Directory::Status::OP_OK;
            break;
        case ENOENT:
            status = Directory::Status::DOESNT_EXIST;
            break;
        case EACCES:
            status = Directory::Status::NO_PERMISSION;
            break;
        case ENOTDIR:
            status = Directory::Status::NOT_DIR;
            break;
        case EEXIST:
            status = Directory::Status::ALREADY_EXISTS;
            break;
        default:
            status = Directory::Status::OTHER_ERROR;
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

Mutex::Status posix_status_to_mutex_status(PlatformIntType posix_status){
    Mutex::Status status = Mutex::Status::ERROR_OTHER;
    switch (posix_status) {
        case 0:
            status = Mutex::Status::OP_OK;
            break;
        case EBUSY:
            status = Mutex::Status::ERROR_BUSY;
            break;
        case EDEADLK:
            status = Mutex::Status::ERROR_DEADLOCK;
            break;
        default:
            status = Mutex::Status::ERROR_OTHER;
            break;
    }
    return status;
}
}
}

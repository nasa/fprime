// ======================================================================
// \title Os/Posix/errno.cpp
// \brief implementation for posix errno conversion
// ======================================================================
#include <cerrno>
#include "Os/Posix/errno.hpp"

namespace Os {
namespace Posix {

File::Status errno_to_file_status(PlatformIntType error_in) {
    File::Status status = File::Status::OP_OK;
    switch (error_in) {
        case 0:
            status = File::Status::OP_OK;
            break;
        case ENOSPC:
            status = File::Status::NO_SPACE;
            break;
        case ENOENT:
            status = File::Status::DOESNT_EXIST;
            break;
        case EACCES:
            status = File::Status::NO_PERMISSION;
            break;
        case EEXIST:
            status = File::Status::FILE_EXISTS;
            break;
        default:
            status = File::Status::OTHER_ERROR;
            break;
    }
    return status;
}

}
}

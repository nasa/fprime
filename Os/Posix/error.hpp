// ======================================================================
// \title Os/Posix/error.hpp
// \brief header for posix errno conversion
// ======================================================================
#include "Os/File.hpp"
#include "Os/Task.hpp"
#ifndef OS_POSIX_ERRNO_HPP
#define OS_POSIX_ERRNO_HPP

namespace Os {
namespace Posix {

//! Convert an errno representation of an error to the Os::File::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::File::Status errno_to_file_status(PlatformIntType errno_input);

//! Convert an posix task representation of an error to the Os::Task::Status representation.
//! \param posix_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status posix_status_to_task_status(PlatformIntType posix_status);

}
}
#endif

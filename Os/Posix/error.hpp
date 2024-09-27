// ======================================================================
// \title Os/Posix/error.hpp
// \brief header for posix errno conversion
// ======================================================================
#ifndef OS_POSIX_ERRNO_HPP
#define OS_POSIX_ERRNO_HPP
#include "Os/File.hpp"
#include "Os/Task.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Directory.hpp"

namespace Os {
namespace Posix {

//! Convert an errno representation of an error to the Os::File::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::File::Status errno_to_file_status(PlatformIntType errno_input);

//! Convert an errno representation of an error to the Os::FileSystem::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::FileSystem::Status errno_to_filesystem_status(PlatformIntType errno_input);

//! Convert an errno representation of an error to the Os::FileSystem::Status representation.
//! \param errno_input: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::Directory::Status errno_to_directory_status(PlatformIntType errno_input);

//! Convert an posix task representation of an error to the Os::Task::Status representation.
//! \param posix_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status posix_status_to_task_status(PlatformIntType posix_status);

//! Convert a Posix return status (int) for mutex operations to the Os::Mutex::Status representation.
//! \param posix_status: return status
//! \return: Os::Mutex::Status representation of the error
//!
Os::Mutex::Status posix_status_to_mutex_status(PlatformIntType posix_status);

}
}
#endif

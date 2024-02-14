// ======================================================================
// \title Os/Posix/errno.hpp
// \brief header for posix errno conversion
// ======================================================================
#include "Os/File.hpp"
#ifndef OS_POSIX_ERRNO_HPP
#define OS_POSIX_ERRNO_HPP

namespace Os {
namespace Posix {

//! Convert an errno representation of an error to the Os::File::Status representation.
//! \param errno: errno representation of the error
//! \return: Os::File::Status representation of the error
//!
Os::File::Status errno_to_file_status(PlatformIntType errno_input);

}
}
#endif

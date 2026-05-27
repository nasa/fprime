// ======================================================================
// \title Os/Darwin/error.hpp
// \brief header for Darwin/Mach kernel return status conversion
// ======================================================================
#ifndef OS_DARWIN_ERROR_HPP
#define OS_DARWIN_ERROR_HPP
#include "Os/CountingSemaphore.hpp"

namespace Os {
namespace Darwin {

//! Convert a Darwin/Mach kern_return_t status to Os::CountingSemaphore::Status representation.
//! \param kern_status: kern_return_t value from Mach kernel operations
//! \return: Os::CountingSemaphore::Status representation of the error
//!
Os::CountingSemaphore::Status kern_return_to_semaphore_status(long kern_status);

}  // namespace Darwin
}  // namespace Os
#endif

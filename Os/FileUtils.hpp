// ======================================================================
// \title  Os/FileUtils.hpp
// \brief  common function definitions for file utility operations
// ======================================================================

#ifndef OS_FILE_UTILS_HPP
#define OS_FILE_UTILS_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/String.hpp>

namespace Os {

namespace FileUtils {

//! \brief Status enum for file utility operations
typedef enum {
    OP_OK,             //!< Operation was successful
    INVALID_PATH,      //!< The supplied path is invalid (null or not absolute)
    BUFFER_TOO_SMALL,  //!< The destination buffer is too small
    OTHER_ERROR,       //!< A catch-all for other errors
} Status;

//! \brief Resolve a Unix path by removing `.` and `..` components
//!
//! This function resolves a Unix path in-place without allocating memory.
//! - `.` components are removed
//! - `..` removes the previous directory component
//! - `..` at the root level results in `/`
//!
//! The source path must be an absolute path (starting with `/`).
//!
//! Examples:
//!   `/../../`                  -> `/`
//!   `/my/folder/../something`  -> `/my/something`
//!   `/my/./folder/./something` -> `/my/folder/something`
//!
//! It is invalid to pass `nullptr` for source or destination.
//! It is invalid to pass a destinationSize of 0.
//!
//! \param source The source path to resolve (null-terminated C string)
//! \param destination Buffer to store the resolved path
//! \param destinationSize Size of the destination buffer
//! \return Status of the operation
Status resolveUnixPath(const char* source, char* destination, FwSizeType destinationSize);

//! \brief Resolve a Unix path by removing `.` and `..` components (Fw::String version)
//!
//! This function resolves a Unix path in-place without allocating memory.
//! - `.` components are removed
//! - `..` removes the previous directory component
//! - `..` at the root level results in `/`
//!
//! The source path must be an absolute path (starting with `/`).
//!
//! Examples:
//!   `/../../`                  -> `/`
//!   `/my/folder/../something`  -> `/my/something`
//!   `/my/./folder/./something` -> `/my/folder/something`
//!
//! \param source The source Fw::String path to resolve
//! \param destination Fw::String to store the resolved path
//! \return Status of the operation
Status resolveUnixPath(const Fw::StringBase& source, Fw::StringBase& destination);

}  // namespace FileUtils

}  // namespace Os

#endif  // OS_FILE_UTILS_HPP

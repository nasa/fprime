// ======================================================================
// \title  CfeStubs.hpp
// \author campuzan
// \brief  CFE stubs that need to be re-worked and replaced
// ======================================================================

#ifndef CCSDS_CFDP_CFE_STUBS_HPP
#define CCSDS_CFDP_CFE_STUBS_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// OS abstraction layers stubs
// ======================================================================

// One CF function was already replaced with an OS call:
// void CF_CFDP_MoveFile(const char *src, const char *dest_dir)

// From <Os/FileSystem.hpp>
//! \brief Move a file from sourcePath to destPath
//!
//! This is done by first trying to rename, and if renaming fails,
//! copy it and then remove the original
//!
//! It is invalid to pass `nullptr` as either the source or destination path.
//!
//! \param sourcePath The path of the source file
//! \param destPath The path of the destination file
//! \return Status of the operation
static Status moveFile(const char* sourcePath, const char* destPath);

I32 OS_remove(const char *path)
{
    // From <Os/FileSystem.hpp>
    //! \brief Remove a file at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the file to remove
    //! \return Status of the operation
    // static Status removeFile(const char* path);
}

}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDP_CFE_STUBS_HPP

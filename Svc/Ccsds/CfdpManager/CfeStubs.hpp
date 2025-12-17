// ======================================================================
// \title  CfeStubs.hpp
// \author campuzan
// \brief  CFE stubs that need to be re-worked and replaced
//         THIS FILE IS MEANT TO BE TEMPROARY AND EVENTUALLY DELETED
// ======================================================================

#ifndef CCSDS_CFDP_CFE_STUBS_HPP
#define CCSDS_CFDP_CFE_STUBS_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Directory.hpp>

namespace Svc {
namespace Ccsds {

// ======================================================================
// OS abstraction layers stubs
// ======================================================================

// BPC: One CF function was already replaced with an OS call:
// void CF_CFDP_MoveFile(const char *src, const char *dest_dir)
// static Status moveFile(const char* sourcePath, const char* destPath);
// BPC: Added TODO's to report the return Status via EVR

// From <Os/FileSystem.hpp>
// static Status FileSystem::removeFile(const char* path);
// int32 OS_remove(const char *path)
I32 OS_remove(const char *path)
{}

// From <Os/Directory.hpp>
// Status Directory::open(const char* path, OpenMode mode) override;
// int32 OS_DirectoryOpen(osal_id_t *dir_id, const char *path)
I32 OS_DirectoryOpen(Os::DirectoryHandle *dir_id, const char *path)
{}


}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDP_CFE_STUBS_HPP

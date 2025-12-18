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

// From <Os/File.hpp>
// #define OS_SEEK_SET 0 /**< Seek offset set */ BPC: The file offset is set to offset bytes.
// #define OS_SEEK_CUR 1 /**< Seek offset current */ BPC: The file offset is set to its current location plus offset bytes.
// #define OS_SEEK_END 2 /**< Seek offset end */ BPC: The file offset is set to the size of the file plus offset bytes.
// Status seek(FwSignedSizeType offset, SeekType seekType) override;
// CFE_Status_t CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
// BPC: All instances of CF_WrappedLseek use OS_SEEK_SET except one call which uses OS_SEEK_END to find the end of the file
I32 CF_WrappedLseek(Os::FileHandle fd, I32 offset, int mode)
{}

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

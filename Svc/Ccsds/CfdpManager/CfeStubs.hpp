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
// Note some of these are CF wrappers around OSAL calls
// The only difference is the addition of performance logging
// ======================================================================

// From <Os/File.hpp>
// Os::FileInterface::Status open(const char* path, Mode mode, OverwriteType overwrite);
// CFE_Status_t CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
I32 CF_WrappedOpenCreate(Os::FileHandle *fd, const char *fname, I32 flags, I32 access)
{}

// Status write(const U8* buffer, FwSizeType& size, WaitType wait)
// CFE_Status_t CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
I32 CF_WrappedWrite(Os::FileHandle fd, const void *buf, size_t write_size)
{}

// Status read(U8* buffer, FwSizeType& size);
// CFE_Status_t CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
I32 CF_WrappedRead(Os::FileHandle fd, void *buf, size_t read_size)
{}

// CFE_Status_t CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)

// BPC: This is being used as a file open check
// BPC: I am replacing this with the `isOpen()` function instead of the
//      `getHandle()` function to match the intent of how the function is used
// bool isOpen() const;
// static inline bool OS_ObjectIdDefined(osal_id_t object_id)
inline bool OS_ObjectIdDefined(Os::FileHandle object_id)
{}

// void close()
// void CF_WrappedClose(osal_id_t fd)
void CF_WrappedClose(Os::FileHandle fd)
{}

// #define OS_SEEK_SET 0 /**< Seek offset set */ BPC: The file offset is set to offset bytes.
// #define OS_SEEK_CUR 1 /**< Seek offset current */ BPC: The file offset is set to its current location plus offset bytes.
// #define OS_SEEK_END 2 /**< Seek offset end */ BPC: The file offset is set to the size of the file plus offset bytes.
// Status seek(FwSignedSizeType offset, SeekType seekType);
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
// Status Directory::open(const char* path, OpenMode mode);
// int32 OS_DirectoryOpen(osal_id_t *dir_id, const char *path)
I32 OS_DirectoryOpen(Os::DirectoryHandle *dir_id, const char *path)
{}


}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDP_CFE_STUBS_HPP

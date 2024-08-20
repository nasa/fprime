// ======================================================================
// \title Os/Posix/FileSystem.hpp
// \brief Posix FileSystem definitions for Os::FileSystem
// ======================================================================
#include "Os/FileSystem.hpp"

#ifndef OS_POSIX_FILESYSTEM_HPP
#define OS_POSIX_FILESYSTEM_HPP
namespace Os {
namespace Posix {
namespace FileSystem {

struct PosixFileSystemHandle : public FileSystemHandle {};

//! \brief Posix implementation of Os::FileSystem
//!
//! Posix implementation of `FileSystemInterface` for use as a delegate class handling error-only fileSystem operations.
//!
class PosixFileSystem : public FileSystemInterface {
  public:
    //! \brief constructor
    //!
    PosixFileSystem() = default;

    //! \brief destructor
    //!
    ~PosixFileSystem() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief create a new directory at location path
    Status _createDirectory(const char* path) override;
    //! \brief remove a directory at location path
    Status _removeDirectory(const char* path) override;
    //! \brief read the contents of a directory.  Size of fileArray should be maxNum. Cleaner implementation found in Directory.hpp
    Status _readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles) override;
    //! \brief removes a file at location path
    Status _removeFile(const char* path) override;
    //! \brief moves a file from origin to destination
    Status _moveFile(const char* originPath, const char* destPath) override;
    //! \brief copies a file from origin to destination
    Status _copyFile(const char* originPath, const char* destPath) override;
    //! \brief append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
    Status _appendFile(const char* originPath, const char* destPath, bool createMissingDest=false) override;
    //! \brief gets the size of the file (in bytes) = 0 at location path
    Status _getFileSize(const char* path, FwSignedSizeType& size) override;
    //! \brief counts the number of files in the given directory
    Status _getFileCount(const char* directory, U32& fileCount) override;
    //! \brief move current directory to path
    Status _changeWorkingDirectory(const char* path) override;
    //! \brief get FS free and total space in bytes on filesystem containing path
    Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;



    //! \brief returns the raw fileSystem handle
    //!
    //! Gets the raw fileSystem handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it//!must* be passed as an opaque type.
    //!
    //! \return raw fileSystem handle
    //!
    FileSystemHandle *getHandle() override;


  private:
    //! FileSystem handle for PosixFileSystem
    PosixFileSystemHandle m_handle;
}; // class PosixFileSystem

// //! \brief a local namespace for file operation helper functions
// namespace FileOperationHelper {
// PosixFileSystem::Status handleFileError(File::Status fileStatus);
// PosixFileSystem::Status initAndCheckFileStats(const char* filePath, struct stat* fileInfo = nullptr);
// PosixFileSystem::Status copyFileData(File& source, File& destination, FwSignedSizeType size);

// }
} // namespace FileSystem
} // namespace Posix
} // namespace Os
#endif // OS_POSIX_FILESYSTEM_HPP

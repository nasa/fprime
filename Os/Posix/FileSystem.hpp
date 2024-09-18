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
    PosixFileSystem() = default;

    //! \brief destructor
    ~PosixFileSystem() override = default;

    // ------------------------------------------------------------
    // Implementation-specific member functions - overrides
    // ------------------------------------------------------------

    //! \brief Remove a directory at the specified path
    //! \param path The path of the directory to remove
    //! \return Status of the operation
    Status _removeDirectory(const char* path) override;

    //! \brief Remove a file at the specified path
    //! \param path The path of the file to remove
    //! \return Status of the operation
    Status _removeFile(const char* path) override;

    //! \brief Rename a file from source to destination
    //! 
    //! If the rename fails due to a cross-device operation, this function should return EXDEV_ERROR
    //! and moveFile should be used instead.
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    Status _rename(const char* sourcePath, const char* destPath) override;

    //! \brief Get filesystem free and total space in bytes on the filesystem containing the specified path
    //! \param path The path on the filesystem to query
    //! \param totalBytes Reference to store the total bytes on the filesystem
    //! \param freeBytes Reference to store the free bytes on the filesystem
    //! \return Status of the operation
    Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;

    //! \brief Get the current working directory
    //! \param path Buffer to store the current working directory path
    //! \param bufferSize Size of the buffer
    //! \return Status of the operation
    Status _getWorkingDirectory(char* path, FwSizeType bufferSize) override;

    //! \brief Change the current working directory to the specified path
    //! \param path The path of the new working directory
    //! \return Status of the operation
    Status _changeWorkingDirectory(const char* path) override;



    //! \brief returns the raw fileSystem handle
    //!
    //! Gets the raw fileSystem handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it//!must* be passed as an opaque type.
    //!
    //! \return raw fileSystem handle
    FileSystemHandle *getHandle() override;


  private:
    //! FileSystem handle for PosixFileSystem
    PosixFileSystemHandle m_handle;
}; // class PosixFileSystem

} // namespace FileSystem
} // namespace Posix
} // namespace Os
#endif // OS_POSIX_FILESYSTEM_HPP
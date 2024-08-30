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
    //! \brief removes a file at location path
    Status _removeFile(const char* path) override;
    //! \brief moves a file from origin to destination
    Status _moveFile(const char* originPath, const char* destPath) override;
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
    FileSystemHandle *getHandle() override;


  private:
    //! FileSystem handle for PosixFileSystem
    PosixFileSystemHandle m_handle;
}; // class PosixFileSystem

} // namespace FileSystem
} // namespace Posix
} // namespace Os
#endif // OS_POSIX_FILESYSTEM_HPP

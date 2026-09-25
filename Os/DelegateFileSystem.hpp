// ======================================================================
// \title Os/DelegateFileSystem.hpp
// \brief Define the Os::DelegateFileSystem class
// ======================================================================

#ifndef OS_DELEGATEFILESYSTEM_HPP_
#define OS_DELEGATEFILESYSTEM_HPP_

#include <Os/FileSystemInterface.hpp>

namespace Os {

//! \brief FileSystem class
//!
//! This class provides a common interface for file system operations.
//! This class uses the singleton pattern and should be accessed through
//! its static functions, for example using `Os::FileSystem::removeFile(path)`.
class DelegateFileSystem final : public FileSystemInterface {
    // Grants FileSystemInterface's convenience static wrappers access to the private constructor below
    friend class FileSystemInterface;

  private:
    DelegateFileSystem();  //!<  Constructor (private because singleton pattern)
  public:
    ~DelegateFileSystem() final;  //!<  Destructor

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal FileSystem handle representation
    FileSystemHandle* getHandle() override;

    // ------------------------------------------------------------
    // Implementation-specific FileSystem member functions
    // ------------------------------------------------------------

    //! \brief Remove a directory at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the directory to remove
    //! \return Status of the operation
    Status _removeDirectory(const char* path) override;

    //! \brief Remove a file at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the file to remove
    //! \return Status of the operation
    Status _removeFile(const char* path) override;

    //! \brief Rename a file from source to destination
    //!
    //! If the rename fails due to a cross-device operation, this function should return EXDEV_ERROR
    //! and moveFile can be used instead to force a copy-and-remove.
    //!
    //! It is invalid to pass `nullptr` as sourcePath or destPath.
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    Status _rename(const char* sourcePath, const char* destPath) override;

    //! \brief Get filesystem free and total space in bytes on the filesystem containing the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path on the filesystem to query
    //! \param totalBytes Reference to store the total bytes on the filesystem
    //! \param freeBytes Reference to store the free bytes on the filesystem
    //! \return Status of the operation
    Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;

    //! \brief Get the current working directory
    //!
    //! Writes the current working directory path to the provided buffer of size bufferSize.
    //! If the buffer is too small to hold the full path, the function will return BUFFER_TOO_SMALL.
    //!
    //! It is invalid to pass `nullptr` as the path.
    //! It is invalid to pass a bufferSize of 0.
    //!
    //! \param path Buffer to store the current working directory path
    //! \param bufferSize Size of the buffer
    //! \return Status of the operation
    Status _getWorkingDirectory(char* path, FwSizeType bufferSize) override;

    //! \brief Change the current working directory to the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the new working directory
    //! \return Status of the operation
    Status _changeWorkingDirectory(const char* path) override;

    //! \brief Get the type of the path (file, directory, etc.)
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path to check
    //! \param pathType Reference to store the path type
    //! \return Status of the operation
    Status _getPathType(const char* path, PathType& pathType) override;

    // Convenience static wrappers (removeDirectory, removeFile, rename, getWorkingDirectory,
    // changeWorkingDirectory, getFreeSpace, exists, getPathType, touch, createDirectory, appendFile,
    // copyFile, moveFile, getFileSize) are inherited from Os::FileSystemInterface.

  public:
    //! \brief initialize singleton
    static void init();

    //! \brief get a reference to singleton
    //! \return reference to singleton
    static DelegateFileSystem& getSingleton();

  private:
    // This section is used to store the implementation-defined FileSystem handle. To Os::FileSystem and fprime, this
    // type is opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle
    // in the byte-array here and set `handle` to that address for storage.

    alignas(FW_HANDLE_ALIGNMENT) FileSystemHandleStorage m_handle_storage;  //!< FileSystem handle storage
    FileSystemInterface& m_delegate;
};

}  // namespace Os

#endif  // OS_DELEGATEFILESYSTEM_HPP_

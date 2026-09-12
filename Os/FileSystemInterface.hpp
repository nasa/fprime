// ======================================================================
// \title Os/FileSystemInterface.hpp
// \brief Os::FileSystemHandle and Os::FileSystemInterface definitions
// ======================================================================

#ifndef OS_FILESYSTEMINTERFACE_HPP_
#define OS_FILESYSTEMINTERFACE_HPP_

#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Os.hpp>
#include "config/OsDelegateFileSystem.hpp"

namespace Os {

struct FileSystemHandle {};

class FileSystemInterface {
  public:
    // Size of file chunks to use for file system operations (e.g. copyFile)
    static constexpr FwSizeType FILE_SYSTEM_FILE_CHUNK_SIZE = FW_FILE_CHUNK_SIZE;  //!< Size of file system chunk

    enum Status {
        OP_OK,             //!<  Operation was successful
        ALREADY_EXISTS,    //!<  File already exists
        NO_SPACE,          //!<  No space left
        NO_PERMISSION,     //!<  No permission to write
        NOT_DIR,           //!<  Path is not a directory
        IS_DIR,            //!< Path is a directory
        NOT_EMPTY,         //!<  directory is not empty
        INVALID_PATH,      //!< Path is too long, too many sym links, etc.
        DOESNT_EXIST,      //!<  Path doesn't exist
        FILE_LIMIT,        //!< Too many files or links
        BUSY,              //!< Operand is in use by the system or by a process
        NO_MORE_FILES,     //!<  Directory stream has no more files
        BUFFER_TOO_SMALL,  //!<  Buffer size is too small to hold full path (for getWorkingDirectory)
        EXDEV_ERROR,       // Operation not supported across devices (e.g. rename)
        OVERFLOW_ERROR,    // Operation failed due to overflow in calculation of the result
        NOT_SUPPORTED,     //!<  Operation is not supported by the current implementation
        OTHER_ERROR,       //!<  other OS-specific error
    };

    enum PathType {
        FILE,       //!< Path is a file
        DIRECTORY,  //!< Path is a directory
        OTHER,      //!< Path is not a file or directory, e.g. a socket
        NOT_EXIST,  //!< Path does not exist
    };

    //! \brief default constructor
    FileSystemInterface() = default;

    //! \brief default virtual destructor
    virtual ~FileSystemInterface() = default;

    //! \brief copy constructor is forbidden
    FileSystemInterface(const FileSystemInterface& other) = delete;

    //! \brief assignment operator is forbidden
    FileSystemInterface& operator=(const FileSystemInterface& other) = delete;  // NO_CODESONAR (cpp:S3657)

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal FileSystem handle representation
    virtual FileSystemHandle* getHandle() = 0;

    //! \brief provide a pointer to a FileSystem delegate object
    static FileSystemInterface* getDelegate(FileSystemHandleStorage& aligned_new_memory);

    // ------------------------------------------------------------------
    // FileSystem operations to be implemented by an OSAL implementation
    // ------------------------------------------------------------------
    // These functions are to be overridden in each OS implementation
    // See an example in in Os/Posix/FileSystem.hpp

    //! \brief Remove a directory at the specified path
    //! \param path The path of the directory to remove
    //! \return Status of the operation
    virtual Status _removeDirectory(const char* path) = 0;

    //! \brief Remove a file at the specified path
    //! \param path The path of the file to remove
    //! \return Status of the operation
    virtual Status _removeFile(const char* path) = 0;

    //! \brief Rename (or move) a file from source to destination
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    virtual Status _rename(const char* sourcePath, const char* destPath) = 0;

    //! \brief Get filesystem free and total space in bytes on the filesystem containing the specified path
    //! \param path The path on the filesystem to query
    //! \param totalBytes Reference to store the total bytes on the filesystem
    //! \param freeBytes Reference to store the free bytes on the filesystem
    //! \return Status of the operation
    virtual Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) = 0;

    //! \brief Get the type of the path (file, directory, etc.)
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path to check
    //! \param pathType Reference to store the path type
    //! \return Status of the operation
    virtual Status _getPathType(const char* path, PathType& pathType) = 0;

    //! \brief Get the current working directory
    //! \param path Buffer to store the current working directory path
    //! \param bufferSize Size of the buffer
    //! \return Status of the operation
    virtual Status _getWorkingDirectory(char* path, FwSizeType bufferSize) = 0;

    //! \brief Change the current working directory to the specified path
    //! \param path The path of the new working directory
    //! \return Status of the operation
    virtual Status _changeWorkingDirectory(const char* path) = 0;

    // ------------------------------------------------------------------
    // Convenience static wrappers
    // ------------------------------------------------------------------
    // These are implemented in terms of the virtual instance functions above,
    // and are inherited (via ordinary class-scope lookup) by every concrete
    // Os::FileSystem backend (e.g. Os::DelegateFileSystem, Os::Stub::FileSystem::StubFileSystem),
    // regardless of whether that backend is a link-time delegate or a direct
    // compile-time alias.

    //! \brief Remove a directory at the specified path
    //! \param path The path of the directory to remove
    //! \return Status of the operation
    static Status removeDirectory(const char* path);

    //! \brief Remove a file at the specified path
    //! \param path The path of the file to remove
    //! \return Status of the operation
    static Status removeFile(const char* path);

    //! \brief Rename (or move) a file from source to destination
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    static Status rename(const char* sourcePath, const char* destPath);

    //! \brief Get filesystem free and total space in bytes on the filesystem containing the specified path
    //! \param path The path on the filesystem to query
    //! \param totalBytes Reference to store the total bytes on the filesystem
    //! \param freeBytes Reference to store the free bytes on the filesystem
    //! \return Status of the operation
    static Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes);

    //! \brief Get the current working directory
    //! \param path Buffer to store the current working directory path
    //! \param bufferSize Size of the buffer
    //! \return Status of the operation
    static Status getWorkingDirectory(char* path, FwSizeType bufferSize);

    //! \brief Change the current working directory to the specified path
    //! \param path The path of the new working directory
    //! \return Status of the operation
    static Status changeWorkingDirectory(const char* path);

    //! \brief Return true if the path exists, false otherwise
    //! \param path The path to check for existence
    //! \return True if the path exists, false otherwise
    static bool exists(const char* path);

    //! \brief Return the type of the path (file, directory, or doesn't exist)
    //! \param path The path to check for existence
    //! \return PathType enum representing the type of the path (FILE, DIRECTORY, NOT_EXIST)
    static PathType getPathType(const char* path);

    //! \brief Touch a file at the specified path, creating it if it doesn't exist
    //! \param path The path of the file to touch
    //! \return Status of the operation
    static Status touch(const char* path);

    //! \brief Create a new directory at the specified path.
    //!
    //! The optional errorIfAlreadyExists (default=false) parameter can be set to true
    //! to return an error status if the directory already exists.
    //!
    //! \param path The path where the new directory will be created
    //! \param errorIfAlreadyExists If true, returns an error if the directory already exists
    //! \return Status of the operation
    static Status createDirectory(const char* path, bool errorIfAlreadyExists = false);

    //! \brief Append the source file to the destination file
    //!
    //! This function opens both files, and iteratively reads the source by chunks and writes
    //! chunks to the destination.
    //! If the destination file does not exist and createMissingDest is true, a new file is created.
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \param createMissingDest If true, creates a new file if the destination doesn't exist
    //! \return Status of the operation
    static Status appendFile(const char* sourcePath, const char* destPath, bool createMissingDest = false);

    //! \brief Copy a file from the source path to the destination path
    //!
    //! This function opens both files, and iteratively reads the source by chunks and writes
    //! chunks to the destination.
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    static Status copyFile(const char* sourcePath, const char* destPath);

    //! \brief Move a file from sourcePath to destPath
    //!
    //! This is done by first trying to rename, and if renaming fails,
    //! copy it and then remove the original
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    static Status moveFile(const char* sourcePath, const char* destPath);

    //! \brief Get the size of the file (in bytes) at the specified path
    //! \param path The path of the file
    //! \param size Reference to store the size of the file
    //! \return Status of the operation
    static Status getFileSize(const char* path, FwSizeType& size);
};

}  // namespace Os

#endif  // OS_FILESYSTEMINTERFACE_HPP_

// ======================================================================
// \title Os/FileSystem.hpp
// \brief Os::FileSystem interface definition
// ======================================================================

#ifndef _OS_FILESYSTEM_HPP_
#define _OS_FILESYSTEM_HPP_

#include <FpConfig.hpp>
#include <Os/Os.hpp>
#include <Os/Directory.hpp>
#include <Os/File.hpp>

namespace Os {

struct FileSystemHandle {};

class FileSystemInterface {
  public:

    // Size of file chunks to use for file system operations (e.g. copyFile)
    static constexpr FwSignedSizeType FILE_SYSTEM_FILE_CHUNK_SIZE = FW_FILE_CHUNK_SIZE; //!< Size of file system chunk

    enum Status {
        OP_OK, //!<  Operation was successful
        ALREADY_EXISTS, //!<  File already exists
        NO_SPACE, //!<  No space left
        NO_PERMISSION, //!<  No permission to write
        NOT_DIR, //!<  Path is not a directory
        IS_DIR, //!< Path is a directory
        NOT_EMPTY, //!<  directory is not empty
        INVALID_PATH, //!< Path is too long, too many sym links, etc.
        DOESNT_EXIST, //!<  Path doesn't exist
        FILE_LIMIT, //!< Too many files or links
        BUSY, //!< Operand is in use by the system or by a process
        NO_MORE_FILES, //!<  Directory stream has no more files
        BUFFER_TOO_SMALL, //!<  Buffer size is too small to hold full path (for getWorkingDirectory)
        EXDEV_ERROR, // Operation not supported across devices (e.g. rename)
        OVERFLOW_ERROR, // Operation failed due to overflow in calculation of the result
        NOT_SUPPORTED, //!<  Operation is not supported by the current implementation
        OTHER_ERROR, //!<  other OS-specific error
    };

    enum PathType {
        FILE,      //!< Path is a file
        DIRECTORY, //!< Path is a directory
        NOT_EXIST, //!< Path does not exist
    };

    //! \brief default constructor
    FileSystemInterface() = default;

    //! \brief default virtual destructor
    virtual ~FileSystemInterface() = default;

    //! \brief copy constructor is forbidden
    FileSystemInterface(const FileSystemInterface& other) = delete;

    //! \brief assignment operator is forbidden
    FileSystemInterface& operator=(const FileSystemInterface& other) = delete;

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

    //! \brief Get the current working directory
    //! \param path Buffer to store the current working directory path
    //! \param bufferSize Size of the buffer
    //! \return Status of the operation
    virtual Status _getWorkingDirectory(char* path, FwSizeType bufferSize) = 0;

    //! \brief Change the current working directory to the specified path
    //! \param path The path of the new working directory
    //! \return Status of the operation
    virtual Status _changeWorkingDirectory(const char* path) = 0;

};

//! \brief FileSystem class
//!
//! This class provides a common interface for file system operations.
//! This class uses the singleton pattern and should be accessed through
//! its static functions, for example using `Os::FileSystem::removeFile(path)`.
class FileSystem final : public FileSystemInterface {
  private:
    FileSystem();         //!<  Constructor (private because singleton pattern)
  public:
    ~FileSystem() final;  //!<  Destructor

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


    // ------------------------------------------------------------
    // Implementation-specific FileSystem static functions
    // ------------------------------------------------------------
    // These are static variants that are exposed to the user, and call the above member functions

    //! \brief Remove a directory at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the directory to remove
    //! \return Status of the operation
    static Status removeDirectory(const char* path);

    //! \brief Remove a file at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the file to remove
    //! \return Status of the operation
    static Status removeFile(const char* path);

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
    static Status rename(const char* sourcePath, const char* destPath);

    //! \brief Get filesystem free and total space in bytes on the filesystem containing the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path on the filesystem to query
    //! \param totalBytes Reference to store the total bytes on the filesystem
    //! \param freeBytes Reference to store the free bytes on the filesystem
    //! \return Status of the operation
    static Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes);

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
    static Status getWorkingDirectory(char* path, FwSizeType bufferSize);

    //! \brief Change the current working directory to the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the new working directory
    //! \return Status of the operation
    static Status changeWorkingDirectory(const char* path);


    // ------------------------------------------------------------
    // Additional functions built on top of OS-specific operations
    // ------------------------------------------------------------

    //! \brief Return true if the path exists, false otherwise
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path to check for existence
    //! \return True if the path exists, false otherwise
    static bool exists(const char* path);

    //! \brief Return the type of the path (file, directory, or doesn't exist)
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path to check for existence
    //! \return PathType enum representing the type of the path (FILE, DIRECTORY, NOT_EXIST)
    static PathType getPathType(const char* path);
    
    //! \brief Touch a file at the specified path, creating it if it doesn't exist
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the file to touch
    //! \return Status of the operation
    static Status touch(const char* path);
    
    //! \brief Create a new directory at the specified path.
    //!
    //! The optional errorIfAlreadyExists (default=false) parameter can be set to true 
    //! to return an error status if the directory already exists.
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path where the new directory will be created
    //! \param errorIfAlreadyExists If true, returns an error if the directory already exists
    //! \return Status of the operation
    static Status createDirectory(const char* path, bool errorIfAlreadyExists=false);
    
    //! \brief Append the source file to the destination file
    //!
    //! This function opens both files, and iteratively reads the source by chunks and writes
    //! chunks to the destination.
    //! If the destination file does not exist and createMissingDest is true, a new file is created.
    //!
    //! It is invalid to pass `nullptr` as either the source or destination path.
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \param createMissingDest If true, creates a new file if the destination doesn't exist
    //! \return Status of the operation
    static Status appendFile(const char* sourcePath, const char* destPath, bool createMissingDest=false);
    
    //! \brief Copy a file from the source path to the destination path
    //!
    //! This function opens both files, and iteratively reads the source by chunks and writes
    //! chunks to the destination.
    //!
    //! It is invalid to pass `nullptr` as either the source or destination path.
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
    //! It is invalid to pass `nullptr` as either the source or destination path.
    //!
    //! \param sourcePath The path of the source file
    //! \param destPath The path of the destination file
    //! \return Status of the operation
    static Status moveFile(const char* sourcePath, const char* destPath);
    
    //! \brief Get the size of the file (in bytes) at the specified path
    //!
    //! It is invalid to pass `nullptr` as the path.
    //!
    //! \param path The path of the file
    //! \param size Reference to store the size of the file
    //! \return Status of the operation
    static Status getFileSize(const char* path, FwSignedSizeType& size);


  public:
    //! \brief initialize singleton
    static void init();

    //! \brief get a reference to singleton
    //! \return reference to singleton
    static FileSystem& getSingleton();

  private:
    // ------------------------------------------------------------
    // Internal helper functions
    // ------------------------------------------------------------

    //! \brief Convert a File::Status to a FileSystem::Status
    static Status handleFileError(File::Status fileStatus);

    //! \brief Convert a Directory::Status to a FileSystem::Status
    static Status handleDirectoryError(Directory::Status dirStatus);

    //! \brief A helper function that writes all the file information in the source
    //! file to the destination file (replaces/appends to end/etc. depending
    //! on destination file mode).
    //!
    //! Files must already be open and will remain open after this function
    //! completes.
    //!
    //! @param source File to copy data from
    //! @param destination File to copy data to
    //! @param size The number of bytes to copy
    static Status copyFileData(File& source, File& destination, FwSignedSizeType size);

  private:
    // This section is used to store the implementation-defined FileSystem handle. To Os::FileSystem and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.

    alignas(FW_HANDLE_ALIGNMENT) FileSystemHandleStorage m_handle_storage;  //!< FileSystem handle storage
    FileSystemInterface& m_delegate;          
};



}

#endif

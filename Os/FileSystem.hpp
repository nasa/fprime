#ifndef _OS_FILESYSTEM_HPP_
#define _OS_FILESYSTEM_HPP_

#include <FpConfig.hpp>
#include "config/FppConstantsAc.hpp"
#include <Os/Os.hpp>
#include <Fw/Types/String.hpp>

#define FILE_SYSTEM_CHUNK_SIZE (256u)

namespace Os {

struct FileSystemHandle {};

class FileSystemInterface {
  public:
    static constexpr FwSizeType FPP_CONFIG_FILENAME_MAX_SIZE = FppConstant_FileNameStringSize::FileNameStringSize;

    typedef enum {
        OP_OK, //!<  Operation was successful
        ALREADY_EXISTS, //!<  File already exists
        NO_SPACE, //!<  No space left
        NO_PERMISSION, //!<  No permission to write
        NOT_DIR, //!<  Path is not a directory
        IS_DIR, //!< Path is a directory
        NOT_EMPTY, //!<  directory is not empty
        INVALID_PATH, //!< Path is too long, too many sym links, doesn't exist, ect
        FILE_LIMIT, //!< Too many files or links
        BUSY, //!< Operand is in use by the system or by a process
        DIR_DOESNT_EXIST, //!<  Directory doesn't exist
        DIR_NOT_OPENED, //!<  Directory hasn't been opened yet
        NO_MORE_FILES, //!<  Directory stream has no more files
        NOT_SUPPORTED, //!<  Operation is not supported by the current implementation
        OTHER_ERROR, //!<  other OS-specific error
    } Status;

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
    static FileSystemInterface* getDelegate(HandleStorage& aligned_new_memory);


    //------------ Os-specific FileSystem Functions ------------

    //! \brief create a new directory at location path
    virtual Status _createDirectory(const char* path) = 0;
    //! \brief remove a directory at location path
    virtual Status _removeDirectory(const char* path) = 0;
    //! \brief removes a file at location path
    virtual Status _removeFile(const char* path) = 0;
    //! \brief moves a file from origin to destination
    virtual Status _moveFile(const char* originPath, const char* destPath) = 0;
    //! \brief copies a file from origin to destination
    virtual Status _copyFile(const char* originPath, const char* destPath) = 0;
    //! \brief append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
    virtual Status _appendFile(const char* originPath, const char* destPath, bool createMissingDest=false) = 0;
    //! \brief gets the size of the file (in bytes) = 0 at location path
    virtual Status _getFileSize(const char* path, FwSignedSizeType& size) = 0;
    //! \brief counts the number of files in the given directory
    virtual Status _getFileCount(const char* directory, U32& fileCount) = 0;
    //! \brief move current directory to path
    virtual Status _changeWorkingDirectory(const char* path) = 0;
    //! \brief get FS free and total space in bytes on filesystem containing path
    virtual Status _getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) = 0;

};


class FileSystem final : public FileSystemInterface {
  private:
    FileSystem();         //!<  Constructor (private because singleton pattern)
  public:
    ~FileSystem() final;  //!<  Destructor

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal FileSystem handle representation
    FileSystemHandle* getHandle() override;

    //! \brief create a new directory at location path
    Status _createDirectory(const char* path) override;
    //! \brief remove a directory at location path
    Status _removeDirectory(const char* path) override;
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



    //! \brief create a new directory at location path
    static Status createDirectory(const char* path);
    //! \brief remove a directory at location path
    static Status removeDirectory(const char* path);
    //! \brief removes a file at location path
    static Status removeFile(const char* path);
    //! \brief moves a file from origin to destination
    static Status moveFile(const char* originPath, const char* destPath);
    //! \brief copies a file from origin to destination
    static Status copyFile(const char* originPath, const char* destPath);
    //! \brief append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
    static Status appendFile(const char* originPath, const char* destPath, bool createMissingDest=false);
    //! \brief gets the size of the file (in bytes) = 0 at location path
    static Status getFileSize(const char* path, FwSignedSizeType& size);
    //! \brief counts the number of files in the given directory
    static Status getFileCount(const char* directory, U32& fileCount);
    //! \brief move current directory to path
    static Status changeWorkingDirectory(const char* path);
    //! \brief get FS free and total space in bytes on filesystem containing path
    static Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes);

    //! \brief read the contents of a directory.  Size of fileArray should be maxNum. Cleaner implementation found in Directory.hpp
    static Status readDirectory(const char* path,  const U32 maxNum, Fw::String fileArray[], U32& numFiles);
    //! \brief Open directory returning a Os::Directory object
    // static Os::Directory openDirectory(const char* path);


  public:
    //! \brief initialize singleton
    static void init();
    //! \brief get a reference to singleton
    //! \return reference to singleton
    static FileSystem& getSingleton();

  private:
    static FileSystem* s_singleton;

    // This section is used to store the implementation-defined FileSystem handle. To Os::FileSystem and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< FileSystem handle storage
    FileSystemInterface& m_delegate;          
};



}

#endif

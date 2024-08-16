#ifndef _FileSystem_hpp_
#define _FileSystem_hpp_

#include <FpConfig.hpp>
#include <Fw/Types/FileNameString.hpp>

#define FILE_SYSTEM_CHUNK_SIZE (256u)

namespace Os {

struct FileSystemHandle {};

class FileSystemInterface {
  public:
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
        NOT_DIR, //!<  Path is not a directory
        NO_MORE_FILES, //!<  Directory stream has no more files
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
    virtual Status createDirectory(const char* path) = 0;
    //! \brief remove a directory at location path
    virtual Status removeDirectory(const char* path) = 0;
    //! \brief read the contents of a directory.  Size of fileArray should be maxNum. Cleaner implementation found in Directory.hpp
    virtual Status readDirectory(const char* path,  const U32 maxNum, Fw::FileNameString fileArray[], U32& numFiles) = 0;
    //! \brief removes a file at location path
    virtual Status removeFile(const char* path) = 0;
    //! \brief moves a file from origin to destination
    virtual Status moveFile(const char* originPath, const char* destPath) = 0;
    //! \brief copies a file from origin to destination
    virtual Status copyFile(const char* originPath, const char* destPath) = 0;
    //! \brief append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
    virtual Status appendFile(const char* originPath, const char* destPath, bool createMissingDest=false) = 0;
    //! \brief gets the size of the file (in bytes) = 0 at location path
    virtual Status getFileSize(const char* path, FwSignedSizeType& size) = 0;
    //! \brief counts the number of files in the given directory
    virtual Status getFileCount(const char* directory, U32& fileCount) = 0;
    //! \brief move current directory to path
    virtual Status changeWorkingDirectory(const char* path) = 0;
    //! \brief get FS free and total space in bytes on filesystem containing path
    virtual Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) = 0;

};


class FileSystem final : public FileSystemInterface {
  public:
    FileSystem();         //!<  Constructor. FileSystem is unlocked when created
    ~FileSystem() final;  //!<  Destructor

    //! \brief return the underlying FileSystem handle (implementation specific)
    //! \return internal FileSystem handle representation
    FileSystemHandle* getHandle() override;

    //! \brief create a new directory at location path
    Status createDirectory(const char* path) override;
    //! \brief remove a directory at location path
    Status removeDirectory(const char* path) override;
    //! \brief read the contents of a directory.  Size of fileArray should be maxNum. Cleaner implementation found in Directory.hpp
    Status readDirectory(const char* path,  const U32 maxNum, Fw::FileNameString fileArray[], U32& numFiles) override;;
    //! \brief removes a file at location path
    Status removeFile(const char* path) override;
    //! \brief moves a file from origin to destination
    Status moveFile(const char* originPath, const char* destPath) override;;
    //! \brief copies a file from origin to destination
    Status copyFile(const char* originPath, const char* destPath) override;;
    //! \brief append file origin to destination file. If boolean true, creates a brand new file if the destination doesn't exist.
    Status appendFile(const char* originPath, const char* destPath, bool createMissingDest=false) override;;
    //! \brief gets the size of the file (in bytes) = 0 at location path
    Status getFileSize(const char* path, FwSignedSizeType& size) override;;
    //! \brief counts the number of files in the given directory
    Status getFileCount(const char* directory, U32& fileCount) override;;
    //! \brief move current directory to path
    Status changeWorkingDirectory(const char* path) override;;
    //! \brief get FS free and total space in bytes on filesystem containing path
    Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) override;;


  private:
        // This section is used to store the implementation-defined FileSystem handle. To Os::FileSystem and fprime, this type is
        // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
        // the byte-array here and set `handle` to that address for storage.
        //
        alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< FileSystem handle storage
        FileSystemInterface& m_delegate;          
    };



}

#endif

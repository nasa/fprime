// ======================================================================
// \title Os/Directory.hpp
// \brief Os::Directory interface definition
// ======================================================================

#ifndef _OS_DIRECTORY_HPP_
#define _OS_DIRECTORY_HPP_

#include <FpConfig.hpp>
#include <Os/Os.hpp>
#include <Fw/Types/String.hpp>

namespace Os {

struct DirectoryHandle {};

class DirectoryInterface {
  public:
    enum Status {
        OP_OK, //!<  Operation was successful
        DOESNT_EXIST, //!<  Directory doesn't exist
        NO_PERMISSION, //!<  No permission to read directory
        NOT_OPENED, //!<  Directory hasn't been opened yet
        NOT_DIR, //!<  Path is not a directory
        NO_MORE_FILES, //!<  Directory stream has no more files
        FILE_LIMIT, //!<  Directory has more files than can be read
        BAD_DESCRIPTOR, //!<  Directory stream descriptor is invalid
        ALREADY_EXISTS, //!<  Directory already exists
        NOT_SUPPORTED, //!<  Operation is not supported by the current implementation
        OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
    };

    enum OpenMode {
        READ,               //!<  Error if directory doesn't exist
        CREATE_IF_MISSING,  //!<  Create directory if it doesn't exist
        CREATE_EXCLUSIVE,   //!<  Create directory and error if it already exists
    };

    //! \brief default constructor
    DirectoryInterface() = default;

    //! \brief default virtual destructor
    virtual ~DirectoryInterface() = default;

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    virtual DirectoryHandle* getHandle() = 0;

    //! \brief provide a pointer to a Directory delegate object
    static DirectoryInterface* getDelegate(HandleStorage& aligned_new_memory);


    // -----------------------------------------------------------------
    // Directory operations to be implemented by an OSAL implementation
    // -----------------------------------------------------------------
    // These functions are to be overridden in each OS implementation
    // See an example in in Os/Posix/Directory.hpp


    //! \brief Open or create a directory
    //!
    //! Using the path provided, this function will open or create a directory. 
    //! Use OpenMode::READ to open an existing directory and error if the directory is not found
    //! Use OpenMode::CREATE_IF_MISSING to open a directory, creating the directory if it doesn't exist
    //! Use OpenMode::CREATE_EXCLUSIVE to open a directory, creating the directory and erroring if it already exists
    //!
    //! \param path: path of directory to open
    //! \param mode: enum (READ, CREATE_IF_MISSING, CREATE_EXCLUSIVE). See notes above for more information
    //! \return status of the operation
    virtual Status open(const char* path, OpenMode mode) = 0;

    //! \brief Check if Directory is open or not
    //! \return true if Directory is open, false otherwise
    virtual bool isOpen() = 0;

    //! \brief Rewind directory.
    //! Each read operation will move the seek position forward. This function resets the seek position to the beginning.
    //! \return status of the operation
    virtual Status rewind() = 0;

    //! \brief Get next filename from directory stream and write it to fileNameBuffer of size buffSize
    //! \param fileNameBuffer: buffer to store filename
    //! \param buffSize: size of fileNameBuffer
    //! \return status of the operation
    virtual Status read(char * fileNameBuffer, FwSizeType buffSize) = 0;

    //! \brief Get next filename from directory stream and write it to a Fw::StringBase object
    //! \param filename: Fw::StringBase (or derived) object to store filename in
    //! \return status of the operation
    virtual Status read(Fw::StringBase& filename) = 0;

    //! \brief Close directory
    virtual void close() = 0;


};

//! \brief Directory class
//!
//! This class provides a common interface for directory operations, such as reading files in a directory
//! and getting the number of files in a directory.
class Directory final : public DirectoryInterface {
  public:
    Directory();         //!<  Constructor (private because singleton pattern)
    ~Directory() final;  //!<  Destructor

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    DirectoryHandle* getHandle() override;


    // ------------------------------------------------------------
    // Implementation-specific Directory member functions
    // ------------------------------------------------------------
    // These functions are overridden in each OS implementation (e.g. in Os/Posix/Directory.hpp)

    //! \brief Open or create a directory
    //!
    //! Using the path provided, this function will open or create a directory. 
    //! Use OpenMode::READ to open an existing directory and error if the directory is not found
    //! Use OpenMode::CREATE to open a directory, creating the directory if it doesn't exist
    //!
    //! \param path: path of directory to open
    //! \param mode: enum (READ, CREATE). See notes above for more information
    //! \return status of the operation
    Status open(const char* path, OpenMode mode) override;

    //! \brief Check if Directory is open or not
    //! \return true if Directory is open, false otherwise
    bool isOpen() override;

    //! \brief Rewind directory.
    //! Each read operation will move the seek position forward. This function resets the seek position to the beginning.
    //! \return status of the operation
    Status rewind() override;

    //! \brief Get next filename from directory stream and write it to fileNameBuffer of size buffSize
    //! \param fileNameBuffer: buffer to store filename
    //! \param buffSize: size of fileNameBuffer
    //! \return status of the operation
    Status read(char * fileNameBuffer, FwSizeType buffSize) override;

    //! \brief Get next filename from directory stream and write it to a Fw::StringBase object
    //! \param filename: Fw::StringBase (or derived) object to store filename in
    //! \return status of the operation
    Status read(Fw::StringBase& filename) override;

    //! \brief Close directory
    void close() override;


    // ------------------------------------------------------------
    // Common functions built on top of OS-specific functions
    // ------------------------------------------------------------

    //! \brief Read the contents of the directory and store filenames in filenameArray of size arraySize.
    //!
    //! Note: the function first rewinds the directory stream to ensure reading starts from the beginning.
    //! After reading, it rewinds the directory stream again, resetting seek position to beginning.
    //!
    //! \param filenameArray: array to store filenames
    //! \param arraySize: size of filenameArray
    //! \param filenameCount: number of filenames written to filenameArray (output)
    //! \return status of the operation
    Status readDirectory(Fw::String filenameArray[], const FwSizeType arraySize, FwSizeType& filenameCount);

    //! \brief Get the number of files in the directory.
    //!
    //! Counts the number of files in the directory by reading each file entry.
    //!
    //! Note: the function first rewinds the directory stream to ensure counting starts from the beginning.
    //! After counting, it rewinds the directory stream again, resetting seek position to beginning.
    //!
    //! \param fileCount Reference to a variable where the file count will be stored.
    //! \return Status indicating the result of the operation.
    Status getFileCount(FwSizeType& fileCount);

  private:
    // This section is used to store the implementation-defined Directory handle. To Os::Directory and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< Directory handle storage
    DirectoryInterface& m_delegate;          
};

}

#endif

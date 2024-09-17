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
        READ,   //!<  Open directory, error if directory doesn't exist
        CREATE, //!<  Open directory, create directory if it doesn't exist
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


    //------------ Os-specific Directory Functions ------------

    // TODO: add comments  back up
    virtual Status open(const char* path, OpenMode mode) = 0; //!<  open/create a directory
    virtual bool isOpen() = 0; //!< check if file descriptor is open or not.
    virtual Status rewind() = 0; //!<  rewind directory stream to the beginning
    virtual Status read(char * fileNameBuffer, FwSizeType buffSize) = 0; //!< get next filename from directory
    virtual Status read(Fw::StringBase& filename) = 0; //!< get next filename from directory
    virtual void close() = 0; //!<  close directory

};

class Directory final : public DirectoryInterface {
  public:
    Directory();         //!<  Constructor (private because singleton pattern)
    ~Directory() final;  //!<  Destructor

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    DirectoryHandle* getHandle() override;

    //------------ Os-specific Directory Functions ------------

    //! \brief Open or create a directory
    //! \param path: path of directory to open
    //! \param mode: enum (READ, CREATE). READ will return an error if directory doesn't exist
    //! \return status of the operation
    Status open(const char* path, OpenMode mode) override;

    //! \brief Check if Directory is open or not
    //! \return true if Directory is open, false otherwise
    bool isOpen() override;

    //! \brief Rewind directory stream to the beginning
    //! \return status of the operation
    Status rewind() override;

    //! \brief Get next filename from directory stream and write it to fileNameBuffer of size buffSize
    //! \param fileNameBuffer: buffer to store filename
    //! \param buffSize: size of fileNameBuffer
    //! \return status of the operation
    Status read(char * fileNameBuffer, FwSizeType buffSize) override;

    //! \brief Get next filename from directory stream and write it to Fw::StringBase object
    //! \param filename: Fw::StringBase (or derived) object to store filename in
    //! \return status of the operation
    Status read(Fw::StringBase& filename) override;

    //! \brief Close directory
    void close() override;

    // ------------ Common Directory Functions (non-OS-specific) ------------

    //! \brief Read the contents of the directory and store filenames in filenameArray of size arraySize
    //! \param filenameArray: array to store filenames
    //! \param arraySize: size of filenameArray
    //! \param filenameCount: number of filenames written to filenameArray (output)
    //! \return status of the operation
    Status readDirectory(Fw::String filenameArray[], const FwSizeType arraySize, FwSizeType& filenameCount);

    //! \brief Get the number of files in the directory.
    //!
    //! This function counts the number of files in the directory by reading each file entry.
    //! It first rewinds the directory stream to ensure counting starts from the beginning.
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

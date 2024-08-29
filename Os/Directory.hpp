#ifndef _OS_DIRECTORY_HPP_
#define _OS_DIRECTORY_HPP_

#include <FpConfig.hpp>
#include <Os/Os.hpp>
#include <config/FppConstantsAc.hpp>
#include <Fw/Types/String.hpp>

namespace Os {

struct DirectoryHandle {};

class DirectoryInterface {
  public:
    static constexpr FwSizeType FPP_CONFIG_FILENAME_MAX_SIZE = FppConstant_FileNameStringSize::FileNameStringSize;
    typedef enum {
        OP_OK, //!<  Operation was successful
        DOESNT_EXIST, //!<  Directory doesn't exist
        NO_PERMISSION, //!<  No permission to read directory
        NOT_OPENED, //!<  Directory hasn't been opened yet
        NOT_DIR, //!<  Path is not a directory
        NO_MORE_FILES, //!<  Directory stream has no more files
        FILE_LIMIT, //!<  Directory has more files than can be read
        BAD_DESCRIPTOR, //!<  Directory stream descriptor is invalid
        NOT_SUPPORTED, //!<  Operation is not supported by the current implementation
        OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
    } Status;

    //! \brief default constructor
    DirectoryInterface() = default;

    //! \brief default virtual destructor
    virtual ~DirectoryInterface() = default;

    //! \brief copy constructor is forbidden
    DirectoryInterface(const DirectoryInterface& other) = delete;

    //! \brief assignment operator is forbidden
    DirectoryInterface& operator=(const DirectoryInterface& other) = delete;

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    virtual DirectoryHandle* getHandle() = 0;

    //! \brief provide a pointer to a Directory delegate object
    static DirectoryInterface* getDelegate(HandleStorage& aligned_new_memory);


    //------------ Os-specific Directory Functions ------------

    virtual Status open(const char* dirName) = 0; //!<  open directory. Directory must already exist
    virtual bool isOpen() = 0; //!< check if file descriptor is open or not.
    virtual Status rewind() = 0; //!<  rewind directory stream to the beginning
    virtual Status read(char * fileNameBuffer, U32 bufSize) = 0; //!< get next filename from directory
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

    //! \brief Open a directory. Directory must already exist
    //! \param dirName: name of directory to open
    //! \return status of the operation
    Status open(const char* dirName) override;

    //! \brief Check if Directory is open or not
    //! \return true if Directory is open, false otherwise
    bool isOpen() override;

    //! \brief Rewind directory stream to the beginning
    //! \return status of the operation
    Status rewind() override;

    //! \brief Get next filename from directory stream
    //! \param fileNameBuffer: buffer to store filename
    //! \param bufSize: size of fileNameBuffer
    //! \return status of the operation
    Status read(char * fileNameBuffer, U32 bufSize) override;

    //! \brief Close directory
    void close() override;

    // ------------ Common Directory Functions (non-OS-specific) ------------

    //! \brief Read the contents of the directory and store filenames in filenameArray of size filenameArraySize
    //! \param filenameArray: array to store filenames
    //! \param filenameArraySize: size of filenameArray
    //! \param filenameCount: number of filenames written to filenameArray (output)
    //! \return status of the operation
    Status readDirectory(Fw::String filenameArray[], const FwSizeType filenameArraySize, FwSizeType& filenameCount);

    //! \brief Get the number of files in the directory
    //! \param fileCount: number of files in the directory (output)
    //! \return status of the operation
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

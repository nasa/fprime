// ======================================================================
// \title Os/DelegateDirectory.hpp
// \brief defines a delegate-based implementation of DirectoryInterface
// ======================================================================
#ifndef OS_DELEGATEDIRECTORY_HPP_
#define OS_DELEGATEDIRECTORY_HPP_

#include <Os/DirectoryInterface.hpp>

namespace Os {

//! \brief Directory delegate class
//!
//! This class provides a common interface for directory operations, such as reading files in a directory
//! and getting the number of files in a directory.
class DelegateDirectory final : public DirectoryInterface {
  public:
    //! \brief Constructor
    DelegateDirectory();

    //! \brief Destructor
    //!
    //! Destructor will close the Directory if it is open
    ~DelegateDirectory() final;

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    DirectoryHandle* getHandle() override;

    // ------------------------------------------------------------
    // Implementation-specific Directory member functions
    // ------------------------------------------------------------

    //! \brief Open or create a directory
    //!
    //! Using the path provided, this function will open or create a directory.
    //! Use OpenMode::READ to open an existing directory and error if the directory is not found
    //! Use OpenMode::CREATE_IF_MISSING to open a directory, creating the directory if it doesn't exist
    //! Use OpenMode::CREATE_EXCLUSIVE to open a directory, creating the directory and erroring if it already exists
    //!
    //! It is invalid to pass `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //!
    //! \param path: path of directory to open
    //! \param mode: enum (READ, CREATE_IF_MISSING, CREATE_EXCLUSIVE). See notes above for more information
    //! \return status of the operation
    Status open(const char* path, OpenMode mode) override;

    //! \brief Check if Directory is open or not
    //! \return true if Directory is open, false otherwise
    bool isOpen();

    //! \brief Rewind directory stream
    //!
    //! Each read operation moves the seek position forward. This function resets the seek position to the beginning.
    //!
    //! \return status of the operation
    Status rewind() override;

    //! \brief Get next filename from directory stream
    //!
    //! Write at most buffSize characters of the file name to fileNameBuffer and guarantee null-termination.
    //! This function skips the current directory (.) and parent directory (..) entries.
    //! Returns NO_MORE_FILES if there are no more files to read from the buffer.
    //!
    //! It is invalid to pass `nullptr` as fileNameBuffer.
    //!
    //! \param fileNameBuffer: buffer to store filename
    //! \param buffSize: size of fileNameBuffer
    //! \return status of the operation
    Status read(char* fileNameBuffer, FwSizeType buffSize) override;

    //! \brief Close directory
    void close() override;

    // ------------------------------------------------------------
    // Common functions built on top of OS-specific functions
    // ------------------------------------------------------------

    //! \brief Get next filename from directory stream and write it to a Fw::StringBase object
    //!
    //! \param filename: Fw::StringBase (or derived) object to store filename in
    //! \return status of the operation
    Status read(Fw::StringBase& filename) override;

    //! \brief Read the contents of the directory and store filenames in the supplied array.
    //!
    //! Reads at most filenameArray.getSize() filenames.
    //! The function first rewinds the directory stream to ensure reading starts from the beginning.
    //! After reading, it rewinds the directory stream again, resetting seek position to beginning.
    //!
    //! \param filenameArray: array to store filenames
    //! \param filenameCount: number of filenames written to filenameArray (output)
    //! \return status of the operation
    Status readDirectory(Fw::ExternalArray<Fw::String>& filenameArray, FwSizeType& filenameCount) override;

    //! \brief Read the contents of the directory and store filenames in filenameArray of size arraySize.
    //!
    //! The function first rewinds the directory stream to ensure reading starts from the beginning.
    //! After reading, it rewinds the directory stream again, resetting seek position to beginning.
    //!
    //! \param filenameArray: array to store filenames
    //! \param arraySize: size of filenameArray
    //! \param filenameCount: number of filenames written to filenameArray (output)
    //! \return status of the operation
    DEPRECATED(Status readDirectory(Fw::String filenameArray[], const FwSizeType arraySize, FwSizeType& filenameCount),
               "Use readDirectory(Fw::ExternalArray<Fw::String>& filenameArray, FwSizeType& filenameCount) instead");

    //! \brief Get the number of files in the directory.
    //!
    //! Counts the number of files in the directory by reading each file entry and writing the count to fileCount.
    //!
    //! The function first rewinds the directory stream to ensure counting starts from the beginning.
    //! After counting, it rewinds the directory stream again, resetting seek position to beginning.
    //!
    //! \param fileCount Reference to a variable where the file count will be stored.
    //! \return Status indicating the result of the operation.
    Status getFileCount(FwSizeType& fileCount) override;

  private:
    bool m_is_open;  //!< Flag indicating if the directory has been open

  private:
    // This section is used to store the implementation-defined Directory handle. To Os::Directory and fprime, this type
    // is opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) DirectoryHandleStorage m_handle_storage;  //!< Directory handle storage
    DirectoryInterface& m_delegate;
};

}  // namespace Os

#endif  // OS_DELEGATEDIRECTORY_HPP_

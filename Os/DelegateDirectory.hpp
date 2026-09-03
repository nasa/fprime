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

    // Bring the DirectoryInterface convenience overloads into scope; overriding a
    // single `read` overload below would otherwise hide all of them.
    using DirectoryInterface::read;

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

  private:
    // This section is used to store the implementation-defined Directory handle. To Os::Directory and fprime, this type
    // is opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) DirectoryHandleStorage m_handle_storage;  //!< Directory handle storage
    DirectoryInterface& m_delegate;
};

}  // namespace Os

#endif  // OS_DELEGATEDIRECTORY_HPP_

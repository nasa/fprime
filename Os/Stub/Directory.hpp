// ======================================================================
// \title Os/Stub/Directory.hpp
// \brief stub definitions for Os::Directory
// ======================================================================
#ifndef OS_STUB_DIRECTORY_HPP
#define OS_STUB_DIRECTORY_HPP

#include "Os/Directory.hpp"
namespace Os {
namespace Stub {
namespace Directory {

struct StubDirectoryHandle : public DirectoryHandle {};

//! \brief stub implementation of Os::Directory
//!
//! Stub implementation of `DirectoryInterface` for use as a delegate class handling error-only file operations.
//!
class StubDirectory : public DirectoryInterface {
  public:
    //! \brief constructor
    StubDirectory() = default;

    //! \brief destructor
    ~StubDirectory() override = default;

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
    //! \param path: path of directory to open
    //! \param mode: enum (READ, CREATE_IF_MISSING, CREATE_EXCLUSIVE). See notes above for more information
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


  private:
    //! Handle for StubDirectory
    StubDirectoryHandle m_handle;
};

} // namespace Directory
} // namespace Stub
} // namespace Os
#endif // OS_STUB_DIRECTORY_HPP

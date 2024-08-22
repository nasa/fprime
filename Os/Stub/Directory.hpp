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
    //!
    StubDirectory() = default;

    //! \brief destructor
    //!
    ~StubDirectory() override = default;

    //! \brief return the underlying Directory handle (implementation specific)
    //! \return internal Directory handle representation
    DirectoryHandle* getHandle() override;

    //------------ Os-specific Directory Functions ------------
    Status open(const char* dirName) override; //!<  open directory. Directory must already exist
    bool isOpen() override; //!< check if file descriptor is open or not.
    Status rewind() override; //!<  rewind directory stream to the beginning
    Status read(char * fileNameBuffer, U32 bufSize) override; //!< get next filename from directory
    Status read(char * fileNameBuffer, U32 bufSize, I64& inode) override; //!< get next filename and inode from directory
    void close() override; //!<  close directory


  private:
    //! Handle for StubDirectory
    StubDirectoryHandle m_handle;
};

} // namespace Directory
} // namespace Stub
} // namespace Os
#endif // OS_STUB_DIRECTORY_HPP

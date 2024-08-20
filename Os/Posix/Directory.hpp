// ======================================================================
// \title Os/Posix/Directory.hpp
// \brief Posix definitions for Os::Directory
// ======================================================================
#ifndef OS_POSIX_DIRECTORY_HPP
#define OS_POSIX_DIRECTORY_HPP
#include <Os/Directory.hpp>
#include <dirent.h>

namespace Os {
namespace Posix {
namespace Directory {

struct PosixDirectoryHandle : public DirectoryHandle {
    DIR* m_dir_descriptor = nullptr;
};

//! \brief Posix implementation of Os::Directory
//!
//! Posix implementation of `DirectoryInterface` for use as a delegate class handling error-only file operations.
//!
class PosixDirectory : public DirectoryInterface {
  public:
    //! \brief constructor
    //!
    PosixDirectory();

    //! \brief destructor
    //!
    ~PosixDirectory() override;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    DirectoryHandle* getHandle() override;

    //------------ Os-specific Directory Functions ------------
    Status open(const char* dirName) override; //!<  open directory. Directory must already exist
    bool isOpen() override; //!< check if file descriptor is open or not.
    Status rewind() override; //!<  rewind directory stream to the beginning
    Status read(char * fileNameBuffer, U32 bufSize) override; //!< get next filename from directory
    Status read(char * fileNameBuffer, U32 bufSize, I64& inode) override; //!< get next filename and inode from directory
    void close() override; //!<  close directory

  private:
    //! Handle for PosixDirectory
    PosixDirectoryHandle m_handle;
};

}  // namespace Directory
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_DIRECTORY_HPP

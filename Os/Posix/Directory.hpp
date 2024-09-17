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
class PosixDirectory : public DirectoryInterface {
  public:
    //! \brief constructor
    PosixDirectory();

    //! \brief destructor
    ~PosixDirectory() = default;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    DirectoryHandle* getHandle() override;


    // ------------------------------------------------------------
    // Implementation-specific Directory member functions
    // ------------------------------------------------------------

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

  private:
    //! Handle for PosixDirectory
    PosixDirectoryHandle m_handle;
};

}  // namespace Directory
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_DIRECTORY_HPP

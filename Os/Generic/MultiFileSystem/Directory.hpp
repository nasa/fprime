// ======================================================================
// \title Os/Generic/MultiFileSystem/Directory.hpp
// \brief MultiFileSystem Directory definitions for Os::Directory
// ======================================================================
#ifndef OS_GENERIC_MULTIFILESYSTEM_DIRECTORY_HPP
#define OS_GENERIC_MULTIFILESYSTEM_DIRECTORY_HPP

#include "Os/Directory.hpp"

namespace Os {
namespace Generic {

struct MultiDirectoryHandle : public DirectoryHandle {
    DirectoryInterface* m_directory_interface =
        nullptr;  //!< Pointer to the underlying directory interface for this directory handle
};

//! \brief MultiFileSystem implementation of Os::Directory
//!
//! MultiFileSystem implementation of `DirectoryInterface` for use as a delegate class handling directory operations
//! across multiple filesystem implementations. Routes directory operations to the appropriate underlying filesystem
//! based on the path used during open().
//!
class MultiDirectory : public DirectoryInterface {
  public:
    //! \brief constructor
    MultiDirectory() = default;

    //! \brief copy constructor
    MultiDirectory(const MultiDirectory& other);

    //! \brief assignment operator
    MultiDirectory& operator=(const MultiDirectory& other);

    //! \brief destructor
    ~MultiDirectory() override = default;

    // ------------------------------------------------------------
    // Implementation-specific member functions - overrides
    // ------------------------------------------------------------

    //! \brief open directory with supplied path and mode
    //!
    //! Open the directory passed in with the given mode. Use OpenMode::READ to open an existing directory and error
    //! if the directory is not found. Use OpenMode::CREATE_IF_MISSING to open a directory, creating the directory if
    //! it doesn't exist. Use OpenMode::CREATE_EXCLUSIVE to open a directory, creating the directory and erroring if
    //! it already exists.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //!
    //! \param path: c-string of path to open
    //! \param mode: directory open mode
    //! \return: status of the open
    Status open(const char* path, OpenMode mode) override;

    //! \brief close the directory, if not opened then do nothing
    //!
    //! Closes the directory, if open. Otherwise this function does nothing.
    //!
    void close() override;

    //! \brief rewind directory stream
    //!
    //! Each read operation moves the seek position forward. This function resets the seek position to the beginning.
    //!
    //! \return OP_OK on success otherwise error status
    Status rewind() override;

    //! \brief get next filename from directory stream
    //!
    //! Write at most buffSize characters of the file name to fileNameBuffer and guarantee null-termination.
    //! This function skips the current directory (.) and parent directory (..) entries.
    //! Returns NO_MORE_FILES if there are no more files to read from the buffer.
    //!
    //! It is invalid to pass `nullptr` as fileNameBuffer.
    //!
    //! \param fileNameBuffer: buffer to store filename
    //! \param buffSize: size of fileNameBuffer
    //! \return OP_OK on success otherwise error status
    Status read(char* fileNameBuffer, FwSizeType buffSize) override;

    //! \brief returns the raw directory handle
    //!
    //! Gets the raw directory handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it must be passed as an opaque type.
    //!
    //! \return raw directory handle
    DirectoryHandle* getHandle() override;

  private:
    // ------------------------------------------------------------
    // Private member variables
    // ------------------------------------------------------------

    //! Directory handle for MultiDirectory
    MultiDirectoryHandle m_handle;

    //! Pointer to the underlying DirectoryInterface implementation
    //! This is populated when open() is called and stored for use in subsequent calls
    DirectoryInterface* m_directory_interface = nullptr;

};  // class MultiDirectory

}  // namespace Generic
}  // namespace Os
#endif  // OS_GENERIC_MULTIFILESYSTEM_DIRECTORY_HPP

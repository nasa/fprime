// ======================================================================
// \title Os/Stub/File.hpp
// \brief stub file definitions for Os::File
// ======================================================================
#include "Os/File.hpp"

#ifndef OS_STUB_FILE_HPP
#define OS_STUB_FILE_HPP
namespace Os {
namespace Stub {
namespace File {

struct StubFileHandle : public FileHandle {};

//! \brief stub implementation of Os::File
//!
//! Stub implementation of `FileInterface` for use as a delegate class handling error-only file operations.
//!
class StubFile : public FileInterface {
  public:
    //! \brief constructor
    //!
    StubFile() = default;

    //! \brief destructor
    //!
    ~StubFile() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief open file with supplied path and mode
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //! It is invalid to supply `overwrite` as a non-enumerated value.
    //!
    //! \param path: c-string of path to open
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return: NOT_IMPLEMENTED
    //!
    Os::FileInterface::Status open(const char *path, Mode mode, OverwriteType overwrite) override;

    //! \brief close the file, if not opened then do nothing
    //!
    //! This implementation does nothing.
    //!
    void close() override;

    //! \brief get size of currently open file
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //! \param size: output parameter for size.
    //! \return NOT_IMPLEMENTED
    //!
    Status size(FwSignedSizeType &size_result) override;

    //! \brief get file pointer position of the currently open file
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //! \param position: output parameter for size.
    //! \return NOT_IMPLEMENTED
    //!
    Status position(FwSignedSizeType &position_result) override;

    //! \brief pre-allocate file storage
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! It is invalid to pass a negative `offset`.
    //! It is invalid to pass a negative `length`.
    //!
    //! \param offset: offset into file
    //! \param length: length after offset to preallocate
    //! \return NOT_IMPLEMENTED
    //!
    Status preallocate(FwSignedSizeType offset, FwSignedSizeType length) override;

    //! \brief seek the file pointer to the given offset
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! \param offset: offset to seek to
    //! \param seekType: `ABSOLUTE` for seeking from beginning of file, `CURRENT` to use current position.
    //! \return NOT_IMPLEMENTED
    //!
    Status seek(FwSignedSizeType offset, SeekType seekType) override;

    //! \brief flush file contents to storage
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! \return NOT_IMPLEMENTED
    //!
    Status flush() override;

    //! \brief read data from this file into supplied buffer bounded by size
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //! It is invalid to supply wait as a non-enumerated value.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: size of data to read
    //! \param wait: `WAIT` to wait for data, `NO_WAIT` to return what is currently available
    //! \return NOT_IMPLEMENTED
    //!
    Status read(U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

    //! \brief read data from this file into supplied buffer bounded by size
    //!
    //! This implementation does nothing but return NOT_IMPLEMENTED.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //! It is invalid to supply wait as a non-enumerated value.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: size of data to read
    //! \param wait: `WAIT` to wait for data to write to disk, `NO_WAIT` to return what is currently available
    //! \return NOT_IMPLEMENTED
    //!
    Status write(const U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

    //! \brief returns the raw file handle
    //!
    //! Gets the raw file handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it//!must* be passed as an opaque type.
    //!
    //! \return raw file handle
    //!
    FileHandle *getHandle() override;


private:
    //! File handle for PosixFile
    StubFileHandle m_handle;
};

} // namespace File
} // namespace Stub
} // namespace Os
#endif // OS_STUB_FILE_HPP

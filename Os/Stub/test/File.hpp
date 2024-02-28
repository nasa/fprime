#include "Os/File.hpp"
#include <string.h>

#ifndef OS_STUB_FILE_TEST_HPP
#define OS_STUB_FILE_TEST_HPP
namespace Os {
namespace Stub {
namespace File {
namespace Test {

//! Data that supports the stubbed File implementation.
//!/
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        OPEN_FN,
        CLOSE_FN,
        SIZE_FN,
        POSITION_FN,
        PREALLOCATE_FN,
        SEEK_FN,
        FLUSH_FN,
        READ_FN,
        WRITE_FN
    };

    //! Last function called
    LastFn lastCalled = NONE_FN;
    //! Path of last open call
    const char *openPath = nullptr;
    //! Mode of last open call
    Os::File::Mode openMode = Os::File::MAX_OPEN_MODE;
    //! Overwrite of last open call
    Os::File::OverwriteType openOverwrite = Os::File::OverwriteType::NO_OVERWRITE;
    //! Offset of last preallocate call
    FwSignedSizeType preallocateOffset = -1;
    //! Length of last preallocate call
    FwSignedSizeType preallocateLength = -1;
    //! Offset of last seek call
    FwSignedSizeType seekOffset = -1;
    //! Absolute of last seek call
    Os::File::SeekType seekType = Os::File::SeekType::ABSOLUTE;
    //! Buffer of last read call
    U8 *readBuffer = nullptr;
    //! Size of last read call
    FwSignedSizeType readSize = -1;
    //! Wait of last read call
    Os::File::WaitType readWait = Os::File::WaitType::NO_WAIT;
    //! Buffer of last write call
    const void *writeBuffer = nullptr;
    //! Size of last write call
    FwSignedSizeType writeSize = -1;
    //! Wait of last write call
    Os::File::WaitType  writeWait = Os::File::WaitType::NO_WAIT;

    //! File pointer
    FwSignedSizeType pointer = 0;

    //! Next status to be returned
    Os::File::Status nextStatus = Os::File::Status::OTHER_ERROR;
    //! Return of next size call
    FwSignedSizeType sizeResult = -1;
    //! Return of next position call
    FwSignedSizeType positionResult = 0;
    //! Result of next read call
    U8 *readResult = nullptr;
    //! Size result of next read data
    FwSignedSizeType readResultSize = -1;
    //! Size result of next read call
    FwSignedSizeType readSizeResult = -1;
    //! Result holding buffer of next write call
    U8 *writeResult = nullptr;
    //! Size result of next write data
    FwSignedSizeType writeResultSize = -1;
    //! Size result of next read call
    FwSignedSizeType writeSizeResult = -1;

    // Singleton data
    static StaticData data;

    //! Set next status to return
    static void setNextStatus(Os::File::Status status);

    //! Set next size to result
    static void setSizeResult(FwSignedSizeType size);

    //! Set next position to result
    static void setPositionResult(FwSignedSizeType position);

    //! Set next read result
    static void setReadResult(U8 *buffer, FwSignedSizeType size);

    //! Set next read size result
    static void setReadSize(FwSignedSizeType size);

    //! Set next write result
    static void setWriteResult(U8 *buffer, FwSignedSizeType size);

    //! Set next write size result
    static void setWriteSize(FwSignedSizeType size);
};

class TestFileHandle : public FileHandle {};

//! \brief tracking implementation of Os::File
//!
//! Tracking implementation of `FileInterface` for use as a delegate class handling test file operations. Posix files use
//! standard `open`, `read`, and `write` posix calls. The handle is represented as a `PosixFileHandle` which wraps a
//! single `int` type file descriptor used in those API calls.
//!
class TestFile : public FileInterface {
  public:
    //! \brief constructor
    //!
    TestFile();

    //! \brief destructor
    //!
    ~TestFile() override;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief open file with supplied path and mode
    //!
    //! Open the file passed in with the given mode. If overwrite is set to OVERWRITE, then opening files in
    //! OPEN_CREATE mode will clobber existing files. Set overwrite to NO_OVERWRITE to preserve existing files.
    //! The status of the open request is returned from the function call. Delegates to the chosen
    //! implementation's `open` function.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //! It is invalid to supply `overwrite` as a non-enumerated value.
    //!
    //! \param path: c-string of path to open
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const char *path, Mode mode, OverwriteType overwrite) override;

    //! \brief close the file, if not opened then do nothing
    //!
    //! Closes the file, if open. Otherwise this function does nothing. Delegates to the chosen implementation's
    //! `closeInternal` function. `mode` is set to `OPEN_NO_MODE`.
    //!
    void close() override;

    //! \brief get size of currently open file
    //!
    //! Get the size of the currently open file and fill the size parameter. Return status of the operation.
    //! \param size: output parameter for size.
    //! \return OP_OK on success otherwise error status
    //!
    Status size(FwSignedSizeType &size_result) override;

    //! \brief get file pointer position of the currently open file
    //!
    //! Get the current position of the read/write pointer of the open file.
    //! \param position: output parameter for size.
    //! \return OP_OK on success otherwise error status
    //!
    Status position(FwSignedSizeType &position_result) override;

    //! \brief pre-allocate file storage
    //!
    //! Pre-allocates file storage with at least `length` storage starting at `offset`. No-op on implementations
    //! that cannot pre-allocate.
    //!
    //! It is invalid to pass a negative `offset`.
    //! It is invalid to pass a negative `length`.
    //!
    //! \param offset: offset into file
    //! \param length: length after offset to preallocate
    //! \return OP_OK on success otherwise error status
    //!
    Status preallocate(FwSignedSizeType offset, FwSignedSizeType length) override;

    //! \brief seek the file pointer to the given offset
    //!
    //! Seek the file pointer to the given `offset`. If `seekType` is set to `ABSOLUTE` then the offset is calculated
    //! from the start of the file, and if it is set to `CURRENT` it is calculated from the current position.
    //!
    //! \param offset: offset to seek to
    //! \param seekType: `ABSOLUTE` for seeking from beginning of file, `CURRENT` to use current position.
    //! \return OP_OK on success otherwise error status
    //!
    Status seek(FwSignedSizeType offset, SeekType seekType) override;

    //! \brief flush file contents to storage
    //!
    //! Flushes the file contents to storage (i.e. out of the OS cache to disk). Does nothing in implementations
    //! that do not support flushing.
    //!
    //! \return OP_OK on success otherwise error status
    //!
    Status flush() override;

    //! \brief read data from this file into supplied buffer bounded by size
    //!
    //! Read data from this file up to the `size` and store it in `buffer`.  When `wait` is set to `WAIT`, this
    //! will block until the requested size has been read successfully read or the end of the file has been
    //! reached. When `wait` is set to `NO_WAIT` it will return whatever data is currently available.
    //!
    //! `size` will be updated to the count of bytes actually read. Status will reflect the success/failure of
    //! the read operation.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //! It is invalid to supply wait as a non-enumerated value.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: size of data to read
    //! \param wait: `WAIT` to wait for data, `NO_WAIT` to return what is currently available
    //! \return OP_OK on success otherwise error status
    //!
    Status read(U8 *buffer, FwSignedSizeType &size, WaitType wait) override;

    //! \brief read data from this file into supplied buffer bounded by size
    //!
    //! Write data to this file up to the `size` from the `buffer`.  When `wait` is set to `WAIT`, this
    //! will block until the requested size has been written successfully to disk. When `wait` is set to
    //! `NO_WAIT` it will return once the data is sent to the OS.
    //!
    //! `size` will be updated to the count of bytes actually written. Status will reflect the success/failure of
    //! the read operation.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //! It is invalid to supply wait as a non-enumerated value.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: size of data to read
    //! \param wait: `WAIT` to wait for data to write to disk, `NO_WAIT` to return what is currently available
    //! \return OP_OK on success otherwise error status
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
    TestFileHandle m_handle;
};


} // namespace Test
} // namespace File
} // namespace Stub
} // namespace Os
#endif // OS_STUB_FILE_TEST_HPP

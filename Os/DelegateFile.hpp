// ======================================================================
// \title Os/DelegateFile.hpp
// \brief Define the Os::DelegateFile class
// ======================================================================
#ifndef OS_DELEGATEFILE_HPP_
#define OS_DELEGATEFILE_HPP_

#include "Os/FileInterface.hpp"
#include "Utils/Hash/Hash.hpp"

// Forward declaration for UTs
namespace Os {
namespace Test {
namespace FileTest {
struct Tester;
}
}  // namespace Test
}  // namespace Os

namespace Os {

class DelegateFile final : public FileInterface {
    friend struct Os::Test::FileTest::Tester;

  public:
    //! \brief constructor
    //!
    DelegateFile();
    //! \brief destructor
    //!
    //! Destructor closes the file if it is open
    ~DelegateFile() final;

    //! \brief copy constructor that copies the internal representation
    DelegateFile(const DelegateFile& other);

    //! \brief assignment operator that copies the internal representation
    DelegateFile& operator=(const DelegateFile& other);

    //! \brief determine if the file is open
    //! \return true if file is open, false otherwise
    //!
    bool isOpen() const;

    // ------------------------------------
    // Functions supplying default values
    // ------------------------------------

    //! \brief open file with supplied path and mode
    //!
    //! Open the file passed in with the given mode. Opening files with `OPEN_CREATE` mode will not clobber existing
    //! files. Use other `open` method to set overwrite flag and clobber existing files. The status of the open
    //! request is returned from the function call. Delegates to the chosen implementation's `open` function.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //!
    //! \param path: c-string of path to open
    //! \param mode: file operation mode
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const char* path, Mode mode);

    //! \brief open file with supplied path, bounded length, and mode
    //!
    //! Open the file passed in with the given mode. The path length is bounded by `length`.
    //! Opening files with `OPEN_CREATE` mode will not clobber existing files. Use the overload
    //! accepting `OverwriteType` to set overwrite flag and clobber existing files.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //! It is invalid for the path to not be null-terminated within `length` characters.
    //!
    //! \param path: c-string of path to open
    //! \param length: bound on the path buffer size
    //! \param mode: file operation mode
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const char* path, FwSizeType length, Mode mode);

    //! \brief open file with supplied string path and mode
    //!
    //! Open the file passed in with the given mode. Opening files with `OPEN_CREATE` mode will not clobber existing
    //! files. Use the overload accepting `OverwriteType` to set overwrite flag and clobber existing files.
    //!
    //! It is invalid to supply `mode` as a non-enumerated value.
    //!
    //! \param path: ConstStringBase reference of path to open
    //! \param mode: file operation mode
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const Fw::ConstStringBase& path, Mode mode);

    //! \brief open file with supplied string path, mode, and overwrite type
    //!
    //! Open the file passed in with the given mode. If overwrite is set to OVERWRITE, then opening files in
    //! OPEN_CREATE mode will clobber existing files. Set overwrite to NO_OVERWRITE to preserve existing files.
    //!
    //! It is invalid to supply `mode` as a non-enumerated value.
    //! It is invalid to supply `overwrite` as a non-enumerated value.
    //!
    //! \param path: ConstStringBase reference of path to open
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const Fw::ConstStringBase& path, Mode mode, OverwriteType overwrite);

    //! \brief read data from this file into supplied buffer bounded by size
    //!
    //! Read data from this file up to the `size` and store it in `buffer`.  This version will
    //! will block until the requested size has been read successfully read or the end of the file has been
    //! reached.
    //!
    //! `size` will be updated to the count of bytes actually read. Status will reflect the success/failure of
    //! the read operation.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: size of data to read
    //! \return OP_OK on success otherwise error status
    //!
    Status read(U8* buffer, FwSizeType& size);

    //! \brief write data to this file from the supplied buffer bounded by size
    //!
    //! Write data from `buffer` up to the `size` and store it in this file. This call
    //! will block until the requested size has been written. Otherwise, this call will write without blocking.
    //!
    //! `size` will be updated to the count of bytes actually written. Status will reflect the success/failure of
    //! the write operation.
    //!
    //! It is invalid to pass `nullptr` to this function call.
    //! It is invalid to pass a negative `size`.
    //!
    //! \param buffer: memory location of data to write to file
    //! \param size: size of data to write
    //! \return OP_OK on success otherwise error status
    //!
    Status write(const U8* buffer, FwSizeType& size);

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
    Os::FileInterface::Status open(const char* path, Mode mode, OverwriteType overwrite) override;

    //! \brief open file with supplied path, bounded length, mode, and overwrite type
    //!
    //! Open the file passed in with the given mode. The path length is bounded by `length`.
    //! If overwrite is set to OVERWRITE, then opening files in OPEN_CREATE mode will clobber
    //! existing files. Set overwrite to NO_OVERWRITE to preserve existing files. This is the
    //! core open implementation to which all other open overloads delegate.
    //!
    //! It is invalid to send `nullptr` as the path.
    //! It is invalid to supply `mode` as a non-enumerated value.
    //! It is invalid to supply `overwrite` as a non-enumerated value.
    //! It is invalid for the path to not be null-terminated within `length` characters.
    //!
    //! \param path: c-string of path to open
    //! \param length: bound on the path buffer size
    //! \param mode: file operation mode
    //! \param overwrite: overwrite existing file on create
    //! \return: status of the open
    //!
    Os::FileInterface::Status open(const char* path, FwSizeType length, Mode mode, OverwriteType overwrite);

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
    Status size(FwSizeType& size_result) override;

    //! \brief get file pointer position of the currently open file
    //!
    //! Get the current position of the read/write pointer of the open file.
    //! \param position: output parameter for size.
    //! \return OP_OK on success otherwise error status
    //!
    Status position(FwSizeType& position_result) override;

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
    Status preallocate(FwSizeType offset, FwSizeType length) override;

    //! \brief seek the file pointer to the given offset
    //!
    //! Seek the file pointer to the given `offset`. If `seekType` is set to `ABSOLUTE` then the offset is calculated
    //! from the start of the file, and if it is set to `RELATIVE` it is calculated from the current position.
    //!
    //! \param offset: offset to seek to
    //! \param seekType: `ABSOLUTE` for seeking from beginning of file, `RELATIVE` to use current position.
    //! \return OP_OK on success otherwise error status
    //!
    Status seek(FwSignedSizeType offset, SeekType seekType) override;

    //! \brief seek the file pointer to the given offset absolutely with the full range
    //!
    //! Seek the file pointer to the given `offset` absolutely from the beginning of the file. This function is
    //! equivalent to calling `seek` with `ABSOLUTE` as the `seekType` with the exception that it can handle the
    //! full range of `FwSizeType` values as returned by `size` and `position` calls.
    //!
    //! Internally, it will perform multiple seeks to reach the desired offset while never exceeding the signed
    //! limit of the basic `seek` function.
    //!
    //! \param offset_unsigned: offset to absolutely seek to
    //! \return OP_OK on success otherwise error status
    Status seek_absolute(FwSizeType offset_unsigned);

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

    //!
    Status read(U8* buffer, FwSizeType& size, WaitType wait) override;

    //! \brief read a line from the file using `\n` as the delimiter
    //!
    //! Reads a single line from the file including the terminating '\n'. This will return an error if no line is
    //! found within the specified buffer size. In the case of EOF, the line is read without the terminating '\n'.
    //!
    //! In the case of an error, this function will seek to the original location in the file. Otherwise, the
    //! pointer will point to the first character after the `\n` or EOF in the case of no `\n`.
    //!
    //! It is invalid to send a null buffer.
    //! It is invalid to send a size less than 0.
    //! It is an error if the file is not opened for reading.
    //!
    //! \param buffer: memory location to store data read from file
    //! \param size: maximum size of buffer to store the new line
    //! \param wait: `WAIT` to wait for data, `NO_WAIT` to return what is currently available
    //! \return OP_OK on success otherwise error status
    Status readline(U8* buffer, FwSizeType& size, WaitType wait);

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
    Status write(const U8* buffer, FwSizeType& size, WaitType wait) override;

    //! \brief returns the raw file handle
    //!
    //! Gets the raw file handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it//!must* be passed as an opaque type.
    //!
    //! \return raw file handle
    //!
    FileHandle* getHandle() override;

    //! \brief calculate the CRC32 of the entire file
    //!
    //! Calculates the CRC32 of the file's contents. The `crc` parameter will be updated to contain the CRC or 0 on
    //! failure. Status will represent failure conditions. This call will be decomposed into calculations on
    //! sections of the file `FW_FILE_CHUNK_SIZE` bytes long.
    //!
    //! This function requires that the file already be opened for "READ" mode.
    //!
    //! On error crc will be set to 0.
    //!
    //! \note: the file pointer will be positioned at the end of the file after this call.
    //!
    //! This function is equivalent to the following pseudo-code:
    //!
    //! ```
    //! U32 crc;
    //! do {
    //!     size = FW_FILE_CHUNK_SIZE;
    //!     m_file.incrementalCrc(size);
    //! while (size == FW_FILE_CHUNK_SIZE);
    //! m_file.finalize(crc);
    //! ```
    //! \param crc: U32 bit value to fill with CRC
    //! \return OP_OK on success otherwise error status
    //!
    Status calculateCrc(U32& crc);

    //! \brief calculate the CRC32 of the next section of data
    //!
    //! Starting at the current file pointer, this will add `size` bytes of data to the currently calculated CRC.
    //! Call `finalizeCrc` to retrieve the CRC or `calculateCrc` to perform a CRC on the entire file. This call will
    //! not block waiting for data on the underlying read, nor will it reset the file position pointer. On error,
    //! the current CRC results should be discarded by reopening the file or calling `finalizeCrc` and
    //! discarding its result. `size` will be updated with the `size` actually read and used in the CRC calculation.
    //!
    //! This function requires that the file already be opened for "READ" mode.
    //!
    //! It is illegal for size to be less than or equal to 0 or greater than FW_FILE_CHUNK_SIZE.
    //!
    //! \param size: size of data to read for CRC
    //! \return: status of the CRC calculation
    //!
    Status incrementalCrc(FwSizeType& size);

    //! \brief finalize and retrieve the CRC value
    //!
    //! Finalizes the CRC computation and returns the CRC value. The `crc` value will be modified to contain the
    //! crc or 0 on error. Note: this will reset any active CRC calculation and effectively re-initializes any
    //! `incrementalCrc` calculation.
    //!
    //! On error crc will be set to 0.
    //!
    //! \param crc: value to fill
    //! \return status of the CRC calculation
    //!
    Status finalizeCrc(U32& crc);

  private:
    static const U32 INITIAL_CRC = 0xFFFFFFFF;  //!< Initial value for CRC calculation

    Mode m_mode = Mode::OPEN_NO_MODE;  //!< Stores mode for error checking

    Utils::Hash m_hash;  //!< Hash object for incremental CRC calculation
    U8 m_crc_buffer[FW_FILE_CHUNK_SIZE];

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) FileHandleStorage m_handle_storage;  //!< Storage for aligned FileHandle data
    FileInterface& m_delegate;                                        //!< Delegate for the real implementation
};
}  // namespace Os

#endif  // OS_DELEGATEFILE_HPP_

// ======================================================================
// \title Os/DelegateFile.hpp
// \brief Define the Os::DelegateFile class
// ======================================================================
#ifndef OS_DELEGATEFILE_HPP_
#define OS_DELEGATEFILE_HPP_

#include <Utils/Hash/Hash.hpp>
#include "Os/FileInterface.hpp"

namespace Os {

class DelegateFile final : public FileInterface {
    // The file unit-test harness reaches into the CRC scratch state (`m_hash`) to check the running
    // accumulator against its shadow model. That state moved here from `FileInterface`, so the tester
    // must befriend this wrapper too (it is already a friend of `FileInterface`).
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

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    // Bring the FileInterface convenience overloads into scope; overriding a
    // single `open` overload below would otherwise hide all of them.
    using FileInterface::open;
    using FileInterface::read;
    using FileInterface::write;

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
    Status read(U8* buffer, FwSizeType& size, WaitType wait) override;

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

    // ------------------------------------
    // CRC overrides
    //
    // The CRC scratch state (`m_hash` accumulator + `m_crc_buffer` read
    // buffer) and the working algorithm live here on the wrapper rather than
    // on `FileInterface`, because concrete `FileInterface` implementations are
    // placement-new'd into a fixed-size (`FW_FILE_HANDLE_MAX_SIZE`) slot and
    // could not afford the ~`FW_FILE_CHUNK_SIZE`-byte buffer. `DelegateFile`'s
    // own storage is not handle-size-constrained, so it can carry the buffer
    // once and drive the CRC through the (virtual) `read()`, transparently
    // reaching the selected delegate. See Os/FileInterface.hpp for the full
    // design note.
    // ------------------------------------

    //! \brief calculate the CRC32 of the entire file. See FileInterface::calculateCrc.
    Status calculateCrc(U32& crc) override;

    //! \brief calculate the CRC32 of the next section of data. See FileInterface::incrementalCrc.
    Status incrementalCrc(FwSizeType& size) override;

    //! \brief finalize and retrieve the CRC value. See FileInterface::finalizeCrc.
    Status finalizeCrc(U32& crc) override;

  private:
    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) FileHandleStorage m_handle_storage;  //!< Storage for aligned FileHandle data
    FileInterface& m_delegate;                                        //!< Delegate for the real implementation

    // CRC scratch state -- see the "CRC overrides" note above. Kept off the
    // handle-size-constrained delegate and on this wrapper instead.
    Utils::Hash m_hash;                   //!< Hash object for incremental CRC calculation
    U8 m_crc_buffer[FW_FILE_CHUNK_SIZE];  //!< Read buffer for incremental CRC calculation
};
}  // namespace Os

#endif  // OS_DELEGATEFILE_HPP_

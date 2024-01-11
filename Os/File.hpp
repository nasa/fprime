#ifndef Os_File_hpp_
#define Os_File_hpp_

#include <FpConfig.hpp>

namespace Os {
    class FileHandle;

    // This class encapsulates a very simple file interface that has the most often-used features

    class File {
        public:

            enum Mode {
                OPEN_NO_MODE,   //!<  File mode not yet selected
                OPEN_READ, //!<  Open file for reading
                OPEN_CREATE, //!< Open file for writing and truncates file if it exists, ie same flags as creat()
                OPEN_WRITE, //!<  Open file for writing
                OPEN_SYNC_WRITE, //!<  Open file for writing; writes don't return until data is on disk
                OPEN_SYNC_DIRECT_WRITE, //!<  Open file for writing, bypassing all caching. Requires data alignment
                OPEN_APPEND, //!< Open file for appending
                MAX_OPEN_MODE //!< Maximum value of mode
            };

            enum Status {
                OP_OK, //!<  Operation was successful
                DOESNT_EXIST, //!<  File doesn't exist (for read)
                NO_SPACE, //!<  No space left
                NO_PERMISSION, //!<  No permission to read/write file
                BAD_SIZE, //!<  Invalid size parameter
                NOT_OPENED, //!<  file hasn't been opened yet
                FILE_EXISTS, //!< file already exist (for CREATE with O_EXCL enabled)
                NOT_SUPPORTED, //!< Kernel or file system does not support operation
                INVALID_MODE, //!< Mode for file access is invalid for current operation
                OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
            };

            /**
             * \brief construct the file object
             * Initializes a file object without specifying the path. A follow-up call to `open` is required to use the
             * file for writing and reading. Delegates to the chosen implementation's `constructInternal` function.
             */
            File();
            /**
             * \brief destruct the file object
             * Destructs the file object. Should the file still be opened, then this will close the file object.
             * Delegates to the chosen implementation's `destructInternal` function after any potential close call.
             */
            virtual ~File();

            /**
             * \brief open file with supplied path and mode
             * Open the file passed in with the given mode. If overwrite is set to true, then opening files in
             * OPEN_CREATE mode will clobber existing files. Set overwrite to false to preserve existing files. The
             * status of the open request is returned from the function call. Delegates to the chosen implementation's
             * `openInternal` function.  On successful open, the `mode` member variable is set. Otherwise it is left
             * unset.
             * \param path: c-string of path to open
             * \param mode: file operation mode
             * \param overwrite: overwrite existing file on create
             * \return: status of the open
             */
            Status open(const char* path, Mode mode, bool overwrite=false);

            /**
             * \brief close the file, if not opened then do nothing
             *
             * Closes the file, if open. Otherwise this function does nothing. Delegates to the chosen implementation's
             * `closeInternal` function. `mode` is set to `OPEN_NO_MODE`.
             */
            void close();

            /**
             * \brief determine if the file is open
             * \return true if file is open, false otherwise
             */
            bool isOpen() const;

            /**
             * \brief pre-allocate file storage
             *
             * Pre-allocates file storage with at least `length` storage starting at `offset`. No-op on implementations
             * that cannot pre-allocate.
             *
             * It is invalid to pass a negative `offset`.
             * It is invalid to pass a negative `length`.
             *
             * \param offset: offset into file
             * \param length: length after offset to preallocate
             * \return OP_OK on success otherwise error status
             */
            Status preallocate(FwSizeType offset, FwSizeType length);

            /**
             * \brief seek the file pointer to the given offset
             *
             * Seek the file pointer to the given `offset`. If `absolute` is true then the offset is calculated from the
             * start of the file, otherwise it is calculated from the current position.
             *
             * \param offset: offset to seek to
             * \param absolute: true for seeking from beginning of file, false to use current position. Default: true.
             * \return OP_OK on success otherwise error status
             */
            Status seek(FwSizeType offset, bool absolute = true);

            /**
             * \brief flush file contents to storage
             *
             * Flushes the file contents to storage (i.e. out of the OS cache to disk). Does nothing in implementations
             * that do not support flushing.
             *
             * \return OP_OK on success otherwise error status
             */
            Status flush();

            /**
             * \brief read data from this file into supplied buffer bounded by size
             *
             * Read data from this file up to the `size` and store it in `buffer`.  If `wait` is true, then this call
             * will block until the requested size has been read successfully read or the end of the file has been
             * reached. Otherwise, this call will read available data and return.
             *
             * `size` will be updated to the count of bytes actually read. Status will reflect the success/failure of
             * the read operation.
             *
             * It is invalid to pass `nullptr` to this function call.
             * It is invalid to pass a negative `size`.
             *
             * \param buffer: memory location to store data read from file
             * \param size: size of data to read
             * \param wait: true if call should block until `size` data read, false otherwise
             * \return OP_OK on success otherwise error status
             */
            Status read(U8* buffer, FwSizeType &size, bool wait = true);

            /**
             * \brief write data to this file from the supplied buffer bounded by size
             *
             * Write data from `buffer` up to the `size` and store it in this file.  If `wait` is true, then this call
             * will block until the requested size has been written. Otherwise, this call will write without blocking.
             *
             * `size` will be updated to the count of bytes actually written. Status will reflect the success/failure of
             * the write operation.
             *
             * It is invalid to pass `nullptr` to this function call.
             * It is invalid to pass a negative `size`.
             *
             * \param buffer: memory location of data to write to file
             * \param size: size of data to write
             * \param wait: true if call should block until `size` data is written, false otherwise
             * \return OP_OK on success otherwise error status
             */
            Status write(const void * buffer, FwSizeType &size, bool wait = true);

            /**
             * \brief calculate the CRC32 of this file
             *
             * Calculates the CRC32 of the file's contents. If `nice` is true, the file will be closed during the
             * computation of the CRC and reopened afterwards. The CRC will be calculated in chunk sizes of
             * FW_FILE_CHUNK_SIZE. The `crc` parameter will be updated to contain the CRC or 0 on failure. Status will
             * represent failure conditions.
             *
             * \param crc: U32 bit value to fill with CRC
             * \param nice: true to use nice, less impactful file system usage by closing the file, false otherwise
             * \return OP_OK on success otherwise error status
             */
            Status calculateCRC32(U32 &crc, bool nice = true); //!< calculates the CRC32 of the file
        private:
          /**
           * Internal implementation of the constructor function. See above.
           */
          void constructInternal();

          /**
           * Internal implementation of the destructor function. See above.
           */
          void destructInternal();

          /**
           * Internal implementation of the `open` call. See above.
           */
          Status openInternal(const char* path, Mode mode, bool overwrite=false);

          /**
           * Internal implementation of the `close` call. See above.
           */
          void closeInternal();

          /**
           * Internal implementation of the `preallocate` call. See above.
           */
          Status preallocateInternal(FwSizeType offset, FwSizeType length);

          /**
           * Internal implementation of the `seek` call. See above.
           */
          Status seekInternal(FwSizeType offset, bool absolute);

          /**
           * Internal implementation of the `flush` call. See above.
           */
          Status flushInternal();

          /**
           * Internal implementation of the `read` call. See above.
           */
          Status readInternal(U8* buffer, FwSizeType &size, bool wait);

          /**
           * Internal implementation of the `write` call. See above.
           */
          Status writeInternal(const void* buffer, FwSizeType &size, bool wait);
        private:
          static const U32 INITIAL_CRC = 0xFFFFFFFF;
          /**
           * Working set for CRC calculations.
           */
          struct CrcWorkingSet {
              FwSizeType offset; //!< File offset tracking CRC calculation
              U32 crc; //!< CRC value currently calculated
              U8 buffer[FW_FILE_CHUNK_SIZE];
              bool eof; //!< End-of-file reached
              //! Constructor initializing CRC to 0xFFFFFFFF
              CrcWorkingSet();
          };
          /**
           * \brief Updates the CRC data by reading a chunk of data
           *
           * Helper function to perform CRC on single chunk of data. Expected to be called within loop until data.eof
           * reads true. Can perform nice calculations by closing and reopening the file to avoid holding file while the
           * CRC loop is performed.
           *
           * \param data: CrcWorkingSet data storing working set data
           * \param nice: true for less intrusive "nice" calculations, false otherwise
           * \return OP_OK on success otherwise error status
           */
          Status updateCRC(CrcWorkingSet& data, bool nice);

        PRIVATE:
          Mode mode; //!< Stores mode for error checking
          const CHAR* path; //!< Path last opened
          /**
           * This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
           * opaque and thus normal allocation cannot be done. Instead we allow the implementor to store then handle in
           * the byte-array here and set `handle` to that address for storage.
           */
          FileHandle* handle; //!< Pointer to the implementation defined file handle
          alignas(FW_HANDLE_ALIGNMENT) U8 handle_storage[FW_HANDLE_MAX_SIZE]; //!< Storage for aligned FileHandle data
    };
}

#endif

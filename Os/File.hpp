#ifndef _File_hpp_
#define _File_hpp_

#include <FpConfig.hpp>

namespace Os {

    // This class encapsulates a very simple file interface that has the most often-used features

    class File {
        public:

            typedef enum {
                OPEN_NO_MODE,   //!<  File mode not yet selected
                OPEN_READ, //!<  Open file for reading
                OPEN_WRITE, //!<  Open file for writing
                OPEN_SYNC_WRITE, //!<  Open file for writing; writes don't return until data is on disk
                OPEN_SYNC_DIRECT_WRITE, //!<  Open file for writing, bypassing all caching. Requires data alignment
                OPEN_CREATE, //!< Open file for writing and truncates file if it exists, ie same flags as creat()
                OPEN_APPEND, //!<  Open file for appending
            } Mode;

            typedef enum {
                OP_OK, //!<  Operation was successful
                DOESNT_EXIST, //!<  File doesn't exist (for read)
                NO_SPACE, //!<  No space left
                NO_PERMISSION, //!<  No permission to read/write file
                BAD_SIZE, //!<  Invalid size parameter
                NOT_OPENED, //!<  file hasn't been opened yet
                FILE_EXISTS, //!< file already exist (for CREATE with O_EXCL enabled)
                OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
            } Status;

            File(); //!<  Constructor
            virtual ~File(); //!<  Destructor. Will close file if still open
            Status prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len);
            Status open(const char* fileName, Mode mode); //!<  open file. Writing creates file if it doesn't exist
            Status open(const char* fileName, Mode mode, bool include_excl); //!<  open file. Writing creates file if it doesn't exist
            bool isOpen(); //!< check if file descriptor is open or not.
            Status seek(NATIVE_INT_TYPE offset, bool absolute = true); //!<  seek to location. If absolute = true, absolute from beginning of file
            Status flush(); //!< flush data to disk. No-op on systems that do not support.
            Status read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull = true); //!<  read data from file; returns amount read or errno.
                                                                            //!<  waitForFull = true to wait for all bytes to be read
                                                                            // size is modified to actual read size
            Status write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone = true); //!<  write size; will return amount written or errno
            Status bulkWrite(const void * buffer, NATIVE_UINT_TYPE &totalSize, NATIVE_INT_TYPE chunkSize); //!<  write size; will return amount written or errno

            void close(); //!<  close file

            NATIVE_INT_TYPE getLastError(); //!< read back last error code (typically errno)
            const char* getLastErrorString(); //!< get a string of the last error (typically from strerror)
            Status calculateCRC32(U32 &crc); //!< calculates the CRC32 of the file

            static Status niceCRC32(U32 &crc, const char* fileName); //!< Calculates CRC32 of file, not burdening FS

        private:

            NATIVE_INT_TYPE m_fd; //!<  Stored file descriptor
            Mode m_mode; //!<  Stores mode for error checking
            NATIVE_INT_TYPE m_lastError; //!<  stores last error

    };

}

#endif

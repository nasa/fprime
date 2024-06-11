#ifndef _Directory_hpp_
#define _Directory_hpp_

#include <FpConfig.hpp>

namespace Os {

    // This class encapsulates a very simple directory interface that has the most often-used features

    class Directory {
        public:

            typedef enum {
                OP_OK, //!<  Operation was successful
                DOESNT_EXIST, //!<  Directory doesn't exist
                NO_PERMISSION, //!<  No permission to read directory
                NOT_OPENED, //!<  Directory hasn't been opened yet
                NOT_DIR, //!<  Path is not a directory
                NO_MORE_FILES, //!<  Directory stream has no more files
                OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
            } Status;

            Directory(); //!<  Constructor
            virtual ~Directory(); //!<  Destructor. Will close directory if still open
            Status open(const char* dirName); //!<  open directory. Directory must already exist
            bool isOpen(); //!< check if file descriptor is open or not.
            Status rewind(); //!<  rewind directory stream to the beginning

            Status read(char * fileNameBuffer, U32 bufSize); //!< get next filename from directory
            Status read(char * fileNameBuffer, U32 bufSize, I64& inode); //!< get next filename and inode from directory
            void close(); //!<  close directory

            NATIVE_INT_TYPE getLastError(); //!< read back last error code (typically errno)
            const char* getLastErrorString(); //!< get a string of the last error (typically from strerror)

        private:

            POINTER_CAST m_dir; //!<  Stored directory pointer (type varies across implementations)
            NATIVE_INT_TYPE m_lastError; //!<  stores last error

    };

}

#endif

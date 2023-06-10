#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC

#ifdef __cplusplus
}
#endif // __cplusplus

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits>
#include <cstring>
#include <cstdio>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Os {

    File::File() :m_fd(-1),m_mode(OPEN_NO_MODE),m_lastError(0) {
    }

    File::~File() {
        if (this->m_mode != OPEN_NO_MODE) {
            this->close();
        }
    }

    File::Status File::open(const char* fileName, File::Mode mode) {
      return this->open(fileName, mode, true);
    }
    File::Status File::open(const char* fileName, File::Mode mode, bool include_excl) {

        NATIVE_INT_TYPE flags = 0;
        Status stat = OP_OK;

        switch (mode) {
            case OPEN_READ:
                flags = O_RDONLY;
                break;
            case OPEN_WRITE:
                flags = O_WRONLY | O_CREAT;
                break;
            case OPEN_SYNC_WRITE:
                flags = O_WRONLY | O_CREAT | O_SYNC;
                break;
#ifndef TGT_OS_TYPE_RTEMS
            case OPEN_SYNC_DIRECT_WRITE:
                flags = O_WRONLY | O_CREAT | O_DSYNC
#ifdef __linux__
                        | O_DIRECT;
#else
                ;
#endif
                break;
#endif
            case OPEN_CREATE:
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                if(include_excl) {
                    flags |= O_EXCL;
                }
                break;
            case OPEN_APPEND:
                flags = O_WRONLY | O_CREAT | O_APPEND;
                break;
            default:
                FW_ASSERT(0, mode);
                break;
        }

        NATIVE_INT_TYPE userFlags =
#ifdef __VXWORKS__
        0;
#else
        S_IRUSR|S_IWRITE;
#endif
        NATIVE_INT_TYPE fd = ::open(fileName,flags,userFlags);

        if (-1 == fd) {
            this->m_lastError = errno;
            switch (errno) {
                case ENOSPC:
                    stat = NO_SPACE;
                    break;
                case ENOENT:
                    stat = DOESNT_EXIST;
                    break;
                case EACCES:
                    stat = NO_PERMISSION;
                    break;
                case EEXIST:
                    stat = FILE_EXISTS;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
        }

        this->m_mode = mode;
        this->m_fd = fd;
        return stat;
    }

    bool File::isOpen() {
      return this->m_fd > 0;
    }

    File::Status File::prealloc(NATIVE_INT_TYPE offset, NATIVE_INT_TYPE len) {
        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        File::Status fileStatus = OP_OK;

#ifdef __linux__
        NATIVE_INT_TYPE stat = posix_fallocate(this->m_fd, offset, len);

        if (stat) {
            switch (stat) {
                case ENOSPC:
                case EFBIG:
                    fileStatus = NO_SPACE;
                    break;
                case EBADF:
                    fileStatus = DOESNT_EXIST;
                    break;
                default:
                    fileStatus = OTHER_ERROR;
                    break;
            }
        }
#endif

        return fileStatus;
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        Status stat = OP_OK;
        NATIVE_INT_TYPE whence = absolute?SEEK_SET:SEEK_CUR;

        off_t act_off = ::lseek(this->m_fd,offset,whence);

        // No error would be a normal one for this simple
        // class, so return other error
        if (act_off != offset) {
            if (-1 == act_off) {
                this->m_lastError = errno;
                stat = OTHER_ERROR;
            } else {
                stat = BAD_SIZE;
            }
        }

        return stat;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {

        FW_ASSERT(buffer);

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            size = 0;
            return NOT_OPENED;
        }
        // Validate read size before entering reading loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        NATIVE_INT_TYPE accSize = 0; // accumulated size

        Status stat = OP_OK;

        NATIVE_INT_TYPE maxIters = size*2; // loop limit; couldn't block more times than number of bytes

        while (maxIters > 0) {

            ssize_t readSize = ::read(this->m_fd,
#ifdef __VXWORKS__
	       static_cast<char*>(buffer)
#else
               buffer
#endif
               ,size-accSize);

            if (readSize != size-accSize) {
                // could be an error
                if (-1 == readSize) {
                    switch (errno) {
                        case EINTR: // read was interrupted
                            maxIters--; // decrement loop count
                            continue;
                        default:
                            stat = OTHER_ERROR;
                            break;
                    }
                    this->m_lastError = errno;
                    accSize = 0;
                    break; // break out of while loop
                } else if (0 == readSize) { // end of file
                    break;
                } else { // partial read so adjust read point and size
                    accSize += readSize;
                    if (not waitForFull) {
                        break; // break out of while loop
                    } else {
                        // in order to move the pointer ahead, we need to cast it
                        U8* charPtr = static_cast<U8*>(buffer);
                        charPtr = &charPtr[readSize];
                        buffer = static_cast<void*>(charPtr);
                    }
                    maxIters--; // decrement loop count
                }

            } else { // got number we wanted
                accSize += readSize;
                break; // break out of while loop
            }

            maxIters--; // decrement loop count

        } // end read while loop

        // make sure we didn't exceed loop count
        FW_ASSERT(maxIters > 0);

        size = accSize;

        return stat;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            size = 0;
            return NOT_OPENED;
        }
        // Validate read size before entering reading loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        Status stat = OP_OK;
        // just check for EINTR
        NATIVE_INT_TYPE maxIters = size*2; // loop limit; couldn't block more times than number of bytes
        while (maxIters > 0) {
            NATIVE_INT_TYPE writeSize = ::write(this->m_fd,
#ifdef __VXWORKS__
	       static_cast<char*>(const_cast<void*>(buffer)) // Ugly, but that's how VxWorks likes to roll...
#else
               buffer
#endif
               ,size);

            if (-1 == writeSize) {
                switch (errno) {
                    case EINTR: // write was interrupted
                        maxIters--; // decrement loop count
                        continue;
                    case ENOSPC:
                        stat = NO_SPACE;
                        break;
                    default:
                        DEBUG_PRINT("Error %d during write of 0x%p, addrMod %d, size %d, sizeMod %d\n",
                                    errno, buffer, static_cast<POINTER_CAST>(buffer) % 512, size, size % 512);
                        stat = OTHER_ERROR;
                        break;
                }
                this->m_lastError = errno;
                break; // break out of while loop
            } else {
                size = writeSize;

#ifdef __linux__
                if ((OPEN_SYNC_DIRECT_WRITE != this->m_mode) && (waitForDone)) {
                    NATIVE_UINT_TYPE position = lseek(this->m_fd, 0, SEEK_CUR);
                    sync_file_range(this->m_fd, position - writeSize, writeSize,
                                    SYNC_FILE_RANGE_WAIT_BEFORE
                                    | SYNC_FILE_RANGE_WRITE
                                    | SYNC_FILE_RANGE_WAIT_AFTER);
                }
#endif

                break; // break out of while loop
            }
        }

        return stat;
    }

    // NOTE(mereweth) - see http://lkml.iu.edu/hypermail/linux/kernel/1005.2/01845.html
    // recommendation from Linus Torvalds, but doesn't seem to be that fast
    File::Status File::bulkWrite(const void * buffer, NATIVE_UINT_TYPE &totalSize,
                                 NATIVE_INT_TYPE chunkSize) {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            totalSize = 0;
            return NOT_OPENED;
        }
        // Validate read size before entering reading loop. Linux's read call expects size_t, which
        // is defined as an unsigned value. Thus 0 and negative values rejected.
        if (totalSize == 0) {
            totalSize = 0;
            return BAD_SIZE;
        }
        else if (chunkSize <= 0) {
            totalSize = 0;
            return BAD_SIZE;
        }

#ifdef __linux__
        const NATIVE_UINT_TYPE startPosition = lseek(this->m_fd, 0, SEEK_CUR);
#endif
        NATIVE_UINT_TYPE newBytesWritten = 0;

        for (NATIVE_UINT_TYPE idx = 0; idx < totalSize; idx += chunkSize) {
            NATIVE_INT_TYPE size = chunkSize;
            // if we're on the last iteration and length isn't a multiple of chunkSize
            if (idx + chunkSize > totalSize) {
                size = totalSize - idx;
            }
            const NATIVE_INT_TYPE toWrite = size;
            FW_ASSERT(idx + size <= totalSize, idx + size);
            const Os::File::Status fileStatus = this->write(static_cast<const U8*>(buffer) + idx, size, false);
            if (!(fileStatus == Os::File::OP_OK
                  && size == static_cast<NATIVE_INT_TYPE>(toWrite))) {
                totalSize = newBytesWritten;
                return fileStatus;
            }

#ifdef __linux__
            sync_file_range(this->m_fd,
                            startPosition + newBytesWritten,
                            chunkSize,
                            SYNC_FILE_RANGE_WRITE);

            if (newBytesWritten) {
                sync_file_range(this->m_fd,
                                startPosition + newBytesWritten - chunkSize,
                                chunkSize,
                                SYNC_FILE_RANGE_WAIT_BEFORE
                                | SYNC_FILE_RANGE_WRITE
                                | SYNC_FILE_RANGE_WAIT_AFTER);

                posix_fadvise(this->m_fd,
                              startPosition + newBytesWritten - chunkSize,
                              chunkSize, POSIX_FADV_DONTNEED);
            }
#endif

            newBytesWritten += toWrite;
        }

        totalSize = newBytesWritten;
        return OP_OK;
    }

    File::Status File::flush() {
        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            return NOT_OPENED;
        }

        File::Status stat = OP_OK;

        if (-1 == fsync(this->m_fd)) {
            switch (errno) {
                case ENOSPC:
                    stat = NO_SPACE;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
        }

        return stat;
    }

    void File::close() {
        if ((this->m_fd != -1) and (this->m_mode != OPEN_NO_MODE)) {
            (void)::close(this->m_fd);
            this->m_fd = -1;
        }
        this->m_mode = OPEN_NO_MODE;
    }

    NATIVE_INT_TYPE File::getLastError() {
        return this->m_lastError;
    }

    const char* File::getLastErrorString() {
        return strerror(this->m_lastError);
    }

    File::Status File::calculateCRC32(U32 &crc)
    {

        // make sure it has been opened
        if (OPEN_NO_MODE == this->m_mode) {
            crc = 0;
            return NOT_OPENED;
        }

        const U32 maxChunkSize = 32;
        const U32 initialSeed = 0xFFFFFFFF;

        // Seek to beginning of file
        Status status = seek(0, true);
        if (status != OP_OK) {
            crc = 0;
            return status;
        }

        U8 file_buffer[maxChunkSize];

        bool endOfFile = false;

        U32 seed = initialSeed;
        const U32 maxIters = std::numeric_limits<U32>::max(); // loop limit
        U32 numIters = 0;

        while (!endOfFile && numIters < maxIters) {

            ++numIters;
            int chunkSize = maxChunkSize;

            status = read(file_buffer, chunkSize, false);
            if (status == OP_OK) {
                // chunkSize modified by file.read

                if (chunkSize == 0) {
                    endOfFile = true;
                    continue;
                }

                int chunkIdx = 0;

                while (chunkIdx < chunkSize) {
                  seed = update_crc_32(seed, file_buffer[chunkIdx]);
                  chunkIdx++;
                }

            } else {
                crc = 0;
                return status;
            }
        }

        if (!endOfFile) {
            crc = 0;
            return OTHER_ERROR;
        }
        else {
            crc = seed;
            return OP_OK;
        }
    }
}

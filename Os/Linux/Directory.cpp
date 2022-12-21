#include <FpConfig.hpp>
#include <Os/Directory.hpp>
#include <Fw/Types/Assert.hpp>

#include <dirent.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>

namespace Os {

    Directory::Directory() :m_dir(0),m_lastError(0) {
    }

    Directory::~Directory() {
        this->close();
    }

    Directory::Status Directory::open(const char* dirName) {

        Status stat = OP_OK;

        DIR* dir = ::opendir(dirName);

        if (dir == nullptr) {
            this->m_lastError = errno;
            switch (errno) {
                case ENOENT:
                    stat = DOESNT_EXIST;
                    break;
                case EACCES:
                    stat = NO_PERMISSION;
                    break;
                case ENOTDIR:
                    stat = NOT_DIR;
                    break;
                default:
                    stat = OTHER_ERROR;
                    break;
            }
            return stat;
        }

        this->m_dir = reinterpret_cast<POINTER_CAST>(dir);
        return stat;
    }

    Directory::Status Directory::rewind() {

        // make sure it has been opened
        if (!this->isOpen()) {
            return NOT_OPENED;
        }

        Status stat = OP_OK;
        DIR* dir = reinterpret_cast<DIR*>(this->m_dir);

        // no errors defined
        ::rewinddir(dir);

        return stat;
    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize) {

      I64 dummy;
      return this->read(fileNameBuffer, bufSize, dummy);

    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize, I64& inode) {

        FW_ASSERT(fileNameBuffer);

        // make sure it has been opened
        if (!this->isOpen()) {
            return NOT_OPENED;
        }

        Status stat = OP_OK;
        DIR* dir = reinterpret_cast<DIR*>(this->m_dir);

        // Set errno to 0 so we know why we exited readdir
        errno = 0;

        struct dirent *direntData = nullptr;
        while ((direntData = ::readdir(dir)) != nullptr) {
            // Skip hidden files
            if (direntData->d_name[0] != '.') {
                strncpy(fileNameBuffer, direntData->d_name, bufSize);
                inode = direntData->d_ino;
                break;
            }
        }

        if (direntData == nullptr) {
            // loop ended because readdir failed, did it error or did we run out of files?
            if(errno != 0) {
                // Only error from readdir is EBADF
                stat = OTHER_ERROR;
                this->m_lastError = errno;
            }
            else {
                stat = NO_MORE_FILES;
            }
        }

        return stat;
    }

    bool Directory::isOpen() {
        return this->m_dir > 0;
    }

    void Directory::close() {
        if (this->isOpen()) {
            DIR* dir = reinterpret_cast<DIR*>(this->m_dir);
            (void)::closedir(dir);
        }
        this->m_dir = 0;
    }

    NATIVE_INT_TYPE Directory::getLastError() {
        return this->m_lastError;
    }

    const char* Directory::getLastErrorString() {
        return strerror(this->m_lastError);
    }

}

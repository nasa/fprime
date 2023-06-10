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

    Directory::Directory() :m_dir(0),m_lastError(-1) {}

    Directory::~Directory() {}

    Directory::Status Directory::open(const char* dirName) {
        return Directory::Status::OTHER_ERROR;
    }

    Directory::Status Directory::rewind() {
        return Directory::Status::OTHER_ERROR;
    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize) {
        return Directory::Status::OTHER_ERROR;
    }

    Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize, I64& inode) {
        return Directory::Status::OTHER_ERROR;
    }

    bool Directory::isOpen() {
        return false;
    }

    void Directory::close() {}

    NATIVE_INT_TYPE Directory::getLastError() {
        return this->m_lastError;
    }

    const char* Directory::getLastErrorString() {
        return "Stub directory implementation";
    }

}

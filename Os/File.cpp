// ======================================================================
// \title Os/File.cpp
// \brief common function implementation for Os::File
// ======================================================================
#include <Os/File.hpp>
namespace Os {

File::File() : m_fd(-1), m_mode(OPEN_NO_MODE), m_lastError(0) {}

File::~File() {
    if (this->m_mode != OPEN_NO_MODE) {
//        this->close();
    }
}

File::Status File::open(const char* path, File::Mode mode, bool overwrite) {
    return this->openInternal(path, mode, overwrite);
}
}
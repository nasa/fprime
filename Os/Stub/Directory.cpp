// ======================================================================
// \title Os/Stub/Directory.cpp
// \brief stub implementation for Os::Directory
// ======================================================================
#include "Os/Stub/Directory.hpp"

namespace Os {
namespace Stub {
namespace Directory {

StubDirectory::Status StubDirectory::open(const char* dirName) {
    return Status::NOT_SUPPORTED;
}

bool StubDirectory::isOpen() {
    return false;
}

StubDirectory::Status StubDirectory::rewind() {
    return Status::NOT_SUPPORTED;
}

StubDirectory::Status StubDirectory::read(char * fileNameBuffer, U32 bufSize) {
    return Status::NOT_SUPPORTED;
}

StubDirectory::Status StubDirectory::read(char * fileNameBuffer, U32 bufSize, I64& inode) {
    return Status::NOT_SUPPORTED;
}

void StubDirectory::close() {
    // no-op
}

DirectoryHandle* StubDirectory::getHandle() {
    return &this->m_handle;
}

} // namespace Directory
} // namespace Stub
} // namespace Os

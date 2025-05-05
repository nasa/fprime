// ======================================================================
// \title Os/Stub/Directory.cpp
// \brief stub implementation for Os::Directory
// ======================================================================
#include "Os/Stub/Directory.hpp"

namespace Os {
namespace Stub {
namespace Directory {

StubDirectory::Status StubDirectory::open(const char* path, OpenMode mode) {
    return Status::NOT_SUPPORTED;
}

StubDirectory::Status StubDirectory::rewind() {
    return Status::NOT_SUPPORTED;
}

StubDirectory::Status StubDirectory::read(char * fileNameBuffer, FwSizeType bufSize) {
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

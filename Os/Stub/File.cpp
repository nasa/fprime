// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include "Os/Stub/File.hpp"

namespace Os {
namespace Stub {
namespace File {

    StubFile::Status StubFile::open(const char *filepath, StubFile::Mode open_mode, OverwriteType overwrite) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    void StubFile::close() {}

    StubFile::Status StubFile::size(FwSignedSizeType &size_result) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::position(FwSignedSizeType &position_result) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::preallocate(FwSignedSizeType offset, FwSignedSizeType length) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::seek(FwSignedSizeType offset, SeekType seekType) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::flush() {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::read(U8 *buffer, FwSignedSizeType &size, StubFile::WaitType wait) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    StubFile::Status StubFile::write(const U8 *buffer, FwSignedSizeType &size, StubFile::WaitType wait) {
        Status status = Status::NOT_SUPPORTED;
        return status;
    }

    FileHandle* StubFile::getHandle() {
        return &this->m_handle;
    }

} // namespace File
} // namespace Stub
} // namespace Os

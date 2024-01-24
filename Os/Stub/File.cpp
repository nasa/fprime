// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include <new>
#include <Os/File.hpp>

namespace Os {
    class FileHandle {};

    void File::constructInternal() {
        // Placement-new the file handle into the opaque file-handle storage
        this->m_handle = new(&this->m_handle_storage[0]) FileHandle;
    }

    void File::destructInternal() {}

    File::Status File::openInternal(const char *filepath, File::Mode open_mode, bool overwrite) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    void File::closeInternal() {}

    File::Status File::sizeInternal(FwSignedSizeType& size_result) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::positionInternal(FwSignedSizeType &position_result) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::preallocateInternal(FwSignedSizeType offset, FwSignedSizeType length) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::seekInternal(FwSignedSizeType offset, bool absolute) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::flushInternal() {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::readInternal(U8 *buffer, FwSignedSizeType &size, bool wait) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }

    File::Status File::writeInternal(const void *buffer, FwSignedSizeType &size, bool wait) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        return status;
    }
} // Os
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
        this->handle = new(&this->handle_storage[0]) FileHandle;
    }

    void File::destructInternal() {}

    File::Status File::openInternal(const char *filepath, File::Mode open_mode, bool overwrite) {
        return Os::File::Status::OTHER_ERROR;
    }

    void File::closeInternal() {}

    File::Status File::preallocateInternal(FwSizeType offset, FwSizeType len) {
        return Os::File::Status::OTHER_ERROR;
    }

    File::Status File::seekInternal(FwSizeType offset, bool absolute) {
        return Os::File::Status::OTHER_ERROR;
    }

    File::Status File::flushInternal() {
        return Os::File::Status::OTHER_ERROR;
    }

    File::Status File::readInternal(U8 *buffer, FwSizeType &size, bool wait) {
        return Os::File::Status::OTHER_ERROR;
    }

    File::Status File::writeInternal(const void *buffer, FwSizeType &size, bool wait) {
        return Os::File::Status::OTHER_ERROR;
    }


} // Os
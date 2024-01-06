// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include <new>
#include <Os/Stub/File.hpp>
namespace Os {
namespace Stub {
namespace { // Prevents external linkage of this namespaced internal variables

    // Status to return on the next call
    Os::File::Status NEXT_STATUS = Os::File::Status::OP_OK;

    // Variable tracking last (internal) function called
    Os::Stub::LastFunctionCalled LAST = Os::Stub::LastFunctionCalled::NONE;

} // Anonymous namespace
    void file_set_next_status(Os::File::Status status) {
        Os::Stub::NEXT_STATUS = status;
    }

    Os::Stub::LastFunctionCalled file_get_last_call() {
        return LAST;
    }
} // Stub

Os::FileHandle::FileHandle() : state(Os::FileHandle::State::UNOPENED) {}

void File::constructInternal() {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::CONSTRUCT_FN;
    // Placement-new the file handle into the opaque file-handle storage
    this->handle = new (&this->handle_storage[0]) FileHandle;
}

void File::destructInternal(){
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::DESTRUCT_FN;
}

File::Status File::openInternal(const char* path, File::Mode open_mode, bool overwrite) {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::OPEN_FN;
    this->handle->state = (Os::Stub::NEXT_STATUS == Os::File::Status::OP_OK) ? Os::FileHandle::State::OPENED : Os::FileHandle::State::FAILED;
    return Os::Stub::NEXT_STATUS;
}

void File::closeInternal() {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::CLOSE_FN;
    this->handle->state = Os::FileHandle::State::CLOSED;
}

File::Status File::preallocateInternal(FwSizeType offset, FwSizeType len) {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::PREALLOCATE_FN;
    return Os::Stub::NEXT_STATUS;
}

File::Status File::seekInternal(FwSizeType offset, bool absolute) {
    Os::Stub::LAST = (absolute) ? Os::Stub::LastFunctionCalled::SEEK_ABSOLUTE_FN : Os::Stub::LastFunctionCalled::SEEK_FN;
    return Os::Stub::NEXT_STATUS;
}

File::Status File::flushInternal() {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::FLUSH_FN;
    return Os::Stub::NEXT_STATUS;
}

File::Status File::readInternal(U8* buffer, FwSizeType &size, bool wait) {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::READ_FN;
    return Os::Stub::NEXT_STATUS;
}

File::Status File::writeInternal(const void* buffer, FwSizeType &size, bool wait) {
    Os::Stub::LAST = Os::Stub::LastFunctionCalled::READ_FN;
    return Os::Stub::NEXT_STATUS;
}


} // Os
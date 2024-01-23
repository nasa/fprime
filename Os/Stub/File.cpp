// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include <new>
#include <Os/File.hpp>
#include <Os/Stub/File.hpp>

namespace Os {
namespace Stub {
namespace File {
namespace Handlers {
    ConstructHandler constructHandler = nullptr;
    DestructHandler destructHandler = nullptr;
    OpenHandler openHandler = nullptr;
    CloseHandler closeHandler = nullptr;
    SizeHandler sizeHandler = nullptr;
    PositionHandler positionHandler = nullptr;
    PreallocateHandler preallocateHandler = nullptr;
    SeekHandler seekHandler = nullptr;
    FlushHandler flushHandler = nullptr;
    ReadHandler readHandler = nullptr;
    WriteHandler writeHandler = nullptr;
} // namespace Handlers
} // namespace File
} // namespace Stub
    /**
     * Stub file handle for implementation left blank
     */
    class FileHandle {
    };

    void File::constructInternal() {
        // Placement-new the file handle into the opaque file-handle storage
        this->m_handle = new(&this->m_handle_storage[0]) FileHandle;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::constructHandler) {
            Os::Stub::File::Handlers::constructHandler();
        }
    }

    void File::destructInternal() {
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::destructHandler) {
            Os::Stub::File::Handlers::destructHandler();
        }
    }

    File::Status File::openInternal(const char *filepath, File::Mode open_mode, bool overwrite) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::openHandler) {
            status = Os::Stub::File::Handlers::openHandler(filepath, open_mode, overwrite);
        }
        return status;
    }

    void File::closeInternal() {
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::closeHandler) {
            Os::Stub::File::Handlers::closeHandler();
        }
    }

    File::Status File::sizeInternal(FwSizeType& size_result) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::sizeHandler) {
            status = Os::Stub::File::Handlers::sizeHandler(size_result);
        }
        return status;
    }

    File::Status File::positionInternal(FwSizeType &position_result) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::positionHandler) {
            status = Os::Stub::File::Handlers::positionHandler(position_result);
        }
        return status;
    }


    File::Status File::preallocateInternal(FwSizeType offset, FwSizeType length) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::preallocateHandler) {
            status = Os::Stub::File::Handlers::preallocateHandler(offset, length);
        }
        return status;
    }

    File::Status File::seekInternal(FwSizeType offset, bool absolute) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::seekHandler) {
            status = Os::Stub::File::Handlers::seekHandler(offset, absolute);
        }
        return status;
    }

    File::Status File::flushInternal() {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::flushHandler) {
            status = Os::Stub::File::Handlers::flushHandler();
        }
        return status;
    }

    File::Status File::readInternal(U8 *buffer, FwSizeType &size, bool wait) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::readHandler) {
            status = Os::Stub::File::Handlers::readHandler(buffer, size, wait);
        }
        return status;
    }

    File::Status File::writeInternal(const void *buffer, FwSizeType &size, bool wait) {
        Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
        // Run assignable handler when set
        if (nullptr != Os::Stub::File::Handlers::writeHandler) {
            status = Os::Stub::File::Handlers::writeHandler(buffer, size, wait);
        }
        return status;
    }
} // Os
// ======================================================================
// \title Os/Generic/MultiFileSystem/File.cpp
// \brief MultiFileSystem implementation for Os::File
// ======================================================================
#include "Os/Generic/MultiFileSystem/File.hpp"
#include "Os/Delegate.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

// Copy constructor - delegates to the underlying file interface
MultiFile::MultiFile(const MultiFile& other) : m_file_sub_delegate(other.m_file_sub_delegate) {
    // NOTE: m_handle is just a container; the actual file operations
    // are delegated to m_file_sub_delegate
    // Should m_file_sub_delegate be INSIDE m_handle ??
}

// Assignment operator - delegates to the underlying file interface
MultiFile& MultiFile::operator=(const MultiFile& other) {
    if (this != &other) {
        this->m_file_sub_delegate = other.m_file_sub_delegate;
    }
    return *this;
}

MultiFile::Status MultiFile::open(const char* path, Mode mode, OverwriteType overwrite) {
    if (path == nullptr) {
        return Status::INVALID_ARGUMENT;
    }

    // Route path to appropriate filesystem implementation
    OsalImplSet* impl = nullptr;
    FwIndexType prefix_len = 0;
    impl = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl == nullptr) {
        return Status::OTHER_ERROR;
    }

    // Create independent instance via placement-new into local storage
    if (impl->file_factory == nullptr) {
        return Status::OTHER_ERROR;
    }
    this->m_file_sub_delegate = impl->file_factory(this->m_sub_delegate_storage);
    FW_ASSERT(this->m_file_sub_delegate != nullptr);

    const char* path_after_prefix = path + prefix_len;
    Status status = this->m_file_sub_delegate->open(path_after_prefix, mode, overwrite);
    if (status != Status::OP_OK) {
        this->m_file_sub_delegate->close();
        this->m_file_sub_delegate->~FileInterface();
        this->m_file_sub_delegate = nullptr;
    }
    return status;
}

void MultiFile::close() {
    if (this->m_file_sub_delegate != nullptr) {
        this->m_file_sub_delegate->close();
        this->m_file_sub_delegate->~FileInterface();
        this->m_file_sub_delegate = nullptr;
    }
}

MultiFile::Status MultiFile::size(FwSizeType& size_result) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->size(size_result);
}

MultiFile::Status MultiFile::position(FwSizeType& position_result) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->position(position_result);
}

MultiFile::Status MultiFile::preallocate(FwSizeType offset, FwSizeType length) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->preallocate(offset, length);
}

MultiFile::Status MultiFile::seek(FwSignedSizeType offset, SeekType seekType) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->seek(offset, seekType);
}

MultiFile::Status MultiFile::flush() {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->flush();
}

MultiFile::Status MultiFile::read(U8* buffer, FwSizeType& size, WaitType wait) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->read(buffer, size, wait);
}

MultiFile::Status MultiFile::write(const U8* buffer, FwSizeType& size, WaitType wait) {
    if (this->m_file_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_sub_delegate->write(buffer, size, wait);
}

FileHandle* MultiFile::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os

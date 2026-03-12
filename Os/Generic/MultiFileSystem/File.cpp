// ======================================================================
// \title Os/Generic/MultiFileSystem/File.cpp
// \brief MultiFileSystem implementation for Os::File
// ======================================================================
#include "Os/Generic/MultiFileSystem/File.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

// Copy constructor - delegates to the underlying file interface
MultiFile::MultiFile(const MultiFile& other) : m_file_interface(other.m_file_interface) {
    // Note: m_handle is just a container; the actual file operations
    // are delegated to m_file_interface
}

// Assignment operator - delegates to the underlying file interface
MultiFile& MultiFile::operator=(const MultiFile& other) {
    if (this != &other) {
        this->m_file_interface = other.m_file_interface;
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

    // Store pointer to routed File interface for use in subsequent operations
    this->m_file_interface = impl->file;
    if (this->m_file_interface == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;
    Status status = this->m_file_interface->open(path_after_prefix, mode, overwrite);
    if (status != Status::OP_OK) {
        this->m_file_interface = nullptr;
    }
    return status;
}

void MultiFile::close() {
    if (this->m_file_interface != nullptr) {
        this->m_file_interface->close();
        this->m_file_interface = nullptr;
    }
}

MultiFile::Status MultiFile::size(FwSizeType& size_result) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->size(size_result);
}

MultiFile::Status MultiFile::position(FwSizeType& position_result) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->position(position_result);
}

MultiFile::Status MultiFile::preallocate(FwSizeType offset, FwSizeType length) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->preallocate(offset, length);
}

MultiFile::Status MultiFile::seek(FwSignedSizeType offset, SeekType seekType) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->seek(offset, seekType);
}

MultiFile::Status MultiFile::flush() {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->flush();
}

MultiFile::Status MultiFile::read(U8* buffer, FwSizeType& size, WaitType wait) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->read(buffer, size, wait);
}

MultiFile::Status MultiFile::write(const U8* buffer, FwSizeType& size, WaitType wait) {
    if (this->m_file_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_file_interface->write(buffer, size, wait);
}

FileHandle* MultiFile::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os

// ======================================================================
// \title Os/Generic/MultiFileSystem/Directory.cpp
// \brief MultiFileSystem implementation for Os::Directory
// ======================================================================
#include "Os/Generic/MultiFileSystem/Directory.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

// Copy constructor - delegates to the underlying directory interface
MultiDirectory::MultiDirectory(const MultiDirectory& other) : m_directory_sub_delegate(other.m_directory_sub_delegate) {
    // TODO?
    // Note: m_handle is just a container; the actual directory operations
    // are delegated to m_directory_sub_delegate
}

// Assignment operator - delegates to the underlying directory interface
MultiDirectory& MultiDirectory::operator=(const MultiDirectory& other) {
    if (this != &other) {
        this->m_directory_sub_delegate = other.m_directory_sub_delegate;
    }
    return *this;
}

MultiDirectory::Status MultiDirectory::open(const char* path, OpenMode mode) {
    if (path == nullptr) {
        return Status::OTHER_ERROR;
    }

    // Route path to appropriate filesystem implementation
    OsalImplSet* impl = nullptr;
    FwIndexType prefix_len = 0;
    impl = OsalRegistry::routePathToImplementation(path, prefix_len);
    if (impl == nullptr) {
        return Status::OTHER_ERROR;
    }
    const char* path_after_prefix = path + prefix_len;

    // Create independent instance of directory implementation via factory
    if (impl->directory_factory == nullptr) {
        return Status::OTHER_ERROR;
    }
    this->m_directory_sub_delegate = impl->directory_factory(this->m_sub_delegate_storage);
    if (this->m_directory_sub_delegate == nullptr) {
        return Status::OTHER_ERROR;
    }

    Status status = this->m_directory_sub_delegate->open(path_after_prefix, mode);
    if (status != Status::OP_OK) {
        this->m_directory_sub_delegate->close();
        this->m_directory_sub_delegate->~DirectoryInterface();
        this->m_directory_sub_delegate = nullptr;
    }
    return status;
}

void MultiDirectory::close() {
    if (this->m_directory_sub_delegate != nullptr) {
        this->m_directory_sub_delegate->close();
        this->m_directory_sub_delegate->~DirectoryInterface();
        this->m_directory_sub_delegate = nullptr;
    }
}

MultiDirectory::Status MultiDirectory::rewind() {
    if (this->m_directory_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_directory_sub_delegate->rewind();
}

MultiDirectory::Status MultiDirectory::read(char* fileNameBuffer, FwSizeType buffSize) {
    if (this->m_directory_sub_delegate == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_directory_sub_delegate->read(fileNameBuffer, buffSize);
}

DirectoryHandle* MultiDirectory::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os

// ======================================================================
// \title Os/Generic/MultiFileSystem/Directory.cpp
// \brief MultiFileSystem implementation for Os::Directory
// ======================================================================
#include "Os/Generic/MultiFileSystem/Directory.hpp"
#include "Os/Generic/MultiFileSystem/OsalRegistry.hpp"

namespace Os {
namespace Generic {

// Copy constructor - delegates to the underlying directory interface
MultiDirectory::MultiDirectory(const MultiDirectory& other) : m_directory_interface(other.m_directory_interface) {
    // TODO?
    // Note: m_handle is just a container; the actual directory operations
    // are delegated to m_directory_interface
}

// Assignment operator - delegates to the underlying directory interface
MultiDirectory& MultiDirectory::operator=(const MultiDirectory& other) {
    if (this != &other) {
        this->m_directory_interface = other.m_directory_interface;
    }
    return *this;
}

MultiDirectory::Status MultiDirectory::open(const char* path, OpenMode mode) {
    if (path == nullptr) {
        return Status::OTHER_ERROR;
    }

    // Route path to appropriate filesystem implementation
    OsalImplSet* impl = OsalRegistry::routePathToImplementation(path);
    if (impl == nullptr) {
        return Status::OTHER_ERROR;
    }

    // Store pointer to routed Directory interface for use in subsequent operations
    this->m_directory_interface = impl->directory;
    if (this->m_directory_interface == nullptr) {
        return Status::OTHER_ERROR;
    }

    Status status = this->m_directory_interface->open(path, mode);
    if (status != Status::OP_OK) {
        this->m_directory_interface = nullptr;
    }
    return status;
}

void MultiDirectory::close() {
    if (this->m_directory_interface != nullptr) {
        this->m_directory_interface->close();
        this->m_directory_interface = nullptr;
    }
}

MultiDirectory::Status MultiDirectory::rewind() {
    if (this->m_directory_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_directory_interface->rewind();
}

MultiDirectory::Status MultiDirectory::read(char* fileNameBuffer, FwSizeType buffSize) {
    if (this->m_directory_interface == nullptr) {
        return Status::NOT_OPENED;
    }
    return this->m_directory_interface->read(fileNameBuffer, buffSize);
}

DirectoryHandle* MultiDirectory::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os

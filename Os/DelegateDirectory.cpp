// ======================================================================
// \title Os/DelegateDirectory.cpp
// \brief common function implementation for Os::DelegateDirectory
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateDirectory.hpp>

namespace Os {

DelegateDirectory::DelegateDirectory()
    : m_handle_storage(), m_delegate(*DirectoryInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
}

DelegateDirectory::~DelegateDirectory() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (this->isOpen()) {
        this->close();
    }
    this->m_delegate.~DirectoryInterface();
}

DirectoryHandle* DelegateDirectory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateDirectory::Status DelegateDirectory::open(const char* path, OpenMode mode) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(path != nullptr);
    FW_ASSERT(mode >= 0 and mode < OpenMode::MAX_OPEN_MODE);
    Status status = this->m_delegate.open(path, mode);
    if (status == Status::OP_OK) {
        this->setOpen(true);
    }
    return status;
}

DelegateDirectory::Status DelegateDirectory::rewind() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (not this->isOpen()) {
        return Status::NOT_OPENED;
    }
    return this->m_delegate.rewind();
}

DelegateDirectory::Status DelegateDirectory::read(char* fileNameBuffer, FwSizeType bufSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    if (not this->isOpen()) {
        return Status::NOT_OPENED;
    }
    FW_ASSERT(fileNameBuffer != nullptr);
    FW_ASSERT(bufSize > 0);
    Status status = this->m_delegate.read(fileNameBuffer, bufSize);
    fileNameBuffer[bufSize - 1] = '\0';  // Guarantee null-termination
    return status;
}

void DelegateDirectory::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    this->setOpen(false);
    return this->m_delegate.close();
}

}  // namespace Os

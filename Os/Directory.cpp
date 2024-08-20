// ======================================================================
// \title Os/Directory.cpp
// \brief common function implementation for Os::Directory
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Directory.hpp>

namespace Os {

Directory::Directory() : m_handle_storage(), m_delegate(*DirectoryInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
}

Directory::~Directory() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    m_delegate.~DirectoryInterface();
}

DirectoryHandle* Directory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

Directory::Status Directory::open(const char* path) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.open(path);
}

bool Directory::isOpen() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.isOpen();
}
Directory::Status Directory::rewind() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.rewind();
}

Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.read(fileNameBuffer, bufSize);
}

Directory::Status Directory::read(char * fileNameBuffer, U32 bufSize, I64& inode) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.read(fileNameBuffer, bufSize, inode);
}

void Directory::close() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<DirectoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.close();
}

}  // namespace Os
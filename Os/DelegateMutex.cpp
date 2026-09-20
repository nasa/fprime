// ======================================================================
// \title Os/DelegateMutex.cpp
// \brief implementation of Os::DelegateMutex, plus common Os::MutexInterface and Os::ScopeLock code
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateMutex.hpp>

namespace Os {

// ----------------------------------------------------------------------
// DelegateMutex: link-time delegating implementation
// ----------------------------------------------------------------------

DelegateMutex::DelegateMutex() : m_delegate(*MutexInterface::getDelegate(m_handle_storage)) {
    // m_handle_storage is placement-new storage populated by getDelegate(); do not value-initialize it (#5297)
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
}

DelegateMutex::~DelegateMutex() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    m_delegate.~MutexInterface();
}

MutexHandle* DelegateMutex::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateMutex::Status DelegateMutex::take() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.take();
}

DelegateMutex::Status DelegateMutex::release() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.release();
}

}  // namespace Os

// Note: MutexInterface::lock()/unLock() and ScopeLock are defined inline in Os/MutexInterface.hpp
// so they are available regardless of the configured implementation and can devirtualize under
// compile-time selection without pulling this delegate TU into the link.

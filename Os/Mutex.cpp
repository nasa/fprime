// ======================================================================
// \title Os/Mutex.cpp
// \brief common function implementation for Os::Mutex
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Mutex.hpp>

namespace Os {

Mutex::Mutex() : m_handle_storage(), m_delegate(*MutexInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
}

Mutex::~Mutex() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    m_delegate.~MutexInterface();
}

MutexHandle* Mutex::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

void Mutex::lock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.lock();
}

void Mutex::unLock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.unLock();
}
}  // namespace Os

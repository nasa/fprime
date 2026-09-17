// ======================================================================
// \title Os/DelegateCountingSemaphore.cpp
// \brief common function implementation for Os::CountingSemaphore
// ======================================================================
#include "Os/DelegateCountingSemaphore.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
DelegateCountingSemaphore::DelegateCountingSemaphore(U32 initial_count)
    : m_delegate(*CountingSemaphoreInterface::getDelegate(m_handle_storage, initial_count)) {}

DelegateCountingSemaphore::~DelegateCountingSemaphore() {
    m_delegate.~CountingSemaphoreInterface();
}

DelegateCountingSemaphore::Status DelegateCountingSemaphore::wait() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.wait();
}

DelegateCountingSemaphore::Status DelegateCountingSemaphore::waitTimeout(const Fw::TimeInterval& interval) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.waitTimeout(interval);
}

DelegateCountingSemaphore::Status DelegateCountingSemaphore::tryWait() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.tryWait();
}

DelegateCountingSemaphore::Status DelegateCountingSemaphore::post() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.post();
}

CountingSemaphoreHandle* DelegateCountingSemaphore::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

}  // namespace Os

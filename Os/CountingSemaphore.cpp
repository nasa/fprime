#include "Os/CountingSemaphore.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
CountingSemaphore::CountingSemaphore(U32 initial_count, int pshared)
    : m_delegate(*CountingSemaphoreInterface::getDelegate(m_handle_storage, initial_count, pshared)) {}

CountingSemaphore::~CountingSemaphore() {
    m_delegate.~CountingSemaphoreInterface();
}

CountingSemaphore::Status CountingSemaphore::wait() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.wait();
}

CountingSemaphore::Status CountingSemaphore::waitTimeout(U32 timeout_ms) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(timeout_ms > 0);
    return this->m_delegate.waitTimeout(timeout_ms);
}

CountingSemaphore::Status CountingSemaphore::tryWait() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.tryWait();
}

CountingSemaphore::Status CountingSemaphore::post() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.post();
}

CountingSemaphoreHandle* CountingSemaphore::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const CountingSemaphoreInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

}  // namespace Os

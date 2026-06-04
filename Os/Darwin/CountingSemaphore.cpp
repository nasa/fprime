// ======================================================================
// \title Os/Darwin/CountingSemaphore.cpp
// \brief Darwin implementations for Os::CountingSemaphore
// ======================================================================
#include "Os/Darwin/CountingSemaphore.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Darwin/error.hpp"

namespace Os {
namespace Darwin {
namespace Semaphore {

DarwinCountingSemaphore::DarwinCountingSemaphore(U32 initial_count) {
    // dispatch_semaphore_create takes initial value (similar to POSIX sem_init)
    this->m_handle.m_semaphore = dispatch_semaphore_create(static_cast<long>(initial_count));
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
}

DarwinCountingSemaphore::~DarwinCountingSemaphore() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    dispatch_release(this->m_handle.m_semaphore);
    this->m_handle.m_semaphore = nullptr;
}

Os::CountingSemaphore::Status DarwinCountingSemaphore::wait() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, DISPATCH_TIME_FOREVER);
    FW_ASSERT(result == 0, static_cast<FwAssertArgType>(result));
    return dispatch_result_to_semaphore_status(result);
}

Os::CountingSemaphore::Status DarwinCountingSemaphore::waitTimeout(const Fw::TimeInterval& interval) {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);

    int64_t nsec = static_cast<int64_t>(interval.getSeconds()) * 1000000000LL +
                   static_cast<int64_t>(interval.getUSeconds()) * 1000LL;
    dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, nsec);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, timeout);

    return dispatch_result_to_semaphore_status(result);
}

Os::CountingSemaphore::Status DarwinCountingSemaphore::tryWait() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, DISPATCH_TIME_NOW);
    return dispatch_result_to_semaphore_status(result);
}

Os::CountingSemaphore::Status DarwinCountingSemaphore::post() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    // dispatch_semaphore_signal returns non-zero if a thread was woken, zero otherwise.
    // Both outcomes are valid — no error condition exists.
    (void)dispatch_semaphore_signal(this->m_handle.m_semaphore);
    return Os::CountingSemaphore::Status::OP_OK;
}

CountingSemaphoreHandle* DarwinCountingSemaphore::getHandle() {
    return &m_handle;
}

}  // namespace Semaphore
}  // namespace Darwin
}  // namespace Os

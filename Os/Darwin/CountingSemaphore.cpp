// ======================================================================
// \title Os/Darwin/CountingSemaphore.cpp
// \brief Darwin implementations for Os::CountingSemaphore
// ======================================================================
#include "Os/Darwin/CountingSemaphore.hpp"
#include <mach/mach_types.h>
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Darwin {
namespace Semaphore {

DarwinCountingSemaphore::DarwinCountingSemaphore(U32 initial_count, int pshared) {
    // Note: GCD semaphores ignore pshared parameter as they are process-local by design
    (void)pshared;
    
    // dispatch_semaphore_create takes initial value (similar to POSIX sem_init)
    this->m_handle.m_semaphore = dispatch_semaphore_create(static_cast<long>(initial_count));
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
}

DarwinCountingSemaphore::~DarwinCountingSemaphore() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    dispatch_release(this->m_handle.m_semaphore);
    this->m_handle.m_semaphore = nullptr;
}

CountingSemaphore::Status DarwinCountingSemaphore::wait() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, DISPATCH_TIME_FOREVER);
    FW_ASSERT(result == 0, static_cast<FwAssertArgType>(result));
    return kern_return_to_semaphore_status(result);
}

CountingSemaphore::Status DarwinCountingSemaphore::waitTimeout(U32 timeout_ms) {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    FW_ASSERT(timeout_ms > 0);
    
    dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(timeout_ms) * 1000000LL);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, timeout);
    
    return kern_return_to_semaphore_status(result);
}

CountingSemaphore::Status DarwinCountingSemaphore::tryWait() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    long result = dispatch_semaphore_wait(this->m_handle.m_semaphore, DISPATCH_TIME_NOW);
    FW_ASSERT(result >= 0);
    
    return kern_return_to_semaphore_status(result);
}

CountingSemaphore::Status DarwinCountingSemaphore::post() {
    FW_ASSERT(this->m_handle.m_semaphore != nullptr);
    long result = dispatch_semaphore_signal(this->m_handle.m_semaphore);
    FW_ASSERT(result <= 0);
    return CountingSemaphore::Status::OP_OK;
}

CountingSemaphoreHandle* DarwinCountingSemaphore::getHandle() {
    return &m_handle;
}

}  // namespace Semaphore
}  // namespace Darwin
}  // namespace Os

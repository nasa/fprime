// ======================================================================
// \title Os/Stub/CountingSemaphore.cpp
// \brief Stub implementations for Os::CountingSemaphore
// ======================================================================
#include "Os/Stub/CountingSemaphore.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Stub {
namespace Semaphore {

StubCountingSemaphore::StubCountingSemaphore(U32 initial_count) {
    (void)initial_count;
}

StubCountingSemaphore::Status StubCountingSemaphore::wait() {
    return StubCountingSemaphore::Status::NOT_SUPPORTED;
}

StubCountingSemaphore::Status StubCountingSemaphore::waitTimeout(const Fw::TimeInterval& interval) {
    (void)interval;
    return StubCountingSemaphore::Status::NOT_SUPPORTED;
}

StubCountingSemaphore::Status StubCountingSemaphore::tryWait() {
    return StubCountingSemaphore::Status::NOT_SUPPORTED;
}

StubCountingSemaphore::Status StubCountingSemaphore::post() {
    return StubCountingSemaphore::Status::NOT_SUPPORTED;
}

CountingSemaphoreHandle* StubCountingSemaphore::getHandle() {
    return &m_handle;
}

}  // namespace Semaphore
}  // namespace Stub
}  // namespace Os

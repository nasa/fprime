// ======================================================================
// \title Os/Posix/CountingSemaphore.cpp
// \brief Posix implementations for Os::CountingSemaphore
// ======================================================================
#include "Os/Posix/CountingSemaphore.hpp"
#include <cerrno>
#include <ctime>
#include "Fw/Types/Assert.hpp"
#include "Os/Posix/error.hpp"

// On macOS, POSIX unnamed semaphores (sem_init/sem_destroy) are deprecated
// and sem_timedwait does not exist. This library is still compiled on macOS
// but the Darwin implementation is used at link time. Suppress deprecation
// warnings so the library compiles cleanly.
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace Os {
namespace Posix {
namespace Semaphore {

PosixCountingSemaphore::PosixCountingSemaphore(U32 initial_count) {
    // Default all pthread semaphores to only be visible to single application (since F' FSW is a single application)
    int status = sem_init(&this->m_handle.m_semaphore, 0, initial_count);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));
}

PosixCountingSemaphore::~PosixCountingSemaphore() {
    (void)sem_destroy(&this->m_handle.m_semaphore);
}

PosixCountingSemaphore::Status PosixCountingSemaphore::wait() {
    int status;
    do {
        status = sem_wait(&this->m_handle.m_semaphore);
    } while (status != 0 && errno == EINTR);
    FW_ASSERT(status == 0 || errno != 0, status);
    return status == 0 ? Status::OP_OK : posix_status_to_semaphore_status(errno);
}

PosixCountingSemaphore::Status PosixCountingSemaphore::waitTimeout(const Fw::TimeInterval& interval) {
#ifdef __APPLE__
    // sem_timedwait does not exist on macOS. This Posix implementation is not
    // used on macOS (the Darwin implementation takes priority at link time).
    return Status::ERROR_NOT_IMPLEMENTED;
#else
    struct timespec abstime;
    int clock_status = clock_gettime(CLOCK_REALTIME, &abstime);
    FW_ASSERT(clock_status == 0, static_cast<FwAssertArgType>(clock_status));

    abstime.tv_sec += interval.getSeconds();
    abstime.tv_nsec += static_cast<long>(interval.getUSeconds()) * 1000L;

    if (abstime.tv_nsec >= 1000000000L) {
        abstime.tv_sec += abstime.tv_nsec / 1000000000L;
        abstime.tv_nsec = abstime.tv_nsec % 1000000000L;
    }

    int status;
    do {
        status = sem_timedwait(&this->m_handle.m_semaphore, &abstime);
    } while (status != 0 && errno == EINTR);
    FW_ASSERT(status == 0 || errno != 0, status);
    return status == 0 ? Status::OP_OK : posix_status_to_semaphore_status(errno);
#endif
}

PosixCountingSemaphore::Status PosixCountingSemaphore::tryWait() {
    int status = sem_trywait(&this->m_handle.m_semaphore);
    FW_ASSERT(status == 0 || errno != 0, status);
    return status == 0 ? Status::OP_OK : posix_status_to_semaphore_status(errno);
}

PosixCountingSemaphore::Status PosixCountingSemaphore::post() {
    int status = sem_post(&this->m_handle.m_semaphore);
    FW_ASSERT(status == 0 || errno != 0, status);
    return status == 0 ? Status::OP_OK : posix_status_to_semaphore_status(errno);
}

CountingSemaphoreHandle* PosixCountingSemaphore::getHandle() {
    return &m_handle;
}

}  // namespace Semaphore
}  // namespace Posix
}  // namespace Os

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

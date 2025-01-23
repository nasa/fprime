// ======================================================================
// \title Os/Posix/ConditionVariable.cpp
// \brief Posix implementations for Os::ConditionVariable
// ======================================================================
#include "Os/Posix/ConditionVariable.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Posix/Mutex.hpp"

namespace Os {
namespace Posix {
namespace Mutex {

PosixConditionVariable::PosixConditionVariable() {
    PlatformIntType status = pthread_cond_init(&this->m_handle.m_condition, nullptr);
    FW_ASSERT(status == 0, status);  // If this fails, something horrible happened.
}
PosixConditionVariable::~PosixConditionVariable() {
    (void)pthread_cond_destroy(&this->m_handle.m_condition);
}

Status PosixConditionVariable::pend(Os::Mutex& mutex) {
    PosixMutexHandle* mutex_handle = reinterpret_cast<PosixMutexHandle*>(mutex.getHandle());
    PlatformIntType status = pthread_cond_wait(&this->m_handle.m_condition, &mutex_handle->m_mutex_descriptor);
    if (status == EPERM) {
        return Status::ERROR_MUTEX_NOT_HELD;
    } else if (status != 0) {
        return Status::ERROR_OTHER;
    } else {
        return Status::OP_OK;
    }
}
void PosixConditionVariable::notify() {
    FW_ASSERT(pthread_cond_signal(&this->m_handle.m_condition) == 0);
}
void PosixConditionVariable::notifyAll() {
    FW_ASSERT(pthread_cond_broadcast(&this->m_handle.m_condition) == 0);
}

ConditionVariableHandle* PosixConditionVariable::getHandle() {
    return &m_handle;
}

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os

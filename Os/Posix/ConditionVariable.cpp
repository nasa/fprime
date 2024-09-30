// ======================================================================
// \title Os/Posix/ConditionVariable.cpp
// \brief Posix implementations for Os::ConditionVariable
// ======================================================================
#include "Os/Posix/ConditionVariable.hpp"
#include "Os/Posix/Mutex.hpp"
#include "Fw/Types/Assert.hpp"

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

void PosixConditionVariable::wait(Os::Mutex& mutex) {
    PosixMutexHandle* mutex_handle = reinterpret_cast<PosixMutexHandle*>(mutex.getHandle());
    FW_ASSERT(pthread_cond_wait(&this->m_handle.m_condition, &mutex_handle->m_mutex_descriptor) == 0);
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

}
}
}

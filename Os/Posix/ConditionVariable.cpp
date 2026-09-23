// ======================================================================
// \title Os/Posix/ConditionVariable.cpp
// \brief Posix implementations for Os::ConditionVariable
// ======================================================================
#include "Os/Posix/ConditionVariable.hpp"
#include <type_traits>
#include "Fw/Types/Assert.hpp"
#include "Os/DelegateMutex.hpp"
#include "Os/Posix/Mutex.hpp"
#include "Os/Posix/error.hpp"

namespace Os {
namespace Posix {
namespace Mutex {

PosixConditionVariable::PosixConditionVariable() {
    int status = pthread_cond_init(&this->m_handle.m_condition, nullptr);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));  // If this fails, something horrible happened.
}
PosixConditionVariable::~PosixConditionVariable() {
    (void)pthread_cond_destroy(&this->m_handle.m_condition);
}

PosixConditionVariable::Status PosixConditionVariable::pend(Os::Mutex& mutex) {
    // The handle is only a PosixMutexHandle when Os::Mutex is PosixMutex or the link-time delegate backed by it
    static_assert(std::is_same<Os::Mutex, Os::DelegateMutex>::value || std::is_same<Os::Mutex, PosixMutex>::value,
                  "Os::Mutex alias is not compatible with the Posix ConditionVariable implementation");
    PosixMutexHandle* mutex_handle = reinterpret_cast<PosixMutexHandle*>(mutex.getHandle());
    FW_ASSERT(mutex_handle != nullptr);
    int status = pthread_cond_wait(&this->m_handle.m_condition, &mutex_handle->m_mutex_descriptor);
    return posix_status_to_conditional_status(status);
}
void PosixConditionVariable::notify() {
    const int status = pthread_cond_signal(&this->m_handle.m_condition);
    FW_ASSERT(status == 0);
}
void PosixConditionVariable::notifyAll() {
    const int status = pthread_cond_broadcast(&this->m_handle.m_condition);
    FW_ASSERT(status == 0);
}

ConditionVariableHandle* PosixConditionVariable::getHandle() {
    return &m_handle;
}

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os

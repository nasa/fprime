// ======================================================================
// \title Os/Stub/ConditionVariable.cpp
// \brief Stub implementations for Os::ConditionVariable
// ======================================================================
#include "Os/Stub/ConditionVariable.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Stub {
namespace Mutex {

void StubConditionVariable::wait(Os::Mutex& mutex) {
    // This stub implementation can only be used in deployments that never need to wait on any ConditionVariable.
    // Trigger an assertion if anyone ever tries to wait.
    FW_ASSERT(0);
}
void StubConditionVariable::notify() {
    // Nobody is waiting, because we assert if anyone tries to wait.
    // Therefore, we can notify all waiters by doing nothing.
}
void StubConditionVariable::notifyAll() {
    // Nobody is waiting, because we assert if anyone tries to wait.
    // Therefore, we can notify all waiters by doing nothing.
}

ConditionVariableHandle* StubConditionVariable::getHandle() {
    return &m_handle;
}

}
}
}

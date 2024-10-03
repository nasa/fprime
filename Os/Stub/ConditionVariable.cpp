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
    FW_ASSERT(0);
}
void StubConditionVariable::notify() {
    FW_ASSERT(0);
}
void StubConditionVariable::notifyAll() {
    FW_ASSERT(0);
}

ConditionVariableHandle* StubConditionVariable::getHandle() {
    return &m_handle;
}

}
}
}

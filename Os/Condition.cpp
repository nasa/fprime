#include "Os/Condition.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
ConditionVariable::ConditionVariable() : m_delegate(*ConditionVariableInterface::getDelegate(m_handle_storage)) {}

ConditionVariable::~ConditionVariable() {
    m_delegate.~ConditionVariableInterface();
}

ConditionVariable::Status ConditionVariable::pend(Os::Mutex& mutex) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    if (this->m_lock != nullptr && this->m_lock != &mutex) {
        return Status::ERROR_DIFFERENT_MUTEX;
    };
    this->m_lock = &mutex;
    return this->m_delegate.pend(mutex);
}
void ConditionVariable::wait(Os::Mutex& mutex) {
    Status status = this->pend(mutex);
    FW_ASSERT(status == Status::OP_OK, static_cast<FwAssertArgType>(status));
}
void ConditionVariable::notify() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notify();
}
void ConditionVariable::notifyAll() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notifyAll();
}

ConditionVariableHandle* ConditionVariable::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const ConditionVariableInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

}  // namespace Os

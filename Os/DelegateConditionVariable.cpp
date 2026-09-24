#include "Os/DelegateConditionVariable.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Condition.hpp"

namespace Os {

// DelegateConditionVariable implementation
DelegateConditionVariable::DelegateConditionVariable()
    : m_delegate(*ConditionVariableInterface::getDelegate(m_handle_storage)) {}

DelegateConditionVariable::~DelegateConditionVariable() {
    m_delegate.~ConditionVariableInterface();
}

ConditionVariableInterface::Status DelegateConditionVariable::pend(Os::Mutex& mutex) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    if (this->m_lock != nullptr && this->m_lock != &mutex) {
        return Status::ERROR_DIFFERENT_MUTEX;
    };
    this->m_lock = &mutex;
    return this->m_delegate.pend(mutex);
}

void DelegateConditionVariable::wait(Os::Mutex& mutex) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    // Dispatch through this object's checked pend() rather than the delegate's wait(), so the
    // mutex-consistency check in DelegateConditionVariable::pend() is not bypassed.
    ConditionVariableInterface::wait(mutex);
}

void DelegateConditionVariable::notify() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notify();
}

void DelegateConditionVariable::notifyAll() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notifyAll();
}

ConditionVariableHandle* DelegateConditionVariable::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

}  // namespace Os

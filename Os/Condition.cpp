#include "Os/Condition.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
ConditionVariable::ConditionVariable() : m_delegate(*ConditionVariableInterface::getDelegate(m_handle_storage)) {}

ConditionVariable::~ConditionVariable() {
    m_delegate.~ConditionVariableInterface();
}

void ConditionVariable::wait(Os::Mutex& mutex) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(this->m_lock == nullptr || this->m_lock == &mutex); // Confirm the same mutex used across waits
    this->m_lock = &mutex;
    // Attempt to lock the mutex and ensure that this was successful or the lock was already held
    Mutex::Status lock_status = this->m_lock->take();
    FW_ASSERT(lock_status == Mutex::Status::OP_OK || lock_status == Mutex::Status::ERROR_DEADLOCK);
    this->m_delegate.wait(mutex);
}
void ConditionVariable::notify() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notify();
}
void ConditionVariable::notifyAll() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.notifyAll();
}

ConditionVariableHandle* ConditionVariable::getHandle(){
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const ConditionVariableInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

}

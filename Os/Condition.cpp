#include "Os/Condition.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
ConditionVariable::ConditionVariable() : m_delegate(*ConditionVariableInterface::getDelegate(m_handle_storage)) {}

ConditionVariable::~ConditionVariable() {
    m_delegate.~ConditionVariableInterface();
}

void ConditionVariable::wait(Os::Mutex& mutex) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConditionVariableInterface*>(&this->m_handle_storage[0]));
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


}
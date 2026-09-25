// ======================================================================
// \title Os/DelegateMemory.cpp
// \brief common function implementations for Os::DelegateMemory
// ======================================================================
#include "Os/DelegateMemory.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {

DelegateMemory::DelegateMemory() : m_handle_storage(), m_delegate(*MemoryInterface::getDelegate(m_handle_storage)) {}

DelegateMemory::~DelegateMemory() {
    m_delegate.~MemoryInterface();
}

void DelegateMemory::init() {
    (void)DelegateMemory::getSingleton();
}

DelegateMemory& DelegateMemory::getSingleton() {
    static DelegateMemory _singleton;
    return _singleton;
}

DelegateMemory::Status DelegateMemory::_getUsage(Usage& memory_usage) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MemoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getUsage(memory_usage);
}

MemoryHandle* DelegateMemory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MemoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}
}  // namespace Os

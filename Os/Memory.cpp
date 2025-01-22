// ======================================================================
// \title Os/Memory.hpp
// \brief common function implementations for Os::Memory
// ======================================================================
#include "Os/Memory.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {

Memory::Memory() : m_delegate(*MemoryInterface::getDelegate(m_handle_storage)) {}

Memory::~Memory() {
    m_delegate.~MemoryInterface();
}

void Memory::init() {
    (void) Memory::getSingleton();
}

Memory& Memory::getSingleton() {
    static Memory _singleton;
    return _singleton;
}

Memory::Status Memory::_getUsage(Os::Memory::Usage& memory_usage) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MemoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getUsage(memory_usage);
}

Memory::Status Memory::getUsage(Os::Memory::Usage& memory_usage) {
    return Memory::getSingleton()._getUsage(memory_usage);
}

MemoryHandle* Memory::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MemoryInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}
}

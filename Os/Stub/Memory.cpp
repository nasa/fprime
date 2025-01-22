// ======================================================================
// \title Os/Stub/Memory.cpp
// \brief stub implementation for Os::Memory
// ======================================================================
#include <Os/Stub/Memory.hpp>

namespace Os {
namespace Stub {
namespace Memory {

MemoryInterface::Status StubMemory::_getUsage(Os::Memory::Usage& memory_usage) {
    memory_usage.used = 0;
    memory_usage.total = 0;
    return Status::ERROR;
}


MemoryHandle* StubMemory::getHandle() {
    return &this->m_handle;
}

} // namespace Memory
} // namespace Stub
} // namespace Os

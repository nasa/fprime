// ======================================================================
// \title Os/Linux/DefaultMemory.cpp
// \brief sets default Os::Memory to Linux implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Linux/Memory.hpp"
#include "Os/Memory.hpp"

namespace Os {
MemoryInterface* MemoryInterface::getDelegate(MemoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MemoryInterface, Os::Linux::Memory::LinuxMemory>(aligned_new_memory);
}
}  // namespace Os

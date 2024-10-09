// ======================================================================
// \title Os/Darwin/DefaultMemory.cpp
// \brief sets default Os::Memory to Darwin implementation via linker
// ======================================================================
#include "Os/Memory.hpp"
#include "Os/Darwin/Memory.hpp"
#include "Os/Delegate.hpp"

namespace Os {
MemoryInterface* MemoryInterface::getDelegate(MemoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MemoryInterface, Os::Darwin::Memory::DarwinMemory>(aligned_new_memory);
}
}

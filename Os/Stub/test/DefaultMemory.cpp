// ======================================================================
// \title Os/Stub/DefaultMemory.cpp
// \brief sets default Os::Memory to stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Memory.hpp"
#include "Os/Stub/test/Memory.hpp"

namespace Os {
MemoryInterface* MemoryInterface::getDelegate(MemoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MemoryInterface, Os::Stub::Memory::Test::TestMemory>(aligned_new_memory);
}
}  // namespace Os

// ======================================================================
// \title Os/MemoryInterface.cpp
// \brief common convenience-wrapper implementation for Os::Memory
// ======================================================================
#include <Os/Memory.hpp>

namespace Os {

MemoryInterface::Status MemoryInterface::getUsage(Usage& memory) {
    return Os::Memory::getSingleton()._getUsage(memory);
}

}  // namespace Os

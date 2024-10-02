// ======================================================================
// \title Os/Linux/Memory.cpp
// \brief Linux implementation for Os::Memory
// ======================================================================
#include <Os/Linux/Memory.hpp>
#include <sys/sysinfo.h>

namespace Os {
namespace Linux {
namespace Memory {

MemoryInterface::Status LinuxMemory::_getUsage(Os::Memory::Usage& memory_usage) {
    sysinfo info;
    // Only error in sysinfo call is invalid address
    FW_ASSERT(sysinfo(&info), 0);
    const FwSizeType MAX_MEASURABLE_RAM_UNITS =  std::numeric_limits<FwSizeType>::max() / info.memunit;
    if ((MAX_MEASURABLE_RAM_UNITS < info.totalram) || (MAX_MEASURABLE_RAM_UNITS < info.freeram)) {
        memory_usage.total = 1;
        memory_usage.used = 1;
        return Status::ERROR;
    }

    memory_usage.total = info.totalram * info.memunit;
    memory_usage.used = info.freeram * info.memunit;
    return Status::OP_OK;
}

MemoryHandle* LinuxMemory::getHandle() {
    return &this->m_handle;
}

} // namespace Memory
} // namespace Linux
} // namespace Os

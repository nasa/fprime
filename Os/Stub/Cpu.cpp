// ======================================================================
// \title Os/Stub/Cpu.cpp
// \brief stub implementation for Os::Cpu
// ======================================================================
#include <Os/Stub/Cpu.hpp>

namespace Os {
namespace Stub {
namespace Cpu {

CpuInterface::Status StubCpu::_getCount(FwSizeType& cpu_count) {
    cpu_count = 0;
    return Status::ERROR;
}

CpuInterface::Status StubCpu::_getTicks(Os::Cpu::Ticks& ticks, FwSizeType cpu_index) {
    ticks.total = 1;
    ticks.used = 1;
    return Status::ERROR;
}

CpuHandle* StubCpu::getHandle() {
    return &this->m_handle;
}

} // namespace Cpu
} // namespace Stub
} // namespace Os

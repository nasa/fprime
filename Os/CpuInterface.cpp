// ======================================================================
// \title Os/CpuInterface.cpp
// \brief common convenience-wrapper implementation for Os::Cpu
// ======================================================================
#include <Os/Cpu.hpp>

namespace Os {

CpuInterface::Status CpuInterface::getCount(FwSizeType& cpu_count) {
    return Os::Cpu::getSingleton()._getCount(cpu_count);
}

CpuInterface::Status CpuInterface::getTicks(Ticks& ticks, FwSizeType cpu_index) {
    return Os::Cpu::getSingleton()._getTicks(ticks, cpu_index);
}

}  // namespace Os

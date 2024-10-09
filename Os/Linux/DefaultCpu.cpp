// ======================================================================
// \title Os/Linux/DefaultCpu.cpp
// \brief sets default Os::Cpu to Linux implementation via linker
// ======================================================================
#include "Os/Cpu.hpp"
#include "Os/Linux/Cpu.hpp"
#include "Os/Delegate.hpp"

namespace Os {
CpuInterface* CpuInterface::getDelegate(CpuHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<CpuInterface, Os::Linux::Cpu::LinuxCpu>(aligned_new_memory);
}
}

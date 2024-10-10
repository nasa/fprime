// ======================================================================
// \title Os/Darwin/DefaultCpu.cpp
// \brief sets default Os::Cpu to Darwin implementation via linker
// ======================================================================
#include "Os/Cpu.hpp"
#include "Os/Darwin/Cpu.hpp"
#include "Os/Delegate.hpp"

namespace Os {
CpuInterface* CpuInterface::getDelegate(CpuHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<CpuInterface, Os::Darwin::Cpu::DarwinCpu>(aligned_new_memory);
}
}

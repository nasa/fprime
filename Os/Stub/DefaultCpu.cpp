// ======================================================================
// \title Os/Stub/DefaultCpu.cpp
// \brief sets default Os::Cpu to stub implementation via linker
// ======================================================================
#include "Os/Cpu.hpp"
#include "Os/Stub/Cpu.hpp"
#include "Os/Delegate.hpp"

namespace Os {
CpuInterface* CpuInterface::getDelegate(CpuHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<CpuInterface, Os::Stub::Cpu::StubCpu>(aligned_new_memory);
}
}

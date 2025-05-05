// ======================================================================
// \title Os/Stub/DefaultCpu.cpp
// \brief sets default Os::Cpu to stub implementation via linker
// ======================================================================
#include "Os/Cpu.hpp"
#include "Os/Stub/test/Cpu.hpp"
#include "Os/Delegate.hpp"

namespace Os {
CpuInterface* CpuInterface::getDelegate(CpuHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<CpuInterface, Os::Stub::Cpu::Test::TestCpu>(aligned_new_memory);
}
}

// ======================================================================
// \title Os/Os.cpp
// \brief common definitions for the OSAL layer
// ======================================================================
#include "Os/Os.hpp"
#include "FpConfig.h"
#include "Os/Console.hpp"
#include "Os/Cpu.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Memory.hpp"

namespace Os {

void init() {
    // Initialize all OSAL singletons
    Os::Console::init();
    Os::FileSystem::init();
    Os::Cpu::init();
    Os::Memory::init();
}

}  // namespace Os

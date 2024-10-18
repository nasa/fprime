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
#include "Os/Task.hpp"

namespace Os {

void init() {
    // Initialize all OSAL singletons
    Os::Console::init();
    Os::FileSystem::init();
    Os::Cpu::init();
    Os::Memory::init();
    Os::Task::init();
}

}  // namespace Os

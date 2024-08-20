// ======================================================================
// \title Os/Os.cpp
// \brief common definitions for the OSAL layer
// ======================================================================
#include "Os/Os.hpp"
#include "FpConfig.h"
#include "Os/Console.hpp"
#include "Os/FileSystem.hpp"

namespace Os {

void init() {
    // Console and FileSystem are singletons and must be initialized
    (void)Os::Console::init();
    (void)Os::FileSystem::init();
}

}  // namespace Os

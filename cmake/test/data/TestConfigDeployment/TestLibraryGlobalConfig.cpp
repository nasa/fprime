// ======================================================================
// \title  TestLibraryGlobalConfig.cpp
// \brief  cpp file for GLOBAL_IMPLICIT_DEPENDENCY library config testing
//
// \description this file will fail to compile if a library configuration
//     marked GLOBAL_IMPLICIT_DEPENDENCY is not reachable without an explicit
//     DEPENDS on the configuration module
// ======================================================================
#include "global_config/GlobalCfg.hpp"
static_assert(MY_LIBRARY_GLOBAL_CONFIG, "MY_LIBRARY_GLOBAL_CONFIG is not set, library global config failed");

int getLibraryGlobalValue() {
    return MY_LIBRARY_GLOBAL_VALUE;
}

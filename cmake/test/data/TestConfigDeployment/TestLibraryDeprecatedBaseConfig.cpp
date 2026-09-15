// ======================================================================
// \title  TestLibraryDeprecatedBaseConfig.cpp
// \brief  cpp file for deprecated BASE_CONFIG compatibility testing
//
// \description this file will fail to compile if a configuration marked
//     with the deprecated BASE_CONFIG flag is no longer reachable without
//     an explicit DEPENDS on the configuration module
// ======================================================================
#include "project/DeprecatedBaseCfg.hpp"
static_assert(MY_DEPRECATED_BASE_CONFIG, "MY_DEPRECATED_BASE_CONFIG is not set, deprecated BASE_CONFIG failed");

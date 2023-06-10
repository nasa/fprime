// ======================================================================
// \title  Baremetal/SystemResources.cpp
// \author mstarch
// \brief  cpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Os/SystemResources.hpp>

namespace Os {

SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32& cpuCount) {
    // Assumes 1 CPU
    cpuCount = 1;
    return SYSTEM_RESOURCES_OK;
}

SystemResources::SystemResourcesStatus SystemResources::getCpuTicks(CpuTicks& cpu_ticks, U32 cpu_index) {
    // Always 100 percent
    cpu_ticks.used = 1;
    cpu_ticks.total = 1;
    return SYSTEM_RESOURCES_OK;
}


SystemResources::SystemResourcesStatus SystemResources::getMemUtil(MemUtil& memory_util) {
    // Always 100 percent
    memory_util.total = 1;
    memory_util.used = 1;
    return SYSTEM_RESOURCES_OK;
}
}  // namespace Os

// ======================================================================
// \title  Baremetal/SystemResources.cpp
// \author mstarch
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

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
    memory_util.total = 0;
    memory_util.util = 0;
    return SYSTEM_RESOURCES_OK;
}
}  // namespace Os

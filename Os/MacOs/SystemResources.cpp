// ======================================================================
// \title  MacOs/SystemResources.cpp
// \author mstarch
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <Fw/Types/Assert.hpp>
#include <Os/SystemResources.hpp>

namespace Os {
/**
 * \brief reads macOS virtual memory statistics for memory calculation
 *
 * Queries the macOS kernel for virtual memory information. These items are returned in units of page-size and are
 * then converted back into bytes.
 *
 * Thanks to: https://stackoverflow.com/questions/8782228/retrieve-ram-info-on-a-mac
 *
 * \param used: used memory in bytes
 * \param total: total memory in bytes
 * \return: kern_return_t with success/failure straight from the kernel
 */
kern_return_t vm_stat_helper(FwSizeType& used, FwSizeType& total) {
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmstat;
    vm_size_t vmsize;

    kern_return_t stat1 = host_statistics(mach_host_self(), HOST_VM_INFO, reinterpret_cast<host_info_t>(&vmstat), &count);
    kern_return_t stat2 = host_page_size(mach_host_self(), &vmsize);

    if (KERN_SUCCESS == stat1 and KERN_SUCCESS == stat2) {
        // Wired (permanently in RAM), active (recently used), and inactive (not recently used) pages
        used = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count;
        total = used + vmstat.free_count;

        // Pages to totals
        used *= vmsize;
        total *= vmsize;
    }
    return (stat1 == KERN_SUCCESS) ? stat2 : stat1;
}

/**
 * \brief helper around raw CPU capture API
 *
 * Calls for the CPU information from the machine, improving readability in cpu_by_index
 *
 * \param cpu_load_info: filled with CPU data
 * \param cpu_count: filled with CPU count
 *
 * \return success/failure using kern_return_t
 */
kern_return_t cpu_data_helper(processor_cpu_load_info_t& cpu_load_info, U32& cpu_count) {
    mach_msg_type_number_t processor_msg_count;
    kern_return_t stat = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_count,
                                             reinterpret_cast<processor_info_array_t*>(&cpu_load_info), &processor_msg_count);
    return stat;
}
/**
 * \brief Query for a single CPU's ticks information
 *
 * Queries all CPU information but only deals with a single CPU's output. This is done because the load average is
 * tracked sample to sample and the call pattern is cpu0, cpu1, ..., cpu last, wait for sample window, cpu0, ... and
 * thus each call should update one CPU's sample or only the last cpu will have the benefit of the sampling window.
 *
 * \param cpu_index: index of current CPU being queried
 * \param used: filled with CPU's used ticks count
 * \param total: filled with CPU's total ticks
 *
 * \return success/failure using kern_return_t
 */
kern_return_t cpu_by_index(U32 cpu_index, FwSizeType& used, FwSizeType& total) {
    processor_cpu_load_info_t cpu_load_info;
    U32 cpu_count = 0;
    kern_return_t stat = cpu_data_helper(cpu_load_info, cpu_count);

    // Failure for CPU index
    if (cpu_count <= cpu_index) {
        stat = KERN_FAILURE;
    } else if (KERN_SUCCESS == stat) {
        FW_ASSERT(cpu_count > cpu_index, static_cast<FwAssertArgType>(cpu_count), static_cast<FwAssertArgType>(cpu_index));  // Will fail if the CPU count changes while running
        processor_cpu_load_info per_cpu_info = cpu_load_info[(cpu_count > cpu_index) ? cpu_index : 0];

        // Total the ticks across the different states: idle, system, user, etc...
        total = 0;
        for (U32 i = 0; i < CPU_STATE_MAX; i++) {
            total += per_cpu_info.cpu_ticks[i];
        }
        used = total - per_cpu_info.cpu_ticks[CPU_STATE_IDLE];
    }
    return stat;
}

SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32& cpuCount) {
    processor_cpu_load_info_t cpu_load_info;
    if (KERN_SUCCESS == cpu_data_helper(cpu_load_info, cpuCount)) {
        return SYSTEM_RESOURCES_OK;
    }
    cpuCount = 0;
    return SYSTEM_RESOURCES_ERROR;
}

SystemResources::SystemResourcesStatus SystemResources::getCpuTicks(CpuTicks& cpu_ticks, U32 cpu_index) {
    // Fallbacks in case of error
    cpu_ticks.used = 1;
    cpu_ticks.total = 1;
    kern_return_t stat = cpu_by_index(cpu_index, cpu_ticks.used, cpu_ticks.total);
    return (stat == KERN_SUCCESS) ? SYSTEM_RESOURCES_OK : SYSTEM_RESOURCES_ERROR;
}


SystemResources::SystemResourcesStatus SystemResources::getMemUtil(MemUtil& memory_util) {
    // Call out VM helper
    if (KERN_SUCCESS == vm_stat_helper(memory_util.used, memory_util.total)) {
        return SYSTEM_RESOURCES_OK;
    }
    // Force something sensible, while preventing divide by zero
    memory_util.total = 1;
    memory_util.used = 1;
    return SYSTEM_RESOURCES_ERROR;
}
}  // namespace Os

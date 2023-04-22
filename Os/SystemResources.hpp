// ======================================================================
// \title  SystemResources.hpp
// \author mstarch
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef _SystemResources_hpp_
#define _SystemResources_hpp_

#include <FpConfig.hpp>

namespace Os {
namespace SystemResources {

enum SystemResourcesStatus {
    SYSTEM_RESOURCES_OK,     //!< Call was successful
    SYSTEM_RESOURCES_ERROR,  //!< Call failed
};

struct CpuTicks {
    FwSizeType used;   //!< Filled with non-idle (system, user) CPU ticks
    FwSizeType total;  //!< Filled with total CPU ticks
};

struct MemUtil {
    FwSizeType used;   //!< Filled with used bytes of volatile memory (permanent, paged-in)
    FwSizeType total;  //!< Filled with total non-volatile memory
};

/**
 * \brief Request the count of the CPUs detected by the system
 *
 * \param cpu_count: (output) filled with CPU count on system
 * \return: SYSTEM_RESOURCES_OK with valid CPU count, SYSTEM_RESOURCES_ERROR when error occurs
 */
SystemResourcesStatus getCpuCount(U32& cpu_count);

/**
 * \brief Get the CPU tick information for a given CPU
 *
 * CPU ticks represent a small time slice of processor time. This will retrieve the used CPU ticks and total
 * ticks for a given CPU. This information in a running accumulation and thus a sample-to-sample
 * differencing is needed to see the 'realtime' changing load. This shall be done by the caller.
 *
 * \param ticks: (output) filled with the tick information for the given CPU
 * \param cpu_index: index for CPU to read
 * \return:  SYSTEM_RESOURCES_ERROR when error occurs, SYSTEM_RESOURCES_OK otherwise.
 */
SystemResourcesStatus getCpuTicks(CpuTicks& ticks, U32 cpu_index = 0);

/**
 * \brief Get system memory usage
 *
 * \param memory_util: (output) data structure used to store memory usage
 * \return:  SYSTEM_RESOURCES_ERROR when error occurs, SYSTEM_RESOURCES_OK otherwise.
 */
SystemResourcesStatus getMemUtil(MemUtil& memory_util);
}  // namespace SystemResources
}  // namespace Os

#endif

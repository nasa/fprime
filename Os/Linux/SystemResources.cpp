// ======================================================================
// \title  Linux/SystemResources.cpp
// \author sfregoso
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <cstdio>
#include <sys/sysinfo.h>
#include <cstring>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {

    SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32 &cpuCount) {
        cpuCount = get_nprocs();
        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getCpuTicks(CpuTicks &cpu_ticks, U32 cpu_index) {
        char line[512] = {0};
        FILE *fp = nullptr;
        U32 cpu_data[4] = {0};
        U32 cpuCount = 0;
        SystemResources::SystemResourcesStatus status  = SYSTEM_RESOURCES_ERROR;
        U64 cpuUsed = 0;
        U64 cpuTotal = 0;

        if ((status = getCpuCount(cpuCount)) != SYSTEM_RESOURCES_OK) {
            return status;
        }

        if (cpu_index >= cpuCount) {
            return SYSTEM_RESOURCES_ERROR;
        }

        if ((fp = fopen("/proc/stat", "r")) == nullptr) {

            return SYSTEM_RESOURCES_ERROR;

        }

        if (fgets(line, sizeof(line), fp) == nullptr) { //1st line.  Aggregate cpu line.
            fclose(fp);
            return SYSTEM_RESOURCES_ERROR;
        }

        for (U32 i = 0; i < cpu_index + 1; i++) {
            if (fgets(line, sizeof(line), fp) == nullptr) { //cpu# line
                fclose(fp);
                return SYSTEM_RESOURCES_ERROR;
            }
            if (i != cpu_index) { continue; }

            if (!(line[0] == 'c' && line[1] == 'p' && line[2] == 'u')) {
                fclose(fp);
                return SYSTEM_RESOURCES_ERROR;
            }
            // No string concerns, as string is discarded
            sscanf(line, "%*s %d %d %d %d", &cpu_data[0],
                   &cpu_data[1],
                   &cpu_data[2],
                   &cpu_data[3]); //cpu#: 4 numbers: usr, nice, sys, idle

            cpuUsed = cpu_data[0] + cpu_data[1] + cpu_data[2];
            cpuTotal = cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];

            break;
        }
        fclose(fp);

        cpu_ticks.used = cpuUsed;
        cpu_ticks.total = cpuTotal;
        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getMemUtil(MemUtil &memory_util) {

        struct sysinfo memory_info;
        sysinfo(&memory_info);
        

        const FwSizeType total_ram = static_cast<FwSizeType>(memory_info.totalram);
        const FwSizeType free_ram = static_cast<FwSizeType>(memory_info.freeram);
        const FwSizeType memory_unit = static_cast<FwSizeType>(memory_info.mem_unit);

        // Check for casting and type errors
        if (((total_ram <= 0) || (static_cast<unsigned long>(total_ram) != memory_info.totalram)) ||
            ((free_ram <= 0) || (static_cast<unsigned long>(free_ram) != memory_info.freeram)) ||
            ((memory_unit <= 0) || (static_cast<unsigned int>(memory_unit) != memory_info.mem_unit))
        ) {
            return SYSTEM_RESOURCES_ERROR;
        }

        // Check for invalid memory calculations
        if (total_ram < free_ram) {
            return SYSTEM_RESOURCES_ERROR;
        }

        // Check for overflow in multiplication
        if (total_ram > (FpLimits::FwSizeType_MAX / memory_unit))
        {
            return SYSTEM_RESOURCES_ERROR;
        }

        memory_util.total = total_ram * memory_unit;
        memory_util.used = (total_ram - free_ram) * memory_unit;

        return SYSTEM_RESOURCES_OK;
    }
}

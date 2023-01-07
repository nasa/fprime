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
#include <array>
#include <cstdio>
#include <cstring>
#include <sys/sysinfo.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>

#define PROC_STAT_PATH "/proc/stat"
#define READ_ONLY "r"
#define LINE_SIZE 256
std::array<char, LINE_SIZE> proc_stat_line;

namespace Os {

    SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32& cpuCount) {
        cpuCount = get_nprocs();
        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getCpuTicks(CpuTicks &cpu_ticks, U32 cpu_index) {
        SystemResources::SystemResourcesStatus status  = SYSTEM_RESOURCES_ERROR;
        std::array<U32, 4> cpu_data = {0};
        FILE* fp = nullptr;
        U32 cpuCount = 0;
        U64 cpuUsed = 0;
        U64 cpuTotal = 0;

        if ((status = getCpuCount(cpuCount)) != SYSTEM_RESOURCES_OK) {
            return status;
        }
        if (cpu_index >= cpuCount) {
            return SYSTEM_RESOURCES_ERROR;
        }
        if ((fp = fopen(PROC_STAT_PATH, READ_ONLY)) == nullptr) {
            return SYSTEM_RESOURCES_ERROR;
        }
        if (fgets(proc_stat_line.data(), proc_stat_line.size(), fp) == nullptr) {  // 1st line.  Aggregate cpu line.
            fclose(fp);
            return SYSTEM_RESOURCES_ERROR;
        }

        for (U32 i = 0; i < cpu_index + 1; i++) {
            if (fgets(proc_stat_line.data(), proc_stat_line.size(), fp) == nullptr) {  // cpu# line
                fclose(fp);
                return SYSTEM_RESOURCES_ERROR;
            }
            if (i != cpu_index) {
                continue;
            }

            if (strncmp(proc_stat_line.data(), "cpu", 3) != 0) {
                fclose(fp);
                return SYSTEM_RESOURCES_ERROR;
            }
            // No string concerns, as string is discarded
            sscanf(proc_stat_line.data(), "%*s %d %d %d %d", &cpu_data[0], &cpu_data[1], &cpu_data[2],
                   &cpu_data[3]);  // cpu#: 4 numbers: usr, nice, sys, idle

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

        if (sysinfo(&memory_info) != 0) {
            return SYSTEM_RESOURCES_ERROR;
        }

        const FwSizeType total_ram = static_cast<FwSizeType>(memory_info.totalram);
        const FwSizeType free_ram = static_cast<FwSizeType>(memory_info.freeram);
        const FwSizeType memory_unit = static_cast<FwSizeType>(memory_info.mem_unit);

        // Check for casting and type errors
        if (((total_ram <= 0) || (static_cast<unsigned long>(total_ram) != memory_info.totalram)) ||
            ((free_ram <= 0) || (static_cast<unsigned long>(free_ram) != memory_info.freeram)) ||
            ((memory_unit <= 0) || (static_cast<unsigned int>(memory_unit) != memory_info.mem_unit))) {
            return SYSTEM_RESOURCES_ERROR;
        }

        // Check for invalid memory calculations
        if (total_ram < free_ram) {
            return SYSTEM_RESOURCES_ERROR;
        }

        // Check for overflow in multiplication
        if (total_ram > (FpLimits::FwSizeType_MAX / memory_unit)) {
            return SYSTEM_RESOURCES_ERROR;
        }

        memory_util.total = total_ram * memory_unit;
        memory_util.used = (total_ram - free_ram) * memory_unit;

        return SYSTEM_RESOURCES_OK;
    }
}

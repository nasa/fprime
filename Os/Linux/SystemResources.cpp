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

    U64 getCpuUsed(std::array<U32, 4>& cpu_data) {
        return cpu_data[0] + cpu_data[1] + cpu_data[2];
    }
    U64 getCpuTotal(std::array<U32, 4>& cpu_data) {
        return cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];
    }

    SystemResources::SystemResourcesStatus openProcStatFile(FILE*& fp) {
        if ((fp = fopen(PROC_STAT_PATH, READ_ONLY)) == nullptr) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus readProcStatLine(FILE* fp, std::array<char, LINE_SIZE>& proc_stat_line) {
        if (fgets(proc_stat_line.data(), proc_stat_line.size(), fp) == nullptr) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus getCpuDataLine(FILE* fp,
                                                          U32 cpu_index,
                                                          std::array<char, LINE_SIZE>& proc_stat_line) {
        for (U32 i = 0; i < cpu_index + 1; i++) {
            if (readProcStatLine(fp, proc_stat_line) != SystemResources::SYSTEM_RESOURCES_OK) {
                return SystemResources::SYSTEM_RESOURCES_ERROR;
            }
            if (i != cpu_index) {
                continue;
            }
            if (strncmp(proc_stat_line.data(), "cpu", 3) != 0) {
                return SystemResources::SYSTEM_RESOURCES_ERROR;
            }
            break;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus parseCpuData(std::array<char, LINE_SIZE>& proc_stat_line,
                                                        std::array<U32, 4>& cpu_data) {
        if (sscanf(proc_stat_line.data(), "%*s %d %d %d %d", &cpu_data[0], &cpu_data[1], &cpu_data[2], &cpu_data[3]) !=
            4) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus getCpuData(U32 cpu_index, std::array<U32, 4>& cpu_data) {
        FILE* fp = nullptr;
        if (openProcStatFile(fp) != SystemResources::SYSTEM_RESOURCES_OK ||
            readProcStatLine(fp, proc_stat_line) != SystemResources::SYSTEM_RESOURCES_OK ||
            getCpuDataLine(fp, cpu_index, proc_stat_line) != SystemResources::SYSTEM_RESOURCES_OK ||
            parseCpuData(proc_stat_line, cpu_data) != SystemResources::SYSTEM_RESOURCES_OK) {
            fclose(fp);
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        fclose(fp);
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getCpuTicks(CpuTicks& cpu_ticks, U32 cpu_index) {
        SystemResources::SystemResourcesStatus status = SYSTEM_RESOURCES_ERROR;
        std::array<U32, 4> cpu_data = {0};
        U32 cpuCount = 0;

        if ((status = getCpuCount(cpuCount)) != SYSTEM_RESOURCES_OK) {
            return status;
        }
        if (cpu_index >= cpuCount) {
            return SYSTEM_RESOURCES_ERROR;
        }

        if ((status = getCpuData(cpu_index, cpu_data)) != SYSTEM_RESOURCES_OK) {
            return status;
        }

        cpu_ticks.used = getCpuUsed(cpu_data);
        cpu_ticks.total = getCpuTotal(cpu_data);
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

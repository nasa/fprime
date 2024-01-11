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
#include <cstring>
#include <sys/sysinfo.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>

constexpr char PROC_STAT_PATH[] = "/proc/stat";
constexpr char READ_ONLY[] = "r";
constexpr int LINE_SIZE = 256;
char proc_stat_line[LINE_SIZE];

namespace Os {

    SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32& cpuCount) {
        cpuCount = get_nprocs();
        return SYSTEM_RESOURCES_OK;
    }

    U64 getCpuUsed(U32 cpu_data[4]) {
        return cpu_data[0] + cpu_data[1] + cpu_data[2];
    }
    U64 getCpuTotal(U32 cpu_data[4]) {
        return cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];
    }

    SystemResources::SystemResourcesStatus openProcStatFile(FILE*& fp) {
        if ((fp = fopen(PROC_STAT_PATH, READ_ONLY)) == nullptr) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus readProcStatLine(FILE* fp, char proc_stat_line[LINE_SIZE]) {
        if (fgets(proc_stat_line, LINE_SIZE, fp) == nullptr) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus getCpuDataLine(FILE* fp,
                                U32 cpu_index,
                                char proc_stat_line[LINE_SIZE]) {
        for (U32 i = 0; i < cpu_index + 1; i++) {
            if (readProcStatLine(fp, proc_stat_line) != SystemResources::SYSTEM_RESOURCES_OK) {
                return SystemResources::SYSTEM_RESOURCES_ERROR;
            }
            if (i != cpu_index) {
                continue;
            }
            if (strncmp(proc_stat_line, "cpu", 3) != 0) {
                return SystemResources::SYSTEM_RESOURCES_ERROR;
            }
            break;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus parseCpuData(char proc_stat_line[LINE_SIZE],
                                                    U32 cpu_data[4]) {
        if (sscanf(proc_stat_line, "%*s %u %u %u %u", &cpu_data[0], &cpu_data[1], &cpu_data[2], &cpu_data[3]) !=
            4) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }
        return SystemResources::SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus getCpuData(U32 cpu_index, U32 cpu_data[4]) {
        FILE* fp = nullptr;
        if (openProcStatFile(fp) != SystemResources::SYSTEM_RESOURCES_OK) {
            return SystemResources::SYSTEM_RESOURCES_ERROR;
        }

        if (readProcStatLine(fp, proc_stat_line) != SystemResources::SYSTEM_RESOURCES_OK ||
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
        U32 cpu_data[4] = {0};
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


    U64 getMemoryTotal(FwSizeType total_ram, FwSizeType memory_unit) {
        return static_cast<U64>(total_ram)*static_cast<U64>(memory_unit);
    }
    U64 getMemoryUsed(FwSizeType total_ram, FwSizeType free_ram, FwSizeType memory_unit) {
        return static_cast<U64>((total_ram - free_ram)) * static_cast<U64>(memory_unit);
    }

    bool checkCastingAndTypeErrors(FwSizeType total_ram,
                                   FwSizeType free_ram,
                                   FwSizeType memory_unit,
                                   const struct sysinfo& memory_info) {
        return ((total_ram <= 0) || (memory_unit <= 0) ||
                (static_cast<unsigned long>(total_ram) != memory_info.totalram) ||
                (static_cast<unsigned long>(free_ram) != memory_info.freeram) ||
                (static_cast<unsigned int>(memory_unit) != memory_info.mem_unit));
    }

    bool checkInvalidMemoryCalculation(FwSizeType total_ram, FwSizeType free_ram) {
        return (total_ram < free_ram);
    }

    bool checkMultiplicationOverflow(FwSizeType total_ram, FwSizeType memory_unit) {
        return (total_ram > (std::numeric_limits<FwSizeType>::max() / memory_unit));
    }

    SystemResources::SystemResourcesStatus SystemResources::getMemUtil(MemUtil& memory_util) {
        struct sysinfo memory_info;

        if (sysinfo(&memory_info) != 0) {
            return SYSTEM_RESOURCES_ERROR;
        }

        const FwSizeType total_ram = static_cast<FwSizeType>(memory_info.totalram);
        const FwSizeType free_ram = static_cast<FwSizeType>(memory_info.freeram);
        const FwSizeType memory_unit = static_cast<FwSizeType>(memory_info.mem_unit);

        if (checkCastingAndTypeErrors(total_ram, free_ram, memory_unit, memory_info)) {
            return SYSTEM_RESOURCES_ERROR;
        }

        if (checkInvalidMemoryCalculation(total_ram, free_ram)) {
            return SYSTEM_RESOURCES_ERROR;
        }

        if (checkMultiplicationOverflow(total_ram, memory_unit)) {
            return SYSTEM_RESOURCES_ERROR;
        }

        memory_util.used = getMemoryUsed(total_ram, free_ram, memory_unit);
        memory_util.total = getMemoryTotal(total_ram, memory_unit);

        return SYSTEM_RESOURCES_OK;
    }
}

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
#include <cstdio>              /* fopen() */
#include <cstdlib>             /* scanf */
#include <sys/vfs.h>            /* statfs() */
#include <sys/sysinfo.h>		/* get_nprocs() */
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
        FILE *fp = nullptr;
        NATIVE_INT_TYPE total = 0;
        NATIVE_INT_TYPE free = 0;
        // Fallbacks
        memory_util.total = 1;
        memory_util.used = 1;

        fp = fopen("/proc/meminfo", "r");
        if (fp == nullptr) {
            return SYSTEM_RESOURCES_ERROR;
        }
        // No string concerns as strings discarded
        if (fscanf(fp, "%*s %d %*s", &total) != 1 ||  /* 1st line is MemTotal */
            fscanf(fp, "%*s %d", &free) != 1) {   /* 2nd line is MemFree */
            fclose(fp);
            return SYSTEM_RESOURCES_ERROR;
        }
        fclose(fp);

        // Check results
        if (total < 0 or free < 0 or total < free) {
            return SYSTEM_RESOURCES_ERROR;
        }
        memory_util.total = static_cast<U64>(total) * 1024; // KB to Bytes
        memory_util.used = static_cast<U64>(total - free) * 1024;
        return SYSTEM_RESOURCES_OK;
    }
}

#include <stdio.h>              /* fopen() */
#include <stdlib.h>             /* scanf */
#include <sys/vfs.h>            /* statfs() */
#include <string.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {

    SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32 &cpuCount)
    {

        cpuCount = 1;

        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getCpuUtil(struct cpuUtil &cpuUtil, bool average, U32 cpu_index)
    {
    
        cpuUtil.cpuUsed = 100;
        cpuUtil.cpuTotal = 100;

        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getMemUtil(struct memUtil &memUtil)
    {


        memUtil.memTotal = 1;
        memUtil.memUsed = 1;

        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getPhysMemUtil(struct physMemUtil &physMem)
    {

        physMem.physMemTotal = 1;
        physMem.physMemUsed = 1;

        return SYSTEM_RESOURCES_OK;
    }

}

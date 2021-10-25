#ifndef _SystemResources_hpp_
#define _SystemResources_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
	namespace SystemResources {
            
            typedef enum {
                SYSTEM_RESOURCES_OK, //!< Call was successful
                SYSTEM_RESOURCES_ERROR, //!< Call failed
            } SystemResourcesStatus;

            struct cpuUtil {
                F32 cpuUsed;
                F32 cpuTotal;
            };

            struct memUtil {
                F32 memTotal;
                F32 memUsed;
            };

            struct physMemUtil{
                F32 physMemTotal;
                F32 physMemUsed;
            };


            SystemResourcesStatus getCpuCount(U32 &cpuCount);
            SystemResourcesStatus getCpuUtil(struct cpuUtil &cpuUtil, bool average, U32 cpu_index = 0);
            SystemResourcesStatus getMemUtil(struct memUtil &memUtil);
            SystemResourcesStatus getPhysMemUtil(struct physMemUtil &physMem);
            

    }
}

#endif

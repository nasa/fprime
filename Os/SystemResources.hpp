#ifndef _SystemResources_hpp_
#define _SystemResources_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
	namespace SystemResources {
            
            typedef enum {
                SYSTEM_RESOURCES_OK, //!< Call was successful
                SYSTEM_RESOURCES_ERROR, //!< Call failed
            } SystemResourcesStatus;

            typedef struct {
                F32 cpuUsed;
                F32 cpuTotal;
            } cpuUtil_t;

            typedef struct {
                F32 memTotal;
                F32 memUsed;
            } memUtil_t;

            typedef struct {
                F32 physMemTotal;
                F32 physMemUsed;
            } physMemUtil_t;


            SystemResourcesStatus getCpuCount(U32 &cpuCount);
            SystemResourcesStatus getCpuUtil(cpuUtil_t &cpuUtil, U32 cpu_index, bool average);
            SystemResourcesStatus getMemUtil(memUtil_t &memUtil);
            SystemResourcesStatus getPhysMemUtil(physMemUtil_t &physMem);
            

    }
}

#endif

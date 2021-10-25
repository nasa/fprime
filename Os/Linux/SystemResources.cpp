#include <stdio.h>              /* fopen() */
#include <stdlib.h>             /* scanf */
#include <sys/vfs.h>            /* statfs() */
#include <string.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {

    SystemResources::SystemResourcesStatus SystemResources::getCpuCount(U32 &cpuCount)
    {

        char line[512];
        FILE *fp;
    	U32 cpu_count = 0;
    
    	if((fp = fopen("/proc/stat", "r")) == NULL) {
    
            return SYSTEM_RESOURCES_ERROR;
    	
    	}

    	fgets(line, sizeof(line), fp); //1st line.  Aggregate cpu line. Skip
    
    	while(true) {
           
    
            if (fgets(line, sizeof(line), fp) == NULL) { //cpu# line
                break;
            } 

            if (!(line[0] == 'c' && line[1] == 'p' && line[2] == 'u')) {

                break;
            }

            cpu_count++;
            
    	}

	fclose(fp);
    
    	cpuCount = cpu_count;

        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getCpuUtil(struct cpuUtil &cpuUtil, bool average, U32 cpu_index)
    {
        char line[512];
        FILE *fp;
	F32 cpu_data[4] = {0};
        U32 cpuCount;
	SystemResources::SystemResourcesStatus status;
        F32 cpuUsed = 0;
	F32 cpuTotal = 0;

        if((status = getCpuCount(cpuCount)) != SYSTEM_RESOURCES_OK) {
            return status;
	}

	if(cpu_index >= cpuCount) {
            return SYSTEM_RESOURCES_ERROR;
        }

        
    	if((fp = fopen("/proc/stat", "r")) == NULL) {
    
            return SYSTEM_RESOURCES_ERROR;
    	
    	}

    	fgets(line, sizeof(line), fp); //1st line.  Aggregate cpu line.

	if (average) {

            sscanf(line, "%*s %f %f %f %f", &cpu_data[0],
                                            &cpu_data[1],
                                            &cpu_data[2],
					    &cpu_data[3]); //cpu#: 4 numbers: usr, nice, sys, idle

            cpuUsed = cpu_data[0] + cpu_data[1] + cpu_data[2];
            cpuTotal = cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];

	}
	else {


	    for (U32 i = 0; i < cpu_index + 1; i++) {

    	        fgets(line, sizeof(line), fp); //cpu# line

		if(i != cpu_index) continue;

		if( !(line[0] == 'c' && line[1] == 'p' && line[2] == 'u') ) {

                    fclose(fp);
		    return SYSTEM_RESOURCES_ERROR;
		}

		sscanf(line, "%*s %f %f %f %f", &cpu_data[0],
				                &cpu_data[1],
						&cpu_data[2],
						&cpu_data[3]); //cpu#: 4 numbers: usr, nice, sys, idle

		cpuUsed = cpu_data[0] + cpu_data[1] + cpu_data[2];
		cpuTotal = cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];

		break;
	    }

	}

	fclose(fp);

	cpuUtil.cpuUsed = cpuUsed;
	cpuUtil.cpuTotal = cpuTotal;


        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getMemUtil(struct memUtil &memUtil)
    {

        F32 memFree = 0;
        F32 memTotal = 100;
        FILE *fp;
    
        fp = fopen("/proc/meminfo","r");
        if(fp == NULL) {

            memUtil.memTotal = (F32) -1.0;
            memUtil.memUsed = (F32) -1.0;
            return SYSTEM_RESOURCES_ERROR;
	}

        fscanf(fp,"%*s %f %*s", &memTotal);   /* 1st line is MemTotal */
        fscanf(fp,"%*s %f",     &memFree);   /* 2nd line is MemFree */
        fclose(fp);

        memUtil.memTotal = (memTotal != 0) ? (F32) memTotal: -1.0;
        memUtil.memUsed = (memTotal != 0) ? (F32)(((memTotal-memFree)/memTotal)*100.0): -1.0;


        return SYSTEM_RESOURCES_OK;
    }

    SystemResources::SystemResourcesStatus SystemResources::getPhysMemUtil(struct physMemUtil &physMem)
    {
        struct statfs buffer;
        I32 status;
     
     
        if ((status  = statfs("/", &buffer)) != 0)
        {
           physMem.physMemUsed = (F32) -1.0;  /* indicate an error */
           return SYSTEM_RESOURCES_ERROR;
        }
        else
        {
           physMem.physMemTotal = (F32) buffer.f_blocks;
           physMem.physMemUsed = (F32)(((F32)(buffer.f_blocks - buffer.f_bavail) / (F32)buffer.f_blocks)*100);
        }

        return SYSTEM_RESOURCES_OK;
    }

}

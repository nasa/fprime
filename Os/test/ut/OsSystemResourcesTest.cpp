#include <gtest/gtest.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>

void testTestSystemResources() {

    Os::SystemResources::SystemResourcesStatus sys_res_status;
    Os::SystemResources::cpuUtil cpuUtil;
    Os::SystemResources::memUtil memUtil;
    Os::SystemResources::physMemUtil physMem;
    U32 cpuCount;
    U32 cpuIndex;

    printf("Get CPU Count...\n");
    sys_res_status = Os::SystemResources::getCpuCount(cpuCount);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );
    printf("CPU Count:  %d\n", cpuCount);

    printf("Get CPU Utilization...\n");
    sys_res_status = Os::SystemResources::getCpuUtil(cpuUtil, true);
    printf("CPU Util:  %f, %f\n", cpuUtil.cpuTotal, cpuUtil.cpuUsed);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );

    cpuIndex = 1;
    printf("Get CPU Utilization for CPU %d...\n", cpuIndex);
    sys_res_status = Os::SystemResources::getCpuUtil(cpuUtil, false, cpuIndex);
    printf("CPU %d Util:  %f, %f\n", cpuIndex, cpuUtil.cpuTotal, cpuUtil.cpuUsed);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );

    cpuIndex = 1000;
    printf("Get CPU Utilization for CPU %d...\n", cpuIndex);
    sys_res_status = Os::SystemResources::getCpuUtil(cpuUtil, false, cpuIndex);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_ERROR );

    printf("Get RAM Memory Utilization...\n");
    sys_res_status = Os::SystemResources::getMemUtil(memUtil);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );
    printf("RAM Memory Used:  %.2f KB, %.2f%%\n", memUtil.memTotal, memUtil.memUsed);

    printf("Get Physical Memory Utilization...\n");
    sys_res_status = Os::SystemResources::getPhysMemUtil(physMem);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );
    printf("Physical Memory Used:  %.2f blocks, %.2f%%\n", physMem.physMemTotal, physMem.physMemUsed);

}

extern "C" {
    void systemResourcesTest(void);
}

void systemResourcesTest(void) {
    testTestSystemResources(); 
}

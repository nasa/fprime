#include <gtest/gtest.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>

void testTestSystemResources() {

    Os::SystemResources::SystemResourcesStatus sys_res_status;
    Os::SystemResources::cpuUtil_t cpuUtil;
    Os::SystemResources::memUtil_t memUtil;
    Os::SystemResources::physMemUtil_t physMem;
    U32 cpuCount;

    printf("Get CPU Count...\n");
    sys_res_status = Os::SystemResources::getCpuCount(cpuCount);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );
    printf("CPU Count:  %d\n", cpuCount);

    printf("Get CPU Utilization...\n");
    sys_res_status = Os::SystemResources::getCpuUtil(cpuUtil, 0, true);
    printf("CPU Util:  %f, %f\n", cpuUtil.cpuTotal, cpuUtil.cpuUsed);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );

    printf("Get CPU Utilization...\n");
    sys_res_status = Os::SystemResources::getCpuUtil(cpuUtil, 1, false);
    printf("CPU Util:  %f, %f\n", cpuUtil.cpuTotal, cpuUtil.cpuUsed);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK );

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

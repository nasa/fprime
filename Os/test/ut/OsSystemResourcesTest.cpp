#include <gtest/gtest.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>

void testTestSystemResources() {

    Os::SystemResources::SystemResourcesStatus sys_res_status;
    Os::SystemResources::CpuTicks cpuUtil;
    Os::SystemResources::MemUtil memUtil;

    U32 cpuCount;
    U32 cpuIndex;

    printf("Get CPU Count...\n");
    sys_res_status = Os::SystemResources::getCpuCount(cpuCount);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK);
    printf("CPU Count:  %d\n", cpuCount);

    cpuIndex = 1;
    printf("Get CPU Utilization for CPU %d...\n", cpuIndex);
    sys_res_status = Os::SystemResources::getCpuTicks(cpuUtil, cpuIndex);
    printf("CPU %d Util:  %f, %f\n", cpuIndex, cpuUtil.total, cpuUtil.used);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK);

    cpuIndex = 1000;
    printf("Get CPU Utilization for CPU %d...\n", cpuIndex);
    sys_res_status = Os::SystemResources::getCpuTicks(cpuUtil, cpuIndex);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_ERROR);

    printf("Get RAM Memory Utilization...\n");
    sys_res_status = Os::SystemResources::getMemUtil(memUtil);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK);
    printf("RAM Memory Used:  %.2f KB, %.2f%%\n", memUtil.total, memUtil.used);
}

extern "C" {
    void systemResourcesTest(void);
}

void systemResourcesTest(void) {
    testTestSystemResources(); 
}

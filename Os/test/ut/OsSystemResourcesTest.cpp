#include <gtest/gtest.h>
#include <Os/SystemResources.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>

void testTestSystemResources() {

    Os::SystemResources::SystemResourcesStatus sys_res_status;
    Os::SystemResources::CpuTicks cpuUtil;

    U32 cpuCount;
    U32 cpuIndex;

    sys_res_status = Os::SystemResources::getCpuCount(cpuCount);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK);

    cpuIndex = 0;
    sys_res_status = Os::SystemResources::getCpuTicks(cpuUtil, cpuIndex);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_OK);

    cpuIndex = 1000;
    sys_res_status = Os::SystemResources::getCpuTicks(cpuUtil, cpuIndex);
    ASSERT_EQ(sys_res_status, Os::SystemResources::SystemResourcesStatus::SYSTEM_RESOURCES_ERROR);
}

extern "C" {
    void systemResourcesTest(void);
}

void systemResourcesTest(void) {
    testTestSystemResources(); 
}

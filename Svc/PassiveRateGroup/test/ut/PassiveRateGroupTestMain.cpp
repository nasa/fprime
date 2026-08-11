/*
 * \author Tim Canham
 * \file
 * \brief
 *
 * This file is the test driver for the active rate group unit test.
 *
 * Code Generated Source Code Header
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 */
#include <config/FppConstantsAc.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroup.hpp>
#include <Svc/PassiveRateGroup/test/ut/PassiveRateGroupTester.hpp>

#include <gtest/gtest.h>

// Test fixture for common setup/teardown
class PassiveRateGroupTestFixture : public ::testing::Test {
  protected:
    PassiveRateGroupTestFixture() : contexts(0) {}

    void SetUp() override {
        // Initialize contexts for all ports
        for (FwSizeType i = 0; i < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; i++) {
            contexts[i] = static_cast<U32>(i + 1);
        }
    }

    void configureAndConnect(Svc::PassiveRateGroup& impl,
                             Svc::PassiveRateGroupTester& tester,
                             FwEnumStoreType instance = 0,
                             Os::RawTimeSource rawTimeSource = Os::RAWTIME_DEFAULT) {
        // Configure with optional rawTimeSource
        impl.configure(contexts, rawTimeSource);

        // Initialize
        tester.init();
        impl.init(instance);

        // Connect ports
        tester.connect_to_CycleIn(0, impl.get_CycleIn_InputPort(0));

        for (FwIndexType portNum = 0;
             portNum < static_cast<FwIndexType>(Svc::PassiveRateGroupTester::getNumRateGroupMemberOutPorts());
             portNum++) {
            impl.set_RateGroupMemberOut_OutputPort(portNum, tester.get_from_RateGroupMemberOut(portNum));
        }

        impl.set_Tlm_OutputPort(0, tester.get_from_Tlm(0));
        impl.set_Time_OutputPort(0, tester.get_from_Time(0));
        impl.set_CmdStatus_OutputPort(0, tester.get_from_CmdStatus(0));
        impl.set_CmdReg_OutputPort(0, tester.get_from_CmdReg(0));
        tester.connect_to_CmdDisp(0, impl.get_CmdDisp_InputPort(0));
    }

    Svc::PassiveRateGroup::ContextArray contexts;
};

TEST_F(PassiveRateGroupTestFixture, NominalSchedule) {
    for (FwEnumStoreType inst = 0; inst < 3; inst++) {
        Svc::PassiveRateGroup impl("PassiveRateGroup");
        Svc::PassiveRateGroupTester tester(impl);

        configureAndConnect(impl, tester, inst);

        tester.runNominal(contexts.getElements(), Svc::PassiveRateGroup::CONNECTION_COUNT_MAX, inst);
    }
}

TEST_F(PassiveRateGroupTestFixture, PortCycleTimes) {
    Svc::PassiveRateGroup impl("PassiveRateGroup");
    Svc::PassiveRateGroupTester tester(impl);

    configureAndConnect(impl, tester);

    tester.runPortCycleTimeTest();
}

TEST_F(PassiveRateGroupTestFixture, ClearStatistics) {
    Svc::PassiveRateGroup impl("PassiveRateGroup");
    Svc::PassiveRateGroupTester tester(impl);

    configureAndConnect(impl, tester);

    tester.runClearStatisticsTest();
}

TEST_F(PassiveRateGroupTestFixture, RawTimeSourceConfiguration) {
    Svc::PassiveRateGroup impl("PassiveRateGroup");
    Svc::PassiveRateGroupTester tester(impl);

    // Explicitly configure with rawTimeSource parameter to exercise the API
    configureAndConnect(impl, tester, 0, Os::RAWTIME_DEFAULT);

    tester.runRawTimeSourceTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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

void connectPorts(Svc::PassiveRateGroup& impl, Svc::PassiveRateGroupTester& tester) {
    tester.connect_to_CycleIn(0, impl.get_CycleIn_InputPort(0));

    for (NATIVE_INT_TYPE portNum = 0;
         portNum < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
        impl.set_RateGroupMemberOut_OutputPort(portNum, tester.get_from_RateGroupMemberOut(portNum));
    }

    impl.set_Tlm_OutputPort(0, tester.get_from_Tlm(0));
    impl.set_Time_OutputPort(0, tester.get_from_Time(0));
}

TEST(PassiveRateGroupTest, NominalSchedule) {
    for (NATIVE_INT_TYPE inst = 0; inst < 3; inst++) {
        NATIVE_INT_TYPE contexts[FppConstant_PassiveRateGroupOutputPorts::PassiveRateGroupOutputPorts] = {1, 2, 3, 4, 5};

        Svc::PassiveRateGroup impl("PassiveRateGroup");
        impl.configure(contexts, FW_NUM_ARRAY_ELEMENTS(contexts));
        Svc::PassiveRateGroupTester tester(impl);

        tester.init();
        impl.init(inst);

        // connect ports
        connectPorts(impl, tester);

        tester.runNominal(contexts, FW_NUM_ARRAY_ELEMENTS(contexts), inst);
    }
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

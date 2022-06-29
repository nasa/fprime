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
*
*/

#include <Svc/ActiveRateGroup/test/ut/ActiveRateGroupImplTester.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroup.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::ActiveRateGroup& impl, Svc::ActiveRateGroupImplTester& tester) {

    tester.connect_to_CycleIn(0,impl.get_CycleIn_InputPort(0));

    for (NATIVE_UINT_TYPE portNum = 0; portNum < static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
        impl.set_RateGroupMemberOut_OutputPort(portNum,tester.get_from_RateGroupMemberOut(portNum));
    }

    impl.set_Log_OutputPort(0,tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0,tester.get_from_LogText(0));

    impl.set_Tlm_OutputPort(0,tester.get_from_Tlm(0));
    impl.set_Time_OutputPort(0,tester.get_from_Time(0));

    impl.set_PingOut_OutputPort(0,tester.get_from_PingOut(0));
    tester.connect_to_PingIn(0,impl.get_PingIn_InputPort(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(ActiveRateGroupTest,NominalSchedule) {

    for (NATIVE_INT_TYPE inst = 0; inst < 3; inst++) {

        NATIVE_INT_TYPE contexts[Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];
        for (U32 i = 0; i < Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; i++) {
            contexts[i] = i + 1;
        }

        Svc::ActiveRateGroup impl("ActiveRateGroup");
        impl.configure(contexts,FW_NUM_ARRAY_ELEMENTS(contexts));

        Svc::ActiveRateGroupImplTester tester(impl);

        tester.init();
        impl.init(10,inst);

        // connect ports
        connectPorts(impl,tester);

        tester.runNominal(contexts,FW_NUM_ARRAY_ELEMENTS(contexts), inst);

    }

}

TEST(ActiveRateGroupTest,CycleOverrun) {

    for (NATIVE_INT_TYPE inst = 0; inst < 3; inst++) {

        NATIVE_INT_TYPE contexts[Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];
        for (U32 i = 0; i < Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; i++) {
            contexts[i] = i + 1;
        }

        Svc::ActiveRateGroup impl("ActiveRateGroup");
        impl.configure(contexts,FW_NUM_ARRAY_ELEMENTS(contexts));

        Svc::ActiveRateGroupImplTester tester(impl);

        tester.init();
        impl.init(10,inst);

        // connect ports
        connectPorts(impl,tester);

        tester.runCycleOverrun(contexts,FW_NUM_ARRAY_ELEMENTS(contexts), inst);

    }
}

TEST(ActiveRateGroupTest,PingPort) {

    NATIVE_INT_TYPE contexts[Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS];
    for (U32 i = 0; i < Svc::ActiveRateGroupComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; i++) {
        contexts[i] = i + 1;
    }

    Svc::ActiveRateGroup impl("ActiveRateGroup");
    impl.configure(contexts,FW_NUM_ARRAY_ELEMENTS(contexts));
    Svc::ActiveRateGroupImplTester tester(impl);

    tester.init();
    impl.init(10,0);

    connectPorts(impl,tester);
    tester.runPingTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



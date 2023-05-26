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
*   acknowledged. Any commercial use must be negotiated with the Office
*   of Technology Transfer at the California Institute of Technology.
*
*   Information included herein is controlled under the International
*   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
*   Export or transfer of this information to a Foreign Person or foreign
*   entity requires an export license issued by the U.S. State Department
*   or an ITAR exemption prior to the export or transfer.
*/

#include <Svc/PassiveRateGroup/test/ut/PassiveRateGroupImplTester.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroupImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::PassiveRateGroupImpl& impl, Svc::PassiveRateGroupImplTester& tester) {

    tester.connect_to_CycleIn(0,impl.get_CycleIn_InputPort(0));

    for (NATIVE_INT_TYPE portNum = 0; portNum < (NATIVE_INT_TYPE)FW_NUM_ARRAY_ELEMENTS(impl.m_RateGroupMemberOut_OutputPort); portNum++) {
        impl.set_RateGroupMemberOut_OutputPort(portNum,tester.get_from_RateGroupMemberOut(portNum));
    }

    impl.set_Log_OutputPort(0,tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0,tester.get_from_LogText(0));

    impl.set_Tlm_OutputPort(0,tester.get_from_Tlm(0));
    impl.set_Time_OutputPort(0,tester.get_from_Time(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(PassiveRateGroupTest,NominalSchedule) {

    for (NATIVE_INT_TYPE inst = 0; inst < 3; inst++) {

        NATIVE_UINT_TYPE contexts[] = {1,2,3,4,5};

        Svc::PassiveRateGroupImpl impl("PassiveRateGroupImpl",contexts,FW_NUM_ARRAY_ELEMENTS(contexts));

        Svc::PassiveRateGroupImplTester tester(impl);

        tester.init();
        impl.init(inst);

        // connect ports
        connectPorts(impl,tester);

        tester.runNominal(contexts,FW_NUM_ARRAY_ELEMENTS(contexts), inst);

    }

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



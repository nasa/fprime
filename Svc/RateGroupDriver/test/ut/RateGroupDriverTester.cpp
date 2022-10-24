/*
 * RateGroupDriverTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/RateGroupDriver/test/ut/RateGroupDriverImplTester.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::RateGroupDriver& impl, Svc::RateGroupDriverImplTester& tester) {

    impl.set_CycleOut_OutputPort(0,tester.get_from_CycleOut(0));
    impl.set_CycleOut_OutputPort(1,tester.get_from_CycleOut(1));
    impl.set_CycleOut_OutputPort(2,tester.get_from_CycleOut(2));

    tester.connect_to_CycleIn(0,impl.get_CycleIn_InputPort(0));
#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(RateGroupDriverTest,NominalSchedule) {

    NATIVE_INT_TYPE dividers[] = {1,2,3};

    Svc::RateGroupDriver impl("RateGroupDriver");
    impl.configure(dividers,FW_NUM_ARRAY_ELEMENTS(dividers));

    Svc::RateGroupDriverImplTester tester(impl);

    tester.init();
    impl.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runSchedNominal(dividers,FW_NUM_ARRAY_ELEMENTS(dividers));

}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



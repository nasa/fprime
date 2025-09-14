/*
 * EventManagerTesterMain.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/EventManager/EventManager.hpp>
#include <Svc/EventManager/test/ut/EventManagerTester.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::EventManager& impl, Svc::EventManagerTester& tester) {
    tester.connect_to_CmdDisp(0, impl.get_CmdDisp_InputPort(0));
    impl.set_CmdStatus_OutputPort(0, tester.get_from_CmdStatus(0));
    impl.set_FatalAnnounce_OutputPort(0, tester.get_from_FatalAnnounce(0));

    tester.connect_to_LogRecv(0, impl.get_LogRecv_InputPort(0));

    impl.set_Log_OutputPort(0, tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0, tester.get_from_LogText(0));

    impl.set_PktSend_OutputPort(0, tester.get_from_PktSend(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(EventManagerTest, NominalEventSend) {
    TEST_CASE(100.1.1, "Nominal Event Logging");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runEventNominal();
}

TEST(EventManagerTest, FilteredEventSend) {
    TEST_CASE(100.1.2, "Nominal Event Filtering");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runFilterEventNominal();
}

TEST(EventManagerTest, FilterIdTest) {
    TEST_CASE(100.1.3, "Filter events by ID");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runFilterIdNominal();
}

TEST(EventManagerTest, FilterDumpTest) {
    TEST_CASE(100.1.3, "Dump filter values");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runFilterDump();
}

TEST(EventManagerTest, InvalidCommands) {
    TEST_CASE(100.2.1, "Off-Nominal Invalid Commands");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runFilterInvalidCommands();
}

TEST(EventManagerTest, FatalTesting) {
    TEST_CASE(100.2.2, "Off-Nominal FATAL processing");

    Svc::EventManager impl("EventManager");

    impl.init(10, 0);

    Svc::EventManagerTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runEventFatal();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

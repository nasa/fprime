/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/ActiveLogger/test/ut/ActiveLoggerImplTester.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Test/UnitTest.hpp>

#include <gtest/gtest.h>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::ActiveLoggerImpl& impl, Svc::ActiveLoggerImplTester& tester) {

    tester.connect_to_CmdDisp(0,impl.get_CmdDisp_InputPort(0));
    impl.set_CmdStatus_OutputPort(0,tester.get_from_CmdStatus(0));
    impl.set_FatalAnnounce_OutputPort(0,tester.get_from_FatalAnnounce(0));

    tester.connect_to_LogRecv(0,impl.get_LogRecv_InputPort(0));

    impl.set_Log_OutputPort(0,tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0,tester.get_from_LogText(0));

    impl.set_PktSend_OutputPort(0,tester.get_from_PktSend(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(ActiveLoggerTest,NominalEventSend) {

    TEST_CASE(100.1.1,"Nominal Event Logging");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runEventNominal();

}

TEST(ActiveLoggerTest,FilteredEventSend) {

    TEST_CASE(100.1.2,"Nominal Event Filtering");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runFilterEventNominal();

}

TEST(ActiveLoggerTest,FilterIdTest) {

    TEST_CASE(100.1.3,"Filter events by ID");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runFilterIdNominal();

}

TEST(ActiveLoggerTest,FilterDumpTest) {

    TEST_CASE(100.1.3,"Dump filter values");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runFilterDump();

}

TEST(ActiveLoggerTest,InvalidCommands) {

    TEST_CASE(100.2.1,"Off-Nominal Invalid Commands");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runFilterInvalidCommands();

}

TEST(ActiveLoggerTest,FatalTesting) {

    TEST_CASE(100.2.2,"Off-Nominal FATAL processing");

    Svc::ActiveLoggerImpl impl("ActiveLoggerImpl");

    impl.init(10,0);

    Svc::ActiveLoggerImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runEventFatal();

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



/*
 * CommandDispatcherTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/CmdDispatcher/test/ut/CommandDispatcherImplTester.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

void connectPorts(Svc::CommandDispatcherImpl& impl, Svc::CommandDispatcherImplTester& tester) {

    //Fw::SimpleObjRegistry simpleReg;

    // command ports
    tester.connect_to_compCmdStat(0,impl.get_compCmdStat_InputPort(0));
    tester.connect_to_seqCmdBuff(0,impl.get_seqCmdBuff_InputPort(0));
    tester.connect_to_compCmdReg(0,impl.get_compCmdReg_InputPort(0));

    impl.set_compCmdSend_OutputPort(0,tester.get_from_compCmdSend(0));
    impl.set_seqCmdStatus_OutputPort(0,tester.get_from_seqCmdStatus(0));
    // local dispatcher command registration
    impl.set_CmdReg_OutputPort(0,impl.get_compCmdReg_InputPort(1));
    impl.set_CmdStatus_OutputPort(0,impl.get_compCmdStat_InputPort(0));

    impl.set_compCmdSend_OutputPort(1,impl.get_CmdDisp_InputPort(0));

    impl.set_Tlm_OutputPort(0,tester.get_from_Tlm(0));
    impl.set_Time_OutputPort(0,tester.get_from_Time(0));

    impl.set_Log_OutputPort(0,tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0,tester.get_from_LogText(0));

#if FW_PORT_TRACING
    //Fw::PortBase::setTrace(true);
#endif

    //simpleReg.dump();

}

TEST(CmdDispTestNominal,NominalDispatch) {

    TEST_CASE(102.1.1,"Nominal Dispatch");
    COMMENT("Dispatch a series of commands and verify they are dispatched correctly.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runNominalDispatch();

}

TEST(CmdDispTestNominal,NopTest) {

    TEST_CASE(102.1.2,"NO_OP Command Test");
    COMMENT("Verify the test NO_OP commands by dispatching them.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runNopCommands();

}

TEST(CmdDispTestNominal, ReregisterCommand) {
    
    TEST_CASE(102.1.3,"Reregister Command");
    COMMENT("Verify user can call command registration port with the same opcode multiple times safely.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runCommandReregister();
}

TEST(CmdDispTestOffNominal,InvalidOpcodeDispatch) {

    TEST_CASE(102.2.1,"Off-nominal Dispatch");
    COMMENT("Verify the correct handling of unregistered opcodes.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runInvalidOpcodeDispatch();

}

TEST(CmdDispTestOffNominal,FailedCommand) {

    TEST_CASE(102.2.2,"Off-nominal Failed command");
    COMMENT("Verify that failed commands operate correctly");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runFailedCommand();

}

TEST(CmdDispTestOffNominal,InvalidCommand) {

    TEST_CASE(102.2.3,"Off-nominal Invalid Command");
    COMMENT("Verify that malformed commands are detected.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runInvalidCommand();

}

TEST(CmdDispTestOffNominal,CommandOverflow) {

    TEST_CASE(102.2.4,"Off-nominal Command Overflow");
    COMMENT("Verify error case where there are too many outstanding commands.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runOverflowCommands();

}

TEST(CmdDispTestOffNominal,ClearSequenceTracker) {

    TEST_CASE(102.1.3,"Clear Command Tracker");
    COMMENT("Verify command to clear command tracker.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10,0);

    Svc::CommandDispatcherImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runClearCommandTracking();

}

#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif


/*
 * CommandDispatcherTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <limits>

#include <gtest/gtest.h>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Svc/CmdDispatcher/test/ut/CommandDispatcherTester.hpp>

void connectPorts(Svc::CommandDispatcherImpl& impl, Svc::CommandDispatcherTester& tester) {
    // Fw::SimpleObjRegistry simpleReg;

    // command ports
    tester.connect_to_compCmdStat(0, impl.get_compCmdStat_InputPort(0));
    tester.connect_to_seqCmdBuff(0, impl.get_seqCmdBuff_InputPort(0));
    tester.connect_to_compCmdReg(0, impl.get_compCmdReg_InputPort(0));
    tester.connect_to_run(0, impl.get_run_InputPort(0));

    impl.set_compCmdSend_OutputPort(0, tester.get_from_compCmdSend(0));
    impl.set_seqCmdStatus_OutputPort(0, tester.get_from_seqCmdStatus(0));
    // local dispatcher command registration
    impl.set_CmdReg_OutputPort(0, impl.get_compCmdReg_InputPort(1));
    impl.set_CmdStatus_OutputPort(0, impl.get_compCmdStat_InputPort(0));

    impl.set_compCmdSend_OutputPort(1, impl.get_CmdDisp_InputPort(0));

    impl.set_Tlm_OutputPort(0, tester.get_from_Tlm(0));
    impl.set_Time_OutputPort(0, tester.get_from_Time(0));

    impl.set_Log_OutputPort(0, tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0, tester.get_from_LogText(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(CmdDispTestNominal, NominalDispatch) {
    TEST_CASE(102.1.1, "Nominal Dispatch");
    COMMENT("Dispatch a series of commands and verify they are dispatched correctly.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runNominalDispatch();
}

TEST(CmdDispTestNominal, NopTest) {
    TEST_CASE(102.1.2, "NO_OP Command Test");
    COMMENT("Verify the test NO_OP commands by dispatching them.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runNopCommands();
}

TEST(CmdDispTestNominal, ReregisterCommand) {
    TEST_CASE(102.1.3, "Reregister Command");
    COMMENT("Verify user can call command registration port with the same opcode multiple times safely.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runCommandReregister();
}

TEST(CmdDispTestNominal, NonZeroPortDispatch) {
    TEST_CASE(102.1.4, "Nonzero Port Index Dispatch");
    COMMENT("Verify registration and dispatch through a nonzero port index.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // connect a nonzero port index (index 1 is used internally for local commands)
    tester.connect_to_compCmdReg(2, impl.get_compCmdReg_InputPort(2));
    tester.connect_to_seqCmdBuff(2, impl.get_seqCmdBuff_InputPort(2));
    impl.set_compCmdSend_OutputPort(2, tester.get_from_compCmdSend(2));
    impl.set_seqCmdStatus_OutputPort(2, tester.get_from_seqCmdStatus(2));

    tester.runNonZeroPortDispatch();
}

TEST(CmdDispTestOffNominal, InvalidOpcodeDispatch) {
    TEST_CASE(102.2.1, "Off-nominal Dispatch");
    COMMENT("Verify the correct handling of unregistered opcodes.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runInvalidOpcodeDispatch();
}

TEST(CmdDispTestOffNominal, FailedCommand) {
    TEST_CASE(102.2.2, "Off-nominal Failed command");
    COMMENT("Verify that failed commands operate correctly");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runFailedCommand();
}

TEST(CmdDispTestOffNominal, InvalidCommand) {
    TEST_CASE(102.2.3, "Off-nominal Invalid Command");
    COMMENT("Verify that malformed commands are detected.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runInvalidCommand();
}

TEST(CmdDispTestOffNominal, CommandOverflow) {
    TEST_CASE(102.2.4, "Off-nominal Command Overflow");
    COMMENT("Verify error case where there are too many outstanding commands.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runOverflowCommands();
}

TEST(CmdDispTestOffNominal, ClearSequenceTracker) {
    TEST_CASE(102.1.3, "Clear Command Tracker");
    COMMENT("Verify command to clear command tracker.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runClearCommandTracking();
}

TEST(CmdDispTestOffNominal, CommandQueueOverflow) {
    TEST_CASE(102.2.5, "Off-nominal Command QueueOverflow");
    COMMENT("Verify error case where the seqCmdBuff port queue overflows and does not ASSERT.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");

    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runCommandQueueOverflow();
}

TEST(CmdDispTestOffNominal, SequenceNumberWrapSkipsTrackedIds) {
    TEST_CASE(102.2.6, "Sequence Number Wraparound");
    COMMENT("Verify sequence number allocation skips IDs that are still tracked across U32 wraparound.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");
    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);
    tester.init();

    const U32 maxSequenceNumber = std::numeric_limits<U32>::max();

    // An early command (sequence number 0) is still outstanding when the counter reaches its maximum
    ASSERT_TRUE(tester.trackSequenceNumber(0));
    tester.setSequenceNumber(maxSequenceNumber);
    EXPECT_FALSE(tester.hasSequenceNumberWrapped());

    // Allocating the maximum value wraps the counter to 0 and latches the wrap flag
    EXPECT_EQ(maxSequenceNumber, tester.allocateSequenceNumber());
    ASSERT_TRUE(tester.trackSequenceNumber(maxSequenceNumber));
    EXPECT_EQ(0U, tester.getSequenceNumber());
    EXPECT_TRUE(tester.hasSequenceNumberWrapped());

    // 0 is still tracked, so the next allocation must skip it
    EXPECT_EQ(1U, tester.allocateSequenceNumber());
    EXPECT_EQ(2U, tester.getSequenceNumber());
}

TEST(CmdDispTestOffNominal, SequenceNumberNoScanBeforeWrap) {
    TEST_CASE(102.2.7, "Sequence Number Allocation Before Wraparound");
    COMMENT("Verify sequence number allocation does not scan the tracker until the U32 counter has wrapped.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");
    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);
    tester.init();

    // Force an (otherwise impossible) collision at the current sequence number; no scan is expected before a wrap
    const U32 sequenceNumber = 7;
    ASSERT_TRUE(tester.trackSequenceNumber(sequenceNumber));
    tester.setSequenceNumber(sequenceNumber);

    EXPECT_FALSE(tester.hasSequenceNumberWrapped());
    EXPECT_EQ(sequenceNumber, tester.allocateSequenceNumber());
    EXPECT_EQ(sequenceNumber + 1, tester.getSequenceNumber());
    EXPECT_FALSE(tester.hasSequenceNumberWrapped());
}

TEST(CmdDispTestOffNominal, SequenceNumberWrapOnInvalidOpcode) {
    TEST_CASE(102.2.8, "Sequence Number Wraparound On Invalid Opcode");
    COMMENT("Verify the wrap flag is latched when an invalid opcode consumes the maximum sequence number.");

    Svc::CommandDispatcherImpl impl("CmdDispImpl");
    impl.init(10, 0);

    Svc::CommandDispatcherTester tester(impl);
    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.setSequenceNumber(std::numeric_limits<U32>::max());
    EXPECT_FALSE(tester.hasSequenceNumberWrapped());
    tester.runInvalidOpcodeDispatch();
    EXPECT_EQ(0U, tester.getSequenceNumber());
    EXPECT_TRUE(tester.hasSequenceNumberWrapped());
}

#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif

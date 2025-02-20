/*
 * CommandDispatcherImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/CmdDispatcher/test/ut/CommandDispatcherImplTester.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Os/IntervalTimer.hpp>

#include <cstdio>

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>


namespace Svc {

    void CommandDispatcherImplTester::init(NATIVE_INT_TYPE instance) {
        CommandDispatcherGTestBase::init();
    }

    CommandDispatcherImplTester::CommandDispatcherImplTester(Svc::CommandDispatcherImpl& inst) :
        CommandDispatcherGTestBase("testerbase",100),
        m_impl(inst) {
    }

    CommandDispatcherImplTester::~CommandDispatcherImplTester() {
    }

    void CommandDispatcherImplTester::from_compCmdSend_handler(FwIndexType portNum, FwOpcodeType opCode, U32 cmdSeq, Fw::CmdArgBuffer &args) {
        this->m_cmdSendOpCode = opCode;
        this->m_cmdSendCmdSeq = cmdSeq;
        this->m_cmdSendArgs = args;
        this->m_cmdSendRcvd = true;
    }

    void CommandDispatcherImplTester::from_seqCmdStatus_handler(FwIndexType portNum, FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdResponse &response) {
        this->m_seqStatusRcvd = true;
        this->m_seqStatusOpCode = opCode;
        this->m_seqStatusCmdSeq = cmdSeq;
        this->m_seqStatusCmdResponse = response;
    }

    void CommandDispatcherImplTester::runNominalDispatch() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event
        printTextLogHistory(stdout);
        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);


        REQUIREMENT("CD-003");
        // register our own command
        FwOpcodeType testOpCode = 0x50;

        this->clearEvents();
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        // dispatch a test command
        REQUIREMENT("CD-001");

        U32 testCmdArg = 100;
        U32 testContext = 110;
        this->clearEvents();
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        REQUIREMENT("CD-002");

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

        // verify sequence table entry
        ASSERT_TRUE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].seq, 0u);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort, 0);

        // verify command received
        ASSERT_TRUE(this->m_cmdSendRcvd);
        ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
        ASSERT_EQ(this->m_cmdSendCmdSeq,0);
        // check argument
        U32 checkVal;
        ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(checkVal,testCmdArg);

        this->clearEvents();
        this->m_seqStatusRcvd = false;
        // perform command response
        this->invoke_to_compCmdStat(0,testOpCode,this->m_cmdSendCmdSeq,Fw::CmdResponse::OK);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // Check dispatch table
        ASSERT_FALSE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].seq,0u);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].context,testContext);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // Verify completed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeCompleted_SIZE(1);
        ASSERT_EVENTS_OpCodeCompleted(0u,testOpCode);

        REQUIREMENT("CD-004");

        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(this->m_seqStatusOpCode,testOpCode);
        ASSERT_EQ(this->m_seqStatusCmdSeq,testContext);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImplTester::runNopCommands() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }

        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event

        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);

        // send NO_OP command
        this->m_seqStatusRcvd = false;
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(static_cast<FwOpcodeType>(CommandDispatcherImpl::OPCODE_CMD_NO_OP)),Fw::FW_SERIALIZE_OK);

        this->clearEvents();
        this->invoke_to_seqCmdBuff(0,buff,12);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1);

        // dispatch for async command
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // dispatch for async command response
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(CommandDispatcherImpl::OPCODE_CMD_NO_OP,this->m_seqStatusOpCode);
        // Verify correct context value is passed back.
        ASSERT_EQ(12u,this->m_seqStatusCmdSeq);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::OK);

        // send NO_OP_STRING command
        this->clearEvents();
        this->m_seqStatusRcvd = false;
        buff.resetSer();
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(FwOpcodeType(CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING)),Fw::FW_SERIALIZE_OK);
        // serialize arg1
        Fw::CmdStringArg argString("BOO!");
        ASSERT_EQ(buff.serialize(argString),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,13);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1);

        // dispatch for async command
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // dispatch for async command response
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,this->m_seqStatusOpCode);
        ASSERT_EQ(13u,this->m_seqStatusCmdSeq);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::OK);

        // send TEST_CMD_1 command
        this->m_seqStatusRcvd = false;
        buff.resetSer();
        ASSERT_EQ(buff.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(static_cast<FwOpcodeType>(CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1)),Fw::FW_SERIALIZE_OK);
        // serialize arg1
        ASSERT_EQ(buff.serialize(static_cast<I32>(1)),Fw::FW_SERIALIZE_OK);
        // serialize arg2
        ASSERT_EQ(buff.serialize(static_cast<F32>(2.3)),Fw::FW_SERIALIZE_OK);
        // serialize arg3
        ASSERT_EQ(buff.serialize(static_cast<U8>(4)),Fw::FW_SERIALIZE_OK);

        this->clearEvents();
        this->invoke_to_seqCmdBuff(0,buff,14);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1);

        // dispatch for async command
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // dispatch for async command response
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,this->m_seqStatusOpCode);
        ASSERT_EQ(14u,this->m_seqStatusCmdSeq);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImplTester::runCommandReregister() {
        // register built-in commands
        this->m_impl.regCommands();
        // clear reg events
        this->clearEvents();

        // register our own command
        FwOpcodeType testOpCode = 0x50;
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        // clear reg events
        this->clearEvents();

        // verify we can call cmdReg port again with the same opcode
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify re-registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeReregistered_SIZE(1);
        ASSERT_EVENTS_OpCodeReregistered(0,testOpCode,0);
    }

    void CommandDispatcherImplTester::runInvalidOpcodeDispatch() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event
        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);

        // register our own command
        FwOpcodeType testOpCode = 0x50;

        this->clearEvents();
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        // dispatch a test command with a bad opcode
        U32 testCmdArg = 100;
        U32 testContext = 13;
        this->clearEvents();
        this->m_seqStatusRcvd = false;
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(FwOpcodeType(testOpCode + 1)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->clearEvents();
        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_InvalidCommand_SIZE(1);
        ASSERT_EVENTS_InvalidCommand(0u,testOpCode+1);

        // Verify status passed back to port

        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(this->m_seqStatusOpCode,testOpCode+1);
        ASSERT_EQ(this->m_seqStatusCmdSeq,testContext);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::INVALID_OPCODE);
    }

    void CommandDispatcherImplTester::runFailedCommand() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event
        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);
        // register our own command
        FwOpcodeType testOpCode = 0x50;

        this->clearEvents();
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        U32 currSeq = 0;

        // dispatch a test command
        U32 testCmdArg = 100;
        U32 testContext = 13;
        this->clearEvents();
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

        // verify sequence table entry
        ASSERT_TRUE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].context,testContext);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // verify command received
        ASSERT_TRUE(this->m_cmdSendRcvd);
        ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
        ASSERT_EQ(currSeq,this->m_cmdSendCmdSeq);
        // check argument
        U32 checkVal;
        ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(checkVal,testCmdArg);

        this->clearEvents();
        this->m_seqStatusRcvd = false;
        // perform command response
        this->invoke_to_compCmdStat(0,testOpCode,this->m_cmdSendCmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // Check dispatch table
        ASSERT_FALSE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // Verify completed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeError_SIZE(1);
        ASSERT_EVENTS_OpCodeError(0,testOpCode, Fw::CmdResponse::EXECUTION_ERROR);

        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(this->m_seqStatusCmdSeq,testContext);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::EXECUTION_ERROR);

        // dispatch a test command
        currSeq++;
        this->clearEvents();
        buff.resetSer();
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

        // verify sequence table entry
        ASSERT_TRUE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].context,testContext);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // verify command received
        ASSERT_TRUE(this->m_cmdSendRcvd);
        ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
        ASSERT_EQ(currSeq,this->m_cmdSendCmdSeq);
        // check argument
        ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(checkVal,testCmdArg);

        this->clearEvents();
        this->m_seqStatusRcvd = false;
        // perform command response
        this->invoke_to_compCmdStat(0,testOpCode,this->m_cmdSendCmdSeq,Fw::CmdResponse::INVALID_OPCODE);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // Check dispatch table
        ASSERT_FALSE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // Verify completed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeError_SIZE(1);
        ASSERT_EVENTS_OpCodeError(0,testOpCode,Fw::CmdResponse::INVALID_OPCODE);

        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(this->m_seqStatusOpCode,testOpCode);
        ASSERT_EQ(testContext,this->m_seqStatusCmdSeq);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::INVALID_OPCODE);

        currSeq++;
        // dispatch a test command
        this->clearEvents();
        buff.resetSer();
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

        // verify sequence table entry
        ASSERT_TRUE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].context,testContext);

        // verify command received
        ASSERT_TRUE(this->m_cmdSendRcvd);
        ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
        ASSERT_EQ(currSeq,this->m_cmdSendCmdSeq);
        // check argument
        ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(checkVal,testCmdArg);

        this->clearEvents();
        this->m_seqStatusRcvd = false;
        // perform command response
        this->invoke_to_compCmdStat(0,testOpCode,this->m_cmdSendCmdSeq,Fw::CmdResponse::VALIDATION_ERROR);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // Check dispatch table
        ASSERT_FALSE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(currSeq,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // Verify completed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeError_SIZE(1);
        ASSERT_EVENTS_OpCodeError(0,testOpCode,Fw::CmdResponse::VALIDATION_ERROR);

        // Verify status passed back to port
        ASSERT_TRUE(this->m_seqStatusRcvd);
        ASSERT_EQ(this->m_seqStatusOpCode,testOpCode);
        ASSERT_EQ(testContext,this->m_seqStatusCmdSeq);
        ASSERT_EQ(this->m_seqStatusCmdResponse,Fw::CmdResponse::VALIDATION_ERROR);
    }

    void CommandDispatcherImplTester::runInvalidCommand() {
        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();

        // dispatch a flawed command
        U32 testCmdArg = 100;
        U32 testContext = 13;
        FwOpcodeType testOpCode = 0x50;
        this->clearEvents();
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(U32(100)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_MalformedCommand_SIZE(1);
        ASSERT_EVENTS_MalformedCommand(0,Fw::DeserialStatus::TYPE_MISMATCH);

    }

    void CommandDispatcherImplTester::runOverflowCommands() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event
        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);

        // register our own command
        FwOpcodeType testOpCode = 0x50;

        this->clearEvents();
        this->invoke_to_compCmdReg(0,0x50);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        for (NATIVE_UINT_TYPE disp = 0; disp < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE + 1; disp++) {
            // dispatch a test command
            U32 testCmdArg = 100;
            U32 testContext = 13;
            this->clearEvents();
            Fw::ComBuffer buff;
            ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

            this->invoke_to_seqCmdBuff(0,buff,testContext);
            ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

            if (disp < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE) {
                // verify dispatch event
                ASSERT_EVENTS_SIZE(1);
                ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
                ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

                // verify sequence table entry
                ASSERT_TRUE(this->m_impl.m_sequenceTracker[disp].used);
                ASSERT_EQ(disp,this->m_impl.m_sequenceTracker[disp].seq);
                ASSERT_EQ(this->m_impl.m_sequenceTracker[disp].opCode,testOpCode);
                ASSERT_EQ(this->m_impl.m_sequenceTracker[disp].context,testContext);
                ASSERT_EQ(this->m_impl.m_sequenceTracker[disp].callerPort,0);

                // verify command received
                ASSERT_TRUE(this->m_cmdSendRcvd);
                ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
                ASSERT_EQ(disp,this->m_cmdSendCmdSeq);
                // check argument
                U32 checkVal;
                ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
                ASSERT_EQ(checkVal,testCmdArg);
            } else {
                // verify failed to find slot
                ASSERT_EVENTS_SIZE(1);
                ASSERT_EVENTS_TooManyCommands_SIZE(1);
            }
        }

    }

    void CommandDispatcherImplTester::runClearCommandTracking() {

        // verify dispatch table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_entryTable); entry++) {
            ASSERT_TRUE(this->m_impl.m_entryTable[entry].used == false);
        }

        // verify sequence tracker table is empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }
        // clear reg events
        this->clearEvents();
        // register built-in commands
        this->m_impl.regCommands();
        // verify registrations
        ASSERT_TRUE(this->m_impl.m_entryTable[0].used);
        ASSERT_EQ(this->m_impl.m_entryTable[0].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP);
        ASSERT_EQ(this->m_impl.m_entryTable[0].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[1].used);
        ASSERT_EQ(this->m_impl.m_entryTable[1].opcode,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING);
        ASSERT_EQ(this->m_impl.m_entryTable[1].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[2].used);
        ASSERT_EQ(this->m_impl.m_entryTable[2].opcode,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1);
        ASSERT_EQ(this->m_impl.m_entryTable[2].port,1);

        ASSERT_TRUE(this->m_impl.m_entryTable[3].used);
        ASSERT_EQ(this->m_impl.m_entryTable[3].opcode,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING);
        ASSERT_EQ(this->m_impl.m_entryTable[3].port,1);

        // verify event
        ASSERT_EVENTS_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(4);
        ASSERT_EVENTS_OpCodeRegistered(0,CommandDispatcherImpl::OPCODE_CMD_NO_OP,1,0);
        ASSERT_EVENTS_OpCodeRegistered(1,CommandDispatcherImpl::OPCODE_CMD_NO_OP_STRING,1,1);
        ASSERT_EVENTS_OpCodeRegistered(2,CommandDispatcherImpl::OPCODE_CMD_TEST_CMD_1,1,2);
        ASSERT_EVENTS_OpCodeRegistered(3,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1,3);

        // register our own command
        FwOpcodeType testOpCode = 0x50;
        U32 testContext = 13;

        this->clearEvents();
        this->invoke_to_compCmdReg(0,testOpCode);
        ASSERT_TRUE(this->m_impl.m_entryTable[4].used);
        ASSERT_EQ(this->m_impl.m_entryTable[4].opcode,testOpCode);
        ASSERT_EQ(this->m_impl.m_entryTable[4].port,0);

        // verify registration event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered_SIZE(1);
        ASSERT_EVENTS_OpCodeRegistered(0,testOpCode,0,4);

        // dispatch a test command
        U32 testCmdArg = 100;
        this->clearEvents();
        Fw::ComBuffer buff;
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testOpCode),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(testCmdArg),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,testOpCode,0);

        // verify sequence table entry
        ASSERT_TRUE(this->m_impl.m_sequenceTracker[0].used);
        ASSERT_EQ(0u,this->m_impl.m_sequenceTracker[0].seq);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].opCode,testOpCode);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].context,testContext);
        ASSERT_EQ(this->m_impl.m_sequenceTracker[0].callerPort,0);

        // verify command received
        ASSERT_TRUE(this->m_cmdSendRcvd);
        ASSERT_EQ(this->m_cmdSendOpCode,testOpCode);
        ASSERT_EQ(0u,this->m_cmdSendCmdSeq);
        // check argument
        U32 checkVal;
        ASSERT_EQ(this->m_cmdSendArgs.deserialize(checkVal),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(checkVal,testCmdArg);
        this->clearEvents();

        // dispatch command to clear sequence tracker table

        buff.resetSer();
        ASSERT_EQ(buff.serialize(FwPacketDescriptorType(Fw::ComPacket::FW_PACKET_COMMAND)),Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(buff.serialize(static_cast<FwOpcodeType>(CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING)),Fw::FW_SERIALIZE_OK);

        this->invoke_to_seqCmdBuff(0,buff,testContext);
        // send buffer to command dispatcher
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify dispatch event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched_SIZE(1);
        ASSERT_EVENTS_OpCodeDispatched(0,CommandDispatcherImpl::OPCODE_CMD_CLEAR_TRACKING,1);

        // dispatch command from dispatcher to command handler
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());
        // verify tracking table empty
        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_sequenceTracker); entry++) {
            ASSERT_TRUE(this->m_impl.m_sequenceTracker[entry].used == false);
        }

        clearHistory();
        // send command complete
        this->invoke_to_compCmdStat(0,testOpCode,this->m_cmdSendCmdSeq,Fw::CmdResponse::OK);
        ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->m_impl.doDispatch());

        // verify no status returned
        ASSERT_CMD_RESPONSE_SIZE(0);

    }

    void CommandDispatcherImplTester::from_pingOut_handler(
              const FwIndexType portNum, /*!< The port number*/
              U32 key /*!< Value to return to pinger*/
          ) {

    }

} /* namespace SvcTest */

/*
 * CommandDispatcherImpl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <cstdio>
#include <cstring>

// Check the CMD_DISPATCHER_DISPATCH_TABLE_SIZE and CMD_DISPATCHER_SEQUENCER_TABLE_SIZE for overflow
static_assert(CMD_DISPATCHER_DISPATCH_TABLE_SIZE <= std::numeric_limits<FwOpcodeType>::max(),
              "Opcode table limited to opcode range");
static_assert(CMD_DISPATCHER_SEQUENCER_TABLE_SIZE <= std::numeric_limits<U32>::max(),
              "Sequencer table limited to range of U32");

namespace Svc {
CommandDispatcherImpl::CommandDispatcherImpl(const char* name)
    : CommandDispatcherComponentBase(name), m_seq(0), m_numCmdsDispatched(0), m_numCmdErrors(0), m_numCmdsDropped(0) {}

CommandDispatcherImpl::~CommandDispatcherImpl() {}

void CommandDispatcherImpl::compCmdReg_handler(FwIndexType portNum, FwOpcodeType opCode) {
    FwIndexType existingPort;
    if (this->m_entryTable.find(opCode, existingPort) == Fw::Success::SUCCESS) {
        // Opcode already present — must be the same port (re-registration)
        FW_ASSERT(existingPort == portNum, static_cast<FwAssertArgType>(opCode));
        this->log_DIAGNOSTIC_OpCodeReregistered(opCode, portNum);
    } else {
        const I32 slot = static_cast<I32>(this->m_entryTable.getSize());
        const Fw::Success status = this->m_entryTable.insert(opCode, portNum);
        FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(opCode));
        this->log_DIAGNOSTIC_OpCodeRegistered(opCode, portNum, slot);
    }
}

void CommandDispatcherImpl::compCmdStat_handler(FwIndexType portNum,
                                                FwOpcodeType opCode,
                                                U32 cmdSeq,
                                                const Fw::CmdResponse& response) {
    // check response and log
    if (Fw::CmdResponse::OK == response.e) {
        this->log_COMMAND_OpCodeCompleted(opCode);
    } else {
        this->m_numCmdErrors++;
        FW_ASSERT(response.e != Fw::CmdResponse::OK);
        this->log_COMMAND_OpCodeError(opCode, response);
    }
    // look for command source
    SequenceTrackerEntry trackedCmd;
    const Fw::Success removeStatus = this->m_sequenceTracker.remove(cmdSeq, trackedCmd);
    if (removeStatus == Fw::Success::SUCCESS) {
        const FwIndexType portToCall = trackedCmd.callerPort;
        const U32 context = trackedCmd.context;
        FW_ASSERT(opCode == trackedCmd.opCode);
        FW_ASSERT(portToCall < this->getNum_seqCmdStatus_OutputPorts());

        // call port to report status
        if (this->isConnected_seqCmdStatus_OutputPort(portToCall)) {
            // NOTE: seqCmdStatus port forwards three arguments: (opCode, cmdSeq, response).
            //       However, the cmdSeq value has no meaning for the calling sequencer.
            //       Instead, the context value is forwarded to allow the caller to utilize it if needed.
            this->seqCmdStatus_out(portToCall, opCode, context, response);
        }
    }
}

void CommandDispatcherImpl::seqCmdBuff_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    Fw::CmdPacket cmdPkt;
    Fw::SerializeStatus stat = cmdPkt.deserializeFrom(data);

    if (stat != Fw::FW_SERIALIZE_OK) {
        Fw::DeserialStatus serErr(static_cast<Fw::DeserialStatus::t>(stat));
        this->log_WARNING_HI_MalformedCommand(serErr);
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            this->seqCmdStatus_out(portNum, cmdPkt.getOpCode(), context, Fw::CmdResponse::VALIDATION_ERROR);
        }
        return;
    }

    // look up opcode in dispatch map
    FwIndexType entryPort;
    Fw::Success findStatus = this->m_entryTable.find(cmdPkt.getOpCode(), entryPort);
    if (findStatus == Fw::Success::SUCCESS and this->isConnected_compCmdSend_OutputPort(entryPort)) {
        // register command in command tracker only if response port is connect
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            SequenceTrackerEntry pendingCmd;
            pendingCmd.opCode = cmdPkt.getOpCode();
            pendingCmd.context = context;
            pendingCmd.callerPort = portNum;

            const Fw::Success pendingInsertStatus = this->m_sequenceTracker.insert(this->m_seq, pendingCmd);

            // if we couldn't find a slot to track the command, quit
            if (pendingInsertStatus != Fw::Success::SUCCESS) {
                this->log_WARNING_HI_TooManyCommands(cmdPkt.getOpCode());
                if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
                    this->seqCmdStatus_out(portNum, cmdPkt.getOpCode(), context, Fw::CmdResponse::EXECUTION_ERROR);
                }
                return;
            }
        }  // end if status port connected
        // pass arguments to argument buffer
        this->compCmdSend_out(entryPort, cmdPkt.getOpCode(), this->m_seq, cmdPkt.getArgBuffer());
        // log dispatched command
        this->log_COMMAND_OpCodeDispatched(cmdPkt.getOpCode(), entryPort);

        // increment command count
        this->m_numCmdsDispatched++;
    } else {
        this->log_WARNING_HI_InvalidCommand(cmdPkt.getOpCode());
        this->m_numCmdErrors++;
        // Fail command back to port, if connected
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            this->seqCmdStatus_out(portNum, cmdPkt.getOpCode(), context, Fw::CmdResponse::INVALID_OPCODE);
        }
    }

    // increment sequence number
    this->m_seq++;
}

void CommandDispatcherImpl ::run_handler(FwIndexType portNum, U32 context) {
    this->tlmWrite_CommandsDropped(this->m_numCmdsDropped);
    this->tlmWrite_CommandErrors(this->m_numCmdErrors);
    this->tlmWrite_CommandsDispatched(this->m_numCmdsDispatched);
}

void CommandDispatcherImpl::CMD_NO_OP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    Fw::LogStringArg no_op_string("Hello, World!");
    // Log event for NO_OP here.
    this->log_ACTIVITY_HI_NoOpReceived();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void CommandDispatcherImpl::CMD_NO_OP_STRING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& arg1) {
    Fw::LogStringArg msg(arg1.toChar());
    // Echo the NO_OP_STRING args here.
    this->log_ACTIVITY_HI_NoOpStringReceived(msg);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void CommandDispatcherImpl::CMD_TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3) {
    this->log_ACTIVITY_HI_TestCmd1Args(arg1, arg2, arg3);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void CommandDispatcherImpl::CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // clear tracking table
    this->m_sequenceTracker.clear();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void CommandDispatcherImpl::pingIn_handler(FwIndexType portNum, U32 key) {
    // respond to ping
    this->pingOut_out(0, key);
}

void CommandDispatcherImpl::seqCmdBuff_overflowHook(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    // Extract command opcode
    Fw::CmdPacket cmdPkt;
    Fw::SerializeStatus stat = cmdPkt.deserializeFrom(data);
    FwOpcodeType opcode = 0;  // Note: 0 = Reserved opcode

    if (stat == Fw::FW_SERIALIZE_OK) {
        opcode = cmdPkt.getOpCode();
    }

    // Log Cmd Buffer Overflow and increment CommandsDroppedBufOverflow counter
    this->m_numCmdsDropped++;
    this->log_WARNING_HI_CommandDroppedQueueOverflow(opcode, context);
}

}  // namespace Svc

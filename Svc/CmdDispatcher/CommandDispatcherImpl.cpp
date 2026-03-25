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
    : CommandDispatcherComponentBase(name), m_seq(0), m_numCmdsDispatched(0), m_numCmdErrors(0), m_numCmdsDropped(0) {
    memset(this->m_sequenceTracker, 0, sizeof(this->m_sequenceTracker));
}

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
    FwIndexType portToCall = -1;
    U32 context;
    for (U32 pending = 0; pending < FW_NUM_ARRAY_ELEMENTS(this->m_sequenceTracker); pending++) {
        if ((this->m_sequenceTracker[pending].seq == cmdSeq) && (this->m_sequenceTracker[pending].used)) {
            portToCall = this->m_sequenceTracker[pending].callerPort;
            context = this->m_sequenceTracker[pending].context;
            FW_ASSERT(opCode == this->m_sequenceTracker[pending].opCode);
            FW_ASSERT(portToCall < this->getNum_seqCmdStatus_OutputPorts());
            this->m_sequenceTracker[pending].used = false;
            break;
        }
    }

    if (portToCall != -1) {
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
            bool pendingFound = false;

            for (U32 pending = 0; pending < FW_NUM_ARRAY_ELEMENTS(this->m_sequenceTracker); pending++) {
                if (not this->m_sequenceTracker[pending].used) {
                    pendingFound = true;
                    this->m_sequenceTracker[pending].used = true;
                    this->m_sequenceTracker[pending].opCode = cmdPkt.getOpCode();
                    this->m_sequenceTracker[pending].seq = this->m_seq;
                    this->m_sequenceTracker[pending].context = context;
                    this->m_sequenceTracker[pending].callerPort = portNum;
                    break;
                }
            }

            // if we couldn't find a slot to track the command, quit
            if (not pendingFound) {
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
    for (FwOpcodeType entry = 0; entry < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE; entry++) {
        this->m_sequenceTracker[entry].used = false;
    }
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

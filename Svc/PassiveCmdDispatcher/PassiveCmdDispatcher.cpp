// ======================================================================
// \title  PassiveCmdDispatcher.cpp
// \author root
// \brief  cpp file for PassiveCmdDispatcher component implementation class
// ======================================================================

#include <Fw/Cmd/CmdPacket.hpp>
#include <Svc/PassiveCmdDispatcher/PassiveCmdDispatcher.hpp>

namespace Svc {

// Check the CMD_DISPATCHER_DISPATCH_TABLE_SIZE and CMD_DISPATCHER_SEQUENCER_TABLE_SIZE constants for overflow
static_assert(CMD_DISPATCHER_DISPATCH_TABLE_SIZE <= std::numeric_limits<FwOpcodeType>::max(),
              "Opcode table limited to opcode range");
static_assert(CMD_DISPATCHER_SEQUENCER_TABLE_SIZE <= std::numeric_limits<U32>::max(),
              "Sequencer table limited to range of U32");

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

PassiveCmdDispatcher::PassiveCmdDispatcher(const char* const compName)
    : PassiveCmdDispatcherComponentBase(compName), m_seq(0) {
    memset(this->m_entryTable, 0, sizeof(this->m_entryTable));
    memset(this->m_sequenceTracker, 0, sizeof(this->m_sequenceTracker));
}

PassiveCmdDispatcher::~PassiveCmdDispatcher() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void PassiveCmdDispatcher::compCmdReg_handler(FwIndexType portNum, FwOpcodeType opCode) {
    // Search for an empty slot
    bool slotFound = false;
    for (FwOpcodeType slot = 0; slot < CMD_DISPATCHER_DISPATCH_TABLE_SIZE; slot++) {
        if ((!this->m_entryTable[slot].used) && (!slotFound)) {
            this->m_entryTable[slot].opcode = opCode;
            this->m_entryTable[slot].port = portNum;
            this->m_entryTable[slot].used = true;
            slotFound = true;
        } else if (this->m_entryTable[slot].used && (this->m_entryTable[slot].opcode == opCode) &&
                   (this->m_entryTable[slot].port == portNum) && (!slotFound)) {
            slotFound = true;
        } else if (this->m_entryTable[slot].used) {
            // Ensure that there are no duplicates
            FW_ASSERT(this->m_entryTable[slot].opcode != opCode, static_cast<FwAssertArgType>(opCode));
        }
    }
    FW_ASSERT(slotFound, static_cast<FwAssertArgType>(opCode));
}

void PassiveCmdDispatcher::compCmdStat_handler(FwIndexType portNum,
                                               FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdResponse& response) {
    // Check the command response and log success/failure
    if (response.e == Fw::CmdResponse::OK) {
        this->log_COMMAND_OpCodeCompleted(opCode);
    } else {
        FW_ASSERT(response.e != Fw::CmdResponse::OK);
        this->log_COMMAND_OpCodeError(opCode, response);
    }

    // Search for the command source
    FwIndexType portToCall = -1;
    U32 context;
    for (U32 pending = 0; pending < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE; pending++) {
        if ((this->m_sequenceTracker[pending].seq == cmdSeq) && this->m_sequenceTracker[pending].used) {
            portToCall = this->m_sequenceTracker[pending].callerPort;
            context = this->m_sequenceTracker[pending].context;
            FW_ASSERT(opCode == this->m_sequenceTracker[pending].opCode);
            FW_ASSERT(portToCall < this->getNum_seqCmdStatus_OutputPorts());
            this->m_sequenceTracker[pending].used = false;
            break;
        }
    }
    // If found, call the port to report the command status
    if ((portToCall != -1) && this->isConnected_seqCmdStatus_OutputPort(portToCall)) {
        // NOTE: seqCmdStatus port forwards three arguments (opCode, cmdSeq, response) but the
        // cmdSeq value has no meaning for the calling sequencer; instead, the context value is
        // forwarded to allow the caller to utilize it if needed.
        this->seqCmdStatus_out(portToCall, opCode, context, response);
    }
}

void PassiveCmdDispatcher::seqCmdBuff_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    // Deserialize the command packet
    Fw::CmdPacket cmdPkt;
    Fw::SerializeStatus stat = cmdPkt.deserialize(data);
    if (stat != Fw::FW_SERIALIZE_OK) {
        Fw::DeserialStatus serErr = static_cast<Fw::DeserialStatus::t>(stat);
        this->log_WARNING_HI_MalformedCommand(serErr);
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            this->seqCmdStatus_out(portNum, cmdPkt.getOpCode(), context, Fw::CmdResponse::VALIDATION_ERROR);
        }
        return;
    }

    // Search for the opcode in the dispatch table
    FwOpcodeType opcode = cmdPkt.getOpCode();
    FwOpcodeType entry;
    bool entryFound = false;
    for (entry = 0; entry < CMD_DISPATCHER_DISPATCH_TABLE_SIZE; entry++) {
        if (this->m_entryTable[entry].used && (this->m_entryTable[entry].opcode == opcode)) {
            entryFound = true;
            break;
        }
    }
    if (entryFound && this->isConnected_compCmdSend_OutputPort(this->m_entryTable[entry].port)) {
        // Register the command in the command tracker only if the response port is connected
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            bool pendingFound = false;
            for (U32 pending = 0; pending < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE; pending++) {
                if (!this->m_sequenceTracker[pending].used) {
                    pendingFound = true;
                    this->m_sequenceTracker[pending].used = true;
                    this->m_sequenceTracker[pending].opCode = opcode;
                    this->m_sequenceTracker[pending].seq = this->m_seq;
                    this->m_sequenceTracker[pending].context = context;
                    this->m_sequenceTracker[pending].callerPort = portNum;
                    break;
                }
            }
            // If no slot was found to track the command, quit
            if (!pendingFound) {
                this->log_WARNING_HI_TooManyCommands(opcode);
                if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
                    this->seqCmdStatus_out(portNum, opcode, context, Fw::CmdResponse::EXECUTION_ERROR);
                }
                return;
            }
        }

        // Pass arguments to the argument buffer and log the dispatched command
        this->compCmdSend_out(this->m_entryTable[entry].port, opcode, this->m_seq, cmdPkt.getArgBuffer());
        this->log_COMMAND_OpCodeDispatched(opcode, this->m_entryTable[entry].port);
    } else {
        // Opcode could not be found in the dispatch table, fail the command
        this->log_WARNING_HI_InvalidCommand(opcode);
        if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
            this->seqCmdStatus_out(portNum, opcode, context, Fw::CmdResponse::INVALID_OPCODE);
        }
    }

    // Increment sequence number
    this->m_seq++;
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void PassiveCmdDispatcher::CMD_NO_OP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->log_ACTIVITY_HI_NoOpReceived();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveCmdDispatcher::CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Clear the sequence tracking table
    for (FwOpcodeType entry = 0; entry < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE; entry++) {
        this->m_sequenceTracker[entry].used = false;
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc

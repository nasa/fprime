/*
 * CommandDispatcherImpl.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Timothy Canham
 */

#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

namespace Svc {
    CommandDispatcherImpl::CommandDispatcherImpl(const char* name) :
        CommandDispatcherComponentBase(name),
        m_seq(0),
        m_numCmdsDispatched(0),
        m_numCmdErrors(0)
    {
        memset(this->m_entryTable,0,sizeof(this->m_entryTable));
        memset(this->m_sequenceTracker,0,sizeof(this->m_sequenceTracker));
    }

    CommandDispatcherImpl::~CommandDispatcherImpl() {
    }

    void CommandDispatcherImpl::init(
            NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
            NATIVE_INT_TYPE instance /*!< The instance number*/
            ) {
        CommandDispatcherComponentBase::init(queueDepth);
    }

    void CommandDispatcherImpl::compCmdReg_handler(NATIVE_INT_TYPE portNum, FwOpcodeType opCode) {
        // search for an empty slot
        bool slotFound = false;
        for (U32 slot = 0; slot < FW_NUM_ARRAY_ELEMENTS(this->m_entryTable); slot++) {
            if ((not this->m_entryTable[slot].used) and (not slotFound)) {
                this->m_entryTable[slot].opcode = opCode;
                this->m_entryTable[slot].port = portNum;
                this->m_entryTable[slot].used = true;
                this->log_DIAGNOSTIC_OpCodeRegistered(opCode,portNum,static_cast<I32>(slot));
                slotFound = true;
            } else if ((this->m_entryTable[slot].used) &&
                (this->m_entryTable[slot].opcode == opCode) &&
                (this->m_entryTable[slot].port == portNum) &&
                (not slotFound)) {
                    slotFound = true;
                    this->log_DIAGNOSTIC_OpCodeReregistered(opCode,portNum);
            } else if (this->m_entryTable[slot].used) { // make sure no duplicates
                FW_ASSERT(this->m_entryTable[slot].opcode != opCode, static_cast<FwAssertArgType>(opCode));
            }
        }
        FW_ASSERT(slotFound,static_cast<FwAssertArgType>(opCode));
    }

    void CommandDispatcherImpl::compCmdStat_handler(NATIVE_INT_TYPE portNum, FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdResponse &response) {
        // check response and log
        if (Fw::CmdResponse::OK == response.e) {
            this->log_COMMAND_OpCodeCompleted(opCode);
        } else {
            this->m_numCmdErrors++;
            this->tlmWrite_CommandErrors(this->m_numCmdErrors);
            FW_ASSERT(response.e != Fw::CmdResponse::OK);
            this->log_COMMAND_OpCodeError(opCode,response);
        }
        // look for command source
        NATIVE_INT_TYPE portToCall = -1;
        U32 context;
        for (U32 pending = 0; pending < FW_NUM_ARRAY_ELEMENTS(this->m_sequenceTracker); pending++) {
            if (
                    (this->m_sequenceTracker[pending].seq == cmdSeq) &&
                    (this->m_sequenceTracker[pending].used)
                ) {
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
                this->seqCmdStatus_out(portToCall,opCode,context,response);
            }
        }
    }

    void CommandDispatcherImpl::seqCmdBuff_handler(NATIVE_INT_TYPE portNum, Fw::ComBuffer &data, U32 context) {

        Fw::CmdPacket cmdPkt;
        Fw::SerializeStatus stat = cmdPkt.deserialize(data);

        if (stat != Fw::FW_SERIALIZE_OK) {
            Fw::DeserialStatus serErr(static_cast<Fw::DeserialStatus::t>(stat));
            this->log_WARNING_HI_MalformedCommand(serErr);
            if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
                this->seqCmdStatus_out(portNum,cmdPkt.getOpCode(),context,Fw::CmdResponse::VALIDATION_ERROR);
            }
            return;
        }

        // search for opcode in dispatch table
        U32 entry;
        bool entryFound = false;

        for (entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_entryTable); entry++) {
            if ((this->m_entryTable[entry].used) and (cmdPkt.getOpCode() == this->m_entryTable[entry].opcode)) {
                entryFound = true;
                break;
            }
        }
        if (entryFound and this->isConnected_compCmdSend_OutputPort(this->m_entryTable[entry].port)) {
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
                        this->seqCmdStatus_out(portNum,cmdPkt.getOpCode(),context,Fw::CmdResponse::EXECUTION_ERROR);
                    }
                    return;
                }
            } // end if status port connected
            // pass arguments to argument buffer
            this->compCmdSend_out(
                this->m_entryTable[entry].port,
                cmdPkt.getOpCode(),
                this->m_seq,
                cmdPkt.getArgBuffer());
            // log dispatched command
            this->log_COMMAND_OpCodeDispatched(cmdPkt.getOpCode(),this->m_entryTable[entry].port);

            // increment command count
            this->m_numCmdsDispatched++;
            // write telemetry channel for dispatched commands
            this->tlmWrite_CommandsDispatched(this->m_numCmdsDispatched);
        } else {
        	this->log_WARNING_HI_InvalidCommand(cmdPkt.getOpCode());
        	this->m_numCmdErrors++;
        	// Fail command back to port, if connected
        	if (this->isConnected_seqCmdStatus_OutputPort(portNum)) {
        	    this->seqCmdStatus_out(portNum,cmdPkt.getOpCode(),context,Fw::CmdResponse::INVALID_OPCODE);
        	}
        	this->tlmWrite_CommandErrors(this->m_numCmdErrors);
        }

        // increment sequence number
        this->m_seq++;
    }

    void CommandDispatcherImpl::CMD_NO_OP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    	Fw::LogStringArg no_op_string("Hello, World!");
    	// Log event for NO_OP here.
    	this->log_ACTIVITY_HI_NoOpReceived();
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImpl::CMD_NO_OP_STRING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& arg1) {
		Fw::LogStringArg msg(arg1.toChar());
    	// Echo the NO_OP_STRING args here.
    	this->log_ACTIVITY_HI_NoOpStringReceived(msg);
    	this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImpl::CMD_TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3) {
    	this->log_ACTIVITY_HI_TestCmd1Args(arg1,arg2,arg3);
    	this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImpl::CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
        // clear tracking table
        for (NATIVE_INT_TYPE entry = 0; entry < CMD_DISPATCHER_SEQUENCER_TABLE_SIZE; entry++) {
            this->m_sequenceTracker[entry].used = false;
        }
        this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
    }

    void CommandDispatcherImpl::pingIn_handler(NATIVE_INT_TYPE portNum, U32 key) {
        // respond to ping
        this->pingOut_out(0,key);
    }

}

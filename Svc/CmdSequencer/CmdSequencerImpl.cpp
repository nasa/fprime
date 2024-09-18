// ======================================================================
// \title  CmdSequencerImpl.cpp
// \author Bocchino/Canham
// \brief  cpp file for CmdDispatcherComponentBase component implementation class
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Svc/CmdSequencer/CmdSequencerImpl.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Types/Serializable.hpp>
extern "C" {
  #include <Utils/Hash/libcrc/lib_crc.h>
}

namespace Svc {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    CmdSequencerComponentImpl::
      CmdSequencerComponentImpl(const char* name) :
        CmdSequencerComponentBase(name),
        m_FPrimeSequence(*this),
        m_sequence(&this->m_FPrimeSequence),
        m_loadCmdCount(0),
        m_cancelCmdCount(0),
        m_errorCount(0),
        m_runMode(STOPPED),
        m_stepMode(AUTO),
        m_executedCount(0),
        m_totalExecutedCount(0),
        m_sequencesCompletedCount(0),
        m_timeout(0),
        m_blockState(Svc::CmdSequencer_BlockState::NO_BLOCK),
        m_opCode(0),
        m_cmdSeq(0),
        m_join_waiting(false)
    {

    }

    void CmdSequencerComponentImpl::init(const NATIVE_INT_TYPE queueDepth,
            const NATIVE_INT_TYPE instance) {
        CmdSequencerComponentBase::init(queueDepth, instance);
    }

    void CmdSequencerComponentImpl::setTimeout(const NATIVE_UINT_TYPE timeout) {
        this->m_timeout = timeout;
    }

    void CmdSequencerComponentImpl ::
      setSequenceFormat(Sequence& sequence)
    {
      this->m_sequence = &sequence;
    }

    void CmdSequencerComponentImpl ::
      allocateBuffer(
          const NATIVE_INT_TYPE identifier,
          Fw::MemAllocator& allocator,
          const NATIVE_UINT_TYPE bytes
      )
    {
        this->m_sequence->allocateBuffer(identifier, allocator, bytes);
    }

    void CmdSequencerComponentImpl ::
      loadSequence(const Fw::StringBase& fileName)
    {
      FW_ASSERT(this->m_runMode == STOPPED, this->m_runMode);
      if (not this->loadFile(fileName)) {
        this->m_sequence->clear();
      }
    }

    void CmdSequencerComponentImpl ::
      deallocateBuffer(Fw::MemAllocator& allocator)
    {
        this->m_sequence->deallocateBuffer(allocator);
    }

    CmdSequencerComponentImpl::~CmdSequencerComponentImpl() {

    }

    // ----------------------------------------------------------------------
    // Handler implementations
    // ----------------------------------------------------------------------

    void CmdSequencerComponentImpl::CS_RUN_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq,
            const Fw::CmdStringArg& fileName,
            Svc::CmdSequencer_BlockState block) {

        if (not this->requireRunMode(STOPPED)) {
            if (m_join_waiting) {
                // Inform user previous seq file is not complete
                this->log_WARNING_HI_CS_JoinWaitingNotComplete();
            }
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        this->m_blockState = block.e;
        this->m_cmdSeq = cmdSeq;
        this->m_opCode = opCode;

        // load commands
        if (not this->loadFile(fileName)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        this->m_executedCount = 0;

        // Check the step mode. If it is auto, start the sequence
        if (AUTO == this->m_stepMode) {
            this->m_runMode = RUNNING;
            if(this->isConnected_seqStartOut_OutputPort(0)) {
                this->seqStartOut_out(0, this->m_sequence->getStringFileName());
            }
            this->performCmd_Step();
        }

        if (Svc::CmdSequencer_BlockState::NO_BLOCK == this->m_blockState) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        }
    }

    void CmdSequencerComponentImpl::CS_VALIDATE_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq,
            const Fw::CmdStringArg& fileName
    ) {

        if (!this->requireRunMode(STOPPED)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        // load commands
        if (not this->loadFile(fileName)) {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        // clear the buffer
        this->m_sequence->clear();

        this->log_ACTIVITY_HI_CS_SequenceValid(this->m_sequence->getLogFileName());

        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);

    }

    //! Handler for input port seqRunIn
    void CmdSequencerComponentImpl::seqRunIn_handler(
           NATIVE_INT_TYPE portNum,
           const Fw::StringBase& filename
       ) {

        if (!this->requireRunMode(STOPPED)) {
            this->seqDone_out(0,0,0,Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        // If file name is non-empty, load a file.
        // Empty file name means don't load.
        if (filename != "") {
            Fw::CmdStringArg cmdStr(filename);
            const bool status = this->loadFile(cmdStr);
            if (!status) {
              this->seqDone_out(0,0,0,Fw::CmdResponse::EXECUTION_ERROR);
              return;
            }
        }
        else if (not this->m_sequence->hasMoreRecords()) {
            // No sequence loaded
            this->log_WARNING_LO_CS_NoSequenceActive();
            this->error();
            this->seqDone_out(0,0,0,Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        this->m_executedCount = 0;

        // Check the step mode. If it is auto, start the sequence
        if (AUTO == this->m_stepMode) {
            this->m_runMode = RUNNING;
            if(this->isConnected_seqStartOut_OutputPort(0)) {
                this->seqStartOut_out(0, this->m_sequence->getStringFileName());
            }
            this->performCmd_Step();
        }

        this->log_ACTIVITY_HI_CS_PortSequenceStarted(this->m_sequence->getLogFileName());
    }

    void CmdSequencerComponentImpl ::
      seqCancelIn_handler(
          const NATIVE_INT_TYPE portNum
      ) {
        if (RUNNING == this->m_runMode) {
            this->performCmd_Cancel();
            this->log_ACTIVITY_HI_CS_SequenceCanceled(this->m_sequence->getLogFileName());
            ++this->m_cancelCmdCount;
            this->tlmWrite_CS_CancelCommands(this->m_cancelCmdCount);
        } else {
            this->log_WARNING_LO_CS_NoSequenceActive();
        }
    }

    void CmdSequencerComponentImpl::CS_CANCEL_cmdHandler(
    FwOpcodeType opCode, U32 cmdSeq) {
        if (RUNNING == this->m_runMode) {
            this->performCmd_Cancel();
            this->log_ACTIVITY_HI_CS_SequenceCanceled(this->m_sequence->getLogFileName());
            ++this->m_cancelCmdCount;
            this->tlmWrite_CS_CancelCommands(this->m_cancelCmdCount);
        } else {
            this->log_WARNING_LO_CS_NoSequenceActive();
        }
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }

    void CmdSequencerComponentImpl::CS_JOIN_WAIT_cmdHandler(
        const FwOpcodeType opCode, const U32 cmdSeq) {

        // If there is no running sequence do not wait
        if (m_runMode != RUNNING) {
            this->log_WARNING_LO_CS_NoSequenceActive();
            this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
            return;
        } else {
            m_join_waiting = true;
            Fw::LogStringArg& logFileName = this->m_sequence->getLogFileName();
            this->log_ACTIVITY_HI_CS_JoinWaiting(logFileName, m_cmdSeq, m_opCode);
            m_cmdSeq = cmdSeq;
            m_opCode = opCode;
        }
    }

    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    bool CmdSequencerComponentImpl ::
      loadFile(const Fw::StringBase& fileName)
    {
      const bool status = this->m_sequence->loadFile(fileName);
      if (status) {
        Fw::LogStringArg& logFileName = this->m_sequence->getLogFileName();
        this->log_ACTIVITY_LO_CS_SequenceLoaded(logFileName);
        ++this->m_loadCmdCount;
        this->tlmWrite_CS_LoadCommands(this->m_loadCmdCount);
      }
      return status;
    }

    void CmdSequencerComponentImpl::error() {
        ++this->m_errorCount;
        this->tlmWrite_CS_Errors(m_errorCount);
    }

    void CmdSequencerComponentImpl::performCmd_Cancel() {
        this->m_sequence->reset();
        this->m_runMode = STOPPED;
        this->m_cmdTimer.clear();
        this->m_cmdTimeoutTimer.clear();
        this->m_executedCount = 0;
        // write sequence done port with error, if connected
        if (this->isConnected_seqDone_OutputPort(0)) {
            this->seqDone_out(0,0,0,Fw::CmdResponse::EXECUTION_ERROR);
        }

        if (Svc::CmdSequencer_BlockState::BLOCK == this->m_blockState || m_join_waiting) {
            // Do not wait if sequence was canceled or a cmd failed
            this->m_join_waiting = false;
            this->cmdResponse_out(this->m_opCode, this->m_cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }

        this->m_blockState = Svc::CmdSequencer_BlockState::NO_BLOCK;
    }

    void CmdSequencerComponentImpl ::
      cmdResponseIn_handler(
          NATIVE_INT_TYPE portNum,
          FwOpcodeType opcode,
          U32 cmdSeq,
          const Fw::CmdResponse& response
      )
    {
        if (this->m_runMode == STOPPED) {
            // Sequencer is not running
            this->log_WARNING_HI_CS_UnexpectedCompletion(opcode);
        } else {
            // clear command timeout
            this->m_cmdTimeoutTimer.clear();
            if (response != Fw::CmdResponse::OK) {
                this->commandError(this->m_executedCount, opcode, response.e);
                this->performCmd_Cancel();
            } else if (this->m_runMode == RUNNING && this->m_stepMode == AUTO) {
                // Auto mode
                this->commandComplete(opcode);
                if (not this->m_sequence->hasMoreRecords()) {
                    // No data left
                    this->m_runMode = STOPPED;
                    this->sequenceComplete();
                } else {
                    this->performCmd_Step();
                }
            } else {
                // Manual step mode
                this->commandComplete(opcode);
                if (not this->m_sequence->hasMoreRecords()) {
                    this->m_runMode = STOPPED;
                    this->sequenceComplete();
                }
            }
        }
    }

    void CmdSequencerComponentImpl ::
      schedIn_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE order)
    {

        Fw::Time currTime = this->getTime();
        // check to see if a command time is pending
        if (this->m_cmdTimer.isExpiredAt(currTime)) {
            this->comCmdOut_out(0, m_record.m_command, 0);
            this->m_cmdTimer.clear();
            // start command timeout timer
            this->setCmdTimeout(currTime);
        } else if (this->m_cmdTimeoutTimer.isExpiredAt(this->getTime())) { // check for command timeout
            this->log_WARNING_HI_CS_SequenceTimeout(
                m_sequence->getLogFileName(),
                this->m_executedCount
            );
            // If there is a command timeout, cancel the sequence
            this->performCmd_Cancel();
        }
    }

    void CmdSequencerComponentImpl ::
      CS_START_cmdHandler(FwOpcodeType opcode, U32 cmdSeq)
    {
        if (not this->m_sequence->hasMoreRecords()) {
            // No sequence loaded
            this->log_WARNING_LO_CS_NoSequenceActive();
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }
        if (!this->requireRunMode(STOPPED)) {
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        this->m_blockState = Svc::CmdSequencer_BlockState::NO_BLOCK;
        this->m_runMode = RUNNING;
        this->performCmd_Step();
        this->log_ACTIVITY_HI_CS_CmdStarted(this->m_sequence->getLogFileName());
        if(this->isConnected_seqStartOut_OutputPort(0)) {
            this->seqStartOut_out(0, this->m_sequence->getStringFileName());
        }
        this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::OK);
    }

    void CmdSequencerComponentImpl ::
      CS_STEP_cmdHandler(FwOpcodeType opcode, U32 cmdSeq)
    {
        if (this->requireRunMode(RUNNING)) {
            this->performCmd_Step();
            // check for special case where end of sequence entry was encountered
            if (this->m_runMode != STOPPED) {
                this->log_ACTIVITY_HI_CS_CmdStepped(
                    this->m_sequence->getLogFileName(),
                    this->m_executedCount
                );
            }
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    }

    void CmdSequencerComponentImpl ::
      CS_AUTO_cmdHandler(FwOpcodeType opcode, U32 cmdSeq)
    {
        if (this->requireRunMode(STOPPED)) {
            this->m_stepMode = AUTO;
            this->log_ACTIVITY_HI_CS_ModeSwitched(CmdSequencer_SeqMode::AUTO);
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    }

    void CmdSequencerComponentImpl ::
      CS_MANUAL_cmdHandler(FwOpcodeType opcode, U32 cmdSeq)
    {
        if (this->requireRunMode(STOPPED)) {
            this->m_stepMode = MANUAL;
            this->log_ACTIVITY_HI_CS_ModeSwitched(CmdSequencer_SeqMode::STEP);
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::OK);
        } else {
            this->cmdResponse_out(opcode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        }
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    bool CmdSequencerComponentImpl::requireRunMode(RunMode mode) {
        if (this->m_runMode == mode) {
            return true;
        } else {
            this->log_WARNING_HI_CS_InvalidMode();
            return false;
        }
    }

    void CmdSequencerComponentImpl ::
      commandError(
          const U32 number,
          const U32 opCode,
          const U32 error
      )
    {
        this->log_WARNING_HI_CS_CommandError(
            this->m_sequence->getLogFileName(),
            number,
            opCode,
            error
        );
        this->error();
    }

    void CmdSequencerComponentImpl::performCmd_Step() {

        this->m_sequence->nextRecord(m_record);
        // set clock time base and context from value set when sequence was loaded
        const Sequence::Header& header = this->m_sequence->getHeader();
        this->m_record.m_timeTag.setTimeBase(header.m_timeBase);
        this->m_record.m_timeTag.setTimeContext(header.m_timeContext);

        Fw::Time currentTime = this->getTime();
        switch (this->m_record.m_descriptor) {
          case Sequence::Record::END_OF_SEQUENCE:
                this->m_runMode = STOPPED;
                this->sequenceComplete();
                break;
          case Sequence::Record::RELATIVE:
                this->performCmd_Step_RELATIVE(currentTime);
                break;
          case Sequence::Record::ABSOLUTE:
                this->performCmd_Step_ABSOLUTE(currentTime);
                break;
          default:
                FW_ASSERT(0, m_record.m_descriptor);
        }
    }

    void CmdSequencerComponentImpl::sequenceComplete() {
        ++this->m_sequencesCompletedCount;
        // reset buffer
        this->m_sequence->clear();
        this->log_ACTIVITY_HI_CS_SequenceComplete(this->m_sequence->getLogFileName());
        this->tlmWrite_CS_SequencesCompleted(this->m_sequencesCompletedCount);
        this->m_executedCount = 0;
        // write sequence done port, if connected
        if (this->isConnected_seqDone_OutputPort(0)) {
            this->seqDone_out(0,0,0,Fw::CmdResponse::OK);
        }

        if (Svc::CmdSequencer_BlockState::BLOCK == this->m_blockState || m_join_waiting) {
            this->cmdResponse_out(this->m_opCode, this->m_cmdSeq, Fw::CmdResponse::OK);
        }

        m_join_waiting = false;
        this->m_blockState = Svc::CmdSequencer_BlockState::NO_BLOCK;

    }

    void CmdSequencerComponentImpl::commandComplete(const U32 opcode) {
        this->log_ACTIVITY_LO_CS_CommandComplete(
            this->m_sequence->getLogFileName(),
            this->m_executedCount,
            opcode
        );
        ++this->m_executedCount;
        ++this->m_totalExecutedCount;
        this->tlmWrite_CS_CommandsExecuted(this->m_totalExecutedCount);
    }

    void CmdSequencerComponentImpl ::
      performCmd_Step_RELATIVE(Fw::Time& currentTime)
    {
        this->m_record.m_timeTag.add(currentTime.getSeconds(),currentTime.getUSeconds());
        this->performCmd_Step_ABSOLUTE(currentTime);
    }

    void CmdSequencerComponentImpl ::
      performCmd_Step_ABSOLUTE(Fw::Time& currentTime)
    {
        if (currentTime >= this->m_record.m_timeTag) {
            this->comCmdOut_out(0, m_record.m_command, 0);
            this->setCmdTimeout(currentTime);
        } else {
            this->m_cmdTimer.set(this->m_record.m_timeTag);
        }
    }

    void CmdSequencerComponentImpl ::
      pingIn_handler(
          NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      )
    {
        // send ping response
        this->pingOut_out(0,key);
    }

    void CmdSequencerComponentImpl ::
      setCmdTimeout(const Fw::Time &currentTime)
    {
        // start timeout timer if enabled and not in step mode
        if ((this->m_timeout > 0) and (AUTO == this->m_stepMode)) {
            Fw::Time expTime = currentTime;
            expTime.add(this->m_timeout,0);
            this->m_cmdTimeoutTimer.set(expTime);
        }
    }

}


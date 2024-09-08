// ======================================================================
// \title  SeqDispatcher.cpp
// \author zimri.leisher
// \brief  cpp file for SeqDispatcher component implementation class
// ======================================================================

#include <Svc/SeqDispatcher/SeqDispatcher.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

SeqDispatcher ::SeqDispatcher(const char* const compName)
    : SeqDispatcherComponentBase(compName) {}

SeqDispatcher ::~SeqDispatcher() {}

FwIndexType SeqDispatcher::getNextAvailableSequencerIdx() {
  for (FwIndexType i = 0; i < SeqDispatcherSequencerPorts; i++) {
    if (this->isConnected_seqRunOut_OutputPort(i) && 
        this->m_entryTable[i].state == SeqDispatcher_CmdSequencerState::AVAILABLE) {
      return i;
    }
  }
  return -1;
}

void SeqDispatcher::runSequence(FwIndexType sequencerIdx,
                                const Fw::StringBase& fileName,
                                Fw::Wait block) {
  // this function is only designed for internal usage
  // we can guarantee it cannot be called with input that would fail
  FW_ASSERT(sequencerIdx >= 0 && sequencerIdx < SeqDispatcherSequencerPorts,
            sequencerIdx);
  FW_ASSERT(this->isConnected_seqRunOut_OutputPort(sequencerIdx));
  FW_ASSERT(this->m_entryTable[sequencerIdx].state == 
              SeqDispatcher_CmdSequencerState::AVAILABLE, 
            this->m_entryTable[sequencerIdx].state);

  if (block == Fw::Wait::NO_WAIT) {
    this->m_entryTable[sequencerIdx].state =
        SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_NO_BLOCK;
  } else {
    this->m_entryTable[sequencerIdx].state =
        SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK;
  }

  this->m_sequencersAvailable--;
  this->tlmWrite_sequencersAvailable(this->m_sequencersAvailable);
  this->m_entryTable[sequencerIdx].sequenceRunning = fileName;

  this->m_dispatchedCount++;
  this->tlmWrite_dispatchedCount(this->m_dispatchedCount);
  this->seqRunOut_out(sequencerIdx,
                      this->m_entryTable[sequencerIdx].sequenceRunning);
}

void SeqDispatcher::seqStartIn_handler(
    NATIVE_INT_TYPE portNum, //!< The port number
    const Fw::StringBase& fileName //!< The sequence file name
) {
  FW_ASSERT(portNum >= 0 && portNum < SeqDispatcherSequencerPorts, portNum);
  if (this->m_entryTable[portNum].state ==
          SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK ||
      this->m_entryTable[portNum].state ==
          SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_NO_BLOCK) {
    // we were aware of this sequencer running a sequence
    if (this->m_entryTable[portNum].sequenceRunning != fileName) {
      // uh oh. entry table is wrong
      // let's just update it to be correct. nothing we can do about
      // it except raise a warning and update our state
      this->log_WARNING_HI_ConflictingSequenceStarted(static_cast<U16>(portNum), fileName, this->m_entryTable[portNum].sequenceRunning);
      this->m_entryTable[portNum].sequenceRunning = fileName;
    }
  } else {
    // we were not aware that this sequencer was running. ground must have
    // directly commanded that specific sequencer

    // warn because this may be unintentional
    this->log_WARNING_LO_UnexpectedSequenceStarted(static_cast<U16>(portNum), fileName);

    // update the state
    this->m_entryTable[portNum].state =
        SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_NO_BLOCK;
    this->m_entryTable[portNum].sequenceRunning = fileName;
    this->m_sequencersAvailable--;
    this->tlmWrite_sequencersAvailable(this->m_sequencersAvailable);
  }
}

void SeqDispatcher::seqDoneIn_handler(
    NATIVE_INT_TYPE portNum,         //!< The port number
    FwOpcodeType opCode,             //!< Command Op Code
    U32 cmdSeq,                      //!< Command Sequence
    const Fw::CmdResponse& response  //!< The command response argument
) {
  FW_ASSERT(portNum >= 0 && portNum < SeqDispatcherSequencerPorts, portNum);
  if (this->m_entryTable[portNum].state !=
          SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK &&
      this->m_entryTable[portNum].state !=
          SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_NO_BLOCK) {
    // this sequencer was not running a sequence that we were aware of.

    // we should have caught this in seqStartIn and updated the state
    // accordingly, but somehow we didn't? very sad and shouldn't happen

    // anyways, don't have to do anything cuz now that this seq we didn't know
    // about is done, the sequencer is available again (which is its current
    // state in our internal entry table already)
    this->log_WARNING_LO_UnknownSequenceFinished(static_cast<U16>(portNum));
  } else {
    // ok, a sequence has finished that we knew about
    if (this->m_entryTable[portNum].state ==
        SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK) {
      // we need to give a cmd response cuz some other sequence is being blocked
      // by this
      this->cmdResponse_out(this->m_entryTable[portNum].opCode,
                            this->m_entryTable[portNum].cmdSeq, response);

      if (response == Fw::CmdResponse::EXECUTION_ERROR) {
        // dispatched sequence errored
        this->m_errorCount++;
        this->tlmWrite_errorCount(this->m_errorCount);
      }
    }
  }

  // all command responses mean the sequence is no longer running
  // so component should be available
  this->m_entryTable[portNum].state = SeqDispatcher_CmdSequencerState::AVAILABLE;
  this->m_entryTable[portNum].sequenceRunning = "<no seq>";
  this->m_sequencersAvailable++;
  this->tlmWrite_sequencersAvailable(this->m_sequencersAvailable);
}

//! Handler for input port seqRunIn
void SeqDispatcher::seqRunIn_handler(NATIVE_INT_TYPE portNum,
                                     const Fw::StringBase& fileName) {
  FwIndexType idx = this->getNextAvailableSequencerIdx();
  // no available sequencers
  if (idx == -1) {
    this->log_WARNING_HI_NoAvailableSequencers();
    return;
  }

  this->runSequence(idx, fileName, Fw::Wait::NO_WAIT);
}
// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void SeqDispatcher ::RUN_cmdHandler(const FwOpcodeType opCode,
                                    const U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    Fw::Wait block) {
  FwIndexType idx = this->getNextAvailableSequencerIdx();
  // no available sequencers
  if (idx == -1) {
    this->log_WARNING_HI_NoAvailableSequencers();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    return;
  }

  this->runSequence(idx, fileName, block);

  if (block == Fw::Wait::NO_WAIT) {
    // return instantly
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  } else {
    // otherwise don't return a response yet. just save the opCode and cmdSeq
    // so we can return a response later
    this->m_entryTable[idx].opCode = opCode;
    this->m_entryTable[idx].cmdSeq = cmdSeq;
  }
}

void SeqDispatcher::LOG_STATUS_cmdHandler(
    const FwOpcodeType opCode,          /*!< The opcode*/
    const U32 cmdSeq) {                   /*!< The command sequence number*/
  for(FwIndexType idx = 0; idx < SeqDispatcherSequencerPorts; idx++) {
    this->log_ACTIVITY_LO_LogSequencerStatus(static_cast<U16>(idx), this->m_entryTable[idx].state, Fw::LogStringArg(this->m_entryTable[idx].sequenceRunning));
  }
}
}  // end namespace components

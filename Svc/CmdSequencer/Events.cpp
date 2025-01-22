// ======================================================================
// \title  Events.cpp
// \author Bocchino
// \brief  Implementation for CmdSequencerComponentImpl::Sequence::Events
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  CmdSequencerComponentImpl::Sequence::Events ::
    Events(Sequence& sequence) :
      m_sequence(sequence)
  {

  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileCRCFailure(const U32 storedCRC, const U32 computedCRC)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileCrcFailure(
        logFileName,
        storedCRC,
        computedCRC
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileInvalid(const CmdSequencer_FileReadStage::t stage, const I32 error)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileInvalid(
        logFileName,
        stage,
        error
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileNotFound()
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileNotFound(logFileName);
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileReadError()
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileReadError(logFileName);
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileSizeError(const U32 size)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileSizeError(
        logFileName,
        size
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    recordInvalid(const U32 recordNumber, const I32 error)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_RecordInvalid(
        logFileName,
        recordNumber,
        error
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    recordMismatch(const U32 numRecords, const U32 extraBytes)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_RecordMismatch(
        logFileName,
        numRecords,
        extraBytes
    );
    // TODO: Should this be an error?
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    timeBaseMismatch(const TimeBase currTimeBase, const TimeBase seqTimeBase)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_TimeBaseMismatch(
        logFileName,
        currTimeBase,
        seqTimeBase
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    timeContextMismatch(
        const FwTimeContextStoreType currTimeContext,
        const FwTimeContextStoreType seqTimeContext
    )
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_TimeContextMismatch(
        logFileName,
        currTimeContext,
        seqTimeContext
    );
    component.error();
  }
  
  void CmdSequencerComponentImpl::Sequence::Events ::
    noRecords()
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_LO_CS_NoRecords(
      logFileName
    );
    component.error();
  }

}


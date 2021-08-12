// ====================================================================== 
// \title  Events.cpp
// \author Bocchino
// \brief  Implementation for CmdSequencerComponentImpl::Sequence::Events
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Types/Assert.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  CmdSequencerComponentImpl::FileReadStage
    CmdSequencerComponentImpl::Sequence::Events::FileReadStage ::
      toComponentEnum(const t fileReadStage)
  {
    CmdSequencerComponentImpl::FileReadStage result =
      static_cast<CmdSequencerComponentImpl::FileReadStage>(0);
    FW_ASSERT(result >= 0, result);
    FW_ASSERT(result < CmdSequencerComponentImpl::FileReadStage_MAX, result);
    switch(fileReadStage) {
      case READ_HEADER:
        result = CmdSequencerComponentImpl::SEQ_READ_HEADER;
        break;
      case READ_HEADER_SIZE:
        result = CmdSequencerComponentImpl::SEQ_READ_HEADER_SIZE;
        break;
      case DESER_SIZE:
        result = CmdSequencerComponentImpl::SEQ_DESER_SIZE;
        break;
      case DESER_NUM_RECORDS:
        result = CmdSequencerComponentImpl::SEQ_DESER_NUM_RECORDS;
        break;
      case DESER_TIME_BASE:
        result = CmdSequencerComponentImpl::SEQ_DESER_TIME_BASE;
        break;
      case DESER_TIME_CONTEXT:
        result = CmdSequencerComponentImpl::SEQ_DESER_TIME_CONTEXT;
        break;
      case READ_SEQ_CRC:
        result = CmdSequencerComponentImpl::SEQ_READ_SEQ_CRC;
        break;
      case READ_SEQ_DATA:
        result = CmdSequencerComponentImpl::SEQ_READ_SEQ_DATA;
        break;
      case READ_SEQ_DATA_SIZE:
        result = CmdSequencerComponentImpl::SEQ_READ_SEQ_DATA_SIZE;
        break;
      default:
        FW_ASSERT(0, fileReadStage);
        break;
    }
    return result;
  }

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
    fileInvalid(const FileReadStage::t stage, const I32 error)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileInvalid(
        logFileName,
        FileReadStage::toComponentEnum(stage),
        error
    );
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileNotFound(void)
  {
    Fw::LogStringArg& logFileName = this->m_sequence.getLogFileName();
    CmdSequencerComponentImpl& component = this->m_sequence.m_component;
    component.log_WARNING_HI_CS_FileNotFound(logFileName);
    component.error();
  }

  void CmdSequencerComponentImpl::Sequence::Events ::
    fileReadError(void)
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
    timeBaseMismatch(const U32 currTimeBase, const U32 seqTimeBase)
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
        const U32 currTimeContext,
        const U32 seqTimeContext
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

}


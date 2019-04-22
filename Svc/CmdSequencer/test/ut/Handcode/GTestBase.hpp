// ======================================================================
// \title  CmdSequencer/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for CmdSequencer component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CmdSequencer_GTEST_BASE_HPP
#define CmdSequencer_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for command history assertions
// ----------------------------------------------------------------------

#define ASSERT_CMD_RESPONSE_SIZE(size) \
  this->assertCmdResponse_size(__FILE__, __LINE__, size)

#define ASSERT_CMD_RESPONSE(index, opCode, cmdSeq, response) \
  this->assertCmdResponse(__FILE__, __LINE__, index, opCode, cmdSeq, response)

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_LoadCommands_SIZE(size) \
  this->assertTlm_CS_LoadCommands_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_LoadCommands(index, value) \
  this->assertTlm_CS_LoadCommands(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_CS_CancelCommands_SIZE(size) \
  this->assertTlm_CS_CancelCommands_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_CancelCommands(index, value) \
  this->assertTlm_CS_CancelCommands(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_CS_Errors_SIZE(size) \
  this->assertTlm_CS_Errors_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_Errors(index, value) \
  this->assertTlm_CS_Errors(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_CS_CommandsExecuted_SIZE(size) \
  this->assertTlm_CS_CommandsExecuted_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_CommandsExecuted(index, value) \
  this->assertTlm_CS_CommandsExecuted(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_CS_SequencesCompleted_SIZE(size) \
  this->assertTlm_CS_SequencesCompleted_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CS_SequencesCompleted(index, value) \
  this->assertTlm_CS_SequencesCompleted(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceLoaded_SIZE(size) \
  this->assertEvents_CS_SequenceLoaded_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceLoaded(index, _fileName) \
  this->assertEvents_CS_SequenceLoaded(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_CS_SequenceCanceled_SIZE(size) \
  this->assertEvents_CS_SequenceCanceled_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceCanceled(index, _fileName) \
  this->assertEvents_CS_SequenceCanceled(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_CS_FileReadError_SIZE(size) \
  this->assertEvents_CS_FileReadError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_FileReadError(index, _fileName) \
  this->assertEvents_CS_FileReadError(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_CS_FileInvalid_SIZE(size) \
  this->assertEvents_CS_FileInvalid_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_FileInvalid(index, _fileName, _stage, _error) \
  this->assertEvents_CS_FileInvalid(__FILE__, __LINE__, index, _fileName, _stage, _error)

#define ASSERT_EVENTS_CS_RecordInvalid_SIZE(size) \
  this->assertEvents_CS_RecordInvalid_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_RecordInvalid(index, _fileName, _recordNumber, _error) \
  this->assertEvents_CS_RecordInvalid(__FILE__, __LINE__, index, _fileName, _recordNumber, _error)

#define ASSERT_EVENTS_CS_FileSizeError_SIZE(size) \
  this->assertEvents_CS_FileSizeError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_FileSizeError(index, _fileName, _size) \
  this->assertEvents_CS_FileSizeError(__FILE__, __LINE__, index, _fileName, _size)

#define ASSERT_EVENTS_CS_FileNotFound_SIZE(size) \
  this->assertEvents_CS_FileNotFound_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_FileNotFound(index, _fileName) \
  this->assertEvents_CS_FileNotFound(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_CS_FileCrcFailure_SIZE(size) \
  this->assertEvents_CS_FileCrcFailure_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_FileCrcFailure(index, _fileName, _storedCRC, _computedCRC) \
  this->assertEvents_CS_FileCrcFailure(__FILE__, __LINE__, index, _fileName, _storedCRC, _computedCRC)

#define ASSERT_EVENTS_CS_CommandComplete_SIZE(size) \
  this->assertEvents_CS_CommandComplete_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_CommandComplete(index, _fileName, _recordNumber, _opCode) \
  this->assertEvents_CS_CommandComplete(__FILE__, __LINE__, index, _fileName, _recordNumber, _opCode)

#define ASSERT_EVENTS_CS_SequenceComplete_SIZE(size) \
  this->assertEvents_CS_SequenceComplete_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceComplete(index, _fileName) \
  this->assertEvents_CS_SequenceComplete(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_CS_CommandError_SIZE(size) \
  this->assertEvents_CS_CommandError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_CommandError(index, _fileName, _recordNumber, _opCode, _errorStatus) \
  this->assertEvents_CS_CommandError(__FILE__, __LINE__, index, _fileName, _recordNumber, _opCode, _errorStatus)

#define ASSERT_EVENTS_CS_InvalidMode_SIZE(size) \
  this->assertEvents_CS_InvalidMode_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_RecordMismatch_SIZE(size) \
  this->assertEvents_CS_RecordMismatch_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_RecordMismatch(index, _fileName, _header_records, _extra_bytes) \
  this->assertEvents_CS_RecordMismatch(__FILE__, __LINE__, index, _fileName, _header_records, _extra_bytes)

#define ASSERT_EVENTS_CS_TimeBaseMismatch_SIZE(size) \
  this->assertEvents_CS_TimeBaseMismatch_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_TimeBaseMismatch(index, _fileName, _time_base, _seq_time_base) \
  this->assertEvents_CS_TimeBaseMismatch(__FILE__, __LINE__, index, _fileName, _time_base, _seq_time_base)

#define ASSERT_EVENTS_CS_TimeContextMismatch_SIZE(size) \
  this->assertEvents_CS_TimeContextMismatch_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_TimeContextMismatch(index, _fileName, _currTimeBase, _seqTimeBase) \
  this->assertEvents_CS_TimeContextMismatch(__FILE__, __LINE__, index, _fileName, _currTimeBase, _seqTimeBase)

#define ASSERT_EVENTS_CS_PortSequenceStarted_SIZE(size) \
  this->assertEvents_CS_PortSequenceStarted_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_PortSequenceStarted(index, _filename) \
  this->assertEvents_CS_PortSequenceStarted(__FILE__, __LINE__, index, _filename)

#define ASSERT_EVENTS_CS_UnexpectedCompletion_SIZE(size) \
  this->assertEvents_CS_UnexpectedCompletion_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_UnexpectedCompletion(index, _opcode) \
  this->assertEvents_CS_UnexpectedCompletion(__FILE__, __LINE__, index, _opcode)

#define ASSERT_EVENTS_CS_ModeSwitched_SIZE(size) \
  this->assertEvents_CS_ModeSwitched_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_ModeSwitched(index, _mode) \
  this->assertEvents_CS_ModeSwitched(__FILE__, __LINE__, index, _mode)

#define ASSERT_EVENTS_CS_NoSequenceActive_SIZE(size) \
  this->assertEvents_CS_NoSequenceActive_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceValid_SIZE(size) \
  this->assertEvents_CS_SequenceValid_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceValid(index, _filename) \
  this->assertEvents_CS_SequenceValid(__FILE__, __LINE__, index, _filename)

#define ASSERT_EVENTS_CS_SequenceTimeout_SIZE(size) \
  this->assertEvents_CS_SequenceTimeout_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_SequenceTimeout(index, _filename, _command) \
  this->assertEvents_CS_SequenceTimeout(__FILE__, __LINE__, index, _filename, _command)

#define ASSERT_EVENTS_CS_CmdStepped_SIZE(size) \
  this->assertEvents_CS_CmdStepped_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_CmdStepped(index, _filename, _command) \
  this->assertEvents_CS_CmdStepped(__FILE__, __LINE__, index, _filename, _command)

#define ASSERT_EVENTS_CS_CmdStarted_SIZE(size) \
  this->assertEvents_CS_CmdStarted_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_CS_CmdStarted(index, _filename) \
  this->assertEvents_CS_CmdStarted(__FILE__, __LINE__, index, _filename)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_comCmdOut_SIZE(size) \
  this->assert_from_comCmdOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_comCmdOut(index, _data, _context) \
  { \
    ASSERT_GT(this->fromPortHistory_comCmdOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_comCmdOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_comCmdOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_comCmdOut& _e = \
      this->fromPortHistory_comCmdOut->at(index); \
    ASSERT_EQ(_data, _e.data) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument data at index " \
    << index \
    << " in history of from_comCmdOut\n" \
    << "  Expected: " << _data << "\n" \
    << "  Actual:   " << _e.data << "\n"; \
    ASSERT_EQ(_context, _e.context) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument context at index " \
    << index \
    << " in history of from_comCmdOut\n" \
    << "  Expected: " << _context << "\n" \
    << "  Actual:   " << _e.context << "\n"; \
  }

#define ASSERT_from_pingOut_SIZE(size) \
  this->assert_from_pingOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_pingOut(index, _key) \
  { \
    ASSERT_GT(this->fromPortHistory_pingOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_pingOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_pingOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_pingOut& _e = \
      this->fromPortHistory_pingOut->at(index); \
    ASSERT_EQ(_key, _e.key) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument key at index " \
    << index \
    << " in history of from_pingOut\n" \
    << "  Expected: " << _key << "\n" \
    << "  Actual:   " << _e.key << "\n"; \
  }

#define ASSERT_from_seqDone_SIZE(size) \
  this->assert_from_seqDone_size(__FILE__, __LINE__, size)

#define ASSERT_from_seqDone(index, _opCode, _cmdSeq, _response) \
  { \
    ASSERT_GT(this->fromPortHistory_seqDone->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_seqDone\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_seqDone->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_seqDone& _e = \
      this->fromPortHistory_seqDone->at(index); \
    ASSERT_EQ(_opCode, _e.opCode) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument opCode at index " \
    << index \
    << " in history of from_seqDone\n" \
    << "  Expected: " << _opCode << "\n" \
    << "  Actual:   " << _e.opCode << "\n"; \
    ASSERT_EQ(_cmdSeq, _e.cmdSeq) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cmdSeq at index " \
    << index \
    << " in history of from_seqDone\n" \
    << "  Expected: " << _cmdSeq << "\n" \
    << "  Actual:   " << _e.cmdSeq << "\n"; \
    ASSERT_EQ(_response, _e.response) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument response at index " \
    << index \
    << " in history of from_seqDone\n" \
    << "  Expected: " << _response << "\n" \
    << "  Actual:   " << _e.response << "\n"; \
  }

namespace Svc {

  //! \class CmdSequencerGTestBase
  //! \brief Auto-generated base class for CmdSequencer component Google Test harness
  //!
  class CmdSequencerGTestBase :
    public CmdSequencerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CmdSequencerGTestBase
      //!
      CmdSequencerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CmdSequencerGTestBase
      //!
      virtual ~CmdSequencerGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Commands
      // ----------------------------------------------------------------------

      //! Assert size of command response history
      //!
      void assertCmdResponse_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      //! Assert command response in history at index
      //!
      void assertCmdResponse(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CommandResponse response /*!< The command response*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_LoadCommands
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CS_LoadCommands_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CS_LoadCommands(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_CancelCommands
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CS_CancelCommands_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CS_CancelCommands(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_Errors
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CS_Errors_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CS_Errors(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_CommandsExecuted
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CS_CommandsExecuted_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CS_CommandsExecuted(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CS_SequencesCompleted
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CS_SequencesCompleted_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CS_SequencesCompleted(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceLoaded
      // ----------------------------------------------------------------------

      void assertEvents_CS_SequenceLoaded_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_SequenceLoaded(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceCanceled
      // ----------------------------------------------------------------------

      void assertEvents_CS_SequenceCanceled_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_SequenceCanceled(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileReadError
      // ----------------------------------------------------------------------

      void assertEvents_CS_FileReadError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_FileReadError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileInvalid
      // ----------------------------------------------------------------------

      void assertEvents_CS_FileInvalid_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_FileInvalid(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          CmdSequencerComponentBase::FileReadStage stage, /*!< The read stage*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_RecordInvalid
      // ----------------------------------------------------------------------

      void assertEvents_CS_RecordInvalid_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_RecordInvalid(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U32 recordNumber, /*!< The record number*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileSizeError
      // ----------------------------------------------------------------------

      void assertEvents_CS_FileSizeError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_FileSizeError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U32 size /*!< Inavalid size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileNotFound
      // ----------------------------------------------------------------------

      void assertEvents_CS_FileNotFound_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_FileNotFound(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_FileCrcFailure
      // ----------------------------------------------------------------------

      void assertEvents_CS_FileCrcFailure_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_FileCrcFailure(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The sequence file*/
          const U32 storedCRC, /*!< The CRC stored in the file*/
          const U32 computedCRC /*!< The CRC computed over the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CommandComplete
      // ----------------------------------------------------------------------

      void assertEvents_CS_CommandComplete_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_CommandComplete(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U32 recordNumber, /*!< The record number of the command*/
          const U32 opCode /*!< The command opcode*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceComplete
      // ----------------------------------------------------------------------

      void assertEvents_CS_SequenceComplete_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_SequenceComplete(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CommandError
      // ----------------------------------------------------------------------

      void assertEvents_CS_CommandError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_CommandError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U32 recordNumber, /*!< The record number*/
          const U32 opCode, /*!< The opcode*/
          const U32 errorStatus /*!< The error status*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_InvalidMode
      // ----------------------------------------------------------------------

      void assertEvents_CS_InvalidMode_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_RecordMismatch
      // ----------------------------------------------------------------------

      void assertEvents_CS_RecordMismatch_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_RecordMismatch(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U32 header_records, /*!< The number of records in the header*/
          const U32 extra_bytes /*!< The number of bytes beyond last record*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_TimeBaseMismatch
      // ----------------------------------------------------------------------

      void assertEvents_CS_TimeBaseMismatch_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_TimeBaseMismatch(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U16 time_base, /*!< The current time*/
          const U16 seq_time_base /*!< The sequence time base*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_TimeContextMismatch
      // ----------------------------------------------------------------------

      void assertEvents_CS_TimeContextMismatch_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_TimeContextMismatch(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the sequence file*/
          const U8 currTimeBase, /*!< The current time base*/
          const U8 seqTimeBase /*!< The sequence time base*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_PortSequenceStarted
      // ----------------------------------------------------------------------

      void assertEvents_CS_PortSequenceStarted_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_PortSequenceStarted(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const filename /*!< The sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_UnexpectedCompletion
      // ----------------------------------------------------------------------

      void assertEvents_CS_UnexpectedCompletion_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_UnexpectedCompletion(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 opcode /*!< The reported opcode*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_ModeSwitched
      // ----------------------------------------------------------------------

      void assertEvents_CS_ModeSwitched_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_ModeSwitched(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          CmdSequencerComponentBase::SeqMode mode /*!< The new mode*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_NoSequenceActive
      // ----------------------------------------------------------------------

      void assertEvents_CS_NoSequenceActive_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceValid
      // ----------------------------------------------------------------------

      void assertEvents_CS_SequenceValid_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_SequenceValid(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const filename /*!< The sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_SequenceTimeout
      // ----------------------------------------------------------------------

      void assertEvents_CS_SequenceTimeout_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_SequenceTimeout(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const filename, /*!< The sequence file*/
          const U32 command /*!< The command that timed out*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CmdStepped
      // ----------------------------------------------------------------------

      void assertEvents_CS_CmdStepped_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_CmdStepped(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const filename, /*!< The sequence file*/
          const U32 command /*!< The command that was stepped*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: CS_CmdStarted
      // ----------------------------------------------------------------------

      void assertEvents_CS_CmdStarted_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_CS_CmdStarted(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const filename /*!< The sequence file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From ports 
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: comCmdOut 
      // ----------------------------------------------------------------------

      void assert_from_comCmdOut_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: pingOut 
      // ----------------------------------------------------------------------

      void assert_from_pingOut_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: seqDone 
      // ----------------------------------------------------------------------

      void assert_from_seqDone_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif

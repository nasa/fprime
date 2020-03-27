// ======================================================================
// \title  ActiveLogger/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for ActiveLogger component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  ActiveLoggerGTestBase ::
    ActiveLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        ActiveLoggerTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  ActiveLoggerGTestBase ::
    ~ActiveLoggerGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertCmdResponse_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ((unsigned long) size, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of command response history\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertCmdResponse(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history (" 
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << index
      << " in command response history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_FILE_WRITE_ERR
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_FILE_WRITE_ERR_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_FILE_WRITE_ERR->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_FILE_WRITE_ERR\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_FILE_WRITE_ERR->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_FILE_WRITE_ERR(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        ActiveLoggerComponentBase::LogWriteError stage,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_FILE_WRITE_ERR->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_FILE_WRITE_ERR\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_FILE_WRITE_ERR->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_FILE_WRITE_ERR& e =
      this->eventHistory_ALOG_FILE_WRITE_ERR->at(index);
    ASSERT_EQ(stage, e.stage)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument stage at index "
      << index
      << " in history of event ALOG_FILE_WRITE_ERR\n"
      << "  Expected: " << stage << "\n"
      << "  Actual:   " << e.stage << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event ALOG_FILE_WRITE_ERR\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_FILE_WRITE_COMPLETE
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_FILE_WRITE_COMPLETE_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_FILE_WRITE_COMPLETE->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_FILE_WRITE_COMPLETE\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_FILE_WRITE_COMPLETE->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_FILE_WRITE_COMPLETE(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 records
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_FILE_WRITE_COMPLETE->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_FILE_WRITE_COMPLETE\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_FILE_WRITE_COMPLETE->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_FILE_WRITE_COMPLETE& e =
      this->eventHistory_ALOG_FILE_WRITE_COMPLETE->at(index);
    ASSERT_EQ(records, e.records)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument records at index "
      << index
      << " in history of event ALOG_FILE_WRITE_COMPLETE\n"
      << "  Expected: " << records << "\n"
      << "  Actual:   " << e.records << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_SEVERITY_FILTER_STATE
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_SEVERITY_FILTER_STATE_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_SEVERITY_FILTER_STATE->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_SEVERITY_FILTER_STATE\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_SEVERITY_FILTER_STATE->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_SEVERITY_FILTER_STATE(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        ActiveLoggerComponentBase::EventFilterState severity,
        const bool recvEnabled,
        const bool sendEnabled
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_SEVERITY_FILTER_STATE->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_SEVERITY_FILTER_STATE\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_SEVERITY_FILTER_STATE->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_SEVERITY_FILTER_STATE& e =
      this->eventHistory_ALOG_SEVERITY_FILTER_STATE->at(index);
    ASSERT_EQ(severity, e.severity)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument severity at index "
      << index
      << " in history of event ALOG_SEVERITY_FILTER_STATE\n"
      << "  Expected: " << severity << "\n"
      << "  Actual:   " << e.severity << "\n";
    ASSERT_EQ(recvEnabled, e.recvEnabled)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument recvEnabled at index "
      << index
      << " in history of event ALOG_SEVERITY_FILTER_STATE\n"
      << "  Expected: " << recvEnabled << "\n"
      << "  Actual:   " << e.recvEnabled << "\n";
    ASSERT_EQ(sendEnabled, e.sendEnabled)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument sendEnabled at index "
      << index
      << " in history of event ALOG_SEVERITY_FILTER_STATE\n"
      << "  Expected: " << sendEnabled << "\n"
      << "  Actual:   " << e.sendEnabled << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_ENABLED
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_ENABLED_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_ID_FILTER_ENABLED->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_ID_FILTER_ENABLED\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_ID_FILTER_ENABLED->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_ENABLED(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 ID
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_ID_FILTER_ENABLED->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_ID_FILTER_ENABLED\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_ID_FILTER_ENABLED->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_ID_FILTER_ENABLED& e =
      this->eventHistory_ALOG_ID_FILTER_ENABLED->at(index);
    ASSERT_EQ(ID, e.ID)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument ID at index "
      << index
      << " in history of event ALOG_ID_FILTER_ENABLED\n"
      << "  Expected: " << ID << "\n"
      << "  Actual:   " << e.ID << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_LIST_FULL
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_LIST_FULL_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_ID_FILTER_LIST_FULL->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_ID_FILTER_LIST_FULL\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_ID_FILTER_LIST_FULL->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_LIST_FULL(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 ID
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_ID_FILTER_LIST_FULL->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_ID_FILTER_LIST_FULL\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_ID_FILTER_LIST_FULL->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_ID_FILTER_LIST_FULL& e =
      this->eventHistory_ALOG_ID_FILTER_LIST_FULL->at(index);
    ASSERT_EQ(ID, e.ID)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument ID at index "
      << index
      << " in history of event ALOG_ID_FILTER_LIST_FULL\n"
      << "  Expected: " << ID << "\n"
      << "  Actual:   " << e.ID << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_REMOVED
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_REMOVED_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_ID_FILTER_REMOVED->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_ID_FILTER_REMOVED\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_ID_FILTER_REMOVED->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_REMOVED(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 ID
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_ID_FILTER_REMOVED->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_ID_FILTER_REMOVED\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_ID_FILTER_REMOVED->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_ID_FILTER_REMOVED& e =
      this->eventHistory_ALOG_ID_FILTER_REMOVED->at(index);
    ASSERT_EQ(ID, e.ID)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument ID at index "
      << index
      << " in history of event ALOG_ID_FILTER_REMOVED\n"
      << "  Expected: " << ID << "\n"
      << "  Actual:   " << e.ID << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_NOT_FOUND
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_NOT_FOUND_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ALOG_ID_FILTER_NOT_FOUND\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->size() << "\n";
  }

  void ActiveLoggerGTestBase ::
    assertEvents_ALOG_ID_FILTER_NOT_FOUND(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 ID
    ) const
  {
    ASSERT_GT(this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ALOG_ID_FILTER_NOT_FOUND\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ALOG_ID_FILTER_NOT_FOUND& e =
      this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->at(index);
    ASSERT_EQ(ID, e.ID)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument ID at index "
      << index
      << " in history of event ALOG_ID_FILTER_NOT_FOUND\n"
      << "  Expected: " << ID << "\n"
      << "  Actual:   " << e.ID << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assertFromPortHistory_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: PktSend
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assert_from_PktSend_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_PktSend->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_PktSend\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_PktSend->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: FatalAnnounce
  // ----------------------------------------------------------------------

  void ActiveLoggerGTestBase ::
    assert_from_FatalAnnounce_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_FatalAnnounce->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_FatalAnnounce\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_FatalAnnounce->size() << "\n";
  }

} // end namespace Svc

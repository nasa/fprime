// ======================================================================
// \title  Health/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for Health component Google Test harness base class
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

  HealthGTestBase ::
    HealthGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        HealthTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  HealthGTestBase ::
    ~HealthGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
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

  void HealthGTestBase ::
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
  // Telemetry
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertTlm_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: PingLateWarnings
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertTlm_PingLateWarnings_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_PingLateWarnings->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel PingLateWarnings\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_PingLateWarnings->size() << "\n";
  }

  void HealthGTestBase ::
    assertTlm_PingLateWarnings(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_PingLateWarnings->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel PingLateWarnings\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_PingLateWarnings->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_PingLateWarnings& e =
      this->tlmHistory_PingLateWarnings->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel PingLateWarnings\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
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
  // Event: HLTH_PING_WARN
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_PING_WARN_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_PING_WARN->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_PING_WARN\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_PING_WARN->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_PING_WARN(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_PING_WARN->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_PING_WARN\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_PING_WARN->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_PING_WARN& e =
      this->eventHistory_HLTH_PING_WARN->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_PING_WARN\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_PING_LATE
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_PING_LATE_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_PING_LATE->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_PING_LATE\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_PING_LATE->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_PING_LATE(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_PING_LATE->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_PING_LATE\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_PING_LATE->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_PING_LATE& e =
      this->eventHistory_HLTH_PING_LATE->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_PING_LATE\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_PING_WRONG_KEY
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_PING_WRONG_KEY_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_PING_WRONG_KEY->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_PING_WRONG_KEY\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_PING_WRONG_KEY->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_PING_WRONG_KEY(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry,
        const U32 badKey
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_PING_WRONG_KEY->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_PING_WRONG_KEY\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_PING_WRONG_KEY->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_PING_WRONG_KEY& e =
      this->eventHistory_HLTH_PING_WRONG_KEY->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_PING_WRONG_KEY\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
    ASSERT_EQ(badKey, e.badKey)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument badKey at index "
      << index
      << " in history of event HLTH_PING_WRONG_KEY\n"
      << "  Expected: " << badKey << "\n"
      << "  Actual:   " << e.badKey << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_CHECK_ENABLE
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_ENABLE_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_CHECK_ENABLE->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_CHECK_ENABLE\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_CHECK_ENABLE->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_ENABLE(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        HealthComponentBase::HealthIsEnabled enabled
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_CHECK_ENABLE->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_CHECK_ENABLE\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_CHECK_ENABLE->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_CHECK_ENABLE& e =
      this->eventHistory_HLTH_CHECK_ENABLE->at(index);
    ASSERT_EQ(enabled, e.enabled)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument enabled at index "
      << index
      << " in history of event HLTH_CHECK_ENABLE\n"
      << "  Expected: " << enabled << "\n"
      << "  Actual:   " << e.enabled << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_CHECK_PING
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_PING_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_CHECK_PING->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_CHECK_PING\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_CHECK_PING->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_PING(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        HealthComponentBase::HealthPingIsEnabled enabled,
        const char *const entry
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_CHECK_PING->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_CHECK_PING\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_CHECK_PING->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_CHECK_PING& e =
      this->eventHistory_HLTH_CHECK_PING->at(index);
    ASSERT_EQ(enabled, e.enabled)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument enabled at index "
      << index
      << " in history of event HLTH_CHECK_PING\n"
      << "  Expected: " << enabled << "\n"
      << "  Actual:   " << e.enabled << "\n";
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_CHECK_PING\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_CHECK_LOOKUP_ERROR
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_LOOKUP_ERROR_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_CHECK_LOOKUP_ERROR->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_CHECK_LOOKUP_ERROR\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_CHECK_LOOKUP_ERROR->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_CHECK_LOOKUP_ERROR(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_CHECK_LOOKUP_ERROR->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_CHECK_LOOKUP_ERROR\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_CHECK_LOOKUP_ERROR->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_CHECK_LOOKUP_ERROR& e =
      this->eventHistory_HLTH_CHECK_LOOKUP_ERROR->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_CHECK_LOOKUP_ERROR\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_PING_UPDATED
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_PING_UPDATED_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_PING_UPDATED->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_PING_UPDATED\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_PING_UPDATED->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_PING_UPDATED(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry,
        const U32 warn,
        const U32 fatal
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_PING_UPDATED->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_PING_UPDATED\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_PING_UPDATED->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_PING_UPDATED& e =
      this->eventHistory_HLTH_PING_UPDATED->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_PING_UPDATED\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
    ASSERT_EQ(warn, e.warn)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument warn at index "
      << index
      << " in history of event HLTH_PING_UPDATED\n"
      << "  Expected: " << warn << "\n"
      << "  Actual:   " << e.warn << "\n";
    ASSERT_EQ(fatal, e.fatal)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument fatal at index "
      << index
      << " in history of event HLTH_PING_UPDATED\n"
      << "  Expected: " << fatal << "\n"
      << "  Actual:   " << e.fatal << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: HLTH_PING_INVALID_VALUES
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assertEvents_HLTH_PING_INVALID_VALUES_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_HLTH_PING_INVALID_VALUES->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event HLTH_PING_INVALID_VALUES\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_HLTH_PING_INVALID_VALUES->size() << "\n";
  }

  void HealthGTestBase ::
    assertEvents_HLTH_PING_INVALID_VALUES(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const entry,
        const U32 warn,
        const U32 fatal
    ) const
  {
    ASSERT_GT(this->eventHistory_HLTH_PING_INVALID_VALUES->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event HLTH_PING_INVALID_VALUES\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_HLTH_PING_INVALID_VALUES->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_HLTH_PING_INVALID_VALUES& e =
      this->eventHistory_HLTH_PING_INVALID_VALUES->at(index);
    ASSERT_STREQ(entry, e.entry.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument entry at index "
      << index
      << " in history of event HLTH_PING_INVALID_VALUES\n"
      << "  Expected: " << entry << "\n"
      << "  Actual:   " << e.entry.toChar() << "\n";
    ASSERT_EQ(warn, e.warn)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument warn at index "
      << index
      << " in history of event HLTH_PING_INVALID_VALUES\n"
      << "  Expected: " << warn << "\n"
      << "  Actual:   " << e.warn << "\n";
    ASSERT_EQ(fatal, e.fatal)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument fatal at index "
      << index
      << " in history of event HLTH_PING_INVALID_VALUES\n"
      << "  Expected: " << fatal << "\n"
      << "  Actual:   " << e.fatal << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
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
  // From port: PingSend
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assert_from_PingSend_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_PingSend->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_PingSend\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_PingSend->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: WdogStroke
  // ----------------------------------------------------------------------

  void HealthGTestBase ::
    assert_from_WdogStroke_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_WdogStroke->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_WdogStroke\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_WdogStroke->size() << "\n";
  }

} // end namespace Svc

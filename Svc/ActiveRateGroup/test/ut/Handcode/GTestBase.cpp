// ======================================================================
// \title  ActiveRateGroup/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for ActiveRateGroup component Google Test harness base class
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

  ActiveRateGroupGTestBase ::
    ActiveRateGroupGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        ActiveRateGroupTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  ActiveRateGroupGTestBase ::
    ~ActiveRateGroupGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
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
  // Channel: RgMaxTime
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assertTlm_RgMaxTime_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_RgMaxTime->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel RgMaxTime\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_RgMaxTime->size() << "\n";
  }

  void ActiveRateGroupGTestBase ::
    assertTlm_RgMaxTime(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_RgMaxTime->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel RgMaxTime\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_RgMaxTime->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_RgMaxTime& e =
      this->tlmHistory_RgMaxTime->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel RgMaxTime\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: RgCycleSlips
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assertTlm_RgCycleSlips_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_RgCycleSlips->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel RgCycleSlips\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_RgCycleSlips->size() << "\n";
  }

  void ActiveRateGroupGTestBase ::
    assertTlm_RgCycleSlips(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_RgCycleSlips->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel RgCycleSlips\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_RgCycleSlips->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_RgCycleSlips& e =
      this->tlmHistory_RgCycleSlips->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel RgCycleSlips\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
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
  // Event: RateGroupStarted
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assertEvents_RateGroupStarted_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_RateGroupStarted)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event RateGroupStarted\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_RateGroupStarted << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: RateGroupCycleSlip
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assertEvents_RateGroupCycleSlip_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_RateGroupCycleSlip->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event RateGroupCycleSlip\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_RateGroupCycleSlip->size() << "\n";
  }

  void ActiveRateGroupGTestBase ::
    assertEvents_RateGroupCycleSlip(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 cycle
    ) const
  {
    ASSERT_GT(this->eventHistory_RateGroupCycleSlip->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event RateGroupCycleSlip\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_RateGroupCycleSlip->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_RateGroupCycleSlip& e =
      this->eventHistory_RateGroupCycleSlip->at(index);
    ASSERT_EQ(cycle, e.cycle)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument cycle at index "
      << index
      << " in history of event RateGroupCycleSlip\n"
      << "  Expected: " << cycle << "\n"
      << "  Actual:   " << e.cycle << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
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
  // From port: RateGroupMemberOut
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assert_from_RateGroupMemberOut_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_RateGroupMemberOut->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_RateGroupMemberOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_RateGroupMemberOut->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: PingOut
  // ----------------------------------------------------------------------

  void ActiveRateGroupGTestBase ::
    assert_from_PingOut_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_PingOut->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_PingOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_PingOut->size() << "\n";
  }

} // end namespace Svc

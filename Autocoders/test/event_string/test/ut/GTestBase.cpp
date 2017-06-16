// ======================================================================
// \title  TestLog/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for TestLog component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ======================================================================

#include "GTestBase.hpp"

namespace Somewhere {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  TestLogGTestBase ::
    TestLogGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        TestLogTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  TestLogGTestBase ::
    ~TestLogGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void TestLogGTestBase ::
    assertEvents_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: SomeEvent
  // ----------------------------------------------------------------------

  void TestLogGTestBase ::
    assertEvents_SomeEvent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_SomeEvent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event SomeEvent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_SomeEvent->size() << "\n";
  }

  void TestLogGTestBase ::
    assertEvents_SomeEvent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const I32 arg1,
        const char *const arg2,
        const U8 arg3
    ) const
  {
    ASSERT_GT(this->eventHistory_SomeEvent->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event SomeEvent\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_SomeEvent->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_SomeEvent& e =
      this->eventHistory_SomeEvent->at(index);
    ASSERT_EQ(arg1, e.arg1)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg1 at index "
      << index
      << " in history of event SomeEvent\n"
      << "  Expected: " << arg1 << "\n"
      << "  Actual:   " << e.arg1 << "\n";
    ASSERT_STREQ(arg2, e.arg2.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg2 at index "
      << index
      << " in history of event SomeEvent\n"
      << "  Expected: " << arg2 << "\n"
      << "  Actual:   " << e.arg2.toChar() << "\n";
    ASSERT_EQ(arg3, e.arg3)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg3 at index "
      << index
      << " in history of event SomeEvent\n"
      << "  Expected: " << arg3 << "\n"
      << "  Actual:   " << e.arg3 << "\n";
  }

} // end namespace Somewhere

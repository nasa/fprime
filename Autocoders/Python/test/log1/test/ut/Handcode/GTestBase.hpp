// ======================================================================
// \title  TestEvent/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestEvent component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestEvent_GTEST_BASE_HPP
#define TestEvent_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for event history assertions
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent_SIZE(size) \
  this->assertEvents_SomeEvent_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent(index, _arg1) \
  this->assertEvents_SomeEvent(__FILE__, __LINE__, index, _arg1)

namespace Tlm {

  //! \class TestEventGTestBase
  //! \brief Auto-generated base class for TestEvent component Google Test harness
  //!
  class TestEventGTestBase :
    public TestEventTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestEventGTestBase
      //!
      TestEventGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TestEventGTestBase
      //!
      virtual ~TestEventGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: SomeEvent
      // ----------------------------------------------------------------------

      void assertEvents_SomeEvent_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertEvents_SomeEvent(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const U32 arg1 //!< The U32 log argument
      ) const;

  };

} // end namespace Tlm

#endif

// ======================================================================
// \title  TestCommand/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestCommand component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestCommand_GTEST_BASE_HPP
#define TestCommand_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent_SIZE(size) \
  this->assertEvents_SomeEvent_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent(index, _arg1, _arg2, _arg3) \
  this->assertEvents_SomeEvent(__FILE__, __LINE__, index, _arg1, _arg2, _arg3)

namespace Cmd {

  //! \class TestCommandGTestBase
  //! \brief Auto-generated base class for TestCommand component Google Test harness
  //!
  class TestCommandGTestBase :
    public TestCommandTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestCommandGTestBase
      //!
      TestCommandGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestCommandGTestBase
      //!
      virtual ~TestCommandGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: SomeEvent
      // ----------------------------------------------------------------------

      void assertEvents_SomeEvent_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_SomeEvent(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const I32 arg1, /*!< The I32 command argument*/
          const F32 arg2, /*!< The F32 command argument*/
          const Ref::Gnc::Quaternion arg3 /*!< The U8 command argument*/
      ) const;

  };

} // end namespace Cmd

#endif

// ======================================================================
// \title  AssertFatalAdapter/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for AssertFatalAdapter component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef AssertFatalAdapter_GTEST_BASE_HPP
#define AssertFatalAdapter_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_0_SIZE(size) \
  this->assertEvents_AF_ASSERT_0_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_0(index, _file, _line) \
  this->assertEvents_AF_ASSERT_0(__FILE__, __LINE__, index, _file, _line)

#define ASSERT_EVENTS_AF_ASSERT_1_SIZE(size) \
  this->assertEvents_AF_ASSERT_1_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_1(index, _file, _line, _arg1) \
  this->assertEvents_AF_ASSERT_1(__FILE__, __LINE__, index, _file, _line, _arg1)

#define ASSERT_EVENTS_AF_ASSERT_2_SIZE(size) \
  this->assertEvents_AF_ASSERT_2_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_2(index, _file, _line, _arg1, _arg2) \
  this->assertEvents_AF_ASSERT_2(__FILE__, __LINE__, index, _file, _line, _arg1, _arg2)

#define ASSERT_EVENTS_AF_ASSERT_3_SIZE(size) \
  this->assertEvents_AF_ASSERT_3_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_3(index, _file, _line, _arg1, _arg2, _arg3) \
  this->assertEvents_AF_ASSERT_3(__FILE__, __LINE__, index, _file, _line, _arg1, _arg2, _arg3)

#define ASSERT_EVENTS_AF_ASSERT_4_SIZE(size) \
  this->assertEvents_AF_ASSERT_4_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_4(index, _file, _line, _arg1, _arg2, _arg3, _arg4) \
  this->assertEvents_AF_ASSERT_4(__FILE__, __LINE__, index, _file, _line, _arg1, _arg2, _arg3, _arg4)

#define ASSERT_EVENTS_AF_ASSERT_5_SIZE(size) \
  this->assertEvents_AF_ASSERT_5_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_5(index, _file, _line, _arg1, _arg2, _arg3, _arg4, _arg5) \
  this->assertEvents_AF_ASSERT_5(__FILE__, __LINE__, index, _file, _line, _arg1, _arg2, _arg3, _arg4, _arg5)

#define ASSERT_EVENTS_AF_ASSERT_6_SIZE(size) \
  this->assertEvents_AF_ASSERT_6_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_ASSERT_6(index, _file, _line, _arg1, _arg2, _arg3, _arg4, _arg5, _arg6) \
  this->assertEvents_AF_ASSERT_6(__FILE__, __LINE__, index, _file, _line, _arg1, _arg2, _arg3, _arg4, _arg5, _arg6)

#define ASSERT_EVENTS_AF_UNEXPECTED_ASSERT_SIZE(size) \
  this->assertEvents_AF_UNEXPECTED_ASSERT_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_AF_UNEXPECTED_ASSERT(index, _file, _line, _numArgs) \
  this->assertEvents_AF_UNEXPECTED_ASSERT(__FILE__, __LINE__, index, _file, _line, _numArgs)

namespace Svc {

  //! \class AssertFatalAdapterGTestBase
  //! \brief Auto-generated base class for AssertFatalAdapter component Google Test harness
  //!
  class AssertFatalAdapterGTestBase :
    public AssertFatalAdapterTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object AssertFatalAdapterGTestBase
      //!
      AssertFatalAdapterGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object AssertFatalAdapterGTestBase
      //!
      virtual ~AssertFatalAdapterGTestBase(void);

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
      // Event: AF_ASSERT_0
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_0_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_0(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line /*!< Line number of the assert*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_1
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_1_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_1(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1 /*!< First assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_2
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_2_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_2(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1, /*!< First assert argument*/
          const U32 arg2 /*!< Second assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_3
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_3_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_3(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1, /*!< First assert argument*/
          const U32 arg2, /*!< Second assert argument*/
          const U32 arg3 /*!< Third assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_4
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_4_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_4(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1, /*!< First assert argument*/
          const U32 arg2, /*!< Second assert argument*/
          const U32 arg3, /*!< Third assert argument*/
          const U32 arg4 /*!< Fourth assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_5
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_5_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_5(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1, /*!< First assert argument*/
          const U32 arg2, /*!< Second assert argument*/
          const U32 arg3, /*!< Third assert argument*/
          const U32 arg4, /*!< Fourth assert argument*/
          const U32 arg5 /*!< Fifth assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_ASSERT_6
      // ----------------------------------------------------------------------

      void assertEvents_AF_ASSERT_6_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_ASSERT_6(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 arg1, /*!< First assert argument*/
          const U32 arg2, /*!< Second assert argument*/
          const U32 arg3, /*!< Third assert argument*/
          const U32 arg4, /*!< Fourth assert argument*/
          const U32 arg5, /*!< Fifth assert argument*/
          const U32 arg6 /*!< Sixth assert argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: AF_UNEXPECTED_ASSERT
      // ----------------------------------------------------------------------

      void assertEvents_AF_UNEXPECTED_ASSERT_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_AF_UNEXPECTED_ASSERT(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file, /*!< The source file of the assert*/
          const U32 line, /*!< Line number of the assert*/
          const U32 numArgs /*!< Number of unexpected arguments*/
      ) const;

  };

} // end namespace Svc

#endif

// ======================================================================
// \title  Example/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for Example component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_GTEST_BASE_HPP
#define Example_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_exampleOutput_SIZE(size) \
  this->assert_from_exampleOutput_size(__FILE__, __LINE__, size)

#define ASSERT_from_exampleOutput(index, _arg1, _arg2, _arg6) \
  { \
    ASSERT_GT(this->fromPortHistory_exampleOutput->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_exampleOutput\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_exampleOutput->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_exampleOutput& _e = \
      this->fromPortHistory_exampleOutput->at(index); \
    ASSERT_EQ(_arg1, _e.arg1) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg1 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg1 << "\n" \
    << "  Actual:   " << _e.arg1 << "\n"; \
    ASSERT_EQ(_arg2, _e.arg2) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg2 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg2 << "\n" \
    << "  Actual:   " << _e.arg2 << "\n"; \
    ASSERT_EQ(_arg6, _e.arg6) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg6 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg6 << "\n" \
    << "  Actual:   " << _e.arg6 << "\n"; \
  }

namespace ExampleComponents {

  //! \class ExampleGTestBase
  //! \brief Auto-generated base class for Example component Google Test harness
  //!
  class ExampleGTestBase :
    public ExampleTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ExampleGTestBase
      //!
      ExampleGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object ExampleGTestBase
      //!
      virtual ~ExampleGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: exampleOutput
      // ----------------------------------------------------------------------

      void assert_from_exampleOutput_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

  };

} // end namespace ExampleComponents

#endif

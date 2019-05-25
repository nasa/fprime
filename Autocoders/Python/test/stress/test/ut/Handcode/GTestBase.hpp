// ======================================================================
// \title  TestPort/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestPort component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestPort_GTEST_BASE_HPP
#define TestPort_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_aport_SIZE(size) \
  this->assert_from_aport_size(__FILE__, __LINE__, size)

#define ASSERT_from_aport(index, _arg4, _arg5, _arg6) \
  { \
    ASSERT_GT(this->fromPortHistory_aport->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_aport\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_aport->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_aport& _e = \
      this->fromPortHistory_aport->at(index); \
    ASSERT_EQ(_arg4, _e.arg4) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg4 at index " \
    << index \
    << " in history of from_aport\n" \
    << "  Expected: " << _arg4 << "\n" \
    << "  Actual:   " << _e.arg4 << "\n"; \
    ASSERT_EQ(_arg5, _e.arg5) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg5 at index " \
    << index \
    << " in history of from_aport\n" \
    << "  Expected: " << _arg5 << "\n" \
    << "  Actual:   " << _e.arg5 << "\n"; \
    ASSERT_EQ(_arg6, _e.arg6) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg6 at index " \
    << index \
    << " in history of from_aport\n" \
    << "  Expected: " << _arg6 << "\n" \
    << "  Actual:   " << _e.arg6 << "\n"; \
  }

#define ASSERT_from_aport2_SIZE(size) \
  this->assert_from_aport2_size(__FILE__, __LINE__, size)

#define ASSERT_from_aport2(index, _arg4, _arg5, _arg6) \
  { \
    ASSERT_GT(this->fromPortHistory_aport2->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_aport2\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_aport2->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_aport2& _e = \
      this->fromPortHistory_aport2->at(index); \
    ASSERT_EQ(_arg4, _e.arg4) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg4 at index " \
    << index \
    << " in history of from_aport2\n" \
    << "  Expected: " << _arg4 << "\n" \
    << "  Actual:   " << _e.arg4 << "\n"; \
    ASSERT_EQ(_arg5, _e.arg5) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg5 at index " \
    << index \
    << " in history of from_aport2\n" \
    << "  Expected: " << _arg5 << "\n" \
    << "  Actual:   " << _e.arg5 << "\n"; \
    ASSERT_EQ(_arg6, _e.arg6) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg6 at index " \
    << index \
    << " in history of from_aport2\n" \
    << "  Expected: " << _arg6 << "\n" \
    << "  Actual:   " << _e.arg6 << "\n"; \
  }

namespace StressTest {

  //! \class TestPortGTestBase
  //! \brief Auto-generated base class for TestPort component Google Test harness
  //!
  class TestPortGTestBase :
    public TestPortTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestPortGTestBase
      //!
      TestPortGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TestPortGTestBase
      //!
      virtual ~TestPortGTestBase(void);

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
      // From port: aport
      // ----------------------------------------------------------------------

      void assert_from_aport_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: aport2
      // ----------------------------------------------------------------------

      void assert_from_aport2_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

  };

} // end namespace StressTest

#endif

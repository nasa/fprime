// ======================================================================
// \title  LinuxTimer/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxTimer component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxTimer_GTEST_BASE_HPP
#define LinuxTimer_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_CycleOut_SIZE(size) \
  this->assert_from_CycleOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_CycleOut(index, _cycleStart) \
  { \
    ASSERT_GT(this->fromPortHistory_CycleOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_CycleOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_CycleOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_CycleOut& _e = \
      this->fromPortHistory_CycleOut->at(index); \
    ASSERT_EQ(_cycleStart, _e.cycleStart) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cycleStart at index " \
    << index \
    << " in history of from_CycleOut\n" \
    << "  Expected: " << _cycleStart << "\n" \
    << "  Actual:   " << _e.cycleStart << "\n"; \
  }

namespace Svc {

  //! \class LinuxTimerGTestBase
  //! \brief Auto-generated base class for LinuxTimer component Google Test harness
  //!
  class LinuxTimerGTestBase :
    public LinuxTimerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxTimerGTestBase
      //!
      LinuxTimerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxTimerGTestBase
      //!
      virtual ~LinuxTimerGTestBase(void);

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
      // From port: CycleOut 
      // ----------------------------------------------------------------------

      void assert_from_CycleOut_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif

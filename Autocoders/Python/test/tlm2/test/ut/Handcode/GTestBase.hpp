// ======================================================================
// \title  TestTlm/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestTlm component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestTlm_GTEST_BASE_HPP
#define TestTlm_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_AQuat_SIZE(size) \
  this->assertTlm_AQuat_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_AQuat(index, value) \
  this->assertTlm_AQuat(__FILE__, __LINE__, index, value)

namespace Tlm {

  //! \class TestTlmGTestBase
  //! \brief Auto-generated base class for TestTlm component Google Test harness
  //!
  class TestTlmGTestBase :
    public TestTlmTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestTlmGTestBase
      //!
      TestTlmGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestTlmGTestBase
      //!
      virtual ~TestTlmGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: AQuat
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_AQuat_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_AQuat(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const Ref::Gnc::Quaternion& val /*!< The channel value*/
      ) const;

  };

} // end namespace Tlm

#endif

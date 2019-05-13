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

#define ASSERT_TLM_somechan_SIZE(size) \
  this->assertTlm_somechan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_somechan(index, value) \
  this->assertTlm_somechan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_enumchan_SIZE(size) \
  this->assertTlm_enumchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_enumchan(index, value) \
  this->assertTlm_enumchan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_quatchan_SIZE(size) \
  this->assertTlm_quatchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_quatchan(index, value) \
  this->assertTlm_quatchan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_stringchan_SIZE(size) \
  this->assertTlm_stringchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_stringchan(index, value) \
  this->assertTlm_stringchan(__FILE__, __LINE__, index, value)

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
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: somechan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_somechan_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_somechan(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: enumchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_enumchan_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_enumchan(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const TestTlmComponentBase::SomeEnum& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: quatchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_quatchan_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_quatchan(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: stringchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_stringchan_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_stringchan(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

  };

} // end namespace Tlm

#endif

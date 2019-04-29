// ======================================================================
// \title  TestTlm/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for TestTlm component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Tlm {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  TestTlmGTestBase ::
    TestTlmGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
        TestTlmTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  TestTlmGTestBase ::
    ~TestTlmGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void TestTlmGTestBase ::
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
  // Channel: somechan
  // ----------------------------------------------------------------------

  void TestTlmGTestBase ::
    assertTlm_somechan_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_somechan->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel somechan\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_somechan->size() << "\n";
  }

  void TestTlmGTestBase ::
    assertTlm_somechan(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_somechan->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel somechan\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_somechan->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_somechan& e =
      this->tlmHistory_somechan->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel somechan\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: anotherchan
  // ----------------------------------------------------------------------

  void TestTlmGTestBase ::
    assertTlm_anotherchan_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_anotherchan->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel anotherchan\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_anotherchan->size() << "\n";
  }

  void TestTlmGTestBase ::
    assertTlm_anotherchan(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U8& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_anotherchan->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel anotherchan\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_anotherchan->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_anotherchan& e =
      this->tlmHistory_anotherchan->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel anotherchan\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: someenumchan
  // ----------------------------------------------------------------------

  void TestTlmGTestBase ::
    assertTlm_someenumchan_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_someenumchan->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel someenumchan\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_someenumchan->size() << "\n";
  }

  void TestTlmGTestBase ::
    assertTlm_someenumchan(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const TestTlmComponentBase::SomeEnum& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_someenumchan->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel someenumchan\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_someenumchan->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_someenumchan& e =
      this->tlmHistory_someenumchan->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel someenumchan\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

} // end namespace Tlm

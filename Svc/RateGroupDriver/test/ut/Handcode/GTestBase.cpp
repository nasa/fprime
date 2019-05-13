// ======================================================================
// \title  RateGroupDriver/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for RateGroupDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  RateGroupDriverGTestBase ::
    RateGroupDriverGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        RateGroupDriverTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  RateGroupDriverGTestBase ::
    ~RateGroupDriverGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void RateGroupDriverGTestBase ::
    assertFromPortHistory_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: CycleOut
  // ----------------------------------------------------------------------

  void RateGroupDriverGTestBase ::
    assert_from_CycleOut_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_CycleOut->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_CycleOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_CycleOut->size() << "\n";
  }

} // end namespace Svc

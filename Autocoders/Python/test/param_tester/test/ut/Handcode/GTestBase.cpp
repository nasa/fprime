// ======================================================================
// \title  ParamTester/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for ParamTester component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Prm {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  ParamTesterGTestBase ::
    ParamTesterGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
        ParamTesterTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  ParamTesterGTestBase ::
    ~ParamTesterGTestBase(void)
  {

  }

} // end namespace Prm

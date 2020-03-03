// ======================================================================
// \title  LinuxI2cDriver/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for LinuxI2cDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LinuxI2cDriverGTestBase ::
    LinuxI2cDriverGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
        LinuxI2cDriverTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  LinuxI2cDriverGTestBase ::
    ~LinuxI2cDriverGTestBase(void)
  {

  }

} // end namespace Drv

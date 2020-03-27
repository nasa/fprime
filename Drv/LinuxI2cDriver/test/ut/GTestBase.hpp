// ======================================================================
// \title  LinuxI2cDriver/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxI2cDriver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxI2cDriver_GTEST_BASE_HPP
#define LinuxI2cDriver_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Drv {

  //! \class LinuxI2cDriverGTestBase
  //! \brief Auto-generated base class for LinuxI2cDriver component Google Test harness
  //!
  class LinuxI2cDriverGTestBase :
    public LinuxI2cDriverTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxI2cDriverGTestBase
      //!
      LinuxI2cDriverGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxI2cDriverGTestBase
      //!
      virtual ~LinuxI2cDriverGTestBase(void);

  };

} // end namespace Drv

#endif

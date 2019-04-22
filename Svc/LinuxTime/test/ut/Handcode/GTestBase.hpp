// ======================================================================
// \title  Time/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for Time component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Time_GTEST_BASE_HPP
#define Time_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Svc {

  //! \class TimeGTestBase
  //! \brief Auto-generated base class for Time component Google Test harness
  //!
  class TimeGTestBase :
    public TimeTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TimeGTestBase
      //!
      TimeGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TimeGTestBase
      //!
      virtual ~TimeGTestBase(void);

  };

} // end namespace Svc

#endif

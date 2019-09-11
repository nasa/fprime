// ======================================================================
// \title  TimeTester/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TimeTester component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TimeTester_GTEST_BASE_HPP
#define TimeTester_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Time {

  //! \class TimeTesterGTestBase
  //! \brief Auto-generated base class for TimeTester component Google Test harness
  //!
  class TimeTesterGTestBase :
    public TimeTesterTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TimeTesterGTestBase
      //!
      TimeTesterGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TimeTesterGTestBase
      //!
      virtual ~TimeTesterGTestBase(void);

  };

} // end namespace Time

#endif

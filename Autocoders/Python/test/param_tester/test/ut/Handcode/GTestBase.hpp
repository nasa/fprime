// ======================================================================
// \title  ParamTester/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for ParamTester component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ParamTester_GTEST_BASE_HPP
#define ParamTester_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Prm {

  //! \class ParamTesterGTestBase
  //! \brief Auto-generated base class for ParamTester component Google Test harness
  //!
  class ParamTesterGTestBase :
    public ParamTesterTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ParamTesterGTestBase
      //!
      ParamTesterGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ParamTesterGTestBase
      //!
      virtual ~ParamTesterGTestBase(void);

  };

} // end namespace Prm

#endif

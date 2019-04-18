// ======================================================================
// \title  TestPrm/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestPrm component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestPrm_GTEST_BASE_HPP
#define TestPrm_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Prm {

  //! \class TestPrmGTestBase
  //! \brief Auto-generated base class for TestPrm component Google Test harness
  //!
  class TestPrmGTestBase :
    public TestPrmTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestPrmGTestBase
      //!
      TestPrmGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestPrmGTestBase
      //!
      virtual ~TestPrmGTestBase(void);

  };

} // end namespace Prm

#endif

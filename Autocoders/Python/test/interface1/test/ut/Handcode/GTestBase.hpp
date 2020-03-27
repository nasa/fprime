// ======================================================================
// \title  Test/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for Test component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Test_GTEST_BASE_HPP
#define Test_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Cmd {

  //! \class TestGTestBase
  //! \brief Auto-generated base class for Test component Google Test harness
  //!
  class TestGTestBase :
    public TestTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestGTestBase
      //!
      TestGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object TestGTestBase
      //!
      virtual ~TestGTestBase(void);

  };

} // end namespace Cmd

#endif

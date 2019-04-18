// ======================================================================
// \title  TelemTester/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TelemTester component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TelemTester_GTEST_BASE_HPP
#define TelemTester_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace Tlm {

  //! \class TelemTesterGTestBase
  //! \brief Auto-generated base class for TelemTester component Google Test harness
  //!
  class TelemTesterGTestBase :
    public TelemTesterTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TelemTesterGTestBase
      //!
      TelemTesterGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TelemTesterGTestBase
      //!
      virtual ~TelemTesterGTestBase(void);

  };

} // end namespace Tlm

#endif

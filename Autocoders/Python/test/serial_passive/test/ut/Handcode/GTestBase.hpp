// ======================================================================
// \title  TestSerial/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for TestSerial component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestSerial_GTEST_BASE_HPP
#define TestSerial_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

namespace TestComponents {

  //! \class TestSerialGTestBase
  //! \brief Auto-generated base class for TestSerial component Google Test harness
  //!
  class TestSerialGTestBase :
    public TestSerialTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestSerialGTestBase
      //!
      TestSerialGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TestSerialGTestBase
      //!
      virtual ~TestSerialGTestBase(void);

  };

} // end namespace TestComponents

#endif

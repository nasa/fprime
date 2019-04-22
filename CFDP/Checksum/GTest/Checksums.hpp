// ====================================================================== 
// \title  CFDP/Checksum/GTest/Checksums.hpp
// \author bocchino
// \brief  hpp file for CFDP Checksum gtest utilities
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef GTest_CFDP_Checksums_HPP
#define GTest_CFDP_Checksums_HPP

#include "gtest/gtest.h"

#include "CFDP/Checksum/Checksum.hpp"

namespace CFDP {

  namespace GTest {

    //! Utilities for testing Checksum operations
    //!
    namespace Checksums {

      void compare(
          const CFDP::Checksum& expected, //!< Expected value
          const CFDP::Checksum& actual //!< Actual value
      );

    }

  }

}

#endif

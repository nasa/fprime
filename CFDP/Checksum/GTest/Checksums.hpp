// ====================================================================== 
// \title  CFDP/Checksum/GTest/Checksums.hpp
// \author bocchino
// \brief  hpp file for CFDP Checksum gtest utilities
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
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

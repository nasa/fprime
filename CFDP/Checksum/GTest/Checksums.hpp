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

# GTest_CFDP_Checksums_HPP
# GTest_CFDP_Checksums_HPP

# "gtest/gtest.h"

# "CFDP/Checksum/Checksum.hpp"

 CFDP {

   GTest {

    //! Utilities testing Checksum operations
    //!
     Checksums {

       (
           CFDP::Checksum& expected, //!< Expected value
           CFDP::Checksum& actual //!< Actual value
      );

    }

  }

}

#

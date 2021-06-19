// ====================================================================== 
// \title  CFDP/Checksum/GTest/Checksums.cpp
// \author bocchino
// \brief  cpp file for CFDP Checksum gtest utilities
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

# "CFDP/Checksum/GTest/Checksums.hpp"

 CFDP {

   GTest {

     Checksums ::
      (
           CFDP::Checksum& expected,
           CFDP::Checksum& actual
      ) 
    {
       U32 expectedValue = expected.getValue();
       U32 actualValue = actual.getValue();
      ASSERT_EQ(expectedValue, actualValue);
    }

  }

}

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

#include "CFDP/Checksum/GTest/Checksums.hpp"

namespace CFDP {

  namespace GTest {

    void Checksums ::
      compare(
          const CFDP::Checksum& expected,
          const CFDP::Checksum& actual
      ) 
    {
      const U32 expectedValue = expected.getValue();
      const U32 actualValue = actual.getValue();
      ASSERT_EQ(expectedValue, actualValue);
    }

  }

}

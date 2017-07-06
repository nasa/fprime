// ====================================================================== 
// \title  CFDP/Checksum/GTest/Checksums.cpp
// \author bocchino
// \brief  cpp file for CFDP Checksum gtest utilities
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
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

// ====================================================================== 
// \title  ASTERIA/Types/GTest/Bytes.cpp
// \author bocchino
// \brief  cpp file for Bytes
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

#include <Fw/Types/GTest/Bytes.hpp>

namespace Fw {

  namespace GTest {

    void Bytes ::
      compare(
          const Bytes& expected,
          const Bytes& actual
      ) 
    {
      ASSERT_EQ(expected.size, actual.size);
      for (size_t i = 0; i < expected.size; ++i)
        ASSERT_EQ(expected.bytes[i], actual.bytes[i])
          << "At i=" << i << "\n";
    }

  }

}

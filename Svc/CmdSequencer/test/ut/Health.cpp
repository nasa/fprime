// ====================================================================== 
// \title  Health.cpp
// \author Canham/Bocchino
// \brief  Test health pings
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
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

#include "Svc/CmdSequencer/test/ut/Health.hpp"

namespace Svc {

  namespace Health {

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      Ping(void) 
    {
      const U32 key = 1234;
      this->invoke_to_pingIn(0, key);
      this->clearAndDispatch();
      ASSERT_from_pingOut_SIZE(1);
      ASSERT_from_pingOut(0, key);
    }

  }

}

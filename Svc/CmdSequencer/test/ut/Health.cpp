// ======================================================================
// \title  Health.cpp
// \author Canham/Bocchino
// \brief  Test health pings
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/Health.hpp"

namespace Svc {

  namespace Health {

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      Ping()
    {
      const U32 key = 1234;
      this->invoke_to_pingIn(0, key);
      this->clearAndDispatch();
      ASSERT_from_pingOut_SIZE(1);
      ASSERT_from_pingOut(0, key);
    }

  }

}

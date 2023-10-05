// ======================================================================
// \title  Health.cpp
// \author bocchino, mereweth
// \brief  Implementation for Buffer Logger health tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Health.hpp"

namespace Svc {

  namespace Health {

    void BufferLoggerTester ::
      Ping()
    {

      U32 key = 42;

      this->invoke_to_pingIn(0, key);
      this->dispatchAll();

      ASSERT_EVENTS_SIZE(0);
      ASSERT_from_pingOut_SIZE(1);
      ASSERT_from_pingOut(0, key);

    }

  }

}

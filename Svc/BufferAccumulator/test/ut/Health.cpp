// ======================================================================
// \title  Health.cpp
// \author bocchino, mereweth
// \brief  Implementation for Buffer Accumulator health tests
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

#include "Health.hpp"

namespace Svc {

  namespace Health {

    void Tester ::
      Ping(void)
    {

      U32 key = 42;

      this->invoke_to_pingIn(0, key);
      this->component.doDispatch();

      ASSERT_EVENTS_SIZE(0);
      ASSERT_from_pingOut_SIZE(1);
      ASSERT_from_pingOut(0, key);

    }

  }

}

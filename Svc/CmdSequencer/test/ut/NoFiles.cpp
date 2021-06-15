// ====================================================================== 
// \title  NoFiles.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2018 California Institute of Technology.
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

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/NoFiles.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace NoFiles {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    Tester ::
      Tester(const SequenceFiles::File::Format::t format) :
        Svc::Tester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      Init(void)
    {
      // Nothing to do
    }

    void Tester ::
      NoSequenceActive(void) 
    {

      // Send cancel command
      this->sendCmd_CS_CANCEL(0, 0);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);

    }

  }

}

// ======================================================================
// \title  NoFiles.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/NoFiles.hpp"

namespace Svc {

  namespace NoFiles {

    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    CmdSequencerTester ::
      CmdSequencerTester(const SequenceFiles::File::Format::t format) :
        Svc::CmdSequencerTester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      Init()
    {
      // Nothing to do
    }

    void CmdSequencerTester ::
      NoSequenceActive()
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

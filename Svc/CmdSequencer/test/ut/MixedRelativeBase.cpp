// ====================================================================== 
// \title  MixedRelativeBase.cpp
// \author Canham/Bocchino
// \brief  Base class for Mixed and Relative
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace MixedRelativeBase {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    CmdSequencerTester ::
      CmdSequencerTester(const SequenceFiles::File::Format::t format) :
        Svc::CmdSequencerTester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests parameterized by file type
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      parameterizedAutoByCommand(
          SequenceFiles::File& file,
          const U32 numCommands,
          const U32 bound
      ) 
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Run the sequence
      this->runSequence(0, fileName);
      // Assert that timer is set
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimer.m_state
      );
      // Run one cycle to make sure nothing is dispatched yet
      this->invoke_to_schedIn(0, 0);
      this->clearAndDispatch();
      ASSERT_from_comCmdOut_SIZE(0);
      ASSERT_EVENTS_SIZE(0);
      // Assert that timer hasn't expired
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimer.m_state
      );
      // Execute commands
      this->executeCommandsAuto(
          fileName,
          numCommands,
          bound,
          CmdExecMode::NO_NEW_SEQUENCE
      );
    }

  }

}

// ====================================================================== 
// \title  MixedRelativeBase.cpp
// \author Canham/Bocchino
// \brief  Base class for Mixed and Relative
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

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace MixedRelativeBase {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    Tester ::
      Tester(const SequenceFiles::File::Format::t format) :
        Svc::Tester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests parameterized by file type
    // ----------------------------------------------------------------------

    void Tester ::
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

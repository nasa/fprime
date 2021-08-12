// ====================================================================== 
// \title  ImmediateEOS.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/ImmediateEOS.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace ImmediateEOS {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    Tester ::
      Tester(const SequenceFiles::File::Format::t format) :
        ImmediateBase::Tester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      AutoByCommand(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      const U32 numCommands = numRecords - 1;
      const U32 bound = numCommands;
      this->parameterizedAutoByCommand(file, numCommands, bound);
    }

    void Tester ::
      Cancel(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      const U32 numCommands = numRecords - 1;
      const U32 bound = numCommands - 1;
      this->parameterizedCancel(file, numCommands, bound);
    }

    void Tester ::
      FileErrors(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedFileErrors(file);
    }

    void Tester ::
      InvalidManualCommands(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedInvalidManualCommands(file);
    }

    void Tester ::
      Manual(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords - 1;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedManual(file, numCommands);
    }

    void Tester ::
      NewSequence(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords - 1;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedNewSequence(file, numCommands, bound);
    }

    void Tester ::
      AutoByPort(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords - 1;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedAutoByPort(file, numCommands, bound);
    }

    void Tester ::
      SequenceTimeout(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedSequenceTimeout(file);
    }

    void Tester ::
      UnexpectedCommandResponse(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords - 1;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedUnexpectedCommandResponse(file, numCommands, bound);
    }

    void Tester ::
      Validate(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateEOSFile file(numRecords, this->format);
      this->parameterizedValidate(file);
    }

    // ----------------------------------------------------------------------
    // Private helper methods 
    // ----------------------------------------------------------------------

    void Tester ::
      executeCommandsManual(
          const char *const fileName, 
          const U32 numCommands
      )
    {
      for (U32 i = 0; i < numCommands; ++i) {
        PRINT("REC %d\n", i);
        // Check command buffer
        Fw::ComBuffer comBuff;
        CommandBuffers::create(comBuff, i, i + 1);
        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0, comBuff, 0U);
        // Assert that timer is clear
        ASSERT_EQ(
            CmdSequencerComponentImpl::Timer::CLEAR,
            this->component.m_cmdTimeoutTimer.m_state
        );
        // Send command response
        this->invoke_to_cmdResponseIn(0, i, 0, Fw::COMMAND_OK);
        this->clearAndDispatch();
        // Assert events
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
        // Assert telemetry
        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
        // Step sequence
        this->stepSequence(12);
        // Assert events
        if (i < numCommands - 1) {
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CmdStepped(0, fileName, i + 1);
        }
        else {
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
        }
      }
    }

  }

}

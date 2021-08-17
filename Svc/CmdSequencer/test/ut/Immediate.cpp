// ====================================================================== 
// \title  Immediate.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with  record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/Immediate.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace Immediate {

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
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedAutoByCommand(file, numCommands, bound);
    }

    void Tester ::
      AutoByPort(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedAutoByPort(file, numCommands, bound);
    }

    void Tester ::
      Cancel(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numRecords - 1;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedCancel(file, numCommands, bound);
    }

    void Tester ::
      FailedCommands(void)
    {
      const U32 numRecords = 3;
      const U32 numCommands = numRecords;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedFailedCommands(file, numCommands);
    }

    void Tester ::
      FileErrors(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedFileErrors(file);
    }

    void Tester ::
      InvalidManualCommands(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedInvalidManualCommands(file);
    }

    void Tester ::
      LoadOnInit(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedLoadOnInit(file, numCommands, bound);
    }

    void Tester ::
      LoadRunRun(void)
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedLoadRunRun(file, numCommands, bound);
    }

    void Tester ::
      Manual(void)
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedManual(file, numCommands);
    }

    void Tester ::
      NeverLoaded(void)
    {
      this->parameterizedNeverLoaded();
    }

    void Tester ::
      NewSequence(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedNewSequence(file, numCommands, bound);
    }

    void Tester ::
      SequenceTimeout(void) 
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedSequenceTimeout(file);
    }

    void Tester ::
      UnexpectedCommandResponse(void) 
    {
      const U32 numRecords = 5;
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
      this->parameterizedUnexpectedCommandResponse(file, numCommands, bound);
    }

    void Tester ::
      Validate(void)
    {
      const U32 numRecords = 5;
      SequenceFiles::ImmediateFile file(numRecords, this->format);
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
        if (i < numCommands - 1) {
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          // Assert telemetry
          ASSERT_TLM_SIZE(1);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
          // Step sequence
          this->stepSequence(12);
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CmdStepped(0, fileName, i + 1);
        }
        else {
          // Assert events
          ASSERT_EVENTS_SIZE(2);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
          // Assert telemetry
          ASSERT_TLM_SIZE(2);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
          ASSERT_TLM_CS_SequencesCompleted(0, 1);
        }
      }
    }

  }

}

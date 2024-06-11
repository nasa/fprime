// ======================================================================
// \title  Relative.cpp
// \author Canham/Bocchino
// \brief  Test relative command sequences
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/Relative.hpp"

namespace Svc {

  namespace Relative {

    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    CmdSequencerTester ::
      CmdSequencerTester(const SequenceFiles::File::Format::t format) :
        MixedRelativeBase::CmdSequencerTester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      AutoByCommand()
    {
      const U32 numRecords = 3;
      SequenceFiles::RelativeFile file(numRecords, this->format);
      const U32 numCommands = numRecords;
      const U32 bound = numCommands;
      this->parameterizedAutoByCommand(file, numCommands, bound);
    }

    void CmdSequencerTester ::
      Validate()
    {
      const U32 numRecords = 5;
      SequenceFiles::RelativeFile file(numRecords, this->format);
      this->parameterizedValidate(file);
    }

    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      executeCommandsAuto(
          const char *const fileName,
          const U32 numCommands,
          const U32 bound,
          const CmdExecMode::t mode
      )
    {
      for (U32 i = 0; i < numCommands; ++i) {
        // Set the time to after time of command
        Fw::Time testTime(TB_WORKSTATION_TIME, 2 * i + 3, 0);
        this->setTestTime(testTime);
        // Run a cycle. Should dispatch timed command
        this->invoke_to_schedIn(0, 0);
        this->clearAndDispatch();
        // Assert that timer is clear
        ASSERT_EQ(
            CmdSequencerComponentImpl::Timer::CLEAR,
            this->component.m_cmdTimer.m_state
        );
        // Check command buffer
        Fw::ComBuffer comBuff;
        CommandBuffers::create(comBuff, i, i + 1);
        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0, comBuff, 0U);
        // Send status back
        this->invoke_to_cmdResponseIn(0, i, 0, Fw::CmdResponse::OK);
        this->clearAndDispatch();
        if (i < numCommands - 1) {
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          // Assert telemetry
          ASSERT_TLM_SIZE(1);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
          // Assert that timer is set for next i
          ASSERT_EQ(
              CmdSequencerComponentImpl::Timer::SET,
              this->component.m_cmdTimer.m_state
          );
        }
        else {
          // Assert events
          ASSERT_EVENTS_SIZE(2);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
          // Assert telemetry
          ASSERT_TLM_SIZE(2);
          ASSERT_TLM_CS_SequencesCompleted(0, 1);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
          // Assert that timer is clear
          ASSERT_EQ(
              CmdSequencerComponentImpl::Timer::CLEAR,
              this->component.m_cmdTimer.m_state
          );
          // Assert command complete on seqDone
          ASSERT_from_seqDone_SIZE(1);
          ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
        }
        // No port call
        ASSERT_from_comCmdOut_SIZE(0);
        // Run a cycle. Should be no output.
        this->invoke_to_schedIn(0, 0);
        this->clearAndDispatch();
        ASSERT_from_comCmdOut_SIZE(0);
        // Command and sequence complete EVR
        ASSERT_EVENTS_SIZE(0);
        ASSERT_TLM_SIZE(0);
      }
    }

  }

}

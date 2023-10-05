// ======================================================================
// \title  Mixed.cpp
// \author Canham/Bocchino
// \brief  Test mixed immediate, relative, and absolute commands
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/Mixed.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace Mixed {

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
      SequenceFiles::MixedFile file(this->format);
      const U32 numCommands = 4;
      const U32 bound = numCommands;
      this->parameterizedAutoByCommand(file, numCommands, bound);
    }

    void CmdSequencerTester ::
      Validate()
    {
      SequenceFiles::MixedFile file(this->format);
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
      ASSERT_EQ(4U, numCommands);
      ASSERT_EQ(4U, bound);
      ASSERT_EQ(CmdExecMode::NO_NEW_SEQUENCE, mode);
      this->executeCommand1(fileName);
      this->executeCommand2(fileName);
      this->executeCommand3(fileName);
      this->executeCommand4(fileName);
    }

    void CmdSequencerTester ::
      executeCommand1(const char* const fileName)
    {
      // Set the time to past absolute command
      Fw::Time testTime(TB_WORKSTATION_TIME, 3, 0);
      this->setTestTime(testTime);
      // Invoke schedIn
      this->invoke_to_schedIn(0, 0);
      this->clearAndDispatch();
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
      // Check command buffer
      Fw::ComBuffer comBuff;
      CommandBuffers::create(comBuff, 0 , 1);
      ASSERT_from_comCmdOut_SIZE(1);
      ASSERT_from_comCmdOut(0, comBuff, 0U);
      // Send status back
      this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_CommandComplete(0, fileName, 0, 0);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_CommandsExecuted(0, 1);
      // Assert that timer is clear - no scheduled command
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
    }

    void CmdSequencerTester ::
      executeCommand2(const char* const fileName)
    {
      // Check command buffer
      Fw::ComBuffer comBuff;
      CommandBuffers::create(comBuff, 2, 3);
      ASSERT_from_comCmdOut_SIZE(1);
      ASSERT_from_comCmdOut(0, comBuff, 0U);
      // Send status back
      this->invoke_to_cmdResponseIn(0, 2, 0, Fw::CmdResponse::OK);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_CommandComplete(0, fileName, 1, 2);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_CommandsExecuted(0, 2);
      // Assert that timer is waiting for relative command
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimer.m_state
      );
    }

    void CmdSequencerTester ::
      executeCommand3(const char* const fileName)
    {
      // Set the time to past relative timer
      Fw::Time testTime(TB_WORKSTATION_TIME, 5, 0);
      this->setTestTime(testTime);
      // Invoke schedIn
      this->invoke_to_schedIn(0, 0);
      this->clearAndDispatch();
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
      // Check command buffer
      Fw::ComBuffer comBuff;
      CommandBuffers::create(comBuff, 4, 5);
      ASSERT_from_comCmdOut_SIZE(1);
      ASSERT_from_comCmdOut(0, comBuff, 0U);
      // Send status back
      this->invoke_to_cmdResponseIn(0, 4, 0, Fw::CmdResponse::OK);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_CommandComplete(0, fileName, 2, 4);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_CommandsExecuted(0, 3);
      // Assert that timer is clear - no scheduled command
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
    }

    void CmdSequencerTester ::
      executeCommand4(const char* const fileName)
    {
      // Assert that timer is clear - immediate command
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
      // Check command buffer
      Fw::ComBuffer comBuff;
      CommandBuffers::create(comBuff, 6, 7);
      ASSERT_from_comCmdOut_SIZE(1);
      ASSERT_from_comCmdOut(0, comBuff, 0U);
      // Send status back
      this->invoke_to_cmdResponseIn(0, 6, 0, Fw::CmdResponse::OK);
      this->clearAndDispatch();
      // Assert that timer is clear - no scheduled command
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimer.m_state
      );
      // Assert events
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_CS_CommandComplete(0, fileName, 3, 6);
      ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
      // Assert telemetry
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_CS_SequencesCompleted(0, 1);
      ASSERT_TLM_CS_CommandsExecuted(0, 4);
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
      // Run a cycle. Should be no output
      this->invoke_to_schedIn(0, 0);
      this->clearAndDispatch();
      ASSERT_from_comCmdOut_SIZE(0);
      // Assert no more events or telemetry
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
    }

  }

}

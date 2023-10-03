// ======================================================================
// \title  ImmediateBase.cpp
// \author Canham/Bocchino
// \brief  Base class for Immediate and ImmediateEOS
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/ImmediateBase.hpp"
#include "Os/Stubs/FileStubs.hpp"

namespace Svc {

  namespace ImmediateBase {

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

      REQUIREMENT("ISF-CMDS-003");

      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Run the sequence
      this->runSequence(0, fileName);
      // Execute commands
      this->executeCommandsAuto(
          fileName,
          numCommands,
          bound,
          CmdExecMode::NO_NEW_SEQUENCE
      );
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));

    }

    void CmdSequencerTester ::
      parameterizedAutoByPort(
          SequenceFiles::File& file,
          const U32 numCommands,
          const U32 bound
      )
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Run the sequence by port call
      this->runSequenceByPortCall(fileName);
      // Execute commands
      this->executeCommandsAuto(
          fileName,
          numCommands,
          bound,
          CmdExecMode::NO_NEW_SEQUENCE
      );
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
    }

    void CmdSequencerTester ::
      parameterizedInvalidManualCommands(SequenceFiles::File& file)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Attempt to start manual mode without a sequence - should fail
      const U32 startCmdSeq = 14;
      this->sendCmd_CS_START(0, startCmdSeq);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_START,
          startCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);
      // Validate the file
      this->validateFile(0, fileName);
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Run the sequence
      this->runSequence(0, fileName);
      // Check command buffers
      Fw::ComBuffer comBuff;
      CommandBuffers::create(comBuff, 0, 1);
      ASSERT_from_comCmdOut_SIZE(1);
      ASSERT_from_comCmdOut(0, comBuff, 0U);
      // Assert that timer is set
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::SET,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Attempt to start a manual sequence - should fail
      this->sendCmd_CS_START(0, startCmdSeq);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_START,
          startCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
      // Attempt to go to auto mode - should fail
      const U32 autoCmdSeq = 14;
      this->sendCmd_CS_AUTO(0, autoCmdSeq);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_AUTO,
          autoCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
      // Attempt to go to manual mode - should fail
      const U32 manualCmdSeq = 14;
      this->sendCmd_CS_MANUAL(0, manualCmdSeq);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_MANUAL,
          manualCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
    }

    void CmdSequencerTester ::
      parameterizedLoadRunRun(
          SequenceFiles::File& file,
          const U32 numCommands,
          const U32 bound
      )
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Load the sequence
      this->loadSequence(fileName);
      // Run another sequence
      this->parameterizedAutoByPort(file, numCommands, bound);
      // Try to run a loaded sequence
      Fw::String fArg("");
      this->invoke_to_seqRunIn(0, fArg);
      this->clearAndDispatch();
      // Assert seqDone response
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0U, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR));
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void CmdSequencerTester ::
      parameterizedManual(SequenceFiles::File& file, const U32 numCommands)
    {

      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Go to manual mode
      this->goToManualMode(10);
      // Run sequence
      // This should validate and load the sequence, then stop
      this->runSequence(0, fileName);
      // Assert no command buffer
      ASSERT_from_comCmdOut_SIZE(0);
      // Send start command to load first command
      this->startSequence(14, fileName);
      // Execute commands
      this->executeCommandsManual(fileName, numCommands);
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
      // Send step command. Should return error since no active sequence
      const U32 stepCmdSeq = 12;
      this->sendCmd_CS_STEP(0, stepCmdSeq);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_STEP,
          stepCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
      // Go back to auto mode
      this->goToAutoMode(stepCmdSeq);

    }

    void CmdSequencerTester ::
      parameterizedNewSequence(
          SequenceFiles::File& file,
          const U32 numCommands,
          const U32 bound
      )
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->validateFile(0, fileName);
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Run the sequence
      this->runSequence(0, fileName);
      // Execute commands
      this->executeCommandsAuto(
          fileName,
          numCommands,
          bound,
          CmdExecMode::NEW_SEQUENCE
      );
      // Assert that timer is clear
      ASSERT_EQ(
          CmdSequencerComponentImpl::Timer::CLEAR,
          this->component.m_cmdTimeoutTimer.m_state
      );
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
    }

    void CmdSequencerTester ::
      parameterizedLoadOnInit(
          SequenceFiles::File& file,
          const U32 numCommands,
          const U32 bound
      )
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const char *const fileName = file.getName().toChar();
      file.write();
      // Load the sequence
      this->loadSequence(fileName);
      // Run the loaded sequence
      this->runLoadedSequence();
      // Execute commands
      this->executeCommandsAuto(
          fileName,
          numCommands,
          bound,
          CmdExecMode::NO_NEW_SEQUENCE
      );
      // Check for command complete on seqDone
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
    }

    // ----------------------------------------------------------------------
    // Protected helper methods
    // ----------------------------------------------------------------------

    void CmdSequencerTester ::
      executeCommandsAuto(
          const char *const fileName,
          const U32 numCommands,
          const U32 bound,
          const CmdExecMode::t mode
      )
    {
      for (U32 i = 0; i < bound; ++i) {
        PRINT("REC %d\n", i);
        // Check command buffer
        Fw::ComBuffer comBuff;
        CommandBuffers::create(comBuff, i, i + 1);
        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0, comBuff, 0U);
        // Assert that timer is set
        ASSERT_EQ(
            CmdSequencerComponentImpl::Timer::SET,
            this->component.m_cmdTimeoutTimer.m_state
        );
        // Start a new sequence if necessary
        if (i == 0 and mode == CmdExecMode::NEW_SEQUENCE) {
          this->startNewSequence(fileName);
        }
        // Send status back
        this->invoke_to_cmdResponseIn(0, i, 0, Fw::CmdResponse(Fw::CmdResponse::OK));
        this->clearAndDispatch();
        if (i < numCommands - 1) {
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          // Assert telemetry
          ASSERT_TLM_SIZE(1);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
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

    void CmdSequencerTester ::
      executeCommandsError(
          const char *const fileName,
          const U32 numCommands
      )
    {
      for (U32 i = 0; i < numCommands - 1; ++i) {
        // Check command buffer
        Fw::ComBuffer comBuff;
        CommandBuffers::create(comBuff, i, i + 1);
        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0, comBuff, 0U);
        if (i == 0) {
          // Send good status back
          this->invoke_to_cmdResponseIn(0, i, 0, Fw::CmdResponse(Fw::CmdResponse::OK));
          this->clearAndDispatch();
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
          // Assert telemetry
          ASSERT_TLM_SIZE(1);
          ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
        }
        else {
          // Send failed status back
          this->invoke_to_cmdResponseIn(
              0,
              i,
              0,
              Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR)
          );
          this->clearAndDispatch();
          // Assert events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_CS_CommandError_SIZE(1);
          ASSERT_EVENTS_CS_CommandError(
              0,
              fileName,
              1,
              i,
              Fw::CmdResponse::EXECUTION_ERROR
          );
          // Assert telemetry
          ASSERT_TLM_SIZE(1);
          ASSERT_TLM_CS_Errors_SIZE(1);
          ASSERT_TLM_CS_Errors(0, 1);
          // Check for command complete on seqDone
          ASSERT_from_seqDone_SIZE(1);
          ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR));
        }
      }
    }

  }

}

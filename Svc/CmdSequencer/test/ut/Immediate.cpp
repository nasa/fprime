// ======================================================================
// \title  Immediate.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with  record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/Immediate.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"

namespace Svc {

namespace Immediate {

// ----------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------

CmdSequencerTester ::CmdSequencerTester(const SequenceFiles::File::Format::t a_format)
    : ImmediateBase::CmdSequencerTester(a_format) {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void CmdSequencerTester ::AutoByCommand() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedAutoByCommand(file, numCommands, bound);
}

void CmdSequencerTester ::AutoByPort() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedAutoByPort(file, numCommands, bound);
}

void CmdSequencerTester ::AutoByFileDispatcherPort() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedAutoByFileDispatchPort(file, numCommands, bound);
}

void CmdSequencerTester ::AutoByCommandMaxFileName() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->setFileNameLength(file, FW_CMD_STRING_MAX_SIZE);
    this->parameterizedAutoByCommand(file, numCommands, bound);
}

void CmdSequencerTester ::AutoByPortLongFileName() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->setFileNameLength(file, static_cast<FwSizeType>(FW_CMD_STRING_MAX_SIZE) + 1);
    // Set the time
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char* const fileName = file.getName().toChar();
    file.write();
    // Run the sequence by port call
    this->runSequenceByPortCall(fileName);
    // Execute commands
    this->executeCommandsAuto(fileName, numCommands, bound, CmdExecMode::NO_NEW_SEQUENCE);
    // Check for command complete on seqDone
    ASSERT_from_seqDone_SIZE(1);
    ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::OK));
}

void CmdSequencerTester ::AutoByFileDispatcherPortMaxFileName() {
    const U32 numRecords = 5;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->setFileNameLength(file, FileNameStringSize);
    // Set the time
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    file.write();
    // Run the sequence by file dispatcher port call
    Fw::String fileName(file.getName());
    this->invoke_to_seqDispatchIn(0, fileName);
    this->clearAndDispatch();
    // Assert no command response
    ASSERT_CMD_RESPONSE_SIZE(0);
    // Events carry the first 60 characters of the file name (fileName size in Events.fppi),
    // CS_CurrentSequence the first FW_TLM_STRING_MAX_SIZE characters
    const FwSizeType eventFileNameSize = 60;
    char eventFileName[eventFileNameSize + 1];
    (void)Fw::StringUtils::string_copy(eventFileName, fileName.toChar(),
                                       static_cast<FwSizeType>(sizeof(eventFileName)));
    char tlmFileName[FW_TLM_STRING_MAX_SIZE + 1];
    (void)Fw::StringUtils::string_copy(tlmFileName, fileName.toChar(), static_cast<FwSizeType>(sizeof(tlmFileName)));
    // Assert events
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_CS_SequenceLoaded(0, eventFileName);
    ASSERT_EVENTS_CS_PortSequenceStarted(0, eventFileName);
    // Assert telemetry
    ASSERT_TLM_CS_CurrentSequence_SIZE(1);
    ASSERT_TLM_CS_CurrentSequence(0, tlmFileName);
    // Assert the first command was sent
    Fw::ComBuffer comBuff;
    CommandBuffers::create(comBuff, 0, 1);
    ASSERT_from_comCmdOut_SIZE(1);
    ASSERT_from_comCmdOut(0, comBuff, 0U);
}

void CmdSequencerTester ::Cancel() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numRecords - 1;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedCancel(file, numCommands, bound);
}

void CmdSequencerTester ::FailedCommands() {
    const U32 numRecords = 3;
    const U32 numCommands = numRecords;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedFailedCommands(file, numCommands);
}

void CmdSequencerTester ::FileErrors() {
    const U32 numRecords = 5;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedFileErrors(file);
}

void CmdSequencerTester ::InvalidManualCommands() {
    const U32 numRecords = 5;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedInvalidManualCommands(file);
}

void CmdSequencerTester ::LoadOnInit() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedLoadOnInit(file, numCommands, bound);
}

void CmdSequencerTester ::LoadRunRun() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedLoadRunRun(file, numCommands, bound);
}

void CmdSequencerTester ::Manual() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedManual(file, numCommands);
}

void CmdSequencerTester ::NeverLoaded() {
    this->parameterizedNeverLoaded();
}

void CmdSequencerTester ::NewSequence() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedNewSequence(file, numCommands, bound);
}

void CmdSequencerTester ::SequenceTimeout() {
    const U32 numRecords = 5;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedSequenceTimeout(file);
}

void CmdSequencerTester ::UnexpectedCommandResponse() {
    const U32 numRecords = 5;
    const U32 numCommands = numRecords;
    const U32 bound = numCommands;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedUnexpectedCommandResponse(file, numCommands, bound);
}

void CmdSequencerTester ::Validate() {
    const U32 numRecords = 5;
    SequenceFiles::ImmediateFile file(numRecords, this->format);
    this->parameterizedValidate(file);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void CmdSequencerTester ::executeCommandsManual(const char* const fileName, const U32 numCommands) {
    for (U32 i = 0; i < numCommands; ++i) {
        PRINT("REC %d\n", i);
        // Check command buffer
        Fw::ComBuffer comBuff;
        CommandBuffers::create(comBuff, i, i + 1);
        ASSERT_from_comCmdOut_SIZE(1);
        ASSERT_from_comCmdOut(0, comBuff, 0U);
        // Assert that timer is clear
        ASSERT_EQ(CmdSequencerComponentImpl::Timer::CLEAR, this->component.m_cmdTimeoutTimer.m_state);
        // Send command response
        this->invoke_to_cmdResponseIn(0, i, 0, Fw::CmdResponse::OK);
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
        } else {
            // Assert events
            ASSERT_EVENTS_SIZE(2);
            ASSERT_EVENTS_CS_CommandComplete(0, fileName, i, i);
            ASSERT_EVENTS_CS_SequenceComplete_SIZE(1);
            // Assert telemetry
            ASSERT_TLM_SIZE(3);
            ASSERT_TLM_CS_CommandsExecuted(0, i + 1);
            ASSERT_TLM_CS_SequencesCompleted(0, 1);
            // Current sequence channel is cleared on completion
            ASSERT_TLM_CS_CurrentSequence(0, this->component.NO_SEQ.toChar());
        }
    }
}

}  // namespace Immediate

}  // namespace Svc

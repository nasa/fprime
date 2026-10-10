// ======================================================================
// \title  AMPCS.cpp
// \author Rob Bocchino
// \brief  AMPCS-specific tests
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/AMPCS.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {

namespace AMPCS {

// ----------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------

CmdSequencerTester ::CmdSequencerTester() : Svc::CmdSequencerTester(SequenceFiles::File::Format::AMPCS) {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void CmdSequencerTester ::MissingCRC() {
    // Write the file
    SequenceFiles::MissingCRCFile file(this->format);
    file.write();
    // Run the sequence
    this->sendCmd_CS_RUN(0, 0, file.getName(), Svc::BlockState::NO_BLOCK);
    this->clearAndDispatch();
    // Assert no response on seqDone
    ASSERT_from_seqDone_SIZE(0);
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, this->getRunOpcode(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    // Assert events
    Fw::String crcFileName(file.getName());
    crcFileName += ".CRC32";
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_FileNotFound(0, crcFileName.toChar());
    // Assert telemetry
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CS_Errors(0, 1);
}

void CmdSequencerTester ::MissingFile() {
    // Remove the file
    SequenceFiles::MissingFile file(this->format);
    file.write();
    file.remove();
    // Run the sequence
    this->sendCmd_CS_RUN(0, 0, file.getName(), Svc::BlockState::NO_BLOCK);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, this->getRunOpcode(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_FileInvalid(0, file.getName().toChar(), CmdSequencer_FileReadStage::READ_HEADER_SIZE,
                                 Os::FileSystem::DOESNT_EXIST);
    // Assert telemetry
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CS_Errors(0, 1);
}

void CmdSequencerTester ::CRCFileNameLimit() {
    // The longest sequence file name whose CRC file name fits in FileNameStringSize
    const FwSizeType crcSuffixLength = static_cast<FwSizeType>(sizeof(".CRC32") - 1);
    const FwSizeType longestLength = static_cast<FwSizeType>(FileNameStringSize) - crcSuffixLength;
    // Set the time
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Load a sequence whose file name is the longest that fits
    SequenceFiles::ImmediateFile longestFile(1, this->format);
    this->setFileNameLength(longestFile, longestLength);
    longestFile.write();
    this->clearHistory();
    this->component.loadSequence(longestFile.getName());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_SequenceLoaded_SIZE(1);
    // Run a sequence whose file name is one character longer. No file is written: its CRC file
    // name would exceed the Os::File path limit.
    SequenceFiles::ImmediateFile tooLongFile(1, this->format);
    this->setFileNameLength(tooLongFile, longestLength + 1);
    Fw::String tooLongName(tooLongFile.getName());
    Svc::SeqArgs emptyArgs{0, 0};
    this->invoke_to_seqRunIn(0, tooLongName, emptyArgs);
    this->clearAndDispatch();
    // Assert response on seqDone
    ASSERT_from_seqDone_SIZE(1);
    ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR));
    // Assert events. Events carry a shortened file name, so the stage and error are checked.
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EQ(1U, this->eventHistory_CS_FileInvalid->size());
    ASSERT_EQ(CmdSequencer_FileReadStage::READ_SEQ_CRC, this->eventHistory_CS_FileInvalid->at(0).stage.e);
    ASSERT_EQ(static_cast<I32>(Fw::FormatStatus::OVERFLOWED), this->eventHistory_CS_FileInvalid->at(0).error);
    // Assert telemetry
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CS_Errors(0, 1);
}

}  // namespace AMPCS

}  // namespace Svc

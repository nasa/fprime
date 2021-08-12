// ====================================================================== 
// \title  InvalidFiles.cpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Os/FileSystem.hpp"
#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/InvalidFiles.hpp"

namespace Svc {

  namespace InvalidFiles {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    Tester ::
      Tester(const SequenceFiles::File::Format::t format) :
        Svc::Tester(format)
    {

    }

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      BadCRC(void)
    {

      REQUIREMENT("ISF-CMDS-002");

      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
      this->setTestTime(testTime);
      // Write the file
      SequenceFiles::BadCRCFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      const CmdSequencerComponentImpl::FPrimeSequence::CRC& crc =
        file.getCRC();
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileCrcFailure(
          0,
          fileName,
          crc.m_stored,
          crc.m_computed
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void Tester ::
      BadRecordDescriptor(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      NATIVE_INT_TYPE numRecords = 1;
      SequenceFiles::BadDescriptorFile file(numRecords, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_FORMAT_ERROR
      );
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_FORMAT_ERROR
      );
    }

    void Tester ::
      BadTimeBase(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const NATIVE_INT_TYPE numRecords = 5;
      SequenceFiles::BadTimeBaseFile file(numRecords, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_TimeBaseMismatch_SIZE(1);
      ASSERT_EVENTS_CS_TimeBaseMismatch(
          0,
          fileName,
          TB_WORKSTATION_TIME,
          TB_PROC_TIME
      );
    }

    void Tester ::
      BadTimeContext(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 0, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const U32 numRecords = 5;
      SequenceFiles::BadTimeContextFile file(numRecords, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_TimeContextMismatch_SIZE(1);
      ASSERT_EVENTS_CS_TimeContextMismatch(0, fileName, 0, 1);
    }

    void Tester ::
      EmptyFile(void)
    {
      // Write the file
      SequenceFiles::EmptyFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          fileName,
          CmdSequencerComponentBase::SEQ_READ_HEADER_SIZE,
          Os::FileSystem::OP_OK
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void Tester ::
      DataAfterRecords(void)
    {

      REQUIREMENT("ISF-CMDS-001");

      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      const U32 numRecords = 5;
      SequenceFiles::DataAfterRecordsFile file(numRecords, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordMismatch_SIZE(1);
      ASSERT_EVENTS_CS_RecordMismatch(0, fileName, 5, 8);

    }

    void Tester ::
      FileTooLarge(void)
    {
      // Write the file
      SequenceFiles::TooLargeFile file(BUFFER_SIZE, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 0, 0);
      this->setTestTime(testTime);
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      const U32 dataSize = file.getDataSize();
      ASSERT_EVENTS_CS_FileSizeError(0, fileName, dataSize);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void Tester ::
      MissingCRC(void)
    {
      // Write the file
      SequenceFiles::MissingCRCFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert no response on seqDone
      ASSERT_from_seqDone_SIZE(0);
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          fileName,
          CmdSequencerComponentBase::SEQ_READ_SEQ_CRC,
          sizeof(U8)
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
      // Validate file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert no response on seqDone
      ASSERT_from_seqDone_SIZE(0);
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          fileName,
          CmdSequencerComponentBase::SEQ_READ_SEQ_CRC,
          sizeof(U8)
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 2);
      // Run the sequence by port call
      Fw::EightyCharString fArg(fileName);
      this->invoke_to_seqRunIn(0, fArg);
      this->clearAndDispatch();
      // Assert seqDone response
      ASSERT_from_seqDone_SIZE(1);
      ASSERT_from_seqDone(0U, 0U, 0U, Fw::COMMAND_EXECUTION_ERROR);
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          fileName,
          CmdSequencerComponentBase::SEQ_READ_SEQ_CRC,
          sizeof(U8)
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 3);
    }

    void Tester ::
      MissingFile(void)
    {
      // Remove the file
      SequenceFiles::MissingFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.remove();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileNotFound(0, fileName);
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void Tester ::
      SizeFieldTooLarge(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      SequenceFiles::SizeFieldTooLargeFile file(BUFFER_SIZE, this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
    }

    void Tester ::
      SizeFieldTooSmall(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      SequenceFiles::SizeFieldTooSmallFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
    }

    void Tester ::
      USecFieldTooShort(void)
    {
      // Set the time
      Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
      this->setTestTime(testTime);
      // Write the file
      SequenceFiles::USecFieldTooShortFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName,CmdSequencerComponentBase::SEQ_NO_BLOCK);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_RecordInvalid(
          0,
          fileName,
          0,
          Fw::FW_DESERIALIZE_SIZE_MISMATCH
      );
    }

  }

}

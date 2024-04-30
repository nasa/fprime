// \title  AMPCSSequence.cpp
// \author Rob Bocchino
// \brief  AMPCSSequence implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/FileSystem.hpp"
#include "Svc/CmdSequencer/formats/AMPCSSequence.hpp"
extern "C" {
#include "Utils/Hash/libcrc/lib_crc.h"
}

namespace Svc {

  AMPCSSequence ::
    AMPCSSequence(CmdSequencerComponentImpl& component) :
      CmdSequencerComponentImpl::Sequence(component)
  {

  }

  bool AMPCSSequence ::
    loadFile(const Fw::StringBase& fileName)
  {
    // Make sure there is a buffer allocated
    FW_ASSERT(this->m_buffer.getBuffAddr());

    Fw::CmdStringArg crcFileName(fileName);
    crcFileName += ".CRC32";

    this->m_header.m_timeBase = TB_DONT_CARE;
    this->m_header.m_timeContext = FW_CONTEXT_DONT_CARE;

    const bool status = this->readCRCFile(crcFileName)
      and this->getFileSize(fileName)
      and this->readSequenceFile(fileName)
      and this->validateCRC()
      and this->m_header.validateTime(this->m_component)
      and this->validateRecords();

    return status;
  }

  bool AMPCSSequence ::
    readCRCFile(Fw::CmdStringArg& crcFileName)
  {

    bool result;

    this->setFileName(crcFileName);

    Os::File::Status status = this->m_crcFile.open(
        crcFileName.toChar(),
        Os::File::OPEN_READ
    );

    if (status == Os::File::OP_OK) {
      result = this->readCRC() and this->deserializeCRC();
    } else if (status == Os::File::DOESNT_EXIST) {
      this->m_events.fileNotFound();
      result = false;
    } else {
      this->m_events.fileReadError();
      result = false;
    }

    this->m_crcFile.close();
    return result;

  }

  bool AMPCSSequence ::
    getFileSize(const Fw::StringBase& seqFileName)
  {
    bool status = true;
    FwSignedSizeType fileSize;
    this->setFileName(seqFileName);
    const Os::FileSystem::Status fileStatus =
      Os::FileSystem::getFileSize(this->m_fileName.toChar(), fileSize);
    if (
        fileStatus == Os::FileSystem::OP_OK and
        fileSize >= static_cast<FwSignedSizeType>(sizeof(this->m_sequenceHeader))
    ) {
      this->m_header.m_fileSize =
        static_cast<U32>(fileSize - static_cast<FwSignedSizeType>(sizeof(this->m_sequenceHeader)));
    }
    else {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_HEADER_SIZE, fileStatus
      );
      status = false;
    }
    return status;
  }

  bool AMPCSSequence ::
    readSequenceFile(const Fw::StringBase& seqFileName)
  {

    bool result;

    this->setFileName(seqFileName);
    Os::File::Status status = this->m_sequenceFile.open(
        this->m_fileName.toChar(),
        Os::File::OPEN_READ
    );

    if (status == Os::File::OP_OK) {
      result = this->readOpenSequenceFile();
    } else if (status == Os::File::DOESNT_EXIST) {
      this->m_events.fileNotFound();
      result = false;
    } else {
      this->m_events.fileReadError();
      result = false;
    }

    this->m_sequenceFile.close();
    return result;

  }

  bool AMPCSSequence ::
    validateCRC()
  {
    bool result = true;
    if (this->m_crc.m_stored != this->m_crc.m_computed) {
      this->m_events.fileCRCFailure(
          this->m_crc.m_stored,
          this->m_crc.m_computed
      );
      result = false;
    }
    return result;
  }

  bool AMPCSSequence ::
    validateRecords()
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Sequence::Record record;

    // Deserialize all records and count the records
    const NATIVE_UINT_TYPE loopBound = buffer.getBuffLeft();
    U32 numRecords = 0;
    for ( ; numRecords < loopBound; ++numRecords) {
      if (not this->hasMoreRecords()) {
        break;
      }
      Fw::SerializeStatus status = this->deserializeRecord(record);
      if (status != Fw::FW_SERIALIZE_OK) {
        this->m_events.recordInvalid(numRecords, status);
        return false;
      }
    }
    // Set the number of records
    this->m_header.m_numRecords = numRecords;
    // Reset deserialization
    this->reset();

    return true;
  }

  bool AMPCSSequence ::
    hasMoreRecords() const
  {
    return this->m_buffer.getBuffLeft() > 0;
  }

  void AMPCSSequence ::
     nextRecord(Sequence::Record& record)
  {
    Fw::SerializeStatus status = this->deserializeRecord(record);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
  }

  void AMPCSSequence ::
    reset()
  {
    this->m_buffer.resetDeser();
  }

  void AMPCSSequence ::
    clear()
  {
    this->m_buffer.resetSer();
  }

  bool AMPCSSequence ::
    readCRC()
  {

    Os::File& file = this->m_crcFile;
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    bool status = true;
    Fw::SerializeStatus ser_status;

    FwSignedSizeType readLen = sizeof(U32);
    FW_ASSERT(readLen >= 0, static_cast<FwAssertArgType>(readLen));

    ser_status = buffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(ser_status == Fw::FW_SERIALIZE_OK, ser_status);

    U8 *const addr = buffer.getBuffAddr();
    Os::File::Status fileStatus = file.read(addr, readLen);

    if (fileStatus != Os::File::OP_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_CRC,
          fileStatus
      );
      status = false;
    }

    return status;

  }

  bool AMPCSSequence ::
    deserializeCRC()
  {
    bool status = true;
    Fw::SerializeStatus serializeStatus =
      this->m_buffer.deserialize(this->m_crc.m_stored);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_CRC,
          serializeStatus
      );
      status = false;
    }
    return status;
  }

  bool AMPCSSequence ::
    readOpenSequenceFile()
  {
    this->m_buffer.resetSer();
    this->m_crc.init();
    bool status = this->readSequenceHeader();
    if (status) {
      this->m_crc.update(
          this->m_sequenceHeader,
          sizeof(this->m_sequenceHeader)
      );
      status = this->readRecords();
    }
    if (status) {
      U8 *const buffAddr = this->m_buffer.getBuffAddr();
      const NATIVE_UINT_TYPE buffLen = this->m_buffer.getBuffLength();
      FW_ASSERT(
          buffLen == this->m_header.m_fileSize,
          static_cast<FwAssertArgType>(buffLen),
          static_cast<FwAssertArgType>(this->m_header.m_fileSize)
      );
      this->m_crc.update(buffAddr, buffLen);
      this->m_crc.finalize();
    }
    return status;
  }

  bool AMPCSSequence ::
    readSequenceHeader()
  {

    Os::File& file = this->m_sequenceFile;

    bool status = true;

    FwSignedSizeType readLen = sizeof this->m_sequenceHeader;
    const Os::File::Status fileStatus = file.read(
        this->m_sequenceHeader,
        readLen
    );

    if (fileStatus != Os::File::OP_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_HEADER,
          fileStatus
      );
      status = false;
    }

    if (status and readLen != sizeof this->m_sequenceHeader) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_HEADER_SIZE,
          static_cast<I32>(readLen)
      );
      status = false;
    }

    return status;

  }


  bool AMPCSSequence ::
    readRecords()
  {
    Os::File& file = this->m_sequenceFile;
    const NATIVE_UINT_TYPE size = this->m_header.m_fileSize;
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    U8 *const addr = buffer.getBuffAddr();

    // Check file size
    if (size > this->m_buffer.getBuffCapacity()) {
      this->m_events.fileSizeError(size);
      return false;
    }

    FwSignedSizeType readLen = size;
    const Os::File::Status fileStatus = file.read(addr, readLen);
    // Check read status
    if (fileStatus != Os::File::OP_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_DATA,
          fileStatus
      );
      return false;
    }
    // Check read size
    const NATIVE_UINT_TYPE readLenUint = static_cast<NATIVE_UINT_TYPE>(readLen);
    if (readLenUint != size) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_DATA_SIZE,
          static_cast<I32>(readLen)
      );
      return false;
    }
    // set buffer size
    const Fw::SerializeStatus serializeStatus = buffer.setBuffLen(size);
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);
    return true;
  }

  Fw::SerializeStatus AMPCSSequence ::
    deserializeRecord(Sequence::Record& record)
  {

    Record::CmdLength::t cmdLength;

    Fw::SerializeStatus status =
     this->deserializeTimeFlag(record.m_descriptor);

    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->deserializeTime(record.m_timeTag);
    }
    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->deserializeCmdLength(cmdLength);
    }
    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->translateCommand(record.m_command, cmdLength);
    }

    return status;

  }

  Fw::SerializeStatus AMPCSSequence ::
    deserializeTimeFlag(Sequence::Record::Descriptor& descriptor)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Record::TimeFlag::Serial::t timeFlagSerial;
    Fw::SerializeStatus status = buffer.deserialize(timeFlagSerial);
    if (status == Fw::FW_SERIALIZE_OK) {
      switch (timeFlagSerial) {
        case Record::TimeFlag::ABSOLUTE:
          descriptor = Sequence::Record::ABSOLUTE;
          break;
        case Record::TimeFlag::RELATIVE:
          descriptor = Sequence::Record::RELATIVE;
          break;
        default:
          status = Fw::FW_DESERIALIZE_FORMAT_ERROR;
          break;
      }
    }
    return status;
  }

  Fw::SerializeStatus AMPCSSequence ::
    deserializeTime(Fw::Time& timeTag)
  {
    Record::Time::t time;
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Fw::SerializeStatus status = buffer.deserialize(time);
    if (status == Fw::FW_SERIALIZE_OK) {
      timeTag.set(time, 0);
    }
    return status;
  }

  Fw::SerializeStatus AMPCSSequence ::
    deserializeCmdLength(Record::CmdLength::t& cmdLength)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Fw::SerializeStatus status = buffer.deserialize(cmdLength);
    if (status == Fw::FW_SERIALIZE_OK and cmdLength > buffer.getBuffLeft()) {
      // Not enough data left
      status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    if (
      status == Fw::FW_SERIALIZE_OK and
      sizeof(FwPacketDescriptorType) + sizeof(U16) + cmdLength > Fw::ComBuffer::SERIALIZED_SIZE
    ) {
      // Record size is too big for com buffer
      status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    return status;
  }

  Fw::SerializeStatus AMPCSSequence ::
    translateCommand(
        Fw::ComBuffer& comBuffer,
        const Record::CmdLength::t cmdLength
    )
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    comBuffer.resetSer();
    // Serialize the command packet descriptor
    const FwPacketDescriptorType cmdDescriptor = Fw::ComPacket::FW_PACKET_COMMAND;
    Fw::SerializeStatus status = comBuffer.serialize(cmdDescriptor);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Zero-extend the two-byte AMPCS opcode by two bytes
    const U16 zeros = 0;
    status = comBuffer.serialize(zeros);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Set the buffer length
    const U32 fixedBuffLen = comBuffer.getBuffLength();
    FW_ASSERT(
        fixedBuffLen == sizeof(cmdDescriptor) + sizeof(zeros),
        static_cast<FwAssertArgType>(fixedBuffLen)
    );
    const U32 totalBuffLen = fixedBuffLen + cmdLength;
    status = comBuffer.setBuffLen(totalBuffLen);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Copy the opcode and argument bytes
    NATIVE_UINT_TYPE size = cmdLength;
    U8 *const addr = comBuffer.getBuffAddr();
    FW_ASSERT(addr != nullptr);
    // true means "don't serialize the length"
    status = buffer.deserialize(&addr[fixedBuffLen], size, true);
    return status;
  }

}


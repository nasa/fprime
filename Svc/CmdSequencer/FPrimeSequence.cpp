// ======================================================================
// \title  FPrimeSequence.cpp
// \author Bocchino/Canham
// \brief  CmdSequencerComponentImpl::FPrimeSequence implementation
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
extern "C" {
#include "Utils/Hash/libcrc/lib_crc.h"
}

namespace Svc {

  CmdSequencerComponentImpl::FPrimeSequence::CRC ::
    CRC() :
      m_computed(INITIAL_COMPUTED_VALUE),
      m_stored(0)
  {

  }

  void CmdSequencerComponentImpl::FPrimeSequence::CRC ::
    init()
  {
    this->m_computed = INITIAL_COMPUTED_VALUE;
  }

  void CmdSequencerComponentImpl::FPrimeSequence::CRC ::
    update(const BYTE* buffer, NATIVE_UINT_TYPE bufferSize)
  {
    FW_ASSERT(buffer);
    for(NATIVE_UINT_TYPE index = 0; index < bufferSize; index++) {
      this->m_computed = static_cast<U32>(update_crc_32(this->m_computed, static_cast<char>(buffer[index])));
    }
  }

  void CmdSequencerComponentImpl::FPrimeSequence::CRC ::
    finalize()
  {
    this->m_computed = ~this->m_computed;
  }

  CmdSequencerComponentImpl::FPrimeSequence ::
    FPrimeSequence(CmdSequencerComponentImpl& component) :
      Sequence(component)
  {

  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
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

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    loadFile(const Fw::StringBase& fileName)
  {

    // make sure there is a buffer allocated
    FW_ASSERT(this->m_buffer.getBuffAddr());

    this->setFileName(fileName);

    const bool status = this->readFile()
     and this->validateCRC()
     and this->m_header.validateTime(this->m_component)
     and this->validateRecords();

    return status;

  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    hasMoreRecords() const
  {
    return this->m_buffer.getBuffLeft() > 0;
  }

  void CmdSequencerComponentImpl::FPrimeSequence ::
     nextRecord(Record& record)
  {
    Fw::SerializeStatus status = this->deserializeRecord(record);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
  }

  void CmdSequencerComponentImpl::FPrimeSequence ::
    reset()
  {
    this->m_buffer.resetDeser();
  }

  void CmdSequencerComponentImpl::FPrimeSequence ::
    clear()
  {
    this->m_buffer.resetSer();
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    readFile()
  {

    bool result;

    Os::File::Status status = this->m_sequenceFile.open(
      this->m_fileName.toChar(),
      Os::File::OPEN_READ
    );

    if (status == Os::File::OP_OK) {
      result = this->readOpenFile();
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

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    readOpenFile()
  {
    U8 *const buffAddr = this->m_buffer.getBuffAddr();
    this->m_crc.init();
    bool status = this->readHeader();
    if (status) {
      this->m_crc.update(buffAddr, Sequence::Header::SERIALIZED_SIZE);
      status = this->deserializeHeader()
        and this->readRecordsAndCRC()
        and this->extractCRC();
    }
    if (status) {
      const NATIVE_UINT_TYPE buffLen = this->m_buffer.getBuffLength();
      this->m_crc.update(buffAddr, buffLen);
      this->m_crc.finalize();
    }
    return status;
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    readHeader()
  {

    Os::File& file = this->m_sequenceFile;
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    bool status = true;

    FwSignedSizeType readLen = Sequence::Header::SERIALIZED_SIZE;
    FW_ASSERT(readLen >= 0, static_cast<FwAssertArgType>(readLen));

    const NATIVE_UINT_TYPE capacity = buffer.getBuffCapacity();
    FW_ASSERT(
        capacity >= static_cast<NATIVE_UINT_TYPE>(readLen),
        static_cast<FwAssertArgType>(capacity),
        static_cast<FwAssertArgType>(readLen)
    );
    Os::File::Status fileStatus = file.read(
        buffer.getBuffAddr(),
        readLen
    );

    if (fileStatus != Os::File::OP_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_HEADER,
          fileStatus
      );
      status = false;
    }

    if (status and readLen != Sequence::Header::SERIALIZED_SIZE) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_HEADER_SIZE,
          static_cast<I32>(readLen)
      );
      status = false;
    }

    if (status) {
      const Fw::SerializeStatus serializeStatus =
        buffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
      FW_ASSERT(
          serializeStatus == Fw::FW_SERIALIZE_OK,
          serializeStatus
      );
    }

    return status;
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    deserializeHeader()
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Header& header = this->m_header;

    // File size
    Fw::SerializeStatus serializeStatus = buffer.deserialize(header.m_fileSize);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::DESER_SIZE,
          serializeStatus
      );
      return false;
    }
    if (header.m_fileSize > buffer.getBuffCapacity()) {
      this->m_events.fileSizeError(header.m_fileSize);
      return false;
    }
    // Number of records
    serializeStatus = buffer.deserialize(header.m_numRecords);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::DESER_NUM_RECORDS,
          serializeStatus
      );
      return false;
    }
    // Time base
    FwTimeBaseStoreType tbase;
    serializeStatus = buffer.deserialize(tbase);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::DESER_TIME_BASE,
          serializeStatus
      );
      return false;
    }
    header.m_timeBase = static_cast<TimeBase>(tbase);
    // Time context
    serializeStatus = buffer.deserialize(header.m_timeContext);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::DESER_TIME_CONTEXT,
          serializeStatus
      );
      return false;
    }
    return true;
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    readRecordsAndCRC()
  {
    Os::File& file = this->m_sequenceFile;
    const NATIVE_UINT_TYPE size = this->m_header.m_fileSize;
    Fw::SerializeBufferBase& buffer = this->m_buffer;

    FwSignedSizeType readLen = size;
    Os::File::Status fileStatus = file.read(
      buffer.getBuffAddr(),
      readLen
    );
    // check read status
    if (fileStatus != Os::File::OP_OK) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_DATA,
          fileStatus
      );
      return false;
    }
    // check read size
    if (static_cast<NATIVE_INT_TYPE>(size) != readLen) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_DATA_SIZE,
          static_cast<I32>(readLen)
      );
      return false;
    }
    // set buffer size
    Fw::SerializeStatus serializeStatus =
     buffer.setBuffLen(size);
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);
    return true;
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    extractCRC()
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    U32& crc = this->m_crc.m_stored;

    // Compute the data size
    const U32 buffSize = buffer.getBuffLength();
    const U32 crcSize = sizeof(crc);
    U8 *const buffAddr = buffer.getBuffAddr();
    if (buffSize < crcSize) {
      this->m_events.fileInvalid(
          CmdSequencer_FileReadStage::READ_SEQ_CRC,
          static_cast<I32>(buffSize)
      );
      return false;
    }
    FW_ASSERT(buffSize >= crcSize, static_cast<FwAssertArgType>(buffSize), crcSize);
    const NATIVE_UINT_TYPE dataSize = buffSize - crcSize;
    // Create a CRC buffer pointing at the CRC in the main buffer, after the data
    Fw::ExternalSerializeBuffer crcBuff(&buffAddr[dataSize], crcSize);
    Fw::SerializeStatus status = crcBuff.setBuffLen(crcSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Deserialize the CRC from the CRC buffer
    status = crcBuff.deserialize(crc);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Set the main buffer size to the data size
    status = buffer.setBuffLen(dataSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    return true;
  }

  Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::
    deserializeRecord(Record& record)
  {
    U32 recordSize;

    Fw::SerializeStatus status =
     this->deserializeDescriptor(record.m_descriptor);

    if (
      status == Fw::FW_SERIALIZE_OK and
      record.m_descriptor == Record::END_OF_SEQUENCE
    ) {
      return Fw::FW_SERIALIZE_OK;
    }

    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->deserializeTimeTag(record.m_timeTag);
    }
    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->deserializeRecordSize(recordSize);
    }
    if (status == Fw::FW_SERIALIZE_OK) {
     status = this->copyCommand(record.m_command, recordSize);
    }

    return status;
  }

  Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::
    deserializeDescriptor(Record::Descriptor& descriptor)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    U8 descEntry;

    Fw::SerializeStatus status = buffer.deserialize(descEntry);
    if (status != Fw::FW_SERIALIZE_OK) {
      return status;
    }

    if (descEntry > Sequence::Record::END_OF_SEQUENCE) {
      return Fw::FW_DESERIALIZE_FORMAT_ERROR;
    }

    descriptor = static_cast<Record::Descriptor>(descEntry);
    return Fw::FW_SERIALIZE_OK;
  }

  Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::
    deserializeTimeTag(Fw::Time& timeTag)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    U32 seconds, useconds;
    Fw::SerializeStatus status = buffer.deserialize(seconds);
    if (status == Fw::FW_SERIALIZE_OK) {
      status = buffer.deserialize(useconds);
    }
    if (status == Fw::FW_SERIALIZE_OK) {
      timeTag.set(seconds,useconds);
    }
    return status;
  }

  Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::
    deserializeRecordSize(U32& recordSize)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    Fw::SerializeStatus status = buffer.deserialize(recordSize);
    if (status == Fw::FW_SERIALIZE_OK and recordSize > buffer.getBuffLeft()) {
      // Not enough data left
      status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    if (
      status == Fw::FW_SERIALIZE_OK and
      recordSize + sizeof(FwPacketDescriptorType) > Fw::ComBuffer::SERIALIZED_SIZE
    ) {
      // Record size is too big for com buffer
      status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }
    return status;
  }

  Fw::SerializeStatus CmdSequencerComponentImpl::FPrimeSequence ::
    copyCommand(Fw::ComBuffer& comBuffer, const U32 recordSize)
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    comBuffer.resetSer();
    NATIVE_UINT_TYPE size = recordSize;
    Fw::SerializeStatus status = comBuffer.setBuffLen(recordSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = buffer.deserialize(comBuffer.getBuffAddr(), size, true);
    return status;
  }

  bool CmdSequencerComponentImpl::FPrimeSequence ::
    validateRecords()
  {
    Fw::SerializeBufferBase& buffer = this->m_buffer;
    const U32 numRecords = this->m_header.m_numRecords;
    Sequence::Record record;

    // Deserialize all records
    for (NATIVE_UINT_TYPE recordNumber = 0; recordNumber < numRecords; recordNumber++) {
      Fw::SerializeStatus status = this->deserializeRecord(record);
      if (status != Fw::FW_SERIALIZE_OK) {
        this->m_events.recordInvalid(recordNumber, status);
        return false;
      }
    }
    // Check there is no data left
    const U32 buffLeftSize = buffer.getBuffLeft();
    if (buffLeftSize > 0) {
      this->m_events.recordMismatch(numRecords, buffLeftSize);
      return false;
    }
    // Rewind deserialization
    buffer.resetDeser();

    return true;
  }

}


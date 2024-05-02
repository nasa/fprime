// ======================================================================
// \title  DataPacket.cpp
// \author bocchino
// \brief  cpp file for FilePacket::DataPacket
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

  void FilePacket::DataPacket ::
    initialize(
        const U32 sequenceIndex,
        const U32 byteOffset,
        const U16 dataSize,
        const U8 *const data
    )
  {
    this->m_header.initialize(FilePacket::T_DATA, sequenceIndex);
    this->m_byteOffset = byteOffset;
    this->m_dataSize = dataSize;
    this->m_data = data;
  }

  U32 FilePacket::DataPacket ::
    bufferSize() const
  {
    return static_cast<U32>(
      this->m_header.bufferSize() +
      sizeof(this->m_byteOffset) +
      sizeof(this->m_dataSize) +
      this->m_dataSize);
  }

  SerializeStatus FilePacket::DataPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        buffer.getData(),
        buffer.getSize()
    );
    return this->toSerialBuffer(serialBuffer);
  }

  SerializeStatus FilePacket::DataPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    FW_ASSERT(this->m_header.m_type == T_DATA);

    SerializeStatus status = serialBuffer.deserialize(this->m_byteOffset);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    status = serialBuffer.deserialize(this->m_dataSize);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    if (serialBuffer.getBuffLeft() != this->m_dataSize) {
      return FW_DESERIALIZE_SIZE_MISMATCH;
    }

    U8 *const addr = serialBuffer.getBuffAddr();
    this->m_data = &addr[this->fixedLengthSize()];

    return FW_SERIALIZE_OK;

  }

  U32 FilePacket::DataPacket ::
    fixedLengthSize() const
  {
    return static_cast<U32>(
      this->m_header.bufferSize() +
      sizeof(this->m_byteOffset) +
      sizeof(this->m_dataSize));
  }

  SerializeStatus FilePacket::DataPacket ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    FW_ASSERT(this->m_header.m_type == T_DATA);

    SerializeStatus status;

    status = this->m_header.toSerialBuffer(serialBuffer);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    status = serialBuffer.serialize(this->m_byteOffset);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    status = serialBuffer.serialize(this->m_dataSize);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    status = serialBuffer.pushBytes(this->m_data, this->m_dataSize);

    return status;

  }

}

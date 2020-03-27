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
        const U32 dataSize,
        const U8 *const data
    )
  {
    this->header.initialize(FilePacket::T_DATA, sequenceIndex);
    this->byteOffset = byteOffset;
    this->dataSize = dataSize;
    this->data = data;
  }

  U32 FilePacket::DataPacket ::
    bufferSize(void) const
  {
    return
      this->header.bufferSize() +
      sizeof(this->byteOffset) +
      sizeof(this->dataSize) +
      this->dataSize;
  }

  SerializeStatus FilePacket::DataPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        reinterpret_cast<U8*>(buffer.getdata()),
        buffer.getsize()
    );
    return this->toSerialBuffer(serialBuffer);
  }

  SerializeStatus FilePacket::DataPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    FW_ASSERT(this->header.type == T_DATA);

    SerializeStatus status = serialBuffer.deserialize(this->byteOffset);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.deserialize(this->dataSize);
    if (status != FW_SERIALIZE_OK)
      return status;

    if (serialBuffer.getBuffLeft() != this->dataSize)
      return FW_DESERIALIZE_SIZE_MISMATCH;

    U8 *const addr = serialBuffer.getBuffAddr();
    this->data = &addr[this->fixedLengthSize()];

    return FW_SERIALIZE_OK;

  }

  U32 FilePacket::DataPacket ::
    fixedLengthSize(void) const
  {
    return
      this->header.bufferSize() +
      sizeof(this->byteOffset) + 
      sizeof(this->dataSize);
  }

  SerializeStatus FilePacket::DataPacket ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    FW_ASSERT(this->header.type == T_DATA);

    SerializeStatus status;

    status = this->header.toSerialBuffer(serialBuffer);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.serialize(this->byteOffset);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.serialize(this->dataSize);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.pushBytes(this->data, dataSize);

    return status;

  }

}

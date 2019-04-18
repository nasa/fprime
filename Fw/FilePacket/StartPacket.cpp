// ====================================================================== 
// \title  StartPacket.cpp
// \author bocchino
// \brief  cpp file for FilePacket::StartPacket
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

  void FilePacket::StartPacket ::
    initialize(
      const U32 fileSize,
      const char *const sourcePath,
      const char *const destinationPath
    )
  {
    const FilePacket::Header header = { FilePacket::T_START, 0 };
    this->header = header;
    this->fileSize = fileSize;
    this->sourcePath.initialize(sourcePath);
    this->destinationPath.initialize(destinationPath);
  }

  U32 FilePacket::StartPacket ::
    bufferSize(void) const
  {
    return this->header.bufferSize() +
      sizeof(this->fileSize) +
      this->sourcePath.bufferSize() +
      this->destinationPath.bufferSize();
  }

  SerializeStatus FilePacket::StartPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        reinterpret_cast<U8*>(buffer.getdata()),
        buffer.getsize()
    );
    return this->toSerialBuffer(serialBuffer);
  }

  SerializeStatus FilePacket::StartPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer) 
  {

    FW_ASSERT(this->header.type == T_START);

    {
      const SerializeStatus status = 
        serialBuffer.deserialize(this->fileSize);
      if (status != FW_SERIALIZE_OK)
        return status;
    }
      
    {
      const SerializeStatus status = 
        this->sourcePath.fromSerialBuffer(serialBuffer);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status = 
        this->destinationPath.fromSerialBuffer(serialBuffer);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    return FW_SERIALIZE_OK;

  }

  SerializeStatus FilePacket::StartPacket ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    FW_ASSERT(this->header.type == T_START);

    {
      const SerializeStatus status = 
        this->header.toSerialBuffer(serialBuffer);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status = 
        serialBuffer.serialize(this->fileSize);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status =
        this->sourcePath.toSerialBuffer(serialBuffer);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status =
        this->destinationPath.toSerialBuffer(serialBuffer);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    return FW_SERIALIZE_OK;

  }

}

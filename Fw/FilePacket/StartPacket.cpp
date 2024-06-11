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
    this->m_header.initialize(FilePacket::T_START, 0);
    this->m_fileSize = fileSize;
    this->m_sourcePath.initialize(sourcePath);
    this->m_destinationPath.initialize(destinationPath);
  }

  U32 FilePacket::StartPacket ::
    bufferSize() const
  {
    return static_cast<U32>(
      this->m_header.bufferSize() +
      sizeof(this->m_fileSize) +
      this->m_sourcePath.bufferSize() +
      this->m_destinationPath.bufferSize());
  }

  SerializeStatus FilePacket::StartPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        buffer.getData(),
        buffer.getSize()
    );
    return this->toSerialBuffer(serialBuffer);
  }

  SerializeStatus FilePacket::StartPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    FW_ASSERT(this->m_header.m_type == T_START);

    {
      const SerializeStatus status =
        serialBuffer.deserialize(this->m_fileSize);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    {
      const SerializeStatus status =
        this->m_sourcePath.fromSerialBuffer(serialBuffer);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    {
      const SerializeStatus status =
        this->m_destinationPath.fromSerialBuffer(serialBuffer);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    return FW_SERIALIZE_OK;

  }

  SerializeStatus FilePacket::StartPacket ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    FW_ASSERT(this->m_header.m_type == T_START);

    {
      const SerializeStatus status =
        this->m_header.toSerialBuffer(serialBuffer);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    {
      const SerializeStatus status =
        serialBuffer.serialize(this->m_fileSize);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    {
      const SerializeStatus status =
        this->m_sourcePath.toSerialBuffer(serialBuffer);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    {
      const SerializeStatus status =
        this->m_destinationPath.toSerialBuffer(serialBuffer);

      if (status != FW_SERIALIZE_OK) {
        return status;
      }
    }

    return FW_SERIALIZE_OK;

  }

}

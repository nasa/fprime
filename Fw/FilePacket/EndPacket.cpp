// ======================================================================
// \title  EndPacket.cpp
// \author bocchino
// \brief  cpp file for FilePacket::EndPacket
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cstring>

#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

  void FilePacket::EndPacket ::
    initialize(
        const U32 sequenceIndex,
        const CFDP::Checksum& checksum
    )
  {
    this->m_header.initialize(FilePacket::T_END, sequenceIndex);
    this->setChecksum(checksum);
  }

  U32 FilePacket::EndPacket ::
    bufferSize() const
  {
    return this->m_header.bufferSize() + sizeof(this->m_checksumValue);
  }

  SerializeStatus FilePacket::EndPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        buffer.getData(),
        buffer.getSize()
    );
    return this->toSerialBuffer(serialBuffer);
  }

  void FilePacket::EndPacket ::
    setChecksum(const CFDP::Checksum& checksum)
  {
    this->m_checksumValue = checksum.getValue();
  }


  void FilePacket::EndPacket ::
    getChecksum(CFDP::Checksum& checksum) const
  {
    CFDP::Checksum c(this->m_checksumValue);
    checksum = c;
  }

  SerializeStatus FilePacket::EndPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    FW_ASSERT(this->m_header.m_type == T_END);

    const SerializeStatus status =
      serialBuffer.deserialize(this->m_checksumValue);

    return status;

  }

  SerializeStatus FilePacket::EndPacket ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    FW_ASSERT(this->m_header.m_type == T_END);

    SerializeStatus status;

    status = this->m_header.toSerialBuffer(serialBuffer);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.serialize(this->m_checksumValue);
    if (status != FW_SERIALIZE_OK)
      return status;

    return FW_SERIALIZE_OK;

  }

}

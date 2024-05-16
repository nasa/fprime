// ======================================================================
// \title  CancelPacket.cpp
// \author bocchino
// \brief  cpp file for FilePacket::CancelPacket
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

  void FilePacket::CancelPacket ::
    initialize(const U32 sequenceIndex)
  {
    this->m_header.initialize(FilePacket::T_CANCEL, sequenceIndex);
  }

  U32 FilePacket::CancelPacket ::
    bufferSize() const
  {
    return this->m_header.bufferSize();
  }

  SerializeStatus FilePacket::CancelPacket ::
    toBuffer(Buffer& buffer) const
  {
    SerialBuffer serialBuffer(
        buffer.getData(),
        buffer.getSize()
    );
    return this->m_header.toSerialBuffer(serialBuffer);
  }

  SerializeStatus FilePacket::CancelPacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    FW_ASSERT(this->m_header.m_type == T_CANCEL);

    if (serialBuffer.getBuffLeft() != 0) {
      return FW_DESERIALIZE_SIZE_MISMATCH;
    }

    return FW_SERIALIZE_OK;

  }

}

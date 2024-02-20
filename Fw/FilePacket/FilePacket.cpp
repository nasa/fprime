// ======================================================================
// \title  FilePacket.cpp
// \author bocchino
// \brief  cpp file for FilePacket
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/FilePacket/FilePacket.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

  // ----------------------------------------------------------------------
  // Public instance methods
  // ----------------------------------------------------------------------

  SerializeStatus FilePacket ::
    fromBuffer(const Buffer& buffer)
  {
    SerialBuffer serialBuffer(
        const_cast<Buffer&>(buffer).getData(),
        const_cast<Buffer&>(buffer).getSize()
    );
    serialBuffer.fill();
    const SerializeStatus status = this->fromSerialBuffer(serialBuffer);
    return status;
  }

  const FilePacket::Header& FilePacket ::
    asHeader() const
  {
    return this->m_header;
  }

  const FilePacket::StartPacket& FilePacket ::
    asStartPacket() const
  {
    FW_ASSERT(this->m_header.m_type == T_START);
    return this->m_startPacket;
  }

  const FilePacket::DataPacket& FilePacket ::
    asDataPacket() const
  {
    FW_ASSERT(this->m_header.m_type == T_DATA);
    return this->m_dataPacket;
  }

  const FilePacket::EndPacket& FilePacket ::
    asEndPacket() const
  {
    FW_ASSERT(this->m_header.m_type == T_END);
    return this->m_endPacket;
  }

  const FilePacket::CancelPacket& FilePacket ::
    asCancelPacket() const
  {
    FW_ASSERT(this->m_header.m_type == T_CANCEL);
    return this->m_cancelPacket;
  }

  void FilePacket ::
    fromStartPacket(const StartPacket& startPacket)
  {
    this->m_startPacket = startPacket;
    this->m_header.m_type = T_START;
  }

  void FilePacket ::
    fromDataPacket(const DataPacket& dataPacket)
  {
    this->m_dataPacket = dataPacket;
    this->m_header.m_type = T_DATA;
  }

  void FilePacket ::
    fromEndPacket(const EndPacket& endPacket)
  {
    this->m_endPacket = endPacket;
    this->m_header.m_type = T_END;
  }

  void FilePacket ::
    fromCancelPacket(const CancelPacket& cancelPacket)
  {
    this->m_cancelPacket = cancelPacket;
    this->m_header.m_type = T_CANCEL;
  }

  U32 FilePacket ::
    bufferSize() const
  {
    switch (this->m_header.m_type) {
      case T_START:
        return this->m_startPacket.bufferSize();
      case T_DATA:
        return this->m_dataPacket.bufferSize();
      case T_END:
        return this->m_endPacket.bufferSize();
      case T_CANCEL:
        return this->m_cancelPacket.bufferSize();
      case T_NONE:
        return 0;
      default:
        FW_ASSERT(0);
        return 0;
    }
  }

  SerializeStatus FilePacket ::
    toBuffer(Buffer& buffer) const
  {
    switch (this->m_header.m_type) {
      case T_START:
        return this->m_startPacket.toBuffer(buffer);
      case T_DATA:
        return this->m_dataPacket.toBuffer(buffer);
      case T_END:
        return this->m_endPacket.toBuffer(buffer);
      case T_CANCEL:
        return this->m_cancelPacket.toBuffer(buffer);
      default:
        FW_ASSERT(0);
        return static_cast<SerializeStatus>(0);
    }
  }

  // ----------------------------------------------------------------------
  // Private instance methods
  // ----------------------------------------------------------------------

  SerializeStatus FilePacket ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {
    SerializeStatus status;
    status = this->m_header.fromSerialBuffer(serialBuffer);
    if (status != FW_SERIALIZE_OK)
      return status;
    switch (this->m_header.m_type) {
      case T_START:
        status = this->m_startPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_DATA:
        status = this->m_dataPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_END:
        status = this->m_endPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_CANCEL:
        status = this->m_cancelPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_NONE:
        status = FW_DESERIALIZE_TYPE_MISMATCH;
        break;
      default:
        FW_ASSERT(0,status);
        break;
    }
    return status;
  }

}

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
        reinterpret_cast<U8*>(const_cast<Buffer&>(buffer).getdata()),
        const_cast<Buffer&>(buffer).getsize()
    );
    serialBuffer.fill();
    const SerializeStatus status = this->fromSerialBuffer(serialBuffer);
    return status;
  }

  const FilePacket::Header& FilePacket ::
    asHeader(void) const
  {
    return this->header;
  }

  const FilePacket::StartPacket& FilePacket ::
    asStartPacket(void) const
  {
    FW_ASSERT(this->header.type == T_START);
    return this->startPacket;
  }

  const FilePacket::DataPacket& FilePacket ::
    asDataPacket(void) const
  {
    FW_ASSERT(this->header.type == T_DATA);
    return this->dataPacket;
  }

  const FilePacket::EndPacket& FilePacket ::
    asEndPacket(void) const
  {
    FW_ASSERT(this->header.type == T_END);
    return this->endPacket;
  }

  const FilePacket::CancelPacket& FilePacket ::
    asCancelPacket(void) const
  {
    FW_ASSERT(this->header.type == T_CANCEL);
    return this->cancelPacket;
  }

  void FilePacket ::
    fromStartPacket(const StartPacket& startPacket)
  {
    this->startPacket = startPacket;
    this->header.type = T_START;
  }

  void FilePacket ::
    fromDataPacket(const DataPacket& dataPacket)
  {
    this->dataPacket = dataPacket;
    this->header.type = T_DATA;
  }

  void FilePacket ::
    fromEndPacket(const EndPacket& endPacket)
  {
    this->endPacket = endPacket;
    this->header.type = T_END;
  }

  void FilePacket ::
    fromCancelPacket(const CancelPacket& cancelPacket)
  {
    this->cancelPacket = cancelPacket;
    this->header.type = T_CANCEL;
  }

  U32 FilePacket ::
    bufferSize(void) const
  {
    switch (this->header.type) {
      case T_START:
        return this->startPacket.bufferSize();
      case T_DATA:
        return this->dataPacket.bufferSize();
      case T_END:
        return this->endPacket.bufferSize();
      case T_CANCEL:
        return this->cancelPacket.bufferSize();
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
    switch (this->header.type) {
      case T_START:
        return this->startPacket.toBuffer(buffer);
      case T_DATA:
        return this->dataPacket.toBuffer(buffer);
      case T_END:
        return this->endPacket.toBuffer(buffer);
      case T_CANCEL:
        return this->cancelPacket.toBuffer(buffer);
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
    status = this->header.fromSerialBuffer(serialBuffer);
    if (status != FW_SERIALIZE_OK)
      return status;
    switch (this->header.type) {
      case T_START:
        status = this->startPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_DATA:
        status = this->dataPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_END:
        status = this->endPacket.fromSerialBuffer(serialBuffer);
        break;
      case T_CANCEL:
        status = this->cancelPacket.fromSerialBuffer(serialBuffer);
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

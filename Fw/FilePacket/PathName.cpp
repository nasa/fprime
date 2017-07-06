// ====================================================================== 
// \title  PathName.cpp
// \author bocchino
// \brief  cpp file for FilePacket::PathName
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include <string.h>

#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

  void FilePacket::PathName ::
    initialize(const char *const value)
  {
    const U32 length = ::strnlen(value, MAX_LENGTH);
    this->length = length;
    this->value = value;
  }

  U32 FilePacket::PathName ::
    bufferSize(void) const
  {
    return sizeof(this->length) + this->length;
  }

  SerializeStatus FilePacket::PathName ::
    fromSerialBuffer(SerialBuffer& serialBuffer) 
  {

    {
      const SerializeStatus status = 
        serialBuffer.deserialize(this->length);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const U8* addrLeft = serialBuffer.getBuffAddrLeft();
      U8 bytes[this->length];
      const SerializeStatus status =
        serialBuffer.popBytes(bytes, this->length);
      if (status != FW_SERIALIZE_OK)
        return status;
      this->value = reinterpret_cast<const char*>(addrLeft);
    }

    return FW_SERIALIZE_OK;

  }

  SerializeStatus FilePacket::PathName ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    {
      const SerializeStatus status =
        serialBuffer.serialize(this->length);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status = serialBuffer.pushBytes(
          reinterpret_cast<const U8 *const>(this->value), 
          this->length
      );
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    return FW_SERIALIZE_OK;

  }

}

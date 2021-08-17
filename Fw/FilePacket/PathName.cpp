// ======================================================================
// \title  PathName.cpp
// \author bocchino
// \brief  cpp file for FilePacket::PathName
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <string.h>

#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

  void FilePacket::PathName ::
    initialize(const char *const value)
  {
    const U8 length = static_cast<U8>(::strnlen(value, MAX_LENGTH));
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
      U8 bytes[MAX_LENGTH];
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
          reinterpret_cast<const U8 *>(this->value),
          this->length
      );
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    return FW_SERIALIZE_OK;

  }

}

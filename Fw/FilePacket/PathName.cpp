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

#include <cstring>

#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Fw {

  void FilePacket::PathName ::
    initialize(const char *const value)
  {
    const U8 length = static_cast<U8>(StringUtils::string_length(value, MAX_LENGTH));
    this->m_length = length;
    this->m_value = value;
  }

  U32 FilePacket::PathName ::
    bufferSize() const
  {
    return sizeof(this->m_length) + this->m_length;
  }

  SerializeStatus FilePacket::PathName ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    {
      const SerializeStatus status =
        serialBuffer.deserialize(this->m_length);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const U8* addrLeft = serialBuffer.getBuffAddrLeft();
      U8 bytes[MAX_LENGTH];
      const SerializeStatus status =
        serialBuffer.popBytes(bytes, this->m_length);
      if (status != FW_SERIALIZE_OK)
        return status;
      this->m_value = reinterpret_cast<const char*>(addrLeft);
    }

    return FW_SERIALIZE_OK;

  }

  SerializeStatus FilePacket::PathName ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    {
      const SerializeStatus status =
        serialBuffer.serialize(this->m_length);
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    {
      const SerializeStatus status = serialBuffer.pushBytes(
          reinterpret_cast<const U8 *>(this->m_value),
          this->m_length
      );
      if (status != FW_SERIALIZE_OK)
        return status;
    }

    return FW_SERIALIZE_OK;

  }

}

// ======================================================================
// \title  Header.cpp
// \author bocchino
// \brief  cpp file for FilePacket::Header
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

  void FilePacket::Header ::
    initialize(
        const Type type,
        const U32 sequenceIndex
    )
  {
    this->m_type = type;
    this->m_sequenceIndex = sequenceIndex;
  }

  U32 FilePacket::Header ::
    bufferSize() const
  {
    return sizeof(U8) + sizeof(this->m_sequenceIndex);
  }

  SerializeStatus FilePacket::Header ::
    fromSerialBuffer(SerialBuffer& serialBuffer)
  {

    U8 new_type;
    SerializeStatus status;

    status = serialBuffer.deserialize(new_type);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }
    this->m_type = static_cast<Type>(new_type);

    status = serialBuffer.deserialize(this->m_sequenceIndex);

    return status;

  }

  SerializeStatus FilePacket::Header ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    const U8 type_casted = static_cast<U8>(this->m_type);
    SerializeStatus status;

    status = serialBuffer.serialize(type_casted);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    status = serialBuffer.serialize(this->m_sequenceIndex);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }

    return FW_SERIALIZE_OK;

  }

}

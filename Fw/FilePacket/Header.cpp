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
    this->type = type;
    this->sequenceIndex = sequenceIndex;
  }

  U32 FilePacket::Header ::
    bufferSize(void) const
  {
    return sizeof(U8) + sizeof(this->sequenceIndex);
  }

  SerializeStatus FilePacket::Header ::
    fromSerialBuffer(SerialBuffer& serialBuffer) 
  {

    U8 type;
    SerializeStatus status;
    
    status = serialBuffer.deserialize(type);
    if (status != FW_SERIALIZE_OK) {
      return status;
    }
    this->type = static_cast<Type>(type);

    status = serialBuffer.deserialize(this->sequenceIndex);
      
    return status;

  }

  SerializeStatus FilePacket::Header ::
    toSerialBuffer(SerialBuffer& serialBuffer) const
  {

    const U8 type = static_cast<U8>(this->type);
    SerializeStatus status;

    status = serialBuffer.serialize(type);
    if (status != FW_SERIALIZE_OK)
      return status;

    status = serialBuffer.serialize(this->sequenceIndex);
    if (status != FW_SERIALIZE_OK)
      return status;

    return FW_SERIALIZE_OK;

  }

}

// ====================================================================== 
// \title  SerialBuffer.cpp
// \author bocchino
// \brief  cpp file for SerialBuffer type
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

  SerialBuffer ::
    SerialBuffer(
        U8 *const data,
        const U32 capacity
    ) : 
      data(data),
      capacity(capacity)
  {
  
  }

  NATIVE_UINT_TYPE SerialBuffer ::
    getBuffCapacity(void) const
  {
    return capacity;
  }

  U8* SerialBuffer :: 
    getBuffAddr(void)
  {
    return data;
  }

  const U8* SerialBuffer ::
    getBuffAddr(void) const 
  {
    return data;
  }

  void SerialBuffer ::
    fill(void)
  {
    const SerializeStatus status = this->setBuffLen(this->capacity);
    FW_ASSERT(status == FW_SERIALIZE_OK);
  }

  SerializeStatus SerialBuffer ::
    pushBytes(const U8 *const addr, const NATIVE_UINT_TYPE n)
  {
    // "true" means "just push the bytes"
    return this->serialize(const_cast<U8*>(addr), n, true);
  }

  SerializeStatus SerialBuffer ::
    popBytes(U8 *const addr, NATIVE_UINT_TYPE n)
  {
    // "true" means "just pop the bytes"
    return this->deserialize(addr, n, true);
  }

}

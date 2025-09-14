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

SerialBuffer ::SerialBuffer(U8* const data, const FwSizeType capacity) : m_data(data), m_capacity(capacity) {}

FwSizeType SerialBuffer ::getBuffCapacity() const {
    return m_capacity;
}

U8* SerialBuffer ::getBuffAddr() {
    return m_data;
}

const U8* SerialBuffer ::getBuffAddr() const {
    return m_data;
}

void SerialBuffer ::fill() {
    const SerializeStatus status = this->setBuffLen(this->m_capacity);
    FW_ASSERT(status == FW_SERIALIZE_OK);
}

SerializeStatus SerialBuffer ::pushBytes(const U8* const addr, const FwSizeType n) {
    return this->serialize(const_cast<U8*>(addr), n, Fw::Serialization::OMIT_LENGTH);
}

SerializeStatus SerialBuffer ::popBytes(U8* const addr, FwSizeType n) {
    return this->deserialize(addr, n, Fw::Serialization::OMIT_LENGTH);
}

}  // namespace Fw

/*
 * TlmPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */
#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

TlmPacket::TlmPacket() : m_numEntries(0) {
    this->m_type = ComPacketType::FW_PACKET_TELEM;
    this->m_tlmBuffer.resetSer();
}

TlmPacket::~TlmPacket() {}

SerializeStatus TlmPacket::resetPktSer() {
    this->m_tlmBuffer.resetSer();
    // reset packet count
    this->m_numEntries = 0;
    // make sure packet type is correct before serializing. It should
    // never be anything but FW_PACKET_TELEM, so assert.
    FW_ASSERT(ComPacketType::FW_PACKET_TELEM == this->m_type, static_cast<FwAssertArgType>(this->m_type));
    // serialize descriptor
    // The function serializeBase inherited from ComPacket converts this->m_type
    // to type FwPacketDescriptorType and serializes the result into this->m_tlmBuffer.
    return this->serializeBase(this->m_tlmBuffer);
}

SerializeStatus TlmPacket::resetPktDeser() {
    this->m_tlmBuffer.resetDeser();
    // deserialize descriptor
    // The function deserializeBase inherited from ComPacket deserializes a
    // value of type FwPacketDescriptorType from this->m_tlmBuffer and stores it
    // into this->m_type.
    Fw::SerializeStatus stat = this->deserializeBase(this->m_tlmBuffer);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    // make sure that this->m_tlmBuffer stores a telemetry packet
    if (this->m_type != ComPacketType::FW_PACKET_TELEM) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    return Fw::FW_SERIALIZE_OK;
}

FwSizeType TlmPacket::getNumEntries() {
    return this->m_numEntries;
}

Fw::ComBuffer& TlmPacket::getBuffer() {
    return this->m_tlmBuffer;
}

void TlmPacket::setBuffer(Fw::ComBuffer& buffer) {
    this->m_tlmBuffer = buffer;
}

SerializeStatus TlmPacket::addValue(FwChanIdType id, Time& timeTag, TlmBuffer& buffer) {
    // check to make sure there is room for all the fields
    FwSizeType left = this->m_tlmBuffer.getBuffCapacity() - this->m_tlmBuffer.getBuffLength();
    if ((sizeof(FwChanIdType) + Time::SERIALIZED_SIZE + buffer.getBuffLength()) > left) {
        return Fw::FW_SERIALIZE_NO_ROOM_LEFT;
    }

    // serialize items into buffer

    // id
    SerializeStatus stat = this->m_tlmBuffer.serializeFrom(id);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // time tag
    stat = this->m_tlmBuffer.serializeFrom(timeTag);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // telemetry buffer
    stat =
        this->m_tlmBuffer.serializeFrom(buffer.getBuffAddr(), buffer.getBuffLength(), Fw::Serialization::OMIT_LENGTH);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // increment number of packets
    this->m_numEntries++;

    return Fw::FW_SERIALIZE_OK;
}

// extract telemetry value
SerializeStatus TlmPacket::extractValue(FwChanIdType& id, Time& timeTag, TlmBuffer& buffer, FwSizeType bufferSize) {
    // deserialize items out of buffer

    // id
    SerializeStatus stat = this->m_tlmBuffer.deserializeTo(id);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // time tag
    stat = this->m_tlmBuffer.deserializeTo(timeTag);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // telemetry buffer
    stat = this->m_tlmBuffer.deserializeTo(buffer.getBuffAddr(), bufferSize, Fw::Serialization::OMIT_LENGTH);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    // set buffer size
    stat = buffer.setBuffLen(bufferSize);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }

    return Fw::FW_SERIALIZE_OK;
}

SerializeStatus TlmPacket::serializeTo(SerializeBufferBase& buffer) const {
    // serialize the number of packets
    SerializeStatus stat = buffer.serializeFrom(this->m_numEntries);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    // Serialize the ComBuffer
    return buffer.serializeFrom(this->m_tlmBuffer.getBuffAddr(), m_tlmBuffer.getBuffLength(),
                                Fw::Serialization::OMIT_LENGTH);
}

SerializeStatus TlmPacket::deserializeFrom(SerializeBufferBase& buffer) {
    // deserialize the number of packets
    SerializeStatus stat = buffer.deserializeTo(this->m_numEntries);
    if (stat != Fw::FW_SERIALIZE_OK) {
        return stat;
    }
    // deserialize the channel value entry buffers
    FwSizeType size = buffer.getBuffLeft();
    stat = buffer.deserializeTo(this->m_tlmBuffer.getBuffAddr(), size, Fw::Serialization::OMIT_LENGTH);
    if (stat == FW_SERIALIZE_OK) {
        // Shouldn't fail
        stat = this->m_tlmBuffer.setBuffLen(size);
        FW_ASSERT(stat == FW_SERIALIZE_OK, static_cast<FwAssertArgType>(stat));
    }
    return stat;
}

} /* namespace Fw */

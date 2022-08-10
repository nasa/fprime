// ======================================================================
// \title  FprimeProtocol.cpp
// \author mstarch
// \brief  cpp file for FprimeProtocol class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <limits>

#include "FprimeProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

FprimeFraming::FprimeFraming(): FramingProtocol() {}

FprimeDeframing::FprimeDeframing(): DeframingProtocol() {}

void FprimeFraming::frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(m_interface != nullptr);
    // Use of I32 size is explicit as ComPacketType will be specifically serialized as an I32
    FpFrameHeader::TokenType real_data_size = size + ((packet_type != Fw::ComPacket::FW_PACKET_UNKNOWN) ? sizeof(I32) : 0);
    FpFrameHeader::TokenType total = real_data_size + FpFrameHeader::SIZE + HASH_DIGEST_LENGTH;
    Fw::Buffer buffer = m_interface->allocate(total);
    Fw::SerializeBufferBase& serializer = buffer.getSerializeRepr();
    Utils::HashBuffer hash;

    // Serialize data
    Fw::SerializeStatus status;
    status = serializer.serialize(FpFrameHeader::START_WORD);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    
    status = serializer.serialize(real_data_size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Serialize packet type if supplied, otherwise it *must* be present in the data
    if (packet_type != Fw::ComPacket::FW_PACKET_UNKNOWN) {
        status = serializer.serialize(static_cast<I32>(packet_type)); // I32 used for enum storage
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    }
    
    status = serializer.serialize(data, size, true);  // Serialize without length
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Calculate and add transmission hash
    Utils::Hash::hash(buffer.getData(), total - HASH_DIGEST_LENGTH, hash);
    status = serializer.serialize(hash.getBuffAddr(), HASH_DIGEST_LENGTH, true);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    buffer.setSize(total);

    m_interface->send(buffer);
}

bool FprimeDeframing::validate(Types::CircularBuffer& ring, U32 size) {
    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    // Initialize the checksum and loop through all bytes calculating it
    hash.init();
    for (U32 i = 0; i < size; i++) {
        char byte;
        const Fw::SerializeStatus status = ring.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        hash.update(&byte, 1);
    }
    hash.final(hashBuffer);
    // Now loop through the hash digest bytes and check for equality
    for (U32 i = 0; i < HASH_DIGEST_LENGTH; i++) {
        char calc = static_cast<char>(hashBuffer.getBuffAddr()[i]);
        char sent = 0;
        const Fw::SerializeStatus status = ring.peek(sent, size + i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        if (calc != sent) {
            return false;
        }
    }
    return true;
}

DeframingProtocol::DeframingStatus FprimeDeframing::deframe(Types::CircularBuffer& ring, U32& needed) {
    FpFrameHeader::TokenType start = 0;
    FpFrameHeader::TokenType size = 0;
    FW_ASSERT(m_interface != nullptr);
    // Check for header or ask for more data
    if (ring.get_allocated_size() < FpFrameHeader::SIZE) {
        needed = FpFrameHeader::SIZE;
        return DeframingProtocol::DEFRAMING_MORE_NEEDED;
    }
    // Read start value from header
    Fw::SerializeStatus status = ring.peek(start, 0);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    if (start != FpFrameHeader::START_WORD) {
        // Start word must be valid
        return DeframingProtocol::DEFRAMING_INVALID_FORMAT;
    }
    // Read size from header
    status = ring.peek(size, sizeof(FpFrameHeader::TokenType));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    const U32 maxU32 = std::numeric_limits<U32>::max();
    if (size > maxU32 - (FpFrameHeader::SIZE + HASH_DIGEST_LENGTH)) {
        // Size is too large to process: needed would overflow
        return DeframingProtocol::DEFRAMING_INVALID_SIZE;
    }
    needed = (FpFrameHeader::SIZE + size + HASH_DIGEST_LENGTH);
    // Check frame size
    const U32 frameSize = size + FpFrameHeader::SIZE + HASH_DIGEST_LENGTH;
    if (frameSize > ring.get_capacity()) {
        // Frame size is too large for ring buffer
        return DeframingProtocol::DEFRAMING_INVALID_SIZE;
    }
    // Check for enough data to deserialize everything;
    // otherwise break and wait for more.
    else if (ring.get_allocated_size() < needed) {
        return DeframingProtocol::DEFRAMING_MORE_NEEDED;
    }
    // Check the checksum
    if (not this->validate(ring, needed - HASH_DIGEST_LENGTH)) {
        return DeframingProtocol::DEFRAMING_INVALID_CHECKSUM;
    }
    Fw::Buffer buffer = m_interface->allocate(size);
    // Some allocators may return buffers larger than requested.
    // That causes issues in routing; adjust size.
    FW_ASSERT(buffer.getSize() >= size);
    buffer.setSize(size);
    status = ring.peek(buffer.getData(), size, FpFrameHeader::SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    m_interface->route(buffer);
    return DeframingProtocol::DEFRAMING_STATUS_SUCCESS;
}
};

// ======================================================================
// \title  FprimeProtocol.cpp
// \author mstarch
// \brief  cpp file for FprimeProtocol class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FprimeProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

const FP_FRAME_TOKEN_TYPE FprimeFraming::START_WORD = static_cast<FP_FRAME_TOKEN_TYPE>(0xdeadbeef);

FprimeFraming::FprimeFraming(): FramingProtocol() {}

FprimeDeframing::FprimeDeframing(): DeframingProtocol() {}

void FprimeFraming::frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) {
    FW_ASSERT(data != NULL);
    FW_ASSERT(m_interface != NULL);
    // Use of I32 size is explicit as ComPacketType will be specifically serialized as an I32
    FP_FRAME_TOKEN_TYPE real_data_size = size + ((packet_type != Fw::ComPacket::FW_PACKET_UNKNOWN) ? sizeof(I32) : 0);
    FP_FRAME_TOKEN_TYPE total = real_data_size + FP_FRAME_HEADER_SIZE + HASH_DIGEST_LENGTH;
    Fw::Buffer buffer = m_interface->allocate(total);
    Fw::SerializeBufferBase& serializer = buffer.getSerializeRepr();
    Utils::HashBuffer hash;

    // Serialize data
    Fw::SerializeStatus status;
    status = serializer.serialize(START_WORD);
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
        ring.peek(byte, i);
        hash.update(&byte, 1);
    }
    hash.final(hashBuffer);
    // Now loop through the hash digest bytes and check for equality
    for (U32 i = 0; i < HASH_DIGEST_LENGTH; i++) {
        char calc = static_cast<char>(hashBuffer.getBuffAddr()[i]);
        char sent = 0;
        ring.peek(sent, size + i);
        if (calc != sent) {
            return false;
        }
    }
    return true;
}

DeframingProtocol::DeframingStatus FprimeDeframing::deframe(Types::CircularBuffer& ring, U32& needed) {
    FP_FRAME_TOKEN_TYPE start = 0;
    FP_FRAME_TOKEN_TYPE size = 0;
    FW_ASSERT(m_interface != NULL);
    // Check for header or ask for more data
    if (ring.get_remaining_size() < FP_FRAME_HEADER_SIZE) {
        needed = FP_FRAME_HEADER_SIZE;
        return DeframingProtocol::DEFRAMING_MORE_NEEDED;
    }
    // Peek into the header and read out values
    Fw::SerializeStatus status = ring.peek(start, 0);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = ring.peek(size, sizeof(FP_FRAME_TOKEN_TYPE));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    needed = (FP_FRAME_HEADER_SIZE + size + HASH_DIGEST_LENGTH);
    // Check the header for correctness
    if ((start != FprimeFraming::START_WORD) || (size >= (ring.get_capacity() - FP_FRAME_HEADER_SIZE - HASH_DIGEST_LENGTH))) {
        return DeframingProtocol::DEFRAMING_INVALID_SIZE;
    }
    // Check for enough data to deserialize everything otherwise break and wait for more.
    else if (ring.get_remaining_size() < needed) {
        return DeframingProtocol::DEFRAMING_MORE_NEEDED;
    }
    // Check the checksum
    if (not this->validate(ring, needed - HASH_DIGEST_LENGTH)) {
        return DeframingProtocol::DEFRAMING_INVALID_CHECKSUM;
    }
    Fw::Buffer buffer = m_interface->allocate(size);
    // some allocators may return buffers larger than requested
    // that causes issues in routing. adjust size
    FW_ASSERT(buffer.getSize() >= size);
    buffer.setSize(size);
    ring.peek(buffer.getData(), size, FP_FRAME_HEADER_SIZE);
    m_interface->route(buffer);
    return DeframingProtocol::DEFRAMING_STATUS_SUCCESS;
}
};

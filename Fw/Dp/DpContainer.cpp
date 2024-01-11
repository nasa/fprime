// ======================================================================
// \title  DpContainer.cpp
// \author bocchino
// \brief  cpp file for DpContainer
// ======================================================================

#include <cstring>

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Dp/DpContainer.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

DpContainer::DpContainer(FwDpIdType id, const Fw::Buffer& buffer)
    : id(id), priority(0), procTypes(0), dpState(), dataSize(0), buffer(), dataBuffer() {
    // Initialize the user data field
    this->initUserDataField();
    // Set the packet buffer
    // This action also updates the data buffer
    this->setBuffer(buffer);
}

DpContainer::DpContainer() : id(0), priority(0), procTypes(0), dataSize(0), buffer(), dataBuffer() {
    // Initialize the user data field
    this->initUserDataField();
}

// ----------------------------------------------------------------------
// Public member functions
// ----------------------------------------------------------------------

Fw::SerializeStatus DpContainer::moveSerToOffset(FwSizeType offset  //!< The offset
) {
    Fw::SerializeBufferBase& serializeRepr = this->buffer.getSerializeRepr();
    return serializeRepr.moveSerToOffset(offset);
}

void DpContainer::serializeHeader() {
    Fw::SerializeBufferBase& serializeRepr = this->buffer.getSerializeRepr();
    // Reset serialization
    serializeRepr.resetSer();
    // Serialize the packet type
    Fw::SerializeStatus status =
        serializeRepr.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_DP));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the container id
    status = serializeRepr.serialize(this->id);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the priority
    status = serializeRepr.serialize(this->priority);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the time tag
    status = serializeRepr.serialize(this->timeTag);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the processing types
    status = serializeRepr.serialize(this->procTypes);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the user data
    const bool omitLength = true;
    status = serializeRepr.serialize(this->userData, sizeof userData, omitLength);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the data product state
    status = serializeRepr.serialize(this->dpState);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Serialize the data size
    status = serializeRepr.serialize(this->dataSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // Update the header hash
    this->updateHeaderHash();
}

void DpContainer::setBuffer(const Buffer& buffer) {
    // Set the buffer
    this->buffer = buffer;
    // Check that the buffer is large enough to hold a data product packet
    FW_ASSERT(buffer.getSize() >= MIN_PACKET_SIZE, buffer.getSize(), MIN_PACKET_SIZE);
    // Initialize the data buffer
    U8* const buffAddr = buffer.getData();
    const FwSizeType dataCapacity = buffer.getSize() - MIN_PACKET_SIZE;
    // Check that data buffer is in bounds for packet buffer
    FW_ASSERT(DATA_OFFSET + dataCapacity <= buffer.getSize());
    U8* const dataAddr = &buffAddr[DATA_OFFSET];
    this->dataBuffer.setExtBuffer(dataAddr, dataCapacity);
}

void DpContainer::updateHeaderHash() {
    Utils::HashBuffer hashBuffer;
    U8* const buffAddr = this->buffer.getData();
    Utils::Hash::hash(buffAddr, Header::SIZE, hashBuffer);
    ExternalSerializeBuffer serialBuffer(&buffAddr[HEADER_HASH_OFFSET], HASH_DIGEST_LENGTH);
    const Fw::SerializeStatus status = hashBuffer.copyRaw(serialBuffer, HASH_DIGEST_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

void DpContainer::updateDataHash() {
    Utils::HashBuffer hashBuffer;
    U8* const buffAddrBase = this->buffer.getData();
    const U8* const dataAddr = &buffAddrBase[DATA_OFFSET];
    const FwSizeType dataSize = this->getDataSize();
    const FwSizeType bufferSize = buffer.getSize();
    FW_ASSERT(DATA_OFFSET + dataSize <= bufferSize, DATA_OFFSET + dataSize, bufferSize);
    Utils::Hash::hash(dataAddr, dataSize, hashBuffer);
    const FwSizeType dataHashOffset = this->getDataHashOffset();
    U8* const dataHashAddr = &buffAddrBase[dataHashOffset];
    FW_ASSERT(dataHashOffset + HASH_DIGEST_LENGTH <= bufferSize, dataHashOffset + HASH_DIGEST_LENGTH, bufferSize);
    ExternalSerializeBuffer serialBuffer(dataHashAddr, HASH_DIGEST_LENGTH);
    const Fw::SerializeStatus status = hashBuffer.copyRaw(serialBuffer, HASH_DIGEST_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

// ----------------------------------------------------------------------
// Private member functions
// ----------------------------------------------------------------------

void DpContainer::initUserDataField() {
    (void)::memset(this->userData, 0, sizeof this->userData);
}

}  // namespace Fw

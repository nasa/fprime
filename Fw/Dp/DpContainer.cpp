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
    : m_id(id), m_priority(0), m_timeTag(), m_procTypes(0), m_dpState(), m_dataSize(0), m_buffer(), m_dataBuffer() {
    // Initialize the user data field
    this->initUserDataField();
    // Set the packet buffer
    // This action also updates the data buffer
    this->setBuffer(buffer);
}

DpContainer::DpContainer()
    : m_id(0), m_priority(0), m_timeTag(), m_procTypes(0), m_dataSize(0), m_buffer(), m_dataBuffer() {
    // Initialize the user data field
    this->initUserDataField();
}

// ----------------------------------------------------------------------
// Public member functions
// ----------------------------------------------------------------------

Fw::SerializeStatus DpContainer::deserializeHeader() {
    FW_ASSERT(this->m_buffer.isValid());
    Fw::SerializeBufferBase& serializeRepr = this->m_buffer.getSerializeRepr();
    // Reset deserialization
    serializeRepr.setBuffLen(this->m_buffer.getSize());
    Fw::SerializeStatus status = serializeRepr.moveDeserToOffset(Header::PACKET_DESCRIPTOR_OFFSET);
    // Deserialize the packet type
    if (status == Fw::FW_SERIALIZE_OK) {
        FwPacketDescriptorType packetDescriptor;
        status = serializeRepr.deserialize(packetDescriptor);
        if (packetDescriptor != Fw::ComPacket::FW_PACKET_DP) {
            status = Fw::FW_SERIALIZE_FORMAT_ERROR;
        }
    }
    // Deserialize the container id
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_id);
    }
    // Deserialize the priority
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_priority);
    }
    // Deserialize the time tag
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_timeTag);
    }
    // Deserialize the processing types
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_procTypes);
    }
    // Deserialize the user data
    if (status == Fw::FW_SERIALIZE_OK) {
        const bool omitLength = true;
        const FwSizeType requestedSize = sizeof this->m_userData;
        FwSizeType receivedSize = requestedSize;
        status = serializeRepr.deserialize(this->m_userData, receivedSize, omitLength);
        if (receivedSize != requestedSize) {
            status = Fw::FW_DESERIALIZE_SIZE_MISMATCH;
        }
    }
    // Deserialize the data product state
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_dpState);
    }
    // Deserialize the data size
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.deserialize(this->m_dataSize);
    }
    return status;
}

void DpContainer::serializeHeader() {
    FW_ASSERT(this->m_buffer.isValid());
    Fw::SerializeBufferBase& serializeRepr = this->m_buffer.getSerializeRepr();
    // Reset serialization
    serializeRepr.resetSer();
    // Serialize the packet type
    Fw::SerializeStatus status =
        serializeRepr.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_DP));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the container id
    status = serializeRepr.serialize(this->m_id);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the priority
    status = serializeRepr.serialize(this->m_priority);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the time tag
    status = serializeRepr.serialize(this->m_timeTag);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the processing types
    status = serializeRepr.serialize(this->m_procTypes);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the user data
    const bool omitLength = true;
    status = serializeRepr.serialize(this->m_userData, sizeof this->m_userData, omitLength);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the data product state
    status = serializeRepr.serialize(this->m_dpState);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the data size
    status = serializeRepr.serialize(this->m_dataSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Update the header hash
    this->updateHeaderHash();
}

void DpContainer::setBuffer(const Buffer& buffer) {
    // Set the buffer
    this->m_buffer = buffer;
    // Check that the buffer is large enough to hold a data product packet
    FW_ASSERT(buffer.getSize() >= MIN_PACKET_SIZE, buffer.getSize(), MIN_PACKET_SIZE);
    // Initialize the data buffer
    U8* const buffAddr = buffer.getData();
    const FwSizeType dataCapacity = buffer.getSize() - MIN_PACKET_SIZE;
    // Check that data buffer is in bounds for packet buffer
    FW_ASSERT(DATA_OFFSET + dataCapacity <= buffer.getSize());
    U8* const dataAddr = &buffAddr[DATA_OFFSET];
    this->m_dataBuffer.setExtBuffer(dataAddr, dataCapacity);
}

void DpContainer::updateHeaderHash() {
    Utils::HashBuffer hashBuffer;
    U8* const buffAddr = this->m_buffer.getData();
    Utils::Hash::hash(buffAddr, Header::SIZE, hashBuffer);
    ExternalSerializeBuffer serialBuffer(&buffAddr[HEADER_HASH_OFFSET], HASH_DIGEST_LENGTH);
    const Fw::SerializeStatus status = hashBuffer.copyRaw(serialBuffer, HASH_DIGEST_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
}

void DpContainer::updateDataHash() {
    Utils::HashBuffer hashBuffer;
    U8* const buffAddrBase = this->m_buffer.getData();
    const U8* const dataAddr = &buffAddrBase[DATA_OFFSET];
    const FwSizeType dataSize = this->getDataSize();
    const FwSizeType bufferSize = this->m_buffer.getSize();
    FW_ASSERT(DATA_OFFSET + dataSize <= bufferSize, DATA_OFFSET + dataSize, bufferSize);
    Utils::Hash::hash(dataAddr, dataSize, hashBuffer);
    const FwSizeType dataHashOffset = this->getDataHashOffset();
    U8* const dataHashAddr = &buffAddrBase[dataHashOffset];
    FW_ASSERT(dataHashOffset + HASH_DIGEST_LENGTH <= bufferSize, dataHashOffset + HASH_DIGEST_LENGTH, bufferSize);
    ExternalSerializeBuffer serialBuffer(dataHashAddr, HASH_DIGEST_LENGTH);
    const Fw::SerializeStatus status = hashBuffer.copyRaw(serialBuffer, HASH_DIGEST_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
}

// ----------------------------------------------------------------------
// Private member functions
// ----------------------------------------------------------------------

void DpContainer::initUserDataField() {
    (void)::memset(this->m_userData, 0, sizeof this->m_userData);
}

}  // namespace Fw

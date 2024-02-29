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
    // Set buffer length
    Fw::SerializeStatus status = serializeRepr.setBuffLen(this->m_buffer.getSize());
    // Reset deserialization
    if (status == Fw::FW_SERIALIZE_OK) {
        status = serializeRepr.moveDeserToOffset(Header::PACKET_DESCRIPTOR_OFFSET);
    }
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
        const FwSizeType requestedSize = sizeof this->m_userData;
        FwSizeType receivedSize = requestedSize;
        status = serializeRepr.deserialize(this->m_userData, receivedSize, Fw::Serialization::OMIT_LENGTH);
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
        status = serializeRepr.deserializeSize(this->m_dataSize);
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
    status = serializeRepr.serialize(this->m_userData, static_cast<FwSizeType>(sizeof this->m_userData),
                                     Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the data product state
    status = serializeRepr.serialize(this->m_dpState);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Serialize the data size
    status = serializeRepr.serializeSize(this->m_dataSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Update the header hash
    this->updateHeaderHash();
}

void DpContainer::setBuffer(const Buffer& buffer) {
    // Set the buffer
    this->m_buffer = buffer;
    // Check that the buffer is large enough to hold a data product packet with
    // zero-size data
    const FwSizeType bufferSize = buffer.getSize();
    FW_ASSERT(bufferSize >= MIN_PACKET_SIZE, static_cast<FwAssertArgType>(bufferSize),
              static_cast<FwAssertArgType>(MIN_PACKET_SIZE));
    // Initialize the data buffer
    U8* const buffAddr = buffer.getData();
    const FwSizeType dataCapacity = buffer.getSize() - MIN_PACKET_SIZE;
    // Check that data buffer is in bounds for packet buffer
    const FwSizeType minBufferSize = DATA_OFFSET + dataCapacity;
    FW_ASSERT(bufferSize >= minBufferSize, static_cast<FwAssertArgType>(bufferSize),
              static_cast<FwAssertArgType>(minBufferSize));
    U8* const dataAddr = &buffAddr[DATA_OFFSET];
    this->m_dataBuffer.setExtBuffer(dataAddr, dataCapacity);
}

Utils::HashBuffer DpContainer::getHeaderHash() const {
    const FwSizeType bufferSize = this->m_buffer.getSize();
    const FwSizeType minBufferSize = HEADER_HASH_OFFSET + HASH_DIGEST_LENGTH;
    FW_ASSERT(bufferSize >= minBufferSize, static_cast<FwAssertArgType>(bufferSize),
              static_cast<FwAssertArgType>(minBufferSize));
    const U8* const buffAddr = this->m_buffer.getData();
    return Utils::HashBuffer(&buffAddr[HEADER_HASH_OFFSET], HASH_DIGEST_LENGTH);
}

Utils::HashBuffer DpContainer::computeHeaderHash() const {
    const FwSizeType bufferSize = this->m_buffer.getSize();
    FW_ASSERT(bufferSize >= Header::SIZE, static_cast<FwAssertArgType>(bufferSize),
              static_cast<FwAssertArgType>(Header::SIZE));
    U8* const buffAddr = this->m_buffer.getData();
    Utils::HashBuffer computedHash;
    Utils::Hash::hash(buffAddr, Header::SIZE, computedHash);
    return computedHash;
}

void DpContainer::setHeaderHash(const Utils::HashBuffer& hash) {
    const FwSizeType bufferSize = this->m_buffer.getSize();
    const FwSizeType minBufferSize = HEADER_HASH_OFFSET + HASH_DIGEST_LENGTH;
    FW_ASSERT(bufferSize >= minBufferSize, static_cast<FwAssertArgType>(bufferSize),
              static_cast<FwAssertArgType>(minBufferSize));
    U8* const buffAddr = this->m_buffer.getData();
    (void)::memcpy(&buffAddr[HEADER_HASH_OFFSET], hash.getBuffAddr(), HASH_DIGEST_LENGTH);
}

void DpContainer::updateHeaderHash() {
    this->setHeaderHash(this->computeHeaderHash());
}

Success::T DpContainer::checkHeaderHash(Utils::HashBuffer& storedHash, Utils::HashBuffer& computedHash) const {
    storedHash = this->getHeaderHash();
    computedHash = this->computeHeaderHash();
    return (storedHash == computedHash) ? Success::SUCCESS : Success::FAILURE;
}

Utils::HashBuffer DpContainer::getDataHash() const {
    const U8* const buffAddr = this->m_buffer.getData();
    const FwSizeType dataHashOffset = this->getDataHashOffset();
    const FwSizeType bufferSize = this->m_buffer.getSize();
    FW_ASSERT(dataHashOffset + HASH_DIGEST_LENGTH <= bufferSize,
              static_cast<FwAssertArgType>(dataHashOffset + HASH_DIGEST_LENGTH),
              static_cast<FwAssertArgType>(bufferSize));
    const U8* const dataHashAddr = &buffAddr[dataHashOffset];
    return Utils::HashBuffer(dataHashAddr, HASH_DIGEST_LENGTH);
}

Utils::HashBuffer DpContainer::computeDataHash() const {
    U8* const buffAddr = this->m_buffer.getData();
    const U8* const dataAddr = &buffAddr[DATA_OFFSET];
    const FwSizeType dataSize = this->getDataSize();
    const FwSizeType bufferSize = this->m_buffer.getSize();
    FW_ASSERT(DATA_OFFSET + dataSize <= bufferSize, static_cast<FwAssertArgType>(DATA_OFFSET + dataSize),
              static_cast<FwAssertArgType>(bufferSize));
    Utils::HashBuffer computedHash;
    Utils::Hash::hash(dataAddr, dataSize, computedHash);
    return computedHash;
}

void DpContainer::setDataHash(Utils::HashBuffer hash) {
    U8* const buffAddr = this->m_buffer.getData();
    const FwSizeType bufferSize = this->m_buffer.getSize();
    const FwSizeType dataHashOffset = this->getDataHashOffset();
    U8* const dataHashAddr = &buffAddr[dataHashOffset];
    FW_ASSERT(dataHashOffset + HASH_DIGEST_LENGTH <= bufferSize,
              static_cast<FwAssertArgType>(dataHashOffset + HASH_DIGEST_LENGTH),
              static_cast<FwAssertArgType>(bufferSize));
    ExternalSerializeBuffer serialBuffer(dataHashAddr, HASH_DIGEST_LENGTH);
    hash.resetSer();
    const Fw::SerializeStatus status = hash.copyRaw(serialBuffer, HASH_DIGEST_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
}

void DpContainer::updateDataHash() {
    this->setDataHash(this->computeDataHash());
}

Success::T DpContainer::checkDataHash(Utils::HashBuffer& storedHash, Utils::HashBuffer& computedHash) const {
    storedHash = this->getDataHash();
    computedHash = this->computeDataHash();
    return (computedHash == storedHash) ? Success::SUCCESS : Success::FAILURE;
}

// ----------------------------------------------------------------------
// Private member functions
// ----------------------------------------------------------------------

void DpContainer::initUserDataField() {
    (void)::memset(this->m_userData, 0, sizeof this->m_userData);
}

}  // namespace Fw

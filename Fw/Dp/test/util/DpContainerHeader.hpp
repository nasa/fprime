// ======================================================================
// \title  DpContainerHeader.hpp
// \author bocchino
// \brief  hpp file for DpContainer header test utility
// ======================================================================

#ifndef Fw_TestUtil_DpContainerHeader_HPP
#define Fw_TestUtil_DpContainerHeader_HPP

#include "gtest/gtest.h"

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Dp/DpContainer.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

#define DP_CONTAINER_HEADER_ASSERT_MSG(actual, expected) \
    << file << ":" << line << "\n"                       \
    << "  Actual value is " << actual << "\n"            \
    << "  Expected value is " << expected
#define DP_CONTAINER_HEADER_ASSERT_EQ(actual, expected) \
    ASSERT_EQ(actual, expected) DP_CONTAINER_HEADER_ASSERT_MSG(actual, expected)
#define DP_CONTAINER_HEADER_ASSERT_GE(actual, expected) \
    ASSERT_GE(actual, expected) DP_CONTAINER_HEADER_ASSERT_MSG(actual, expected)

namespace Fw {
namespace TestUtil {

//! A container packet header for testing
struct DpContainerHeader {
    DpContainerHeader() : m_id(0), m_priority(0), m_timeTag(), m_procTypes(0), m_dpState(), m_dataSize(0) {}

    //! Move the buffer deserialization to the specified offset
    static void moveDeserToOffset(const char* const file,                               //!< The call site file name
                                  const U32 line,                                       //!< The call site line number
                                  ExternalSerializeBufferWithMemberCopy& deserializer,  //!< The buffer
                                  FwSizeType offset                                     //!< The offset
    ) {
        // Reset deserialization
        deserializer.resetDeser();
        deserializer.moveDeserToOffset(offset);
    }

    //! Deserialize a header from a packet buffer
    //! Check that the serialization succeeded at every step
    //! Check the header hash and the data hash
    void deserialize(const char* const file,  //!< The call site file name
                     const U32 line,          //!< The call site line number
                     Fw::Buffer& buffer       //!< The packet buffer
    ) {
        auto deserializer = buffer.getDeserializer();
        // Deserialize the packet descriptor
        FwPacketDescriptorType packetDescriptor = Fw::ComPacketType::FW_PACKET_UNKNOWN;
        // Deserialize the packet descriptor
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::PACKET_DESCRIPTOR_OFFSET);
        Fw::SerializeStatus status = deserializer.deserialize(packetDescriptor);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        DP_CONTAINER_HEADER_ASSERT_EQ(packetDescriptor, Fw::ComPacketType::FW_PACKET_DP);
        // Deserialize the container id
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::ID_OFFSET);
        status = deserializer.deserialize(this->m_id);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // Deserialize the priority
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::PRIORITY_OFFSET);
        status = deserializer.deserialize(this->m_priority);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // Deserialize the time tag
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::TIME_TAG_OFFSET);
        status = deserializer.deserialize(this->m_timeTag);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // Deserialize the processing type
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::PROC_TYPES_OFFSET);
        status = deserializer.deserialize(this->m_procTypes);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // Deserialize the user data
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::USER_DATA_OFFSET);
        FwSizeType size = sizeof this->m_userData;
        status = deserializer.deserialize(this->m_userData, size, Fw::Serialization::OMIT_LENGTH);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        DP_CONTAINER_HEADER_ASSERT_EQ(size, sizeof this->m_userData);
        // Deserialize the data product state
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::DP_STATE_OFFSET);
        status = deserializer.deserialize(this->m_dpState);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // Deserialize the data size
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::Header::DATA_SIZE_OFFSET);
        status = deserializer.deserializeSize(this->m_dataSize);
        DP_CONTAINER_HEADER_ASSERT_EQ(status, FW_SERIALIZE_OK);
        // After deserializing time, the deserialization index should be at
        // the header hash offset
        checkDeserialAtOffset(deserializer, DpContainer::HEADER_HASH_OFFSET);
        // Check the header hash
        checkHeaderHash(file, line, buffer);
        // Check the data hash
        this->checkDataHash(file, line, buffer);
        // Move the deserialization pointer to the data offset
        DpContainerHeader::moveDeserToOffset(file, line, deserializer, DpContainer::DATA_OFFSET);
    }

    //! Check the header hash
    static void checkHeaderHash(const char* const file,  //!< The call site file name
                                const U32 line,          //!< The call site line number
                                Fw::Buffer& buffer       //!< The packet buffer
    ) {
        Utils::HashBuffer computedHashBuffer;
        U8* const buffAddr = buffer.getData();
        Utils::Hash::hash(buffAddr, DpContainer::Header::SIZE, computedHashBuffer);
        Utils::HashBuffer storedHashBuffer(&buffAddr[DpContainer::HEADER_HASH_OFFSET], HASH_DIGEST_LENGTH);
        DP_CONTAINER_HEADER_ASSERT_EQ(computedHashBuffer, storedHashBuffer);
    }

    //! Check the data hash
    void checkDataHash(const char* const file,  //!< The call site file name
                       const U32 line,          //!< The call site line number
                       Fw::Buffer& buffer       //!< The packet buffer
    ) {
        Utils::HashBuffer computedHashBuffer;
        U8* const buffAddrBase = buffer.getData();
        U8* const dataAddr = &buffAddrBase[DpContainer::DATA_OFFSET];
        Utils::Hash::hash(dataAddr, this->m_dataSize, computedHashBuffer);
        DpContainer container(this->m_id, buffer);
        container.setDataSize(this->m_dataSize);
        const FwSizeType dataHashOffset = container.getDataHashOffset();
        Utils::HashBuffer storedHashBuffer(&buffAddrBase[dataHashOffset], HASH_DIGEST_LENGTH);
        DP_CONTAINER_HEADER_ASSERT_EQ(computedHashBuffer, storedHashBuffer);
    }

    //! Check a packet header against a buffer
    void check(const char* const file,                         //!< The call site file name
               const U32 line,                                 //!< The call site line number
               const Fw::Buffer& buffer,                       //!< The buffer
               FwDpIdType id,                                  //!< The expected id
               FwDpPriorityType priority,                      //!< The expected priority
               const Fw::Time& timeTag,                        //!< The expected time tag
               DpCfg::ProcType::SerialType procTypes,          //!< The expected processing types
               const DpContainer::Header::UserData& userData,  //!< The expected user data
               DpState dpState,                                //!< The expected dp state
               FwSizeType dataSize                             //!< The expected data size
    ) const {
        // Check the buffer size
        const FwSizeType bufferSize = buffer.getSize();
        const FwSizeType minBufferSize = Fw::DpContainer::MIN_PACKET_SIZE;
        DP_CONTAINER_HEADER_ASSERT_GE(bufferSize, minBufferSize);
        // Check the container id
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_id, id);
        // Check the priority
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_priority, priority);
        // Check the time tag
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_timeTag, timeTag);
        // Check the deserialized processing types
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_procTypes, procTypes);
        // Check the user data
        for (FwSizeType i = 0; i < DpCfg::CONTAINER_USER_DATA_SIZE; ++i) {
            DP_CONTAINER_HEADER_ASSERT_EQ(this->m_userData[i], userData[i]);
        }
        // Check the deserialized data product state
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_dpState, dpState);
        // Check the data size
        DP_CONTAINER_HEADER_ASSERT_EQ(this->m_dataSize, dataSize);
    }

    //! Check that the serialize repr is at the specified deserialization offset
    static void checkDeserialAtOffset(SerializeBufferBase& serialRepr,  //!< The serialize repr
                                      FwSizeType offset                 //!< The offset
    ) {
        const U8* buffAddr = serialRepr.getBuffAddr();
        const U8* buffAddrLeft = serialRepr.getBuffAddrLeft();
        ASSERT_EQ(buffAddrLeft, &buffAddr[offset]);
    }

    //! The container id
    FwDpIdType m_id;

    //! The priority
    FwDpPriorityType m_priority;

    //! The time tag
    Time m_timeTag;

    //! The processing types
    DpCfg::ProcType::SerialType m_procTypes;

    //! The user data
    U8 m_userData[DpCfg::CONTAINER_USER_DATA_SIZE];

    //! The data product state
    DpState m_dpState;

    //! The data size
    FwSizeType m_dataSize;
};

}  // namespace TestUtil

}  // end namespace Fw

#endif

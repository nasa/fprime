// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include <cstring>
#include <limits>

#include "gtest/gtest.h"

#include "Fw/Dp/DpContainer.hpp"
#include "Fw/Dp/test/util/DpContainerHeader.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"

using namespace Fw;

constexpr FwSizeType DATA_SIZE = 100;
constexpr FwSizeType PACKET_SIZE = DpContainer::getPacketSizeForDataSize(DATA_SIZE);
U8 bufferData[PACKET_SIZE];
DpContainer::Header::UserData userData;

void checkHeader(FwDpIdType id, Fw::Buffer& buffer, DpContainer& container) {
    // Check the packet size
    const FwSizeType expectedPacketSize = Fw::DpContainer::MIN_PACKET_SIZE;
    ASSERT_EQ(container.getPacketSize(), expectedPacketSize);
    // Set the priority
    const FwDpPriorityType priority = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpPriorityType>::max());
    container.setPriority(priority);
    // Set the time tag
    const U32 seconds = STest::Pick::any();
    const U32 useconds = STest::Pick::startLength(0, 1000000);
    Fw::Time timeTag(seconds, useconds);
    container.setTimeTag(timeTag);
    // Set the processing types
    const FwSizeType numProcTypeStates = 1 << DpCfg::ProcType::NUM_CONSTANTS;
    const DpCfg::ProcType::SerialType procTypes = STest::Pick::startLength(0, numProcTypeStates);
    container.setProcTypes(procTypes);
    // Set the user data
    for (U8& data : userData) {
        data = static_cast<U8>(STest::Pick::any());
    }
    FW_ASSERT(sizeof userData == sizeof container.m_userData);
    (void)::memcpy(container.m_userData, userData, sizeof container.m_userData);
    // Set the DP state
    const DpState dpState(static_cast<DpState::T>(STest::Pick::startLength(0, DpState::NUM_CONSTANTS)));
    container.setDpState(dpState);
    // Set the data size
    container.setDataSize(DATA_SIZE);
    // Test serialization: Serialize the header
    container.serializeHeader();
    TestUtil::DpContainerHeader header;
    // Update the data hash
    container.updateDataHash();
    // Deserialize the header and check the hashes
    header.deserialize(__FILE__, __LINE__, buffer);
    // Check the deserialized header fields
    header.check(__FILE__, __LINE__, buffer, id, priority, timeTag, procTypes, userData, dpState, DATA_SIZE);
    // Test deserialization: Deserialize the header into a new container
    DpContainer deserContainer;
    deserContainer.setBuffer(container.getBuffer());
    const Fw::SerializeStatus serialStatus = deserContainer.deserializeHeader();
    ASSERT_EQ(serialStatus, Fw::FW_SERIALIZE_OK);
    // Clear out the header in the buffer
    FW_ASSERT(buffer.isValid());
    ::memset(buffer.getData(), 0, DpContainer::Header::SIZE);
    // Serialize the header from the new container
    deserContainer.serializeHeader();
    // Deserialize and check the header
    header.deserialize(__FILE__, __LINE__, buffer);
    header.check(__FILE__, __LINE__, buffer, id, priority, timeTag, procTypes, userData, dpState, DATA_SIZE);
    // Test the flight code that checks the hashes
    Utils::HashBuffer storedHash;
    Utils::HashBuffer computedHash;
    Fw::Success status = deserContainer.checkHeaderHash(storedHash, computedHash);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    ASSERT_EQ(storedHash, computedHash);
    status = deserContainer.checkDataHash(storedHash, computedHash);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    ASSERT_EQ(storedHash, computedHash);
}

void checkBuffers(DpContainer& container, FwSizeType bufferSize) {
    // Check the packet buffer
    ASSERT_EQ(container.m_buffer.getSize(), bufferSize);
    // Check the data buffer
    U8* const buffPtr = container.m_buffer.getData();
    U8* const dataPtr = &buffPtr[Fw::DpContainer::DATA_OFFSET];
    const FwSizeType dataCapacity = container.m_buffer.getSize() - Fw::DpContainer::MIN_PACKET_SIZE;
    ASSERT_EQ(container.m_dataBuffer.getBuffAddr(), dataPtr);
    ASSERT_EQ(container.m_dataBuffer.getBuffCapacity(), dataCapacity);
}

void fillWithData(Fw::Buffer& buffer) {
    U8* const buffAddrBase = buffer.getData();
    U8* const dataAddr = &buffAddrBase[DpContainer::DATA_OFFSET];
    for (FwSizeType i = 0; i < DATA_SIZE; i++) {
        dataAddr[i] = static_cast<U8>(STest::Pick::any());
    }
}

TEST(Header, BufferInConstructor) {
    COMMENT("Test header serialization with buffer in constructor");
    // Create a buffer
    Fw::Buffer buffer(bufferData, sizeof bufferData);
    // Fill with data
    fillWithData(buffer);
    // Use the buffer to create a container
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    DpContainer container(id, buffer);
    // Check the header
    checkHeader(id, buffer, container);
    // Check the buffers
    checkBuffers(container, sizeof bufferData);
    // Perturb the header hash
    Utils::HashBuffer goodHash = container.getHeaderHash();
    Utils::HashBuffer badHash = goodHash;
    ++(badHash.getBuffAddr()[0]);
    container.setHeaderHash(badHash);
    // Check that the hashes don't match
    Utils::HashBuffer storedHash;
    Utils::HashBuffer computedHash;
    Fw::Success status = container.checkHeaderHash(storedHash, computedHash);
    ASSERT_EQ(status, Fw::Success::FAILURE);
    ASSERT_EQ(storedHash, badHash);
    ASSERT_EQ(computedHash, goodHash);
    // Perturb the data hash
    goodHash = container.getDataHash();
    badHash = goodHash;
    ++(badHash.getBuffAddr()[0]);
    container.setDataHash(badHash);
    // Check that the hashes don't match
    status = container.checkDataHash(storedHash, computedHash);
    ASSERT_EQ(status, Fw::Success::FAILURE);
    ASSERT_EQ(storedHash, badHash);
    ASSERT_EQ(computedHash, goodHash);
}

TEST(Header, BufferSet) {
    COMMENT("Test header serialization with buffer set");
    // Create a buffer
    Fw::Buffer buffer(bufferData, sizeof bufferData);
    // Fill with data
    fillWithData(buffer);
    // Use the buffer to create a container
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    DpContainer container;
    container.setId(id);
    container.setBuffer(buffer);
    // Check the header
    checkHeader(id, buffer, container);
    // Check the buffers
    checkBuffers(container, sizeof bufferData);
    // Invalidate the buffer
    container.invalidateBuffer();
    // Check that the buffer is invalid
    ASSERT_EQ(container.getBuffer(), Fw::Buffer());
    // Check that the data size is zero
    ASSERT_EQ(container.getDataSize(), 0);
}

TEST(Header, BadPacketDescriptor) {
    COMMENT("Test header serialization with bad packet descriptor");
    // Create a buffer
    Fw::Buffer buffer(bufferData, sizeof bufferData);
    // Set the packet descriptor to a bad value
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    const FwPacketDescriptorType badPacketDescriptor = Fw::ComPacket::FW_PACKET_DP + 1;
    Fw::SerializeStatus status = serialRepr.serialize(badPacketDescriptor);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // Use the buffer to create a container
    DpContainer container;
    container.setBuffer(buffer);
    // Deserialize the header
    const Fw::SerializeStatus serialStatus = container.deserializeHeader();
    // Check the error
    ASSERT_EQ(serialStatus, Fw::FW_SERIALIZE_FORMAT_ERROR);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}

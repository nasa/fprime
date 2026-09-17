// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include <cstring>
#include <limits>
#include <vector>

#include "gtest/gtest.h"

#include "Fw/Dp/DpContainer.hpp"
#include "Fw/Dp/test/ut/DpContainerTester.hpp"
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
    const auto procTypes = static_cast<DpCfg::ProcType::SerialType>(STest::Pick::startLength(0, numProcTypeStates));
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
    ASSERT_TRUE(Fw::DpContainerTester::verifyBufferSize(container, bufferSize));
    // Check the data buffer
    U8* const buffPtr = Fw::DpContainerTester::getBufferPointers(container);
    U8* const dataPtr = &buffPtr[Fw::DpContainer::DATA_OFFSET];
    const FwSizeType dataCapacity = bufferSize - Fw::DpContainer::MIN_PACKET_SIZE;
    ASSERT_TRUE(Fw::DpContainerTester::verifyDataBufferAddress(container, dataPtr));
    ASSERT_TRUE(Fw::DpContainerTester::verifyDataBufferCapacity(container, dataCapacity));
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
    const FwDpIdType id =
        static_cast<FwDpIdType>(STest::Pick::lowerUpper(0, static_cast<U32>(std::numeric_limits<FwDpIdType>::max())));
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
    const FwDpIdType id =
        static_cast<FwDpIdType>(STest::Pick::lowerUpper(0, static_cast<U32>(std::numeric_limits<FwDpIdType>::max())));
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
    auto serializer = buffer.getSerializer();
    const FwPacketDescriptorType badPacketDescriptor = Fw::ComPacketType::FW_PACKET_DP + 1;
    Fw::SerializeStatus status = serializer.serializeFrom(badPacketDescriptor);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // Use the buffer to create a container
    DpContainer container;
    container.setBuffer(buffer);
    // Deserialize the header
    const Fw::SerializeStatus serialStatus = container.deserializeHeader();
    // Check the error
    ASSERT_EQ(serialStatus, Fw::FW_SERIALIZE_FORMAT_ERROR);
}

TEST(DataSize, CapacityBoundedByHeader) {
    COMMENT("Test that the data capacity is bounded by what the header can represent");
    // Keep the test buffer to a reasonable size when FwSizeStoreType is wide
    constexpr FwSizeType MAX_TEST_DATA_SIZE = 1024 * 1024;
    if (DpContainer::MAX_DATA_SIZE > MAX_TEST_DATA_SIZE) {
        GTEST_SKIP() << "FwSizeStoreType is too wide to exercise the data size bound";
    }
    // Create a buffer with room for one byte more than the header can represent
    const FwSizeType bufferSize = DpContainer::getPacketSizeForDataSize(DpContainer::MAX_DATA_SIZE + 1);
    std::vector<U8> bigBufferData(bufferSize, 0);
    Fw::Buffer buffer(bigBufferData.data(), static_cast<Fw::Buffer::SizeType>(bufferSize));
    DpContainer container;
    container.setBuffer(buffer);
    // The data capacity must be clamped to the maximum data size
    ASSERT_TRUE(Fw::DpContainerTester::verifyDataBufferCapacity(container, DpContainer::MAX_DATA_SIZE));
    // Filling the data buffer to the maximum data size succeeds
    Fw::ExternalSerializeBuffer& dataBuffer = Fw::DpContainerTester::getDataBuffer(container);
    std::vector<U8> data(DpContainer::MAX_DATA_SIZE, 0xA5);
    Fw::SerializeStatus status =
        dataBuffer.serializeFrom(data.data(), DpContainer::MAX_DATA_SIZE, Fw::Serialization::OMIT_LENGTH);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // One more byte reports no room left rather than exceeding the bound
    const U8 extraByte = 0;
    status = dataBuffer.serializeFrom(extraByte);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_NO_ROOM_LEFT);
    // A data size at the bound serializes into the header and round trips
    container.setDataSize(DpContainer::MAX_DATA_SIZE);
    container.serializeHeader();
    container.updateDataHash();
    DpContainer deserContainer;
    deserContainer.setBuffer(container.getBuffer());
    status = deserContainer.deserializeHeader();
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(deserContainer.getDataSize(), DpContainer::MAX_DATA_SIZE);
    ASSERT_EQ(deserContainer.getPacketSize(), DpContainer::getPacketSizeForDataSize(DpContainer::MAX_DATA_SIZE));
    Utils::HashBuffer storedHash;
    Utils::HashBuffer computedHash;
    ASSERT_EQ(deserContainer.checkDataHash(storedHash, computedHash), Fw::Success::SUCCESS);
}

TEST(DataSize, SetTooLarge) {
    COMMENT("Test that setting a data size larger than the header can represent asserts");
    if (DpContainer::MAX_DATA_SIZE >= std::numeric_limits<FwSizeType>::max()) {
        GTEST_SKIP() << "FwSizeStoreType is as wide as FwSizeType; the bound cannot be exceeded";
    }
    Fw::Buffer buffer(bufferData, sizeof bufferData);
    DpContainer container;
    container.setBuffer(buffer);
    // The bound itself is accepted
    container.setDataSize(DpContainer::MAX_DATA_SIZE);
    ASSERT_EQ(container.getDataSize(), DpContainer::MAX_DATA_SIZE);
    // One past the bound is rejected
    ASSERT_DEATH(container.setDataSize(DpContainer::MAX_DATA_SIZE + 1), "Assert");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
